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

// Implementations of abstract virtual base class Token and derived
//   token classes Number, Identifier, String, Punctuation and
//   EndOfFile.

#include "Token.hh"
#include "Exceptions.hh"
#include "charconv.hh"
#include "OTokenStream.hh"

#include <string>
#include <cstdio>

using std::string;
using namespace TT;


namespace
{
    string tmpString;

    bool isMatch( const string& options, const string& right )
    {
        int i = options.size();
        int length = 0;
        
        for ( --i; i >= 0; --i )
        {
            if ( options[ i ] == '#' && length )
            {
                if ( options.substr( i + 1, length ) == right )
                {
                    return true;
                }
                
                length = 0;
                continue;
            }
            
            ++length;
        }
        
        return options.substr( 0, length ) == right;
    }
    
}


// Token
// PUBLIC:
bool
TT::Token::isNatNumber() const { return false; }
bool
TT::Token::isNatNumber( lsts_index_t ) const { return false; }
bool
TT::Token::isNumber() const { return false; }
bool
TT::Token::isNumber( lsts_index_t ) const { return false; }

bool
TT::Token::isFloat() const { return false; }

unsigned
TT::Token::isReservedWord() const { return 0; }
unsigned
TT::Token::isReservedWord( const string& ) const { return 0; }

bool
TT::Token::isIdentifier() const { return false; }
bool
TT::Token::isIdentifier( const string& ) const { return false; }

bool
TT::Token::isString() const { return false; }
bool
TT::Token::isString( const string& ) const { return false; }

bool
TT::Token::isPunctuation() const { return false; }
bool
TT::Token::isPunctuation( const string& ) const { return false; }
bool
TT::Token::isPunctuator() const { return false; }
bool
TT::Token::isPunctuator( const string& ) const { return false; }

bool
TT::Token::isEndOfSection() const { return false; }

bool
TT::Token::isEndOfFile() const { return false; }

bool
TT::Token::isSame( TokenType, const string& ) const { return false; }
    
string&
TT::Token::stringData()
{
    static string zero;
    Exs::Throw( "Token::stringData(): wrong type of token" );
    return zero;
}

const string&
TT::Token::stringData() const
{
    static string zero;
    Exs::Throw( "Token::stringData(): wrong type of token" );
    return zero;
}

lsts_index_t
TT::Token::numberData() const
{
    Exs::Throw( "Token::numberData(): wrong type of token" );
    return 0;
}

lsts_float_t
TT::Token::floatData() const
{
    Exs::Throw( "Token::floatData(): wrong type of token" );
    return 0;
}

void
TT::Token::setStringData( const string& )
{ Exs::Throw( "Token::setStringData(): wrong type of token" ); }

void
TT::Token::setNumberData( lsts_index_t )
{ Exs::Throw( "Token::setNumberData(): wrong type of token" ); }

void
TT::Token::setFloatData( lsts_float_t )
{ Exs::Throw( "Token::setFloatData(): wrong type of token" ); }

// Token
// PROTECTED:
    
TT::Token::Token() {}//: idx( 0 ) { }
TT::Token::~Token() { }

// NatNumber
// PUBLIC:
TT::NatNumber::NatNumber( lsts_index_t uns ) : Token(), data( uns ) { }
TT::TokenType TT::NatNumber::type() const { return NAT_NUMBER; }
bool TT::NatNumber::isNatNumber() const { return true; }
bool TT::NatNumber::isNatNumber( lsts_index_t num ) const
{ return num == data; }
bool TT::NatNumber::isNumber() const { return true; }
bool TT::NatNumber::isNumber( lsts_index_t num ) const { return num == data; }
lsts_index_t TT::NatNumber::numberData() const { return data; }
std::string& TT::NatNumber::stringData() { return Token::stringData(); }

const string&
TT::Number::stringData() const
{
    OTokenStream::number2string( data, tmpString );
    return tmpString;
}

void
TT::Number::setNumberData( lsts_index_t new_data ) { data = new_data; }

