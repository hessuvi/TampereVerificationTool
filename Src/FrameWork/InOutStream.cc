/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen, Timo Erkkilä.
*/

// FILE_DES: InOutStream.cc: Src/FileFormats/FileFormat/Lexical
// Juha Nieminen & Timo Erkkilä

static const char * const ModuleVersion=
"Module version: $Id: InOutStream.cc 1.2.1.24 Tue, 25 Feb 2003 17:00:14 +0200 timoe $";

// 
// The implementation of InOutStream module.
//

// $Log:$

#include "InOutStream.hh"
#include "History.hh"
#include "error_handling.hh"
#include "FileFormat/Lexical/charconv.hh"

#include <cerrno>
#include <cstring> // strerror
#include <iostream>
using namespace std;


class StreamData
{
 public:
    StreamData() : file_type( "not_given" ),
                   ref_count( 1 ),
                   stream( 0 ) { }
    ~StreamData() { check_claim( closeStream(), "~StreamData(): ref != 1" ); }

    StreamData* oneRefMore() { ++ref_count; return this; }
    int oneRefLess() { return --ref_count; }

    bool closeStream()
    {
        if ( stream && ref_count <= 1 )
        {
            fclose( stream );
            stream = 0;
        }

        return !stream;
    }

    bool isOpen() { return stream; }

    string filename;
    string file_type;

    int ref_count;
    FILE* stream;
    History history;
};


// Stream class
// PUBLIC:

Stream::Stream( const Stream& st )
{ stream_data = st.stream_data->oneRefMore(); }

Stream&
Stream::operator=( const Stream& st )
{
    removeOldStreamData();
    stream_data = st.stream_data->oneRefMore();
    return *this;
}

Stream::~Stream()
{ removeOldStreamData(); }

void
Stream::SetFilename( const string& fName ) { stream_data->filename = fName; }

bool
Stream::isFileOpen() const { return stream_data->isOpen(); }

const string&
Stream::GiveFilename() const { return stream_data->filename; }

bool
Stream::CloseFile() { return stream_data->closeStream(); }


// Stream class
// PROTECTED:
Stream::Stream() : stream_data( new StreamData() ) { }

void
Stream::setStream( FILE* st ) { stream_data->stream = st; }

void
Stream::resetStream()
{
    removeOldStreamData();
    stream_data = new StreamData();
}

FILE*
Stream::giveStream() const { return stream_data->stream; }


// Stream class
// PRIVATE:
void
Stream::removeOldStreamData()
{
    check_claim( stream_data, "Stream::removeOldStreamData(): bug" );

    if ( !stream_data->oneRefLess() )
    {
        delete stream_data;
        stream_data = 0;
    }
}


// InStream class
// PUBLIC:
InStream::InStream() : Stream(), allow_interrupted_lsts( false ) { }
InStream::InStream( const string& fname ) :
    Stream(), allow_interrupted_lsts( false ) 
{ OpenInputFile( fname ); }
InStream::InStream( const InStream& st ) :
    Stream( st ), allow_interrupted_lsts( st.allow_interrupted_lsts ) { }
InStream::~InStream() { }

InStream&
InStream::operator=( const InStream& ist )
{
    if ( this != &ist )
    {
        Stream::operator=( ist );
        allow_interrupted_lsts = ist.allow_interrupted_lsts;
    }
    return *this;
}

bool
InStream::isInStream() const { return true; }
bool
InStream::isOutStream() const { return false; }

InStream&
InStream::OpenInputFile( const string& fname )
{
    resetStream();

    if ( fname == "-" )
    {
        OpenStdin();
        return *this;
    }

    istream_t stream = fopen( fname.c_str(), "r" );
    check_claim( stream, "Can't open " + fname + ": "
                 + strerror( errno ) );

    setStream( stream );
    SetFilename( fname );

    return *this;
}

InStream&
InStream::OpenStdin( const string& fname )
{
    resetStream();
    istream_t stream = stdin;
    setStream( stream );
    SetFilename( fname );
    return *this;
}

bool
InStream::isStdin() const { return giveStream() == stdin; }

InStream::istream_t
InStream::GiveInStream() const { return giveStream(); }


History&
InStream::GiveHistory() { return stream_data->history; }

void
InStream::SetFileType( const string& type ) { stream_data->file_type = type; }

const string&
InStream::GiveFileType() const { return stream_data->file_type; }


// OutStream class
// PUBLIC:

OutStream::OutStream() : Stream() { }
OutStream::OutStream( const string& fname ) : Stream()
{ OpenOutputFile( fname ); }

OutStream::OutStream( const string& fname, bool overwrite_if_exists ) :
    Stream()
{ OpenOutputFile( fname, overwrite_if_exists ); }

OutStream::~OutStream() { }

bool OutStream::isOutStream() const { return true; }
bool OutStream::isInStream() const { return false; }


#define INIT_COMM_AND_TIME  command_line = "<command line not given>"; \
                            begin_time = time( NULL )


