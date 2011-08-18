/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// FILE_DES: ITokenStream.cc: Src/FileFormats/FileFormat/Lexical
// Timo Erkkilä

static const char * const ModuleVersion=
"Module version: $Id: ITokenStream.cc 1.30.2.22 Mon, 25 Oct 2004 13:16:18 +0300 timoe $";
// 
// The implementation of ITokenStream class.
//

// $Log: ITokenStream.cc,v $
// Revision 1.1  1999/08/09 10:33:51  timoe
// Initial revision
//


// C libraries:
#include <cstring>  // strerror()
#include <cerrno>   // errno
#include <climits> // UCHAR_MAX
#include <cstdio>
#include <cstdlib> // strtol

#include "ITokenStream.hh"

#include "StringMatcher.hh"
#include "chartests.hh"
#include "charconv.hh"
#include "Exceptions.hh"

#include <iostream>

using std::string;


//----------------------------------------------------------------------
// Add "invisible" scripting support if TVT_NO_SCRIPTING_SUPPORT is not
// defined. If it is, just read raw input.
//----------------------------------------------------------------------
#ifndef TVT_NO_SCRIPTING_SUPPORT
#include "ITS_scripting.cc"
#else
#define tvt_getc() getc(char_pipe_in)
#define tvt_ungetc( X )  ungetc( ( X ) , char_pipe_in )
#endif
//----------------------------------------------------------------------


static char commStartMark = '(';
static char commMiddleMark = '*';
static char commEndMark = ')';
#define CommStart  "(*"
#define CommEnd  "*)"


// check_claim with formatted error message :)
#define check_claim_wfm( C, M ) check_claim( C, \
                                 errorMsgPartA() + M );

#define CONS_INIT  TokenStream(), \
\
                   is( 0 ), \
                   number_token( 0 ), \
                   float_token( 0.0 ), \
                   r_word_token( 0 ), \
                   identifier_token( cs ? \
                         (new TT::CaseSensIdentifier("")) : \
                         (new TT::Identifier("")) ), \
                   identifier_string( identifier_token->stringData() ), \
                   string_token( "" ), \
                   string_string( string_token.stringData() ), \
                   punctuation_token( "" ), \
                   punctuation_string( punctuation_token.stringData() ), \
\
                   punctuation( 0 ), \
                   case_sensitive_ids( cs ), \
                   case_sensitive_r_words( false ), \
                   reserved_words( 0 )


typedef TT::Token* ( ITokenStream::*ReadMethodPtr )();
static ReadMethodPtr choose_token[ UCHAR_MAX + 1 ];

enum TOK { not_wsTok = 0, eolTok, wsTok, comm_lineTok, comm_sectionTok,
              scriptTok };
static TOK tok[ UCHAR_MAX + 1 ];

static bool areArraysInitialized = false;


struct FileState
{
    FileState( InStream* ist, int pc, unsigned int l, unsigned int cc,
               InStream* incIs = 0 )
        : is( ist ), peek_char( pc ), line( l ), chars( cc ),
          inc_is( incIs ) { }
    ~FileState() { delete inc_is; }

    bool isSameFilename( const string& filename ) const
    { return filename == is->GiveFilename(); }

    InStream* is;
    int peek_char;
    unsigned int line;
    unsigned int chars;

    InStream* inc_is;
};


//###################################################
// ### Methods of the ITokenStream class ###
//###################################################
// You can find the list of definitions of the methods from
//   ITokenStream.hh file.

// Constructors.
ITokenStream::ITokenStream( bool cs ) : CONS_INIT
#ifndef TVT_NO_SCRIPTING_SUPPORT
, evaluatedValueIndex(~0U)
#endif
{ }
ITokenStream::ITokenStream( InStream& from, bool cs ) : CONS_INIT
#ifndef TVT_NO_SCRIPTING_SUPPORT
, evaluatedValueIndex(~0U)
#endif
{ Open( from ); }

// The destructor.
ITokenStream::~ITokenStream()
{
    if ( punctuation ) { delete punctuation; }
    if ( reserved_words ) { delete reserved_words; }
    if ( r_word_token ) { delete r_word_token; }

    delete identifier_token;
}

