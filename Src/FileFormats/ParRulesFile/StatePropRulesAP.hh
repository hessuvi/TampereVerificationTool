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

// FILE_DES: StatePropRulesAP.hh: ParrulesFile
// Timo Erkkilä

#ifdef CIRC_STATEPROPRULESAP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATEPROPRULESAP_HH_
#define ONCE_STATEPROPRULESAP_HH_
#define CIRC_STATEPROPRULESAP_HH_

#include "../LSTS_File/SectionAP.hh"

#include <string>


// Luokan esittely:

class iStatePropRulesAP : public iSectionAP
{
 public:
    virtual ~iStatePropRulesAP();

    virtual void lsts_StartStatePropRules( class Header& ) = 0;
    virtual void lsts_StatePropRule( const std::string& rule ) = 0;
    virtual void lsts_EndStatePropRules() = 0;

    typedef class iStatePropRulesSection section_t;
};



class oStatePropRulesAP : public oSectionAP
{
 public:
    virtual ~oStatePropRulesAP();
        
    virtual bool lsts_doWeWriteStatePropRules() = 0;
    virtual void lsts_WriteStatePropRules( iStatePropRulesAP& writer ) = 0;

    typedef class oStatePropRulesSection section_t;
};


#undef CIRC_STATEPROPRULESAP_HH_
#endif
