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

// FILE_DES: OTokenStream.hh: FileFormats/FileFormat/Lexical
// Timo Erkkilä

// $Id: OTokenStream.hh 1.30 Thu, 11 Mar 2004 18:07:16 +0200 timoe $
// 
// Defines the OTokenStream class which writes lexical tokens to a 
//   character stream.
//

// $Log: OTokenStream.hh,v $
// Revision 1.1  1999/08/09 10:33:12  timoe
// Initial revision
//

#ifndef OTOKENSTREAM_HH
#define OTOKENSTREAM_HH

#include "TokenStream.hh"

#include <string>


class OTokenStream : public TokenStream
{
 public: 
    OTokenStream();
    explicit OTokenStream( OutStream& );
    // Constructs a token stream that is ready to write lexical tokens
    //   into an output stream.
    
    // OTokenStream also tests tokens given to it (except numbers)
    //   against capsuled rules in TokenStream class.

    virtual ~OTokenStream();

    void Open( OutStream& );
    // Opens the file :)
    
    inline void spaceOff();
    inline void spaceOn();
    // With these two methods the user can tell the stream whether a
    //   single preceding space is written before a token or not.
  
    void Put( const TT::Token& );
    // Writes a token.
    
    void PutString( const std::string& str );
    // Writes a string.
    void PutNumber( lsts_index_t number );
    // Writes a natural number.
    inline void PutNatNumber( lsts_index_t number ) { PutNumber( number ); }
    // Writes a natural number.
    void PutFloat( lsts_float_t fl_number );
    // Writes a floating point number.
    void PutIdentifier( const std::string& id );
    // Writes an identifier.
    void PutPunctuator( const std::string& punc );
    void PutPunctuation( const std::string& punc ) { PutPunctuator( punc ); }
    // Writes a punctuator.

    void PutFileInclusion( const std::string& file_path );
    // Writes a file inclusion command.
  
    void Endl();
    // Ends the current line.


    void UseAsIsIdentifiers();  
    // Uses "as is" style when writing identifier tokens.
    // ExaMple_Id -> ExaMple_Id

    void UseLowerCaseIdentifiers();
    // Uses lower case when writing identifier tokens.
    // ExaMple_Id -> example_id
    
    void UseUpperCaseIdentifiers();
    // Uses upper case when writing identifier tokens.
    // ExaMple_Id -> EXAMPLE_ID

    void UseCapitalIdentifiers();
    // Capitalizes the first letter when writing identifier tokens.
    // ExaMple_Id -> Example_id  

    void PutWord( const std::string& word );
    // Simply puts the 'word' (that can contain any characters) to the stream.

    void PutLine( const std::string& line,
                  char end_of_line_char,
                  char string_separator_char );
    
    OutStream* GiveOutStream();

//############################################################################
//**########################################################################**
//****####################################################################****
 
    static void number2string( unsigned number, std::string& str );
    static void number2string( int number, std::string& str );

 private:
    
    OTokenStream( const OTokenStream& );
    // Preventing the use of copy constructor.
    OTokenStream& operator=( const OTokenStream& );  
    // Preventing the use of assignment operator.

    virtual std::string errorMsgPartA() const;
    // TokenStream needs this one.
    
    void PrepareForWrite( const std::string& str );
    // For checking if we must write str to a new line and taking care of
    //   indending or putting a preceding space.
    void Write( const std::string& );
    // For writing identifiers, punctuators or numbers in string format.
    void WriteSTRING( const std::string& );
    // For writing STRING-type tokens.
    void WriteString( const std::string& );
    // For help of WriteSTRING()-method.

    typedef unsigned cursor_pos_t;

    void put( int c );
    // putc( c, char_pipe_out ).

    void putNewline();
    // Writes a newline.

    void putCSpace();
    // Writes space if var. space == 1 (see below). 

    void putStringCutBegin();
    // Writes "\\^\n".

    void putString( const std::string& str );
    // For writing a string.

    void putNumber( lsts_index_t number );
    // For writing a number in number table.

    void putNumberWithZeros( lsts_index_t number );
    // For writing a number with possible preceding zero digits.

    OutStream* os;
    std::FILE* char_pipe_out; // The output character stream.

    cursor_pos_t chars_written_in_line;
    
    enum writing_mode { AS_IS = 0, LOWER_CASE, UPPER_CASE, CAPITAL };
    writing_mode identifiers_mode;
    // This decides if "as is" mode, upper case or lower case or capital mode
    //  is used to write identifier tokens. The default mode is "as is".

    cursor_pos_t space;
    // Value of this var. decides whether a preceding space is written
    //   or not.
};

typedef OTokenStream oTokenStream;

// ### The implementation of the inlines:


// With these two methods the user can tell the stream whether a
//   preceding space is written before a token or not:
inline void
OTokenStream::spaceOn() { space = 1; }
inline void
OTokenStream::spaceOff() { space = 0; }


#endif
