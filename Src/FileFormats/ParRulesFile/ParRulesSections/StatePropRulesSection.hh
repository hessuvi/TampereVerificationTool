/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen, Timo Erkkilä, Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: StatePropRulesSection.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä


#ifdef CIRC_STATEPROPRULESSECTION_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATEPROPRULESSECTION_HH_
#define ONCE_STATEPROPRULESSECTION_HH_
#define CIRC_STATEPROPRULESSECTION_HH_


#include "FileFormat/Section.hh"
#include "../StatePropRulesAP.hh"


// Class predeclaration:
class Header;


class iStatePropRulesSection : public iSection
{
 public:
    iStatePropRulesSection( iStatePropRulesAP& ap ) :
        iap( ap ), remove_white_space( true ) { }
    virtual ~iStatePropRulesSection() { }
    virtual const char* GiveName() const { return "STATE_PROP_RULES"; }
    virtual void ReadSection( Header& );
 private:
    iStatePropRulesAP& iap;
    bool remove_white_space;
};


class oStatePropRulesSection : public oSection,
                               public iStatePropRulesAP
{
 public:
    oStatePropRulesSection( oStatePropRulesAP& ap ) : oap( &ap ) { }
    oStatePropRulesSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oStatePropRulesSection() { }
    virtual const char* GiveName() const { return "STATE_PROP_RULES"; }

    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );    

    virtual void lsts_StartStatePropRules( Header& );
    virtual void lsts_StatePropRule( const std::string& rule );
    virtual void lsts_EndStatePropRules();

 private:
    oStatePropRulesAP* oap;
};


#undef CIRC_STATEPROPRULESSECTION_HH_
#endif

