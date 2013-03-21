/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#include "FileFormat.hh"
#include "Section.hh"
#include "Forwarder.hh"
#include "LSTS_File/SectionAP.hh"
#include "Exceptions.hh"
#include "FileFormat/Lexical/charconv.hh"


// PUBLIC:
FileFormat::FileFormat( const std::string& begin_file,
                        const std::string& end_file ) :
    beginFile( begin_file ), endFile( end_file ),

    no_reader_action( ERROR ),
    warn_when_skipping( true ),
    ignore_duplicate_readers( false ),

    mainInS( 0 ), mainOutS( 0 ),

    from( &mainFrom ), to( &mainTo ),

    includeFrom( 0 ) { }

FileFormat::~FileFormat() { Clear(); }

void
FileFormat::OpenInputOTS( OutStream& os )
{ mainOutS = &os; mainTo.Open( os ); }

OTokenStream&
FileFormat::WriteBeginFile( OutStream& os )
{
    if ( !mainOutS ) { OpenInputOTS( os ); }
    // Writing the beginning of the file:
    mainTo.spaceOff();
    if ( beginFile == "LSTS" )
    { mainTo.PutIdentifier( "Begin" ); mainTo.spaceOn(); }
    mainTo.PutIdentifier( beginFile );
    mainTo.spaceOn();
    return mainTo;
}

void
FileFormat::WriteEndFile()
{
    // Writing the end of the file:    
    mainTo.Endl();
    mainTo.Endl();

    mainTo.spaceOff();
    if ( endFile == "LSTS" )
    {
        mainTo.PutIdentifier( "End" );
        mainTo.spaceOn();
    }
    mainTo.PutIdentifier( endFile );
    mainTo.spaceOn();
    mainTo.Endl();

    if ( endFile == "LSTS" )
    {
        mainTo.Endl();
        mainTo.PutString( "Run time: " + mainOutS->CreateRunTimeString() );
        mainTo.Endl();
    }
}

static bool areOptionsChecked = false;

void
FileFormat::WriteFile( OutStream& os )
{
    if ( !areOptionsChecked )
    {
        std::vector<const std::string*> secNames( sectionsOrder.size(), 0 );
        for ( unsigned i = 0; i < sectionsOrder.size(); ++i )
        { secNames[i] = &sectionsOrder[i]->name; }

        OutStream::CheckCommandLineOptions( secNames );
        areOptionsChecked = true;
    }

    WriteBeginFile( os );
      

    // Initializing the sections:
    std::vector<SecInfo*> activeWriters;
    passed_sections.clear();


    for ( unsigned i = 0; i < sectionsOrder.size(); ++i )
    {
        SecInfo& si = *sectionsOrder[i];
        oSection* writer = si.writer;
        
        if ( writer )
        {
            // Initializing a section:
            if ( si.includeStream &&
                 writer->doWeWriteWithInit( *si.includeStream, *header ) ||
                 writer->doWeWriteWithInit( mainTo, *header ) )
            {
                // This section will be written:
                activeWriters.push_back( &si );
                passed_sections.push_back( writer->GiveName() );
            }

        }
        else
        {
            // Missing a compulsory section?
            check_claim( si.optional, mainTo.errorMsg(
                           std::string( "missing a writer for " ) + si.name +
                           " section that is compulsory in this file format "
                           "(" + beginFile + ")" ) );
        }

    } // End of for.


    // Writing the sections:

    for ( unsigned i = 0; i < activeWriters.size(); ++i )
    {
        SecInfo& info = *activeWriters[i];
        mainTo.SetSectionName( info.name );

        std::string incFilename;
        if ( info.writer->GiveoAP() )
        { incFilename = info.writer->GiveoAP()->GiveIncludeFilename(); }

        const OutStream& outs = *(ots().GiveOutStream());
        OutStream* const ext = outs.isExtractedSection( info.name );

        if ( ext || info.includeStream || incFilename.size() )
        {
            mainTo.Endl();
            mainTo.Endl();

            mainTo.spaceOff();
            mainTo.PutIdentifier( "Include" );
            mainTo.spaceOn();
            
            mainTo.PutIdentifier( info.name );
            mainTo.PutIdentifier( "From" );
         
            // The user has asked the section to be written into a
            // file of its own?
            if ( ext )
            {
                mainTo.PutString( ext->GiveFilename() );
                oTokenStream incOts( *ext );
                incOts.SetSectionName( info.name );
                info.writer->Write( *header, &incOts );
                incOts.Endl();
                delete ext;
                continue;
            }

            // Forward a reference to a section file only?
            if ( incFilename.size() )
            { mainTo.PutString( incFilename ); continue; }

            mainTo.PutString( info.includeStream->GiveFilename() );
            
            info.includeStream->UseCapitalIdentifiers();
            info.includeStream->spaceOff();
            info.includeStream->PutWord( std::string(
                   "# An LSTS include file attached originally to " ) +
                                         mainTo.GiveFilename() );
        }
            
        info.writer->Write( *header );
    }
    
    WriteEndFile();
    DoEndFileChecks( passed_sections, to );
}

