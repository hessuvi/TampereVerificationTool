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

// FILE_DES: InOutStream.hh
// Nieminen Juha

// $Id: InOutStream.hh 1.4.1.24 Tue, 25 Feb 2003 17:00:14 +0200 timoe $
// 
// InOutStream piilottaa käytössä olevat syöte- ja tulosmerkkivuot ja
// niihin liittyvien palvelujen toteutukset luokkien InStream ja
// OutStream sisälle.
//

// $Log:$

#ifndef INOUTSTREAM_HH
#define INOUTSTREAM_HH

#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

#include "History.hh"


class Stream
{
 public:
    Stream();
    Stream( const Stream& );
    virtual ~Stream();

    Stream& operator=( const Stream& );

    virtual bool isInStream() const = 0;
    virtual bool isOutStream() const = 0;

    void SetFilename( const std::string& );
    // Asettaa tiedostonimen.
    const std::string& GiveFilename() const;
    // Palauttaa vuolle annetun tiedostonimen (voi olla myös stdin:in tai
    //   stdout:in yhteydessä annettu nimi).

    bool isFileOpen() const;
    // Kertoo onko vuohon avattu tiedostoa (ei siis koske stdin:iä tai
    //   stdout:ia; niiden tapauksessa palautetaan false).
    bool CloseFile();
    // Tarkistaa onko vuohon avattu tiedostoa ja sulkee sen. Jos
    //   referenssilaskuri on kuitenkin suurempi kuin yksi, ei tiedostoa
    //   suljeta ja palautetaan false. Jos tiedoston sulkeminen
    //   onnistui on paluuarvona true.

 protected:
    void setStream( std::FILE* );
    void resetStream();
    std::FILE* giveStream() const;
    class StreamData* stream_data;

 private:
    void removeOldStreamData();
};


class InStream : public Stream
{
 public:
    InStream();
    InStream( const std::string& filename );
    // Tämä rakentaja suorittaa lisäksi kutsun
    //   OpenInputFile( filename ).
    InStream( const InStream& );

    virtual ~InStream();

    InStream& operator=( const InStream& );

    InStream& OpenInputFile( const std::string& filename );
    // Avaa tiedoston `filename' lukemista varten
    //   ( `-' tulkitaan stdin:iksi).

    InStream& OpenStdin( const std::string& filename = "stdin" );
    // Avaa vuon lukemaan standard input:ista.

    bool isStdin() const;
    // Palauttaa true jos vuohon on avattu stdin.

    void SetFileType( const std::string& type );
    const std::string& GiveFileType() const;

    History& GiveHistory();

    void SetAllowInterruptedLSTS( bool b = true )
    { allow_interrupted_lsts = b; }
    bool isInterruptedLSTS_Allowed() const
    { return allow_interrupted_lsts; }


    // FOR THE INTERNAL USE OF THE LSTS I/O LIB:

    typedef std::FILE* istream_t;
    istream_t GiveInStream() const;

 private:
    virtual bool isInStream() const;
    // Palauttaa aina true.
    virtual bool isOutStream() const;
    // Palauttaa aina false.
    bool allow_interrupted_lsts;
};


class OutStream : public Stream
{
 public:
    OutStream();
    // Rakentaja suorittaa alustukset, ei tee mitään muuta.

    OutStream( const std::string& filename, bool overwrite_if_exists );
    // Tämä rakentaja suorittaa lisäksi kutsun
    //   OpenOutputFile( filename, overwrite_if_exists ).

    virtual ~OutStream();

    OutStream& OpenOutputFile( const std::string& filename,
                               bool overwrite_if_exists );
    // Avaa tiedoston `filename' kirjoitusta varten
    //   ( `-' tulkitaan stdout:iksi).
    // Jos 2. parametri on true, kirjoitetaan kirjoitushakemistossa
    // mahdollisesti olevan saman nimisen tiedoston päälle.
    // Jos ko. parametri on false ja kirjoitushakemistosta löytyy
    // saman niminen tiedosto, heitetään poikkeus.

    OutStream& OpenStdout( const std::string& filename = "stdout" );
    // Avaa vuon lukemaan standard output:ista.

    bool isStdout() const;
    // Palauttaa true jos vuohon on avattu stdout.


    // Metodit jotka eivät liene käyttäjälle mielenkiintoisia:

    OutStream( const std::string& );
    // Tämä rakentaja on erikseen tapausta "-" varten.

    OutStream& OpenOutputFile( const std::string& );
    // Tämä metodi on erikseen tapausta "-" varten.

    typedef std::FILE* ostream_t;
    ostream_t GiveOutStream() const;

    // Komentorivin ja kommenttirivien käsittely:

    void SetCommandLine( const std::string& );
    void AddCommentLine( const std::string& );

    std::string GiveCommandLine() const;
    const std::vector<std::string>& GiveCommentLines() const;

    // Komentorivijäsentäjän kertoma aika:

    void SetBeginTime( std::time_t );
    std::time_t GiveBeginTime() const;

    void AddInStream( InStream& );

    static void SetForce( bool b ) { overWritingPermitted = b; }

    // The section will be copied to output LSTS file:
    static void CopySection( const std::string& section_name );
    static bool isCopiedSection( const std::string& section_name );

    // The section will be written to a file of its own while
    // writing the output LSTS file:
    static void ExtractSection( const std::string& section_name,
                                const std::string& include_filename );
    // If a section given as a parameter is to be extracted from the
    // output, a pointer to its name is returned, otherwise zero:
    static OutStream* const
    isExtractedSection( const std::string& section_name );

    static void
    CheckCommandLineOptions( const std::vector
                             <const std::string*>& sectionNames );

    // FOR THE INTERNAL USE OF THE LSTS I/O LIB:

    const std::vector<InStream*>& GiveInStreams() const;

    std::string CreateRunTimeString() const;

private:
    virtual bool isInStream() const;
    // Palauttaa aina false.
    virtual bool isOutStream() const;
    // Palauttaa aina true.

    OutStream( const OutStream& );
    OutStream& operator=( const OutStream& );
    // Näiden käyttö on estetty.

    std::string command_line;
    std::vector<std::string> comment_lines;

    static std::vector<std::string> copied_sections;
    static std::vector<std::string> extracted_sections;
    static std::vector<OutStream*> section_oss;
    static bool overWritingPermitted;

    std::time_t begin_time;

    std::vector<InStream*> in_streams;
};

/*
  Principles of copying and extracting sections.

  --copy <section>
  * If <section> is in an input file -> no effect.
  * If <section> is referred to in an input file ->
    <section> will be in the output file.
  
  --extract <section> <file>
  * If <section> is referred to in an input file and
    content of <section> is not altered -> no effect.
  * If <section> is in an input file or
    content of <section> is changed -> <section> will be written
    to a file of its own and referred to in the output file.
*/


#endif
