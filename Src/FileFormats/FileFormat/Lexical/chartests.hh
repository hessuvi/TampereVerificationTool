/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2002 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen, Timo Erkkilä, Terhi Kilamo, Heikki Virtanen.
*/

// chartests.hh
//
// Timo Erkkilä
//
// Contains functions for testing lexical types of characters.


#include "constants.hh"


// Tests for characters:

static inline bool isWhiteSpaceChar( unsigned char c )
{ 
    // CR == 13 == '\r', FF == 12 == '\f', HT == 9 == '\t',
    // NL == 10 == '\n', VT == 11, space == 32 == ' '

    if ( c > 32 )
    {
        // This is an optimisation.
        return false;
    }
    
    return c == ' ' || c == '\n' || c >= 9 && c <= 13;
}

static inline bool isSignificantChar( unsigned char c )
{ return c > '(' || !isWhiteSpaceChar( c ) && c != '#' && c != '('; }

// Short "is-" tests for characters:

//*************************************

// isEndOfLineChar() tests if a character is the end of line character:
static inline bool
isEndOfLineChar( int c )
{
    // If the character is the line feed (ascii code 10), we'll return true:
    return c == TSC::LINE_FEED;
}

// End of line test that also considers EOF, the end of file character.
static inline bool
isEndOfLine( int c ) { return isEndOfLineChar( c ) || c == EOF; }

// Defines a rule for legal characters in numbers.
static inline bool
isNumberChar( int c ) { return c >= 48 && c <= 57; }

// Defines a rule for the first character of identifiers.
static inline bool
isIdentifierBeginChar( unsigned char c )
{ return isalpha( c ) || c >= 160 && c <= 254 || c == '_'; }

// Defines a rule for legal characters in identifiers.  
// BNF rule:
// <identifier char> ::= "A"|...|"Z"|"a"|...|"z"|"0"|...|"9"|160|...|254|"_"
// The character set used is "ISO latin 1".
static inline bool
isIdentifierChar( unsigned char c )
{ return isalnum( c ) || c >= 160 /*&& c <= 255*/ || c == '_'; }

static inline bool
isIdentifierChar( int c )
{ return isalnum( c ) || c >= 160 && c <= 255 || c == '_'; }

// Defines a rule for legal characters within a string and in comment lines
//   (all visible characters in the ascii chart).
static inline bool
isPrintableChar( unsigned char c )
{ return c >= 32 && c <= 126 || c >= 160 && c <= 254; }

// This one gives a rule for the beginning and the ending characters of
//   strings (quotation marks). Strings themselves may have any characters
//   that pass the test of isPrintableChar() (see above).
static inline bool
isStringQuoteChar( int c ) { return c == '\"'; }
