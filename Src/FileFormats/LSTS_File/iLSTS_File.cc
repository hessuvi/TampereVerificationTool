/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä
*/

#include "iLSTS_File.hh"

#include "LSTS_Sections/HeaderSection.hh"
#include "LSTS_Sections/HistorySection.hh"
#include "LSTS_Sections/LSTS_Sections.hh"
#include "LSTS_Sections/LSTS_Checkers.hh"

#include "oLSTS_File.hh"


iLSTS_File::iLSTS_File( InStream& is, bool ch, bool allowInterruptedLSTS )
    : LSTS_File( ch ),
      can_read_interrupted_lsts( allowInterruptedLSTS )
{
    if ( is.isInterruptedLSTS_Allowed() )
    { can_read_interrupted_lsts = true; }

    const std::string& begin_file = ReadBeginFile( is );
    readHeader( begin_file );
}

iLSTS_File::~iLSTS_File() { }

void
iLSTS_File::AddStateNamesReader( iStateNamesAP& iap ) { AddInputAP( iap ); }

void
iLSTS_File::AddDivBitsReader( iDivBitsAP& iap ) { AddInputAP( iap ); }

void
iLSTS_File::AddAccSetsReader( iAccSetsAP& iap ) { AddInputAP( iap ); }

void
iLSTS_File::AddStatePropsReader( iStatePropsAP& iap ) { AddInputAP( iap ); }

void
iLSTS_File::AddActionNamesReader( iActionNamesAP& iap ) { AddInputAP( iap ); }

void
iLSTS_File::AddTransitionsReader( iTransitionsAP& iap ) { AddInputAP( iap ); }

void
iLSTS_File::AddLayoutReader( iLayoutAP& iap ) { AddInputAP( iap ); }

OTokenStream&
iLSTS_File::ConnectOutput( OutStream& os ) { return WriteBeginFile( os ); }

void
iLSTS_File::SyncReadAndWrite()
{
    OTokenStream& ot = ots();
    Header& hd = GiveHeader();
    
    oHistorySection ohistory( ot );
    ohistory.Write( hd );
    oHeaderSection oheader( hd, ot );
    oheader.Write( hd );

    IgnoreDuplicateReaders();

    oActionNamesSection oaction_names( ot );
    ActionNamesChecker an_ch( oaction_names, *this );
    AddActionNamesReader( an_ch ); //oaction_names );

    oStateNamesSection ostate_names( ot );
    AddStateNamesReader( ostate_names );

    oTransitionsSection otransitions( ot );
    TransitionsChecker tr_ch( otransitions, *this );
    AddTransitionsReader( tr_ch ); //otransitions );

    oDivBitsSection odiv_bits( ot );
    AddDivBitsReader( odiv_bits );

    oAccSetsSection oacc_sets( ot );
    AddAccSetsReader( oacc_sets );

    oStatePropsSection ostate_props( ot );
    StatePropsChecker sp_ch( ostate_props, *this );
    AddStatePropsReader( sp_ch ); //ostate_props );
    
    //oLayoutSection olayout( ot );
    //AddLayoutReader( olayout );

    IgnoreDuplicateReaders( false );

    FileFormat::ReadFile();

    WriteEndFile();
}


/*void
iLSTS_File::ForwardActionNames( oLSTS_File& olsts, ActionNamesStore* )
{
    //   if ( ots().GiveOutStream()->isExtractedSection( "Action_names" ) )
    //{ return; }

    ActionNamesChecker* anc = new ActionNamesChecker( *this );
    AddInputSection<iActionNamesSection, iActionNamesAP>( *anc );
    ForwardReference( "ACTION_NAMES" );

    anc->action_cnt = GiveHeader().GiveActionCnt();
    bool ch = olsts.areChecksOn();
    olsts.SetChecks( false );
    olsts.AddOutputAP<oActionNamesAP>( *anc );
    olsts.SetChecks( ch );
}*/

// PROTECTED:


iLSTS_File::iLSTS_File( bool ch ) : LSTS_File( ch )
{ }

void
iLSTS_File::readHeader( const std::string& begin_file )
{
    check_claim( begin_file == "LSTS" || begin_file == "LSTS_FILE",
                 its().errorMsg( begin_file, "an LSTS file expected" ) );
    
    std::string sectionName;
    ReadBeginSection( sectionName );
    
    if ( sectionName == "HISTORY" )
    {
        iHistorySection ihistory_section;
        ReadSection( ihistory_section );
        ReadBeginSection( sectionName );
    }
    
    check_claim( sectionName == "HEADER",
                 its().errorMsg( "Header section expected" ) );


    Header& hd = GiveHeader();
    iHeaderSection iheader_section( hd );
    ReadSection( iheader_section );

    check_claim( !hd.isInterrupted() || can_read_interrupted_lsts,
                 its().errorMsg( "the LSTS is interrupted "
                                 "-- unable to continue" ) );
    DoHeaderChecks( &its() );
}
