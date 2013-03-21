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

// FILE_DES: constants.hh: FileFormats/FileFormat/Lexical
// Timo Erkkilä

// $Id: constants.hh 1.13 Wed, 04 Jun 2003 23:03:58 +0300 timoe $
// 
// This module consists of the definitions of constants and
//   predeclarations needed by the FileFormats IO-library.
//

// $Log: constants.hh,v $
// Revision 1.1  1999/08/10 08:46:03  timoe
// Initial revision
//

#ifndef CONSTANTS_HH
#define CONSTANTS_HH

#include "config.hh"

// Some type definitions:
#define str_size_t  std::string::size_type
#define lsts_vec_size_t  std::vector<lsts_index_t>::size_type


// The available token classes and types.
// WHO NEEDS: Token, ITokenStream, OTokenStream, FileFormat, sections.
namespace TT 
{
    // Predeclarations of Token classes:
    class Token;

    class NatNumber;
    class Float;

    class StringDataToken;

    class ReservedWord;
    class CaseSensReservedWord;
    class Identifier;
    class CaseSensIdentifier;
    class String;
    class Punctuator;

    class EndOfFile;

    // Indexes of token types:
    enum TokenType 
    {
        NAT_NUMBER = 0,   // Number: <digit>+
        FLOAT,        // Float: <digit>*.<digit>+

        RESERVED_WORD,// Reserved word: <letter | digit | "_">+
	IDENTIFIER,   // Identifier: <letter | digit | "_">+
	STRING,       // String: "<any char>*"
	PUNCTUATOR,  // Punctuation: <any char>+
        END_OF_FILE,   // Informs about the end of the
                       //   stream (ITokenStream).

        // OBSOLETE:
        PUNCTUATION = PUNCTUATOR,
        NUMBER = NAT_NUMBER
    };

    // Names of token types.
    static const char* TokenTypeNames[] = 
    {
        "natural number",
        "float",

        "reserved word",
        "identifier",
        "string",
        "punctuator",

        "end of file",
    };
    static char warningRemover = TokenTypeNames[0][0];
    // The reason that above variable exists is that it prevents
    //   compiler of giving a meaningless warning ("defined but not used")
    //   about TokenTypeNames.

    // For backwards compatibility:
    typedef Punctuator Punctuation;
    typedef NatNumber Number;
}


// Predeclarations:
struct InStream;
struct OutStream;


// The namespace TSC (Token Stream Constants) that is used only by token
//   streams.
// WHO NEEDS: ITokenStream and OTokenStream.
namespace TSC
{
    // Constant characters indicating the end of line (two optional ways):
    const int LINE_FEED = '\n';  // Ascii code 10, UNIX uses this!
    const int CARRIAGE_RETURN = '\r'; //13; // Used in Windows & DOS.

    // COMMENT_LINE indicates the start of a comment line (comment
    //   line ends in end of line characters (see above)):
    const int COMMENT_LINE = '#';

    // Special characters appearing inside strings:
    const int STRING_ESCAPE = '\\';
    const int STRING_CUT = '^';
    const int STRING_CONTINUATION = '^';
    const int STRING_BACKSLASH = '!';
    const int STRING_QUOTE = '\"';
    const int STRING_NEWLINE = 'n';
    
  
    // The maximum length of a single line:  
    const unsigned int MAX_CHARS_IN_LINE = 72;
}


#endif