// SetPunctuation() sets the legal punctuation vector.
//===========================================================================
//
void
ITokenStream::SetPunctuation( string punct )
{
    if ( !punctuation ) { punctuation = new StringMatcher; }
    else { punctuation->ClearPatterns(); }

    int i = punct.size();
    int length = 0;

    for ( --i; i >= 0; --i )
    {
        if ( punct[ i ] == '#' && length )
        {
            punctuation->AddPattern( punct.substr( i + 1, length ) );
            choose_token[ static_cast<unsigned char>( punct[ i + 1 ] ) ] =
                &ITokenStream::readPunctuation;
            length = 0;
        }
        else { ++length; }
    }

    if ( length )
    {
        punctuation->AddPattern( punct.substr( 0, length ) );
        choose_token[ static_cast<unsigned char>( punct[ 0 ] ) ] =
            &ITokenStream::readPunctuation;
    }
}

void
ITokenStream::UseReservedWords( bool cs )
{
    if ( r_word_token ) { delete r_word_token; }

    if ( !reserved_words ) { reserved_words = new StringMatcher; }
    else { reserved_words->ClearPatterns(); }

    reserved_words->SetCaseSensitive( cs );
    case_sensitive_r_words = cs;

    if ( cs ) { r_word_token = new TT::CaseSensReservedWord( "" ); }
    else { r_word_token = new TT::ReservedWord( "" ); }
}

unsigned
ITokenStream::AddReservedWord( const string& r_word )
{
    check_claim( r_word.size(), "ITokenStream::AddReservedWord(): "
                 "the parameter string is empty" );
    
    return reserved_words->AddPattern( r_word );
}

void
ITokenStream::SetReservedWords( string r_words, bool cs )
{
    check_claim( r_words.size(), "ITokenStream::SetReservedWords(): "
                 "the argument string is empty" );

    UseReservedWords( cs );

    int i = r_words.size();
    int length = 0;

    for ( --i; i >= 0; --i )
    {
        if ( r_words[ i ] == '#' && length )
        {
            reserved_words->AddPattern( r_words.substr( i + 1, length ) );
            length = 0;
        }
        else { ++length; }
    }

    if ( length )
    { reserved_words->AddPattern( r_words.substr( 0, length ) ); }
}

void
ITokenStream::p_OpenFile( InStream& iSt, int pc, unsigned int l,
                          unsigned int cc )
{
    is = &iSt;
    
    char_pipe_in = iSt.GiveInStream();
    SetFilename( iSt.GiveFilename() );
     
    if ( pc ) { peek_char = pc; }
    else { get(); }
     
    line = l;
    chars = cc;
}
    
// Opening the file:
void
ITokenStream::Open( InStream& iSt )
{
    p_OpenFile( iSt );

    // EOF?
    check_claim( !isEndOfFile(), string("reading ") + GiveFilename() +
                 ": the file is empty" );
    
    read_token = 0;
    is_Peek = false;

    check_punctuation = true;

    if ( areArraysInitialized ) { return; }


    // Initializing the choose_token vector:

    for ( int c = 0; c < UCHAR_MAX; ++c )
    {
        tok[ c ] = not_wsTok;
        
        if ( isNumberChar( c ) )
        {
            choose_token[ c ] = &ITokenStream::readNumber;
            continue;
        }

        if ( c == '.' )
        {
            choose_token[ c ] = &ITokenStream::readFloat;
            continue;
        }

        if ( isIdentifierBeginChar( c ) )
        {
            choose_token[ c ] = &ITokenStream::readIdentifier;
            continue;
        }

        if ( isStringQuoteChar( c ) )
        {
            choose_token[ c ] = &ITokenStream::readString;
            continue;
        }

        if ( isEndOfLineChar( c ) )
        {
            tok[ c ] = eolTok;
            continue;
        }

        if ( isspace( c ) )
        {
            tok[ c ] = wsTok;
            continue;
        }
 
        if ( c == TSC::COMMENT_LINE )
        {
            tok[ c ] = comm_lineTok;
            continue;
        }

        if ( c == commStartMark )
        {
            tok[ c ] = comm_sectionTok;
            choose_token[ c ] = &ITokenStream::readPunctuation;
            continue;
        }

        if ( isPrintableChar( c ) )
        {
            choose_token[ c ] = &ITokenStream::readPunctuation;
            continue;
        }

        choose_token[ c ] = &ITokenStream::readUnidentified;

    } // End of for.

    choose_token[ UCHAR_MAX ] = &ITokenStream::readEOF;
    tok[ UCHAR_MAX ] = not_wsTok;
}