void
FileFormat::CheckValidity( const SecInfo& info, const std::string& name,
                           std::string fun_name )
{
    if ( fun_name.size() ) { fun_name = "FileFormat::" + fun_name + "(): "; }
    check_claim( info.priority, from->errorMsg( fun_name + name +
                                 " section does not belong to this "
                                 "file format (" + beginFile + ")" ) );
}


void
FileFormat::ReadFile()
{
    mainFrom.GiveInStream()->SetFileType( beginFile );

    passed_sections.clear();
    std::string name; int expPriority = 1;

    std::vector<SecInfo*> compulsories;
    // Collecting readable compulsory sections in reverse priority order:
    for ( int i = sectionsOrder.size() - 1; i >= 0; --i )
    {
        if ( sectionsOrder[i]->priority > 0 && !sectionsOrder[i]->optional )
        { compulsories.push_back( sectionsOrder[i] ); }
    }

    for ( ; ReadBeginSection( name );
          ( compulsories.size() && ( name == compulsories.back()->name ) ) ?
              compulsories.pop_back() :
              static_cast<void>( 0 ) )
        // Nice piece of code, eh :)
    {
        SecInfo& info = sectionsFastAccess[ name ];
        
        // Valid section? (map's []-op. creates a new container
        //   for key that's not found, and priority of a new struct
        //   is initialized to zero - so that's how we know the query
        //   was not succesful.)
        CheckValidity( info, name );

        // Wrong location?
        check_claim( info.priority >= expPriority,
                     from->errorMsg( name + " section "
                                     "should locate earlier in the file" ) );

        if ( info.priority > expPriority ) { expPriority = info.priority; }

        // Missing a compulsory section?
        check_claim( compulsories.empty() || 
                     expPriority <= compulsories.back()->priority,
                     from->errorMsg( compulsories.back()->name +
                                     " section expected" ) );
        if ( info.skip ||
             info.skip_if_not_supported && !info.reader && !info.forwarder )
        {
            skipSection( name );
            warn_if_fails( !warn_when_skipping,
                           from->errorMsg( name + " section skipped" ) );
            continue;
        }

        if ( info.reader ) { ReadSection( *(info.reader) ); }
        else if ( !info.forwarder )
        { noReader( "unsupported section" ); skipSection( name ); }

        passed_sections.push_back( name );
        if ( stop_reading_asap && passed_sections.size() == number_of_readers )
        { return; }

    } // End of for.

    // Missing a compulsory section?
    check_claim( compulsories.empty(),
                 from->errorMsg( compulsories.back()->name +
                                 " section expected" ) );
    DoEndFileChecks( passed_sections, from );
}

void
FileFormat::SkipUnwantedSections( bool warn )
{
    if ( warn ) { no_reader_action = WARN; return; }
    no_reader_action = IGNORE;
}

void
FileFormat::WarnWhenSkippingSection() { warn_when_skipping = true; }

void
FileFormat::QuietWhenSkippingSection() { warn_when_skipping = false; }

void
FileFormat::SetNoReaderAction( NoReaderAction action )
{ no_reader_action = action; }


// PROTECTED:
void
FileFormat::AddSection( const std::string& name, int priority,
                        bool optional, bool skip_if_not_supp )
{
    SecInfo& info = sectionsFastAccess[ name ];
    sectionsOrder.push_back( &info );

    info.name = name;
    info.priority = priority;
    info.optional = optional;
    info.skip_if_not_supported = skip_if_not_supp;
}

void
FileFormat::Clear()
{
    for ( int i = sectionsOrder.size() - 1; i >= 0; --i )    
    {
        const SecInfo& sec = *sectionsOrder[i];
        if ( sec.includeStream ) { delete sec.includeStream; }
    }
    
    sectionsFastAccess.clear(); sectionsOrder.clear();
    number_of_readers = 0; stop_reading_asap = false;
}

void
FileFormat::AddWriter( oSection& writer )
{
    SecInfo& info = sectionsFastAccess[ writer.GiveName() ];
    CheckValidity( info, writer.GiveName(), "AddWriter" );

    info.name = writer.GiveName(); info.writer = &writer;

    oSectionAP* oSecAP = writer.GiveoAP();

    if ( !oSecAP ) { return; }

    if ( oSecAP->GiveIncludeFile() )
    {
        info.includeStream = new OTokenStream( *oSecAP->GiveIncludeFile() );
        info.includeStream->SetSectionName( writer.GiveName() );
    }
}

void
FileFormat::AddReader( iSection& reader )
{
    SecInfo& info = sectionsFastAccess[ reader.GiveName() ];
    if ( ignore_duplicate_readers && info.reader ) { return; }
    CheckValidity( info, reader.GiveName(), "AddReader" );
    info.name = reader.GiveName(); info.reader = &reader;
    ++number_of_readers;
}

const std::string&
FileFormat::ReadBeginFile( InStream& is )
{
    mainInS = &is;
    // First we open an input token stream:
    mainFrom.Open( is );

    // In the beginning of the file there should be a proper start of file
    //   identifier.
    beginFileLabel = mainFrom.Get( "unknown file type",
                                   TT::IDENTIFIER ).stringData();
    if ( beginFileLabel == "BEGIN" )
    { beginFileLabel = mainFrom.Get( "file format identifier expected",
                                     TT::IDENTIFIER ).stringData(); }
    return beginFileLabel;
}

