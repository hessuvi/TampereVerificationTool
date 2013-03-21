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

#ifndef SECTION_HH
#define SECTION_HH

#include "FileFormat/Lexical/OTokenStream.hh"

// Predeclarations:
class Header;
class ITokenStream;
class iSectionAP;
class oSectionAP;


class Section
{
 public:
    virtual ~Section();
    virtual const char* GiveName() const = 0;
};


class iSection : public Section
{
 public:
    virtual ~iSection();

    virtual iSectionAP* GiveiAP() { return 0; }

    void Read( ITokenStream& from, Header& );
    // Sets ITokenStream pointer and calls ReadSection().

 protected:
    iSection() : Section(), from( 0 ) { }
    // This class cannot be instantiated, only inherited.

    virtual void ReadSection( Header& ) = 0;

    ITokenStream& its() const { return *from; }
    // Gives input token stream.

 private:
    // No copy constructor or assignment in use:
    iSection( const iSection& );
    iSection& operator=( const iSection& );

    ITokenStream* from;
};


class oSection : public Section
{
 public:
    virtual ~oSection();

    virtual oSectionAP* GiveoAP() { return 0; }

    bool doWeWriteWithInit( OTokenStream& to, Header& );
    // Initializes the section. If return value is false, the section
    //   will not be written.
    void Write( Header&, OTokenStream* oTs = 0 );
    // Calls WriteSection().

 protected:
    oSection();
    oSection( OTokenStream& ot ) { tout.to = &ot; }
    // This class cannot be instantiated, only inherited.

    virtual bool doWeWriteSectionWithInit( Header& ) = 0;
    virtual void WriteSection( Header& ) = 0;

    void WriteBeginSection();
    void WriteEndSection();
    
    OTokenStream& ots() const { return *tout.to; }
    // Gives the output token stream.

    struct TOut
    {
        TOut() : to( 0 ) { }

        oTokenStream* to;
        std::string tmp_str;
        enum string_mode { STRI, ID, PUNCT };
        string_mode str_mode;

        TOut& operator<<( int i )
        {
            if ( i < 0 ) { to->PutPunctuator( "-" ); to->spaceOff(); i = -i; }
            to->PutNatNumber( i ); to->spaceOn(); return *this;
        }
        TOut& operator<<( unsigned nat_number )
        { to->PutNatNumber( nat_number ); return *this; }
        TOut& operator<<( char punctuator )
        { tmp_str = punctuator; to->PutPunctuator( tmp_str ); return *this; }
        TOut& operator<<( const std::string& str )
        {
            switch( str_mode )
            {
              case STRI: to->PutString( str ); break;
              case ID: to->PutIdentifier( str ); break;
              case PUNCT: to->PutPunctuator( str ); break;
              default: break;
            }
            str_mode = STRI;
            return *this;
        }
        TOut& operator<<(TOut& (*fp)(TOut&)) { return (*fp)(*this); }
    };

    static TOut& space_on( TOut& To ) { To.to->spaceOn(); return To; }
    static TOut& space_off( TOut& To ) { To.to->spaceOff(); return To; }
    static TOut& id( TOut& To ) { To.str_mode = TOut::ID; return To; }
    static TOut& punct( TOut& To ) { To.str_mode = TOut::PUNCT; return To; }

    static TOut& endl( TOut& To ) { To.to->Endl(); return To; }
    
    TOut tout;

 private:
    // No copy constructor or assignment in use:
    oSection( const oSection& );
    oSection& operator=( const oSection& );

    unsigned startSectionCalls;
    unsigned endSectionCalls;
};


#endif
