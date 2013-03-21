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

// FILE_DES: RulesSection_AP.hh: ParrulesFile
// Nieminen Juha

// $Id: RulesAP.hh 1.1 Thu, 19 Dec 2002 02:14:34 +0200 timoe $
// 

// $Log: RulesSection_AP.hh,v $
// Revision 1.2  1999/09/08 10:53:16  warp
// *** empty log message ***
//
// Revision 1.1  1999/08/12 13:22:18  warp
// Initial revision
//

#ifdef CIRC_RULESAP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_RULESAP_HH_
#define ONCE_RULESAP_HH_
#define CIRC_RULESAP_HH_

#include "../LSTS_File/SectionAP.hh"


// Luokan esittely:

class iRulesAP : public iSectionAP
{
 public:
    virtual ~iRulesAP();

    virtual void lsts_StartRules( unsigned amnt ) = 0;
    virtual void lsts_Rule(lsts_index_t inputLSTS,
                           lsts_index_t transitionNumber) = 0;
    virtual void lsts_RuleDestinationTransitionNumber(lsts_index_t
                                                      transitionNumber) = 0;
    virtual void lsts_EndRules() = 0;

    typedef class iRulesSection section_t;
    typedef class RulesChecker checker_t;
};


class oRulesAP : public oSectionAP
{
 public:
    virtual ~oRulesAP();

    virtual unsigned lsts_numberOfRulesToWrite() = 0;
    virtual void lsts_WriteRules( iRulesAP& writer ) = 0;

    typedef class oRulesSection section_t;
    typedef class RulesChecker checker_t;
};


#undef CIRC_RULESAP_HH_
#endif

