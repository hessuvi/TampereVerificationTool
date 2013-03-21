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

#ifndef LAYOUTSECTION_HH
#define LAYOUTSECTION_HH

#include "FileFormat/Section.hh"
#include "LSTS_File/LayoutAP.hh"

// Class predeclaration:
class Header;


class iLayoutSection : public iSection
{
 public:
    iLayoutSection( iLayoutAP& ap ) : iap( ap ) { } 
    virtual ~iLayoutSection() { }
    virtual const char* GiveName() const { return "LAYOUT"; }

    virtual void ReadSection( Header& );
 private:
    iLayoutAP& iap;
};


class oLayoutSection : public oSection, public iLayoutAP
{
 public:
    oLayoutSection( oLayoutAP& oAp ) : oap( &oAp ) { }
    oLayoutSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oLayoutSection() { }
    virtual const char* GiveName() const { return "LAYOUT"; }

    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );
    
    virtual void lsts_StartLayout( Header& );
    virtual void lsts_StateLayout( lsts_index_t state_number,
                                   int x, int y );
    virtual void lsts_EndLayout();
    
 private:
    oLayoutAP* oap;
};


#endif
