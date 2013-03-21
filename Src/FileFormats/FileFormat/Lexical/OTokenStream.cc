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

#include "OTokenStream.hh"
#include "Exceptions.hh"

#include "chartests.hh"
#include "charconv.hh"
#include "number2string.hh"

// C++ libraries:
#include <string>
using std::string;
// C libraries:
#include <cctype>
#include <cstdio>


// Preventing cutting of a string near the rightmost edge of the line or
//   cutting only a few last characters of a string:
namespace
{
    const unsigned noCut = 5;
    std::string tmp_str;
}


// General functions (inlines):

// Macro for making code better looking:
inline void
OTokenStream::put( int c )
{
    putc( c, char_pipe_out );
}

// Macro for writing a new line:
inline void
OTokenStream::putNewline()
{
    // put( TSC::LINE_FEED );
    // Portable newline:
    fputs( "\n", char_pipe_out );
}

// For writing a string:
inline void
OTokenStream::putString( const string& str )
{
    for( unsigned f = 0; f < str.size(); ++f )
    {
        put( str[f] );
    }
}

//For writing a number in number table:
inline void
OTokenStream::putNumber( lsts_index_t number )
{
    for ( const char* f = &numberTable[ number ] [ 1 ];
          *f;
          ++f )
    {
        put( *f );
    }
}

// For writing a number with possible preceding zero digits:
inline void
OTokenStream::putNumberWithZeros( lsts_index_t number )
{
    if ( number <= 99 )
    {
        put('0');
        if ( number <= 9 )
        {
            put('0');
        }
    }
    
    putNumber( number );
}


//###################################################
// ### Methods of the OTokenStream class ###
//###################################################
// You can find the list of definitions of the methods from
//   OTokenStream.hh file.


#define CONS_INIT    TokenStream(), \
                     os( 0 ), \
                     identifiers_mode( CAPITAL ), \
                     space( 1 )


// Constructors.
//===========================================================================
//
OTokenStream::OTokenStream() : CONS_INIT { }
    
OTokenStream::OTokenStream( OutStream& to ) : CONS_INIT
{
    Open( to );
}

// Destructor:
OTokenStream::~OTokenStream() { }

// Opening the file:
void
OTokenStream::Open( OutStream& to )
{
    os = &to;
    char_pipe_out = to.GiveOutStream();
    SetFilename( to.GiveFilename() );
    chars_written_in_line = 0;
}


// WriteCSpace() writes space if space != 0.
//===========================================================================
// private:
inline void
OTokenStream::putCSpace()
{
    if ( space ) 
    { 
        put(' ');
    }
}

inline void
OTokenStream::putStringCutBegin()
{
    put( TSC::STRING_ESCAPE );
    put( TSC::STRING_CUT );
    putNewline();
}

// Ends the current line.
//===========================================================================
void
OTokenStream::Endl()
{
    putNewline();
    chars_written_in_line = 0;
}

//  For checking if we must write str to a new line and taking care of
//    indending or putting a preceding space.
//===========================================================================
// private:
inline void
OTokenStream::PrepareForWrite( const string& str )
{
    chars_written_in_line += str.size();
    chars_written_in_line += space;
    
    // Is the string longer than would fit to current line?
    if ( chars_written_in_line > TSC::MAX_CHARS_IN_LINE )
    {
        putNewline();
        chars_written_in_line = str.size();
        chars_written_in_line += 1;
        put(' ');
    }
    else
    {
        putCSpace();
    }
}

//  Write() writes a string in format:
//    "[ ]<string>"
//===========================================================================
// private:
inline void
OTokenStream::Write( const string& str )
{
    PrepareForWrite( str );
    putString( str );
}

// WriteString() does exactly what it promises to do :)
//===========================================================================
//
inline void
OTokenStream::WriteString( const string& str )
{
    putCSpace();
    put( TSC::STRING_QUOTE );
    putString( str );
    put( TSC::STRING_QUOTE );
}

