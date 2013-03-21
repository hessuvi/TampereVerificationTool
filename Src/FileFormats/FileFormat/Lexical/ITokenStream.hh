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

// This input stream reads lexical tokens from input character stream
//   and delivers them to caller.

#ifndef ITOKENSTREAM_HH
#define ITOKENSTREAM_HH

#include "TokenStream.hh"

#include <cstdio>

#ifndef TVT_NO_SCRIPTING_SUPPORT
#include <map>
#endif

// Predeclaration:
class StringMatcher;

class ITokenStream : public TokenStream
{
 public: 
    ITokenStream( bool case_sensitive_ids = false );
    // Builds a token stream that reads lexical tokens from a
    // character stream. Identifiers and reserved words are read case
    // insensitively by default.
    ITokenStream( InStream&, bool case_sensitive_ids = false );
    // Builds a token stream and opens it to read from InStream&.
    // Identifiers and reserved words are read case insensitively by
    // default.

    virtual ~ITokenStream();
    // The destructor frees memory allocated by punctuation.

    void Open( InStream& );
    // Opens the token stream for reading from InStream&.

    void SetPunctuation( std::string users_punctuation );
    // Clears and sets the legal punctuation. All punctuator marks can
    // be given in the same string separated by `#' character (which
    // is reserved for comment).

    void UseReservedWords( bool case_sensitive );
    unsigned AddReservedWord( const std::string& res_word );

    void SetReservedWords( std::string users_res_words, bool case_sensitive );
    // Clears and sets the reserved words. All words can be given in
    // the same string separated by `#' character. The second argument
    // defines whether reserved words are read case sensitively or
    // not.

    void SetIdCaseSens( bool case_sens ) { case_sensitive_ids = case_sens; }

    const TT::Token& Get();
    // Removes a lexical token from input stream and returns a reference
    //   to it.
 
    const TT::Token& Get( TT::TokenType wanted_type );
    // Get() with test against wanted token type.

    const TT::Token& Get( TT::TokenType wanted_type,
                          const std::string& wanted_data );
    // Get() with test against wanted token type and data.

    const TT::Token& Get( const std::string& error_message,
                          TT::TokenType wanted_type );
    // Get() with test against wanted token type. The first parameter
    //   also defines a special error message given if the test fails.

    const TT::Token& Get( const std::string& error_message,
                          TT::TokenType wanted_type,
                          const std::string& wanted_data );
    // Get() with test against wanted token type and data. The first
    //   parameter also defines a special error message given if tests
    //   fail.


    const TT::Token& Peek();
    // Returns a reference to next token in the token stream but does
    //   not remove it.

    bool skip( TT::TokenType wanted_type );
    bool skip( TT::TokenType wanted_type, const std::string& wanted_data );
    // Skip the next token if parameters match.

    bool GetNumber( lsts_index_t& number );
    // If the next token in the stream is number, this method reads
    //   and removes it from input stream, assigns the number to the
    //   reference parameter and returns true. If the next token in the
    //   stream is not a number it returns false.


    // #### "USE WITH YOUR OWN RISK"-METHODS == ####
    // #### "DON'T USE THESE IF ONLY YOU DON'T HAVE TO!"-METHODS ####

    // GetWord(), PeekChar(), GetChar() and GetLine() all provide a
    //   direct access to an underlying input stream and thus they
    //   _don't_work_with_method_Peek()_.

    // If you really have to use these make sure that Get() is called
    //   after Peek() before calling one of these methods!

    int PeekChar() const;
    // Peeks and returns the next character in the input stream. 

    int GetChar();
    // Removes and returns the next character in the input stream.

    const std::string& GetWord();
    // Reads, removes and returns a 'word' --- just any sequence of
    //   characters, where the first character is the next non white
    //   space character and the last character is the last non white
    //   space character.

    bool peekIdentifier( const std::string& id );

    const std::string& GetLine( char end_of_line_char,
                                char string_separator_char,
                                bool remove_white_space = false );
    // Removes and returns a line starting from the first non white space
    //   character and ending with end_of_line_char (which is however
    //   omitted from the return string). The usual quotes that mark the 
    //   beginning and the end of a lexical string token are replaced with
    //   string_separator_char.
    // Moreover, the last parameter decides if the white space characters
    //   outside strings are omitted from the return string.


    void SetPunctuationCheck( bool );
    // Decides whether punctuation tokens are compared to the list
    //   of legal punctuation given with SetPunctuation() or not.
    
    InStream* GiveInStream();
    // Returns a pointer to the InStream object. For a very very
    //   special use.

    unsigned GiveLineNumber() const { return line; }

    struct itsException {};

    ITokenStream& operator>>( int& i )
    { 
        i = 1;
        if ( skip( TT::PUNCTUATOR, "-" ) ) { i = -1; }
        i *= Get( TT::NAT_NUMBER ).numberData();
        return *this;
    }

    //#######################################################################
    //**#####################################################################
    //****###############################################################****

    bool isOk() const;
    bool isEndOfFile() const;
    bool isError() const;
    void Refresh();

 private:
    ITokenStream( const ITokenStream& );
    // Preventing the use of copy constructor.
    ITokenStream& operator=( const ITokenStream& );  
    // Preventing the use of assignment operator.

    virtual std::string errorMsgPartA() const;
    // Gives a string containing filename, line and column number (if
    //   possible) of the last read character from the character stream.
    // Format of the string:
    //   "<filename>``: line ''<line number>[``, col. ''<col. number>]``: ''"
    // TokenStream needs it.

    InStream* is;
    std::FILE* char_pipe_in;
    int peek_char;
    unsigned int line;
    unsigned int chars;
    std::vector<struct FileState*> file_states;

    void p_OpenFile( InStream& iSt, int pc = 0, unsigned int l = 1,
                     unsigned int cc = 0 );

    inline int get();
    inline int peek() const;
    void unget( int c );

    void SkipStringCut();
    int SkipStringCutAndGet();

    TT::Token* readNumber();
    TT::Token* readFloat();
    TT::Token* readDecimalPart( lsts_index_t integer_part = 0 );
    TT::Token* readIdentifier();

    TT::Token* readString();
    TT::Token* readPunctuation();

    TT::Token* readEOF();
    TT::Token* readUnidentified();

    void readToken();
    TT::Token* read_token;
    bool is_Peek;

    TT::Number number_token;
    TT::Float float_token;

    TT::ReservedWord* r_word_token;

    TT::Identifier* identifier_token;
    std::string& identifier_string;

    TT::String string_token;
    std::string& string_string;

    TT::Punctuation punctuation_token;
    std::string& punctuation_string;

    TT::EndOfFile eof_token;

    StringMatcher* punctuation; // A punctuation string match automaton.
    bool check_punctuation; // Are punctuation checked?

    std::string word;

    int skipComments();
    inline unsigned char getSignificantChar();
    inline unsigned char peekSignificantChar();

    bool case_sensitive_ids;
    bool case_sensitive_r_words;
    StringMatcher* reserved_words;

//----------------------------------------------------------------------
// Add "invisible" scripting support if TVT_NO_SCRIPTING_SUPPORT is not
// defined.
//----------------------------------------------------------------------
#ifndef TVT_NO_SCRIPTING_SUPPORT
#include "ITS_scripting.hh"
#endif
//----------------------------------------------------------------------

};

typedef ITokenStream iTokenStream;


#endif