OutStream&
OutStream::OpenOutputFile( const string& fname )
{
    check_claim( fname == "-", "OutStream::OpenOutputFile(): this method must "
                 "only be called with \"-\" as its parameter" );

    INIT_COMM_AND_TIME;

    return OpenOutputFile( fname, false );
}

OutStream&
OutStream::OpenOutputFile( const string& fname,
                           bool overwrite_if_exists )
{
    resetStream();

    INIT_COMM_AND_TIME;

    if ( fname == "-" )
    {
        OpenStdout();
        return *this;
    }

    if( !overwrite_if_exists )
    {
        InStream::istream_t check = fopen( fname.c_str(), "r" );

        if( check )
        {
            fclose( check );
            AbortWithMessage( string("the output file `") + fname +
                              "' exists already. Use --force to overwrite.");
        }
    }

    ostream_t stream = fopen( fname.c_str(), "w" );
    check_claim( stream, "can't open " + fname + ": "
                 + strerror( errno ) );

    setStream( stream );
    SetFilename( fname );

    return *this;
}

OutStream&
OutStream::OpenStdout( const string& fname )
{
    resetStream();
    setStream( stdout );
    SetFilename( fname );
    INIT_COMM_AND_TIME;
    return *this;
}

bool
OutStream::isStdout() const { return giveStream() == stdout; }

OutStream::ostream_t
OutStream::GiveOutStream() const { return giveStream(); }

void
OutStream::SetCommandLine( const string& cl ) { command_line = cl; }

void
OutStream::AddCommentLine( const string& comm )
{ comment_lines.push_back( comm ); }

string
OutStream::GiveCommandLine() const { return command_line; }

const vector<string>&
OutStream::GiveCommentLines() const { return comment_lines; }

void
OutStream::SetBeginTime( time_t t ) { begin_time = t; }

time_t
OutStream::GiveBeginTime() const { return begin_time; }

void
OutStream::AddInStream( InStream& is ) { in_streams.push_back( &is ); }


void
OutStream::CopySection( const string& section_name )
{ copied_sections.push_back( section_name ); }

void
OutStream::ExtractSection( const string& section_name,
                           const std::string& section_filename )
{
    extracted_sections.push_back( section_name );
    section_oss.push_back( new OutStream( section_filename,
                                          overWritingPermitted ) );
}

OutStream* const
OutStream::isExtractedSection( const string& section_name )
{
    for ( unsigned i = 0; i < extracted_sections.size(); ++i )
    {
        if ( areStringsEqual( extracted_sections[i], section_name ) )
        { return section_oss[i]; }
    }

    return 0;
}

bool
OutStream::isCopiedSection( const string& section_name )
{
    for ( unsigned i = 0; i < copied_sections.size(); ++i )
    {
        if ( areStringsEqual( copied_sections[i], section_name ) )
        { return true; }
    }

    return false;
}

static void checkSubset( vector<string>& setA,
                         const vector<const string*>& setB )
{
    for ( unsigned i = 0; i < setA.size(); ++i )
    {
        bool matches = false;
        for ( unsigned j = 0; j < setB.size(); ++j )
        {
            if ( areStringsEqual( setA[i], *setB[j] ) )
            { matches = true; break; }
        }

        check_claim( matches, string ( "`" ) + setA[i] +
                     "' is an unknown section." );
    }

}

void
OutStream::CheckCommandLineOptions( const vector<const string*>& sectionNames )
{
    checkSubset( extracted_sections, sectionNames );
    checkSubset( copied_sections, sectionNames );
}

const vector<InStream*>&
OutStream::GiveInStreams() const { return in_streams; }


std::string
OutStream::CreateRunTimeString() const
{
    std::string timeLine;
    // Setting counting time to history:
    const int MAX_TIME_STR = 50;
    char time_str[ MAX_TIME_STR ];
    time_t endTime = time(NULL);
    unsigned readAndCountTime =
        static_cast<int>( difftime( endTime, GiveBeginTime() ) );

    // Minutes.
    if ( readAndCountTime > 60 )
    {
        unsigned mins = readAndCountTime / 60;

        //check_claim( MAX_TIME_STR >
        sprintf( time_str, "%u", mins ); /*,
                         "HistorySection::WriteSection(): "
                         "somehow time string grew too big." );*/

        timeLine += time_str;
        timeLine += " min. ";
    }

    // Seconds.

    if ( readAndCountTime >= 5 )
    {
        unsigned secs = readAndCountTime % 60;

        //check_claim( MAX_TIME_STR >
        sprintf( time_str, "%u", secs ); /*,
                         "HistorySection::WriteSection(): "
                         "somehow time string grew too big." );*/

        timeLine += time_str;
        timeLine += " sec.";
    }
    else
    {
        timeLine += "less than 5 sec.";
    }

    return timeLine;
}


std::vector<std::string> OutStream::copied_sections;
std::vector<std::string> OutStream::extracted_sections;
std::vector<OutStream*> OutStream::section_oss;
bool OutStream::overWritingPermitted = false;