// WriteSTRING() is a special write operation for STRING token type.
//===========================================================================
//
inline void
OTokenStream::WriteSTRING( const string& str )
{
    cursor_pos_t token_length = str.size();
    token_length += 2;  // +2 comes from quotes.
    token_length += space;

    // Preventing the cutting of a string near the rightmost edge of the line:
    if ( chars_written_in_line + noCut > TSC::MAX_CHARS_IN_LINE ) {
	if ( token_length >= noCut ) {
	    Endl();
	}
	else // token_length < noCut
	{
	    // This is the case where short string fits nicely the small
	    //   space.
	    WriteString( str );
	    Endl();
	    return;
	}
    }
    
    chars_written_in_line += token_length;
    
    // This is propably the most frequent case:
    if ( chars_written_in_line <= TSC::MAX_CHARS_IN_LINE )
    {
	WriteString( str );
	return;
    }
    
    // Preventing cutting only a few last characters of a string:
    if ( (chars_written_in_line - noCut) < TSC::MAX_CHARS_IN_LINE )
    {
	if ( token_length <= TSC::MAX_CHARS_IN_LINE )
	{
	    Endl();
	    WriteString( str );
	    chars_written_in_line += token_length;
	    return;
	}
    }
    
    // Cutting the long string:
    
    // Writing the first line:
    
    unsigned int fitting_chars = TSC::MAX_CHARS_IN_LINE;
    fitting_chars -= chars_written_in_line;
    fitting_chars += token_length;
    fitting_chars -= space;
    --fitting_chars;
    fitting_chars -=  2;
    // -1 comes from quote, -2 from \^
    
    putCSpace();
    put( TSC::STRING_QUOTE );
    putString( str.substr( 0, fitting_chars ) );
    putStringCutBegin();
        
    // Writing the following lines:
    
    unsigned int next_char = fitting_chars;
    unsigned int last_char = str.size();
    --last_char;
    
    
    while ( last_char - next_char + space +1+1+1 > TSC::MAX_CHARS_IN_LINE )
    {
	// Above 1. +1 is there because next_char is counted also,
	//   2. +1 is for ^ and 3. +1 is for quote.
	putCSpace();
	put( TSC::STRING_CONTINUATION ); 
        putString( str.substr( next_char,
                               TSC::MAX_CHARS_IN_LINE - space - 1 - 2 ) );
        putStringCutBegin();

	next_char += ( TSC::MAX_CHARS_IN_LINE - space - 1 - 2 );
	// -1 is for quote, -2 is for \^
    }
    
    // Writing the last line.
    putCSpace();
    put( TSC::STRING_CONTINUATION );
    putString( str.substr( next_char ) );
    put( TSC::STRING_QUOTE );
    
    chars_written_in_line = last_char - next_char + space + 1 + 1 + 1;
    // Above 1. +1 is there because next_char is counted also,
    //   2. +1 is for ^ and 3. +1 is for quote.
    
    // End of method.
}


// Put() writes a space and a token to the output stream.
//===========================================================================
//
void
OTokenStream::Put( const TT::Token& token ) 
{
    // *** NUMBER ***
    if ( token.isNumber() )
    {
        PutNumber( token.numberData() );
        return;
    }

    const string& data = token.stringData();

    // *** IDENTIFIER ***
    if ( token.isIdentifier() )
    {
        PutIdentifier( data );
        return;
    }
  
    // *** PUNCTUATOR ***
    if ( token.isPunctuator() )
    {
        PutPunctuator( data );
        return;
    }

    // *** STRING ***
    check_claim( token.isString(),
                 errorMsg( token, "Put(): bad type of token" ) );
    PutString( data );
    return;
}