void
ITokenStream::Refresh() { choose_token[ static_cast<unsigned>( '.' ) ] =
                              &ITokenStream::readFloat; }

void
ITokenStream::SetPunctuationCheck( bool check ) { check_punctuation = check; }

// Is the counter stream ready for read operation?
//===========================================================================
bool
ITokenStream::isOk() const
{
    return char_pipe_in && !ferror( char_pipe_in ) && !feof( char_pipe_in );
}

// Is the counter stream at EOF?
//===========================================================================
bool
ITokenStream::isEndOfFile() const { return feof( char_pipe_in ); }

// Is the counter stream in error state?
//===========================================================================
bool
ITokenStream::isError() const
{ return !char_pipe_in || ferror( char_pipe_in ); }

int
ITokenStream::get()
{
    int c = peek_char;
    peek_char = tvt_getc();
    ++chars;
    return c;
}

void
ITokenStream::unget( int c )
{
    tvt_ungetc( peek_char );
    peek_char = c;
    --chars;
}

#define SkipCommentLine  while ( !isEndOfLine( get() ) ); \
                         ++line; \
                         chars = 0
int
ITokenStream::skipComments()
{
    unsigned depth = 1;

    switch ( tok[ static_cast<unsigned char>( peek_char ) ] )
    {
      case comm_lineTok:
          get();
          SkipCommentLine;
          return 0;

      case comm_sectionTok:
          get();
          if ( peek() != commMiddleMark )
          {
              unget( commStartMark );
              return peek();
          }
          get();

          for ( int c = get(); depth; )
          {
              check_claim ( c != EOF, errorMsg( "endless comment section "
                                                "(closing `" CommEnd
                                                "' missing)" ) );
              if ( isEndOfLineChar( c ) )
              {
                  ++line;
                  chars = 0;
                  c = get();
                  continue;
              }
              
              if ( c == commStartMark )
              {
                  c = get();
                  if ( c == commMiddleMark ) { c = get(); ++depth; }
                  continue;
              }

              if ( c == commMiddleMark )
              {
                  c = get();
                  if ( c == commEndMark ) { --depth; }
                  continue;
              }
              
              if ( c == TSC::COMMENT_LINE ) { SkipCommentLine; }
              
              c = get();
          }
          
          return 0;
                    
      default:
          return peek();
    }

    return peek();
}    
#undef SkipCommentLine
    
inline unsigned char
ITokenStream::peekSignificantChar()
{
    while ( tok[ static_cast<unsigned char>( peek_char ) ] )
    {
        if ( tok[ peek_char ] == wsTok )
        {
            get();
            continue;
        } 
        
        if ( tok[ peek_char ] == eolTok )
        {
            get();
            ++line;
            chars = 0;
            continue;
        }
        
        if ( skipComments() ) { return peek_char; }
    }

    return peek_char;
}

inline unsigned char
ITokenStream::getSignificantChar()
{
    peekSignificantChar();
    return get();
}

// peek() returns next character in the input stream but does not remove it.
//===========================================================================
inline int
ITokenStream::peek() const { return peek_char; }

TT::Token*
ITokenStream::readNumber()
{
    int number = get();
    number ^= 48;

    while ( choose_token[ peek() ] == &ITokenStream::readNumber )
    {
        int c = get();
        c ^= 48;
        number *= 10;
        number += c;
    }

    if ( choose_token[ peek() ] == &ITokenStream::readFloat )
    { return readDecimalPart( number ); }

    number_token.setNumberData( number );
    return &number_token;
}

//     check_clam_wfm( GetNumber( decimal ), 
//                 "syntax error --- floating point number expected" );