// Float
// PUBLIC:
TT::Float::Float( lsts_float_t fl_number ) : Token(), data( fl_number ) { }

TT::TokenType TT::Float::type() const { return FLOAT; }
bool TT::Float::isFloat() const { return true; }
lsts_float_t
TT::Float::floatData() const { return data; }
void
TT::Float::setFloatData( lsts_float_t new_data ) { data = new_data; }
/*
string& 
TT::Number::stringData()
{
    static char tmp[ 50 ];
    std::sprintf( tmp, "%u", data );
    strData = tmp;
    return strData;
}*/

// StringDataToken
// PUBLIC:
TT::StringDataToken::StringDataToken( const string& str ) :
    TT::Token(), data( str ) { }
string& 
TT::StringDataToken::stringData() { return data; }
const string&
TT::StringDataToken::stringData() const { return data; }
void
TT::StringDataToken::setStringData( const string& new_data )
{ data = new_data; }

// ReservedWord
// PUBLIC:
TT::ReservedWord::ReservedWord( const std::string& str ) :
    TT::StringDataToken( str ), index( 0 ) { }
TT::TokenType
TT::ReservedWord::type() const { return RESERVED_WORD; }
unsigned
TT::ReservedWord::isReservedWord() const { return index; }
unsigned
TT::ReservedWord::isReservedWord( const string& canditate ) const
{
    if ( areStringsEqual( canditate, data ) ) { return index; }
    return 0;
}
bool
TT::ReservedWord::isSame( TokenType t, const string& canditate ) const
{ return t == RESERVED_WORD && areStringsEqual( canditate, data ); }

// CaseSensReservedWord
unsigned
TT::CaseSensReservedWord::isReservedWord() const { return index; }
unsigned
TT::CaseSensReservedWord::isReservedWord( const string& canditate ) const
{
    if ( canditate == data ) { return index; }
    return 0;
}
bool
TT::CaseSensReservedWord::isSame( TokenType t, const string& canditate ) const
{ return t == RESERVED_WORD && canditate == data; }

// Identifier
TT::TokenType TT::Identifier::type() const { return IDENTIFIER; }
bool TT::Identifier::isIdentifier() const { return true; }
bool
TT::Identifier::isIdentifier( const string& canditate ) const
{ return areStringsEqual( canditate, data ); }
bool
TT::Identifier::isSame( TokenType t, const string& canditate ) const
{ return t == IDENTIFIER && areStringsEqual( canditate, data ); }
bool
TT::Identifier::isEndOfSection() const
{ return areStringsEqual( "END", data ); }

// CaseSensIdentifier
bool
TT::CaseSensIdentifier::isIdentifier() const { return true; }
bool
TT::CaseSensIdentifier::isIdentifier( const string& canditate ) const
{ return canditate == data; }
bool
TT::CaseSensIdentifier::isSame( TokenType t, const string& canditate ) const
{ return t == IDENTIFIER && canditate == data; }

// String
TT::TokenType TT::String::type() const { return STRING; }
bool TT::String::isString() const { return true; }
bool TT::String::isString( const string& str ) const { return str == data; }
bool
TT::String::isSame( TokenType t, const string& canditate ) const
{ return t == STRING && canditate == data; }

// Punctuator
TT::TokenType TT::Punctuator::type() const { return PUNCTUATOR; }
bool TT::Punctuator::isPunctuator() const { return true; }
bool TT::Punctuator::isPunctuation() const { return true; }
bool
TT::Punctuator::isPunctuator( const string& str ) const
{ return str == data || isMatch( str, data ); }
bool
TT::Punctuator::isPunctuation( const string& str ) const
{ return str == data || isMatch( str, data ); }
bool
TT::Punctuator::isSame( TokenType t, const string& str ) const
{ return t == PUNCTUATOR && ( str == data || isMatch( str, data ) ) ; }

// EndOfFile
TT::TokenType TT::EndOfFile::type() const { return END_OF_FILE; }
bool TT::EndOfFile::isEndOfFile() const { return true; }