//  WriteNumber() writes a number in format:
//   "[ ]<number>"
//===========================================================================
//
void
OTokenStream::PutNumber( lsts_index_t number ) 
{
    int i = -1;
    cursor_pos_t token_length = space;
    
    while ( number > 1000 )
    {
        ++i;
        numbers[i] = number;
        numbers[i] %= 1000;
        number /= 1000;
        
        token_length += 3;
    }
    
    token_length += sizeOfNumber( number );
    
    chars_written_in_line += token_length;
    
    // Is the number longer than would fit to current line?
    if ( chars_written_in_line > TSC::MAX_CHARS_IN_LINE )
    {
	putNewline();
	chars_written_in_line = token_length;
        put(' ');
        ++chars_written_in_line;
    }
    else
    {
        putCSpace();
    }

    for ( putNumber( number ); i >= 0; --i )
    {
        putNumberWithZeros( numbers[i] );
    }
}

void
OTokenStream::PutFloat( lsts_float_t fl_number ) 
{
    chars_written_in_line +=
        fprintf( char_pipe_out, " %.8f", fl_number );

    /*
    cout.precision( 50 );
    cout << "<<" << fl_number << endl;

    // First we write the possible integer part of the number:
    if ( fl_number >= 1.0 )
    {
        PutNumber( fl_number );
        fl_number -= static_cast<unsigned>( fl_number );
    }
    else
    {
        putCSpace();
        chars_written_in_line += space;
    }

    put('.');
    ++chars_written_in_line;

    unsigned long multiplier = 1000;
    unsigned digits_written = 0;

    unsigned long helper = multiplier;
    helper *= fl_number;

    unsigned long three_digits = helper;

    while ( three_digits && digits_written < 15 )
    {
        putNumberWithZeros( three_digits );
        digits_written += 3;
        multiplier *= 1000;
        unsigned long helper = multiplier;
        helper *= fl_number;
        helper %= 1000;
        three_digits = helper;
    }

    chars_written_in_line += digits_written;
    */
}

// Writes a string:
void
OTokenStream::PutString( const string& str )
{
    cursor_pos_t token_length = str.size();
    
    // Checking the validity of characters:
    for ( unsigned i = 0; i < token_length; i++ )
    {
        if ( !isPrintableChar( str[i] ) ||
             str[i] == TSC::STRING_ESCAPE ||
             str[i] == TSC::STRING_QUOTE )
        {
            string new_str = str.substr( 0, i );
            for ( ; i < token_length; ++i )
            {
                // Do we have '/n', '//' or '/"' in the string?
                switch ( str[i] )
                {
                    // Newline?
                  case TSC::LINE_FEED:
                      new_str += TSC::STRING_ESCAPE;
                      new_str += TSC::STRING_NEWLINE;
                      break;
                      
                      // Backslash?
                  case TSC::STRING_ESCAPE:
                      new_str += TSC::STRING_ESCAPE;
                      new_str += TSC::STRING_BACKSLASH;
                      break;
                      
                      // Quote?
                  case TSC::STRING_QUOTE:
                      new_str += TSC::STRING_ESCAPE;
                      new_str += TSC::STRING_QUOTE;
                      break;
                      
                  default:
                      check_claim( isPrintableChar( str[i] ),
                                   errorMsg( str,
                                     string("has invalid string character `") +
                                                    str.at(i) + "'") );
                      
                      new_str += str[i];
                      
                } // End of switch.
                
            } // End of for.
            
            WriteSTRING( new_str );
            return;

        } // End of if.
        
    } // End of for.
    
    WriteSTRING( str );
}