inline TT::Token*
ITokenStream::readDecimalPart( lsts_index_t integer_part )
{
    get();
 
    /* check_claim_wfm( choose_token[ peek() ] == &ITokenStream::readNumber,  
       "syntax error --- decimal part of the floating point "
       "number missing" );
    */

    if ( choose_token[ peek() ] != &ITokenStream::readNumber )
    {
        unget( '.' );
        number_token.setNumberData( integer_part );
        return &number_token;
    }

    lsts_float_t decimal_number = integer_part;
    lsts_index_t decimal = get();
    decimal ^= 48;

    unsigned long div = 10;
    
    while ( choose_token[ peek() ] == &ITokenStream::readNumber )
    {
        int c = get();
        c ^= 48;
        decimal *= 10;
        decimal += c;
        div *= 10;
    }
    
    lsts_float_t fl_decimal = decimal;
    fl_decimal /= div;
    decimal_number += fl_decimal;

    float_token.setFloatData( decimal_number );
    return &float_token;
}

/*
    
    ostringstream oss;
    oss << integer_part << ".";

    lsts_index_t decimal_part = 0;
    check_claim_wfm( GetNumber( decimal_part ),
                     "syntax error --- corrupted decimal number" );
    oss << decimal_part << ends;

    istringstream iss( oss.str() );
    lsts_float_t decimal_number;
    iss >> decimal_number;

    float_token.setFloatData( decimal_number );
    return &float_token;
}
    
// cout.precision( 50 );
// cout << fl_number << endl;
*/    



TT::Token*
ITokenStream::readFloat() { return readDecimalPart( 0 ); } 

TT::Token*
ITokenStream::readIdentifier()
{
    for ( identifier_string = get();
          isIdentifierChar( peek() );
          identifier_string += get() );

    if ( reserved_words )
    {
        string& r_word_string = r_word_token->stringData();
        r_word_string = identifier_string;
        if ( !case_sensitive_r_words ) { ToUpperCase( r_word_string ); }

        unsigned i = 0;
        for ( reserved_words->Reset();
              i < r_word_string.size() &&
                  reserved_words->input( r_word_string[i] );
              ++i );
              
        if ( i == r_word_string.size() && reserved_words->isAccState() )
        {
            r_word_token->SetIndex( reserved_words->isAccState() );
            return r_word_token;
        }

    }

    if ( !case_sensitive_ids ) { ToUpperCase( identifier_string ); }

    if ( areStringsEqual( identifier_string, "include_file" ) )
    {
        const bool isPeek = is_Peek;
        if ( isPeek ) { Get(); }

        const string incFile( Get( TT::STRING ).stringData() );
        InStream* incIs = new InStream( incFile );

        check_claim_wfm( incFile != is->GiveFilename(),
                         "file inclusion loop" );
        for ( unsigned i = 0; i < file_states.size(); ++i )
        {
            check_claim_wfm( !file_states[i]->isSameFilename( incFile ),
                             "file inclusion loop" );
        }


        FileState* fs = new FileState( is, peek_char, line, chars, incIs );
        file_states.push_back( fs );

        p_OpenFile( *incIs );

        readToken();
        is_Peek = isPeek;
        return read_token;
    }

    return identifier_token;
}

// The method assumes that '^' is just been read and removes the rest
// of string cut.
inline void
ITokenStream::SkipStringCut()
{
    // If the character isn't the cap, here we have
    // another error.  This also handles the end of the
    // stream case (EOF != '^').
    check_claim_wfm( getSignificantChar() == TSC::STRING_CONTINUATION,
                     "line cut syntax error" );
}

inline int
ITokenStream::SkipStringCutAndGet()
{
    int c = get();
    
    if ( c == TSC::STRING_ESCAPE )
    {
        get();
        SkipStringCut();
        return get();
    }
    
    return c;
}   


