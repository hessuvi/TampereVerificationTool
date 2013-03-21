/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// FILE_DES: FileFormat.hh: FileFormats/FileFormat
// Timo Erkkilä

// $Id: FileFormat.hh 1.31 Fri, 24 Sep 2004 16:40:13 +0300 timoe $

// FileFormat is a general class for reading and writing that uses a
//   section principle and can read from or write to a file using any
//   kind of Section based objects.

// $Log: FileFormat.hh,v $
// Revision 1.1  1999/08/09 13:06:15  timoe
// Initial revision
//

#ifndef FILEFORMAT_HH
#define FILEFORMAT_HH

#include "Lexical/ITokenStream.hh"
#include "Lexical/OTokenStream.hh"

#include <string>
#include <vector>
#include <map>

// Predeclarations:
class iSection;
class oSection;
class Header;


class FileFormat
{
 public:
    FileFormat( const std::string& begin_file, const std::string& end_file );
    virtual ~FileFormat();
    
    void WriteFile( OutStream& os );
    void ReadFile();

    void StopReadingASAP() { stop_reading_asap = true; }

    void SkipUnwantedSections( bool warn_when_skipping = true );

    enum NoReaderAction { IGNORE = 0, WARN, ERROR };

    void SetNoReaderAction( NoReaderAction );

    void WarnWhenSkippingSection();
    void QuietWhenSkippingSection();

    ITokenStream& its() { return *from; }
    // Returns a reference to ITokenStream instance.
    OTokenStream& ots() { return *to; }
    // Returns a reference to OTokenStream instance.
    TokenStream& ts() { if ( mainInS ) { return *from; } return *to; }
    // Returns a reference to either ITokenStream or OTokenStream instance.

    Header& GiveHeader() { return *header; }
    const Header& GiveHeader() const  { return *header; }
    // These return a reference to the Header instance.

    void WriteEndFile();
    
    void OpenInputOTS( OutStream& os );

 protected:
    void AddSection( const std::string& name, int priority, bool optional,
                     bool skip_if_not_supported = false );
    void Clear();

    void SetHeader( Header& hd ) { header = &hd; }
    void SetInputFile( InStream& is ) { mainFrom.Open( is ); }

    void AddReader( iSection& );
    void IgnoreDuplicateReaders( bool ignore = true )
    { ignore_duplicate_readers = ignore; }

    void SkipSection( const std::string& name );
    void AddForwarder( const std::string& section_name, class Forwarder* fw );

    void AddWriter( oSection& );

    const std::string& ReadBeginFile( InStream& );
    bool ReadBeginSection( std::string& name );
    void ReadSection( iSection& reader );

    void SetFileIdentifiers( const std::string& beginFile,
                             const std::string& endFile );
    // Changes new identifiers for the beginning and the end of the file.

    OTokenStream& WriteBeginFile( OutStream& );

    virtual void DoEndFileChecks( const std::vector<std::string>&
                                  passed_sections,
                                  TokenStream* tokenStream );
 private:
    // Preventing the use of copy constructor and assignment:
    FileFormat ( const FileFormat& );
    FileFormat& operator=( const FileFormat& );

    void noReader( const std::string& msg );
    void skipSection( const std::string& name );

    struct SecInfo
    {
        SecInfo();

        std::string name; int priority;
        bool optional; bool skip; bool skip_if_not_supported;
        iSection* reader;
        class Forwarder* forwarder;

        oSection* writer;
        OTokenStream* includeStream;
    };

    typedef std::map<std::string, SecInfo> SectionMap;

    void CheckValidity( const SecInfo&, const std::string&,
                        std::string fun_name = "" );

    SectionMap sectionsFastAccess;
    
    std::vector<SecInfo*> sectionsOrder;

    Header* header;

    std::string beginFile; std::string endFile;
    std::string beginFileLabel;

    NoReaderAction no_reader_action;
    bool warn_when_skipping;
    bool ignore_duplicate_readers;
    bool stop_reading_asap;
    unsigned number_of_readers;

    InStream* mainInS;
    iTokenStream mainFrom;
    OutStream* mainOutS;
    oTokenStream mainTo;

    iTokenStream* from; oTokenStream* to;

    iTokenStream* includeFrom;
    InStream include_is;

    std::vector<std::string> passed_sections;
};


#endif
