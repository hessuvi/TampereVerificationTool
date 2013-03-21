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

// Character and string conversions

#ifndef CHARCONV_HH
#define CHARCONV_HH

#include <string>
#include <vector>
#include <cctype>

static inline unsigned ToUpperCase( unsigned c );
// Converts a lower case character to upper case if it's in range
//   'a'...'z' or ascii code 224...255 (E0...FF) (scandinavian
//   characters etc.).

static inline std::string& ToUpperCase( std::string& str );
// This version of ToUpperCase() converts all characters contained
//   in a string to upper case. Example: eX_String -> EX_STRING
// NOTE: the method changes the original string.

static inline unsigned ToLowerCase( unsigned c );
// Converts an upper case character to lower case if it's in
//   range 'A'...'Z' or ascii code 192...223 (C0...DF) (scandinavian
//   characters etc.).

static inline std::string& ToLowerCase( std::string& str );
// This version of ToLowerCase() converts all characters in a
//   string to lower case. Example: eX_String -> ex_string
// NOTE: the method changes the original string.

static inline std::string& ToCapital( std::string& str );
// Converts the first character of a string to upper case and the
//   rest of the string to lower case. Example: ex_String -> Ex_string
// NOTE: the method changes the original string.

static inline bool areStringsEqual( const std::string& strA,
                                    const std::string& strB );
// Case insensitive equality test of two strings.



//*************************************
// Character and string conversions:


// ToUpperCase() converts a lower case character to upper case IF it's in
//   range 'a'...'z' or ascii code 224...255 (scandinavian characters etc.).
static inline unsigned ToUpperCase( unsigned c )
{
#ifdef ISO_LATIN_1_CHAR_SET
    if ( islower(c) || ( (c & 0xE0) == 0xE0 ) )
    {    
        return (c &= 0xDF);
    }
    return c;
#else
    return toupper(c);
#endif
}

// This version of ToUpperCase() converts all characters contained
//  in a string to upper case.
static inline std::string& ToUpperCase( std::string& str )
{
    for ( int i = str.size() - 1; i >= 0; --i )
    {
        str[i] = ToUpperCase( str[i] );
    }
    return str;
}

// ToLowerCase() converts an upper case character to lower case IF it's in
//   range 'A'...'Z' or ascii code 192...223 (C0...DF) (scandinavian
//   characters etc.).
static inline unsigned ToLowerCase( unsigned c )
{
#ifdef ISO_LATIN_1_CHAR_SET
    if ( isupper( c ) || ( ( c & 0xE0 ) == 0xC0 ) )
    {    
        return ( c |= 0x20 );
    }
    return c;
#else
    return tolower(c);
#endif
}

// This version of ToLowerCase() converts all characters in a
// string to lower case.
static inline std::string& ToLowerCase( std::string& str )
{
    for ( int i = str.size() - 1; i >= 0; --i )
    {
        str[i] = ToLowerCase( str[i] );
    }
    return str;
}

// ToCapital converts the first character of a string to upper case and
//   the rest of the string to lower case.
static inline std::string& ToCapital( std::string& str )
{
    str[0] = ToUpperCase( str[0] );
    // And the rest of the string is converted to lowercase:
    for ( unsigned i = str.size() - 1; i; --i )
    {
        str[i] = ToLowerCase( str[i] );
    }
    return str;
}

// Checking whether the strings are case insensitively equal:
static inline bool areStringsEqual( const std::string& strA,
                                    const std::string& strB )
{
    if ( strA.size() != strB.size() ) { return false; }

    for ( int i = strA.size() - 1; i >= 0; --i )
    {
        if ( ToUpperCase( strA[i] ) != ToUpperCase( strB[i] ) )
        { return false; }
    }

    return true;
}

static inline bool
doesInclude( const std::vector<std::string>& sections,
             const std::string& section )
{
    for ( unsigned i = 0; i < sections.size(); ++i )
    {
        if ( areStringsEqual( section, sections[ i ] ) ) { return true; }
    }

    return false;
}


#endif