// CheckIfString() stores a string into the result token in positive case
//   and returns true.
//===========================================================================
TT::Token*
ITokenStream::readString()
{
    get();
    if ( string_string.size() )
    {
        string_string = ""; //.erase();
    }

    // Here we read the string:
    for ( int c = get(); !isStringQuoteChar( c ); c = get() )
    {
        // Stream ended, string didn't?
        check_claim_wfm( peek() != EOF,
                         "endless string (closing quote missing)" );
        
        // All characters in a string should be visible:
        check_claim_wfm( isPrintableChar( c ),
                         "illegal character in the string" );

        // Do we have special characters in the string?
        //   If not, we just add the character into string:
        if ( c != TSC::STRING_ESCAPE )
        {
            string_string += c;
        }
        else
        {
            // Checking next character:
            
            c = SkipStringCutAndGet();
            
            switch ( c )
            {
                
                // Line cut?
              case TSC::STRING_CUT:
                  SkipStringCut();	  
                  break;

                  // Newline?
              case TSC::STRING_NEWLINE:
                  string_string += TSC::LINE_FEED;
                  break;

                  // Backslash?
              case TSC::STRING_BACKSLASH:
                  string_string += TSC::STRING_ESCAPE;
                  break;

                  // Quote?
              case TSC::STRING_QUOTE:
                  string_string += TSC::STRING_QUOTE;
                  break;

                  // The last special case: hex number pair?
              default:

                  // Is first hex number ok?
                  check_claim_wfm( isxdigit( c ),
                                   "wrong first hex code" );
                  
                  string hex_string;
                  hex_string = c;
                  // Notice the assignment operator '='.
                  
                  // This is a nasty location for a string cut, but it is
                  //   possible:
                  c = SkipStringCutAndGet();

                  // Then we'll check the second hex number:
                  check_claim_wfm( isxdigit( c ),
                                   "wrong second hex code" );	  
                 
                  hex_string += c;
                  
                  // strtol() converts the string into long int
                  // and static_cast<char> converts long int into
                  // char. Finally that char is the ascii code of
                  // our character here:
                  string_string +=
		    static_cast<char>(strtol(hex_string.c_str(), 0, 16));

              } // End of switch.
            
        } // End of inspecting special code.

    } // End of while.
  
    // At this point we have a complete string without the first 
    //   and the last quote character.

    return &string_token;
}

// CheckIfPunctuation() tests if the token is a punctuation mark and
// stores it into the result token in positive case and returns true.
//===================================================================

TT::Token*
ITokenStream::readPunctuation()
{
    // Check off?
    if ( !punctuation || !check_punctuation )
    {
        punctuation_string = get();
        
        // We'll gather every valid punctuation character:
        while ( isPrintableChar( peek() ) ) { punctuation_string += get(); }

        return &punctuation_token;  
    }

    punctuation->Reset();
    int c = get();
    if ( !punctuation->input( c ) )
    {
        check_claim( c != commMiddleMark || get() != commEndMark,
                     errorMsg( CommEnd,
                               "misplaced comment section end mark" ) );
        Exs::Throw( errorMsg( string( 1, static_cast<char>( c ) ), 
                                    "syntax error -- "
                                    "unidentified punctuator" ) );
    }
    
    while ( punctuation->input( peek() ) ) { get(); }

    punctuation_string = punctuation->longestMatch();
    const string& left_over = punctuation->leftOver();

    check_claim ( punctuation_string.size(),
                  errorMsg( left_over + static_cast<char>( get() ),
                            "syntax error -- unidentified punctuator" ) );
    for ( unsigned i = 0; i < left_over.size(); ++i )
    { unget( left_over[i] ); }

    return &punctuation_token;
}

TT::Token*
ITokenStream::readEOF()
{
    check_claim_wfm( !isError(),
                     string( "unexpected input error: " ) +
                     strerror( errno ) );
        
    if ( isEndOfFile() )
    {
        get();

        if ( file_states.size() )
        {
            FileState* fs = file_states.back(); file_states.pop_back();
            p_OpenFile( *fs->is, fs->peek_char, fs->line, fs->chars );
            delete fs;

            readToken();
            return read_token;
        }

        return &eof_token;
    }

    return readIdentifier();
}


TT::Token*
ITokenStream::readUnidentified()
{
    check_claim_wfm( false, "unidentified token" );
    return 0;
}


// readToken() reads a token from input character stream and assigns it
//   to read_token variable.
inline void
ITokenStream::readToken()
{ read_token = ( this->*choose_token[ peekSignificantChar() ] )(); }


const TT::Token&
ITokenStream::Get()
{
    if ( is_Peek )
    {
        is_Peek = false;
        return *read_token;
    }

    readToken();
    return *read_token;
}

const TT::Token&
ITokenStream::Peek()
{
    if ( is_Peek ) { return *read_token; }

    is_Peek = true;
    readToken();
    return *read_token;
}


