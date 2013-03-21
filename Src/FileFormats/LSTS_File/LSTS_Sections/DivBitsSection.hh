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

#ifndef DIVBITSSECTION_HH
#define DIVBITSSECTION_HH

#include "FileFormat/Section.hh"
#include "LSTS_File/DivBitsAP.hh"
#include "Interval.hh"


// Class predeclaration:
class Header;


class iDivBitsSection : public iSection
{
 public:
    iDivBitsSection( iDivBitsAP& ap ) : iap( ap ) { } 
    virtual ~iDivBitsSection() { }
    virtual const char* GiveName() const { return "DIV_BITS"; }
    virtual void ReadSection( Header& );
 private:
    iDivBitsAP& iap;
};


class oDivBitsSection : public oSection, public iDivBitsAP
{
 public:
    oDivBitsSection( oDivBitsAP& ap ) : oap( &ap ) { }
    oDivBitsSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oDivBitsSection() { }
    virtual const char* GiveName() const { return "DIV_BITS"; }

    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );

    virtual void lsts_StartDivBits( Header& );
    // Writes the beginning of the section.

    virtual void lsts_DivBit( lsts_index_t state_number );
    virtual void lsts_DivBits( lsts_index_t subrange_start,
                               lsts_index_t subrange_end );
       
    virtual void lsts_EndDivBits();
    // Writes the end of the section.

 private:
    Interval interv;
    oDivBitsAP* oap;
};


#endif