// Reads the beginning identifier and the name of the section.
//   Assigns the section name to the reference parameter and returns true
//   if the reading succeeded, otherwise section name will be erased and
//   false returned.
bool
FileFormat::ReadBeginSection( std::string& sectionName )
{
    if ( includeFrom )
    {
        delete includeFrom; includeFrom = 0;
        from = &mainFrom;
        include_is.CloseFile();
    }

    const std::string* idf = 0;

    if ( mainFrom.Peek().isEndOfFile() ||   // For some Ara files
         *(idf = &mainFrom.Get( TT::IDENTIFIER ).stringData() ) == "END" &&
         mainFrom.Get( TT::IDENTIFIER ).stringData() == endFile ||
         *idf == endFile ||
         idf->substr( idf->size() - 3 ) ==   // For some Ara files
         endFile.substr( endFile.size() - 3 ) )
    {
        sectionName.erase();
        // For error messages:
        mainFrom.SetSectionName( "EOF" );

        if ( mainFrom.Peek().isString() )
        { mainInS->GiveHistory().back().
                AddCommentLine( mainFrom.Get().stringData() ); }
        return false;
    }

    if ( *idf == "INCLUDE" )
    {
        sectionName = mainFrom.Get( "section name expected",
                                    TT::IDENTIFIER ).stringData();
        mainFrom.Get( TT::IDENTIFIER, "FROM" );
        const std::string& filename = mainFrom.Get( TT::STRING ).stringData();

        SecInfo& info = sectionsFastAccess[ sectionName ];
        CheckValidity( info, sectionName );

        if ( info.forwarder && !OutStream::isCopiedSection( sectionName ) &&
             !OutStream::isExtractedSection( sectionName ) &&
             !info.forwarder->isReadAlways() && false )
        {
            info.forwarder->SetSectionFilename( filename );
            info.forwarder->ShowHeader( *header );
            info.reader = 0;
            return true;
        }

        include_is.OpenInputFile( filename );

        includeFrom = new ITokenStream( include_is );
        from = includeFrom;

        idf = &from->Get( TT::IDENTIFIER ).stringData();
    }


    if ( *idf == "BEGIN" )
    {
        idf = &from->Get( "section name expected",
                          TT::IDENTIFIER ).stringData();
        check_claim( !includeFrom || areStringsEqual( sectionName, *idf ),
                     from->errorMsg( *idf, sectionName + " expected" ) );
    }

    sectionName = *idf;

    // For error messages:
    from->SetSectionName( sectionName );
    return true;
}

void
FileFormat::ReadSection( iSection& reader )
{
    from->Refresh();
    reader.Read( *from, *header );

    // Reading the end of the section:
    if ( from->Get( TT::IDENTIFIER ).stringData() == "END" )
    { from->Get( TT::IDENTIFIER, reader.GiveName() ); }
}

// Marks a section to be skipped during reading.
void
FileFormat::SkipSection( const std::string& name )
{
    SecInfo& info = sectionsFastAccess[ name ];
    CheckValidity( info, name, "SkipSection" );
    info.skip = true;
}

void
FileFormat::AddForwarder( const std::string& section_name, Forwarder* fw )
{
    SecInfo& info = sectionsFastAccess[ section_name ];
    CheckValidity( info, section_name, "AddForwarder" );
    info.forwarder = fw;
}
   
// Changes new identifiers for the beginning and the end of the file.
void
FileFormat::SetFileIdentifiers( const std::string& begin_file,
                                const std::string& end_file )
{ beginFile = begin_file; endFile = end_file; }


// PRIVATE:
// This is called when a compulsory section is missing from a file.
void
FileFormat::noReader( const std::string& msg )
{
    if ( no_reader_action )
    {
        check_claim( no_reader_action != ERROR, mainFrom.errorMsg( msg ) );
        warn_if_fails( no_reader_action != WARN, mainFrom.errorMsg( msg ) );
    }
}

void
FileFormat::skipSection( const std::string& name )
{
    from->SetPunctuationCheck( false );
    // The following is for over riding the floating number recognition
    // in iTokenStream:
    from->SetPunctuation( "." );

    const TT::Token* token = 0;
    // For reading ARA files:
    const std::string araEndSection = "EnD_" + name;

    do { token = &mainFrom.Get(); }
    while( !token->isIdentifier( "End" ) &&
           !token->isIdentifier( araEndSection ) );
    
    if ( token->isIdentifier( "eNd" ) )
    { from->Get( TT::IDENTIFIER, name ); }

    from->SetPunctuationCheck( true );
}

void
FileFormat::DoEndFileChecks( const std::vector<std::string>&,
                             TokenStream* ) { }


FileFormat::SecInfo::SecInfo() :
    priority( 0 ), optional( false ), skip( false ),
    reader( 0 ), forwarder( 0 ), writer( 0 ), includeStream( 0 ) { }