bool
ITokenStream::GetNumber( lsts_index_t& number )
{
    if ( is_Peek )
    {
        if ( read_token->isNumber() )
        {
            is_Peek = false;
            number = read_token->numberData();
            return true;
        }
        
        return false;
    }

    int num = peekSignificantChar();
    
    if ( choose_token[ num ] != &ITokenStream::readNumber )
    {
        return false;
    }

    // We convert the first valid number character to number:
    num ^= 48;
    get();

    // Then we read and convert all the rest of number characters (or
    // until numbers characters end).
    while ( choose_token[ peek() ] == &ITokenStream::readNumber )
    {
        num *= 10;
        int c = get();
        c ^= 48;
        num += c;
    }

    if ( choose_token[ peek() ] == &ITokenStream::readFloat )
    {
        read_token = readDecimalPart( num );
        is_Peek = true;
        return false;
    }

    number = num;

    return true;
}

/*
bool
ITokenStream::GetNumberPair( lsts_index_t& number, lsts_index_t& second )
{
    if ( is_Peek )
    {
        if ( read_token->isNumber() )
        {
            is_Peek = false;
            number = read_token->numberData();
            return true;
        }
        
        return false;
    }

    int num = peekSignificantChar();
    
    if ( choose_token[ num ] != &ITokenStream::readNumber )
    {
        return false;
    }

    // We convert the first valid number character to number:
    num ^= 48;
    get();

    // Then we read and convert all the rest of number characters (or
    // until numbers characters end).
    while ( choose_token[ peek() ] == &ITokenStream::readNumber )
    {
        num *= 10;
        int c = get();
        c ^= 48;
        num += c;
    }

    if ( peek() == ',' )
    {
        read_token = readDecimalPart( num );
        is_Peek = true;
        return false;
    }



    number = num;

    return true;
} */

// Get() with test against wanted token type.
const TT::Token&
ITokenStream::Get( TT::TokenType wanted_type ) 
{
    const TT::Token& token = Get();

    if ( token.type() != wanted_type )
    {
        string error_reason = "syntax error: ";
        error_reason += TT::TokenTypeNames[ wanted_type ];
        error_reason += " expected";
        Exs::Throw( errorMsg( token, error_reason ) );
    }

    return token;
}

// Takes a parameter string "<punct>(#<punct>)*" and gives a return
//   string with format:
//   "['<punct>']" | "'<punct>'(, '<punct>')* or '<punct>'"
static string
options_str( const string& ops )
{
    int i = 0;
    int f = 0;
    int length = 0;
    int d_size = ops.size();
    string ops_str = "";

    for ( i = 0; i < d_size && ops[ i ] != '#'; ++i );

    ops_str = "`" + ops.substr( 0, i ) + "'";

    if ( i == d_size )
    {
        return ops_str;
    }

    for ( f = d_size - 1; f > i && ops[ f ] != '#'; --f );

    if ( i == f )
    {
        ops_str += " or `" + ops.substr( f + 1 ) + "'";
        return ops_str;
    }

    for ( ++i; i <= f; ++i )
    {
        if ( ops[ i ] == '#' && length )
        {
            ops_str += ", `" + ops.substr( i - length, length ) + "'";
            length = 0;
            continue;
        }
        
        ++length;
    }

    ops_str += " or `" + ops.substr( f + 1 ) + "'";
    return ops_str;
}

bool
ITokenStream::skip( TT::TokenType wanted_type ) 
{
    if ( Peek().type() == wanted_type ) { Get(); return true; }
    return false;
}

bool
ITokenStream::skip( TT::TokenType wanted_type,
                    const string& wanted_data ) 
{
    if ( Peek().isSame( wanted_type, wanted_data ) )
    { Get(); return true; }
    return false;
}

// Get() with test against wanted token type and data.
const TT::Token&
ITokenStream::Get( TT::TokenType wanted_type,
                   const string& wanted_data ) 
{
    const TT::Token& token = Get();

    if ( !token.isSame( wanted_type, wanted_data ) )
    {
        string error_reason = "syntax error: ";
        error_reason += TT::TokenTypeNames[ wanted_type ];
        error_reason += " ";

        if ( wanted_type == TT::PUNCTUATION && wanted_data != "#" )
        {
            error_reason += options_str( wanted_data );
        }
        else
        {
            error_reason += "`" + wanted_data + "'";
        }

        error_reason += " expected";
        Exs::Throw( errorMsg( token, error_reason ) );
    }

    return token;
}

