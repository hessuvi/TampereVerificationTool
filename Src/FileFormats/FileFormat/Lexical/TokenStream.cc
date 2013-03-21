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

// Implementation of the TokenStream class, which is inherited by
//   either ITokenStream or OTokenStream classes. It capsules Token class
//   and lexical rules of characters.

#include "TokenStream.hh"
#include "Exceptions.hh"

#include <cerrno>


TokenStream::~TokenStream() { }

// For neat error messages :) The format is:
// "<file_name>: `<error_token>', <message>"
std::string
TokenStream::errorMsg( const TT::Token& error_token, std::string message )
{
    std::string token;

    if ( error_token.isNumber() )
    { token = Exs::catString( "`", error_token.numberData(), "'" ); }
    else if ( error_token.isEndOfFile() ) { token = "`EOF'"; }
    else { token = "`" + error_token.stringData() + "'"; }

    return errorMsgPartA() + token + ", " + message;
}

// Returns an error message with format:
// "<partA> `<error_data>', <message>"
std::string 
TokenStream::errorMsg( std::string error_data, std::string message )
{ return errorMsgPartA() + "`" + error_data + "', " + message; }

// Returns an error message with format:
// "<partA> `<error_data>', <message>"
std::string 
TokenStream::errorMsg( lsts_index_t error_data, std::string message )
{ return errorMsgPartA() + Exs::catString( "`", error_data, "', " ) + message; }

// Returns an error message with format:
// "<partA> <message>"
std::string
TokenStream::errorMsg( std::string message )
{ return errorMsgPartA() + message; }

// Returns an error message with format:
// "<partA> <first> <value> <last>"
std::string
TokenStream::errorMsg( std::string first, lsts_index_t value, std::string last )
{ return errorMsgPartA() + Exs::catString( first, value, last ); }

// Returns an error message with format:
// "<partA> `<value>', <message> should be in range [<rangeFirst>,<rangeLast>]"
std::string
TokenStream::errorMsg( lsts_index_t value, std::string message,
                       lsts_index_t rangeFirst, lsts_index_t rangeLast )
{
    return Exs::catString( errorMsgPartA() + "`", value, "', " + message ) +
        Exs::catString( " should be in range [", rangeFirst, "," ) +
        Exs::catString( "", rangeLast, "]" );
}
