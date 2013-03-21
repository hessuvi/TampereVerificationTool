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

// FILE_DES: Token.hh: FileFormats/FileFormat/Lexical
// Timo Erkkilä

// $Id: Token.hh 1.8.2.10 Wed, 15 Sep 2004 21:48:51 +0300 timoe $
// 
// Defines the abstract base class Token and derived token classes
//   Number, Identifier, String, Punctuation and EndOfFile. 
//

#ifndef TOKEN_HH
#define TOKEN_HH

#include "constants.hh"
#include <string>


class TT::Token
{
 public:
    virtual ~Token();

    virtual TokenType type() const = 0;

    virtual bool isNatNumber() const;
    virtual bool isNatNumber( lsts_index_t ) const;

    virtual bool isFloat() const;

    virtual unsigned isReservedWord() const;
    virtual unsigned isReservedWord( const std::string& ) const;
    virtual bool isIdentifier() const;
    virtual bool isIdentifier( const std::string& ) const;
    virtual bool isString() const;
    virtual bool isString( const std::string& ) const;
    virtual bool isPunctuation() const;
    virtual bool isPunctuation( const std::string& ) const;
    virtual bool isPunctuator() const;
    virtual bool isPunctuator( const std::string& ) const;
    virtual bool isEndOfSection() const;
    virtual bool isEndOfFile() const;
    
    virtual bool isSame( TokenType, const std::string& ) const;

    virtual std::string& stringData();
    virtual const std::string& stringData() const;
    virtual lsts_index_t numberData() const;
    virtual lsts_float_t floatData() const;

    virtual void setStringData( const std::string& new_string_data );
    virtual void setNumberData( lsts_index_t new_number_data );
    virtual void setFloatData( lsts_float_t new_float_data );

    // OBSOLETE:    
    virtual bool isNumber() const;
    virtual bool isNumber( lsts_index_t ) const;

 protected:
    Token();

 private:
    // Copy constructor and assignment operator not in use:
    Token( const Token& );  
    Token& operator=( const Token& );
};

// Derived tokens:

// NatNumber, natural number
class TT::NatNumber : public TT::Token
{
 public:
    NatNumber( lsts_index_t );
    
    virtual TokenType type() const;
    virtual bool isNatNumber() const;
    virtual bool isNatNumber( lsts_index_t ) const;
    
    virtual lsts_index_t numberData() const;
    virtual std::string& stringData(); // NOT IN USE!
    virtual const std::string& stringData() const;

    virtual void setNumberData( lsts_index_t );

    // OBSOLETE:
    virtual bool isNumber() const;
    virtual bool isNumber( lsts_index_t ) const;

 private:
    lsts_index_t data;
};

// Float
class TT::Float : public TT::Token
{
 public:
    Float( lsts_float_t );

    virtual TokenType type() const;
    virtual bool isFloat() const;
    
    virtual lsts_float_t floatData() const;
    virtual void setFloatData( lsts_float_t );
    
 private:
    lsts_float_t data;
};

// String data token base class
class TT::StringDataToken : public TT::Token
{
 public:
    StringDataToken( const std::string& str );

    virtual std::string& stringData();
    virtual const std::string& stringData() const;
    virtual void setStringData( const std::string& );

 protected:
    std::string data;
};

// Reserved word
class TT::ReservedWord : public TT::StringDataToken
{
 public:
    ReservedWord( const std::string& );
    
    virtual TokenType type() const;
    virtual unsigned isReservedWord() const;
    virtual unsigned isReservedWord( const std::string& ) const;
    virtual bool isSame( TokenType, const std::string& ) const;
    
    inline void SetIndex( unsigned i ) { index = i; }

 protected:
    unsigned index;
};

// Case-sensitive reserved word
struct TT::CaseSensReservedWord : public TT::ReservedWord
{
    CaseSensReservedWord( const std::string& str ) : ReservedWord( str ) { }
    
    virtual unsigned isReservedWord() const;
    virtual unsigned isReservedWord( const std::string& ) const;
    virtual bool isSame( TokenType, const std::string& ) const;
};

// Identifier
struct TT::Identifier : public TT::StringDataToken
{
    Identifier( const std::string& str ) : StringDataToken( str ) { }

    virtual TokenType type() const;    
    virtual bool isIdentifier() const;
    virtual bool isIdentifier( const std::string& ) const;
    virtual bool isSame( TokenType, const std::string& ) const;
    virtual bool isEndOfSection() const;
};

// Case-sensitive identifier
struct TT::CaseSensIdentifier : public TT::Identifier
{
    CaseSensIdentifier( const std::string& str ) : Identifier( str ) { }

    virtual bool isIdentifier() const;
    virtual bool isIdentifier( const std::string& ) const;
    virtual bool isSame( TokenType, const std::string& ) const;    
};

// String
struct TT::String : public TT::StringDataToken
{
    String( const std::string& str ) : StringDataToken( str ) { }
    
    virtual TokenType type() const;
    virtual bool isString() const;
    virtual bool isString( const std::string& ) const;
    virtual bool isSame( TokenType, const std::string& ) const;
};

// Punctuator
struct TT::Punctuator : public TT::StringDataToken
{
    Punctuator( const std::string& str ) : StringDataToken( str ) { }
    
    virtual TokenType type() const;
    virtual bool isPunctuation() const;
    virtual bool isPunctuation( const std::string& ) const;
    virtual bool isPunctuator() const;
    virtual bool isPunctuator( const std::string& ) const;
    virtual bool isSame( TokenType, const std::string& ) const;
};

// EndOfFile
struct TT::EndOfFile : public TT::Token
{
    virtual TokenType type() const;
    virtual bool isEndOfFile() const;
};


#endif