// Get() with test against wanted token type. The first parameter
//   also defines a special error message given if tests fail.
const TT::Token&
ITokenStream::Get( const string& error_message, TT::TokenType wanted_type )
{
    const TT::Token& token = Get();

    check_claim( token.type() == wanted_type,
                 errorMsg( token, error_message ) );

    return token;
}

// Get() with test against wanted token type and data. The first parameter
//   also defines a special error message given if tests fail.
const TT::Token&
ITokenStream::Get( const string& error_message,
                   TT::TokenType wanted_type,
                   const string& wanted_data )
{
    const TT::Token& token = Get();

    check_claim( token.isSame( wanted_type, wanted_data ),
                 errorMsg( token, error_message ) );
    
    return token;
}


InStream*
ITokenStream::GiveInStream() { return is; }



// PRIVATE:

// Gives a ready made string which consists of filename, line number and
//   column number.
string
ITokenStream::errorMsgPartA() const
{
    string section_name = "";

    if ( !GiveSectionName().empty() )
    {
        section_name = ", " + GiveSectionName(); 
    }

    string beginning = "reading " + GiveFilename() + section_name + ": line ";
  
    return Exs::catString( beginning, line, ", ")
        + Exs::catString("col. ", chars, ": ");
}





// #### "USE WITH YOUR OWN RISK" METHODS ####


// GetWord(), PeekChar(), GetChar() and GetLine() all provide a direct
// access to an underlying input stream and thus they don't work with
// method Peek().

#define WAS_PEEK_LAST( M )  check_claim_wfm( !is_Peek, \
   "ITokenStream::" M "(): of methods Peek() and Get(), Get() must be the " \
   "last one to use before calling " M "().\n" \
   "The explanation for this is that " M "() goes around " \
   "the usual interface of the ITokenStream and thus it " \
   "doesn't work with Peek()." )


int
ITokenStream::PeekChar() const
{
    WAS_PEEK_LAST( "PeekChar" );

    return peek();
}

int
ITokenStream::GetChar()
{
    WAS_PEEK_LAST( "GetChar" );

    const int c = get();
    
    if ( isEndOfLineChar( c ) )
    {
        ++line;
        chars = 0;
        return c;
    }

    return c;
}



const string&
ITokenStream::GetWord()
{
    WAS_PEEK_LAST( "GetWord" );

    word.erase();
    word = getSignificantChar();
    
    for ( int c = peek(); c != EOF && !isspace( c ); c = peek() )
    {
        word += c;
        get();
    }
    
    return word;
}

// Hmm.. this is used only in StatePropRulesSection.cc. Should it be
// removed in the future?

bool
ITokenStream::peekIdentifier( const string& id )
{
    if ( id.empty() ) { return false; }
    
    if ( is_Peek ) { return read_token->isIdentifier( id ); }

    word.erase();
    peekSignificantChar();
       
    for ( unsigned int i = 0; i < id.size(); ++i )
    {
        if ( peek_char == EOF ) { break; }

        if ( !case_sensitive_ids &&
             ToUpperCase( peek_char ) != ToUpperCase( id[ i ] ) ||
             case_sensitive_ids &&
             peek_char != id[ i ] ) { break; }
        
        word += peek_char;
        peek_char = tvt_getc();
    }
    
    if ( word.empty() ) { return false; }
    
    tvt_ungetc( peek_char );
    
    for ( int i = word.size() - 1; i > 0; --i )
    { tvt_ungetc( word[ i ] ); }
    
    peek_char = word[ 0 ];

    return word.size() == id.size();
}    


const std::string&
ITokenStream::GetLine( char end_of_line_char,
                       char string_separator_char,
                       bool remove_white_space )
{
    WAS_PEEK_LAST( "GetLine" );
    
    word.erase();

    //    cout << end_of_line_char << endl << endl;
    for ( char c = peekSignificantChar();
          c != end_of_line_char;
          c = peek() )
    {
        check_claim_wfm( c != EOF, string( "endless line (`" ) +
                         end_of_line_char +
                         "' line end character missing)" );

        if ( c != TSC::STRING_QUOTE )
        {
            word += c;
            get();
        }
        else
        {
            word += string_separator_char;
            word += Get( TT::STRING ).stringData();
            word += string_separator_char;
        }

        if ( remove_white_space )
        {
            peekSignificantChar();
        }
        else
        {
            skipComments();
        }

    }

    get();

    return word;
}