// Writes an identifier:
void
OTokenStream::PutIdentifier( const string& id )
{
    int i = id.size();

    // Checking the validity of characters:
    for ( --i; i >= 0; --i )
    {
        check_claim( isIdentifierChar( id[i] ),
                     errorMsg( id,
                           string("has invalid identifier character `") +
                           id.at(i) + "'") );
    }


    switch( identifiers_mode )
    {
      case CAPITAL:
          PrepareForWrite( id );

          put( ToUpperCase( id[0] ) );
          for( unsigned f = 1; f < id.size(); ++f )
          {
              put( ToLowerCase( id[f] ) );
          }
          return;
      
      case AS_IS:
          Write( id );
          return;
          
      case LOWER_CASE:
          PrepareForWrite( id );

          for( unsigned f = 0; f < id.size(); ++f )
          {
              put( ToLowerCase( id[f] ) );
          }
          return;
          
      default: // UPPER_CASE:
          PrepareForWrite( id );

          for( unsigned f = 0; f < id.size(); ++f )
          {
              put( ToUpperCase( id[f] ) );
          }
    }
}
    
// Writes a punctuator:
void
OTokenStream::PutPunctuator( const string& punc )
{
    int i = punc.size();

    // Checking the validity of characters:
    for ( --i; i >= 0; --i )
    {
        check_claim( isPrintableChar( punc[i] ),
                     errorMsg( punc,
                       string("has invalid punctuator character `") +
                                       punc.at(i) + "'") );
    }
    
    Write( punc );
}

// Writes a file inclusion command:
void
OTokenStream::PutFileInclusion( const std::string& file_path )
{
    const cursor_pos_t sp = space;
    spaceOff();
    PutPunctuator( "include_file" );
    spaceOn();
    PutString( file_path );
    space = sp;
}

// Uses "as is" style when writing identifier tokens.  
//===========================================================================
void
OTokenStream::UseAsIsIdentifiers()
{
    identifiers_mode = AS_IS;
}


// Uses lower case when writing identifier tokens.  
//===========================================================================
void
OTokenStream::UseLowerCaseIdentifiers()
{
    identifiers_mode = LOWER_CASE;
}


// Uses upper case when writing identifier tokens.
//===========================================================================
void
OTokenStream::UseUpperCaseIdentifiers()
{
    identifiers_mode = UPPER_CASE;
}


// Capitalizes the first letter when writing identifier tokens.
//===========================================================================
void
OTokenStream::UseCapitalIdentifiers()
{
    identifiers_mode = CAPITAL;
}


void
OTokenStream::PutWord( const string& word )
{
    Write( word );
}


OutStream*
OTokenStream::GiveOutStream() { return os; }


// PRIVATE:


string
OTokenStream::errorMsgPartA() const
{
    string section_name = "";

    if ( !GiveSectionName().empty() )
    {
        section_name = ", " + GiveSectionName(); 
    }

    return string( "writing " ) + GiveFilename() + section_name + ": ";
}


// ### End of methods of OTokenStream class ###

//############################################################################
//**########################################################################**
//****####################################################################****


void
OTokenStream::PutLine( const string& line,
                       char end_of_line_char,
                       char string_separator_char )
{
    cursor_pos_t sp = space;
    string::size_type i = 0;
    string::size_type f = 0;

    while ( f < line.size() )
    {
        if ( line[ f ] == string_separator_char ||
             line[ f ] == '\n' )
        {
            Write( line.substr( i, f - i ) );
            space = 0;

            ++f;
            i = f;

            if ( line[ f - 1 ] == '\n' )
            {
                continue;
            }

            while ( f < line.size() && line[ f ] != string_separator_char )
            {
                ++f;
            }
            
            PutString( line.substr( i, f - i ) );

            ++f;
            i = f;

            continue;
        }
        
        ++f;
    }
    
    Write( line.substr( i, f - i ) + end_of_line_char );
    
    space = sp;
}


// PUBLIC STATIC:
void
OTokenStream::number2string( unsigned number, std::string& str )
{ NumberToString( number, str ); }

void
OTokenStream::number2string( int number, std::string& str )
{
    if ( number >= 0 )
    {
        NumberToString( number, str );
        return;
    }
    
    number = -number;
    NumberToString( number, tmp_str );
    str = "-" + tmp_str;
}
