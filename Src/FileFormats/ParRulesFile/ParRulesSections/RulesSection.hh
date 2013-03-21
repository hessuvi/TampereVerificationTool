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

// FILE_DES: RulesSection.hh: ParrulesFile
// Nieminen Juha

// $Id: RulesSection.hh 1.3 Wed, 15 Sep 2004 21:48:51 +0300 timoe $
// 

// $Log: RulesSection.hh,v $
// Revision 1.3  1999/09/14 14:06:56  warp
// *** empty log message ***
//
// Revision 1.2  1999/09/08 10:53:16  warp
// *** empty log message ***
//
// Revision 1.1  1999/08/12 13:22:18  warp
// Initial revision
//

#ifdef CIRC_RULESSECTION_HH_
#error "Include recursion"
#endif

#ifndef ONCE_RULESSECTION_HH_
#define ONCE_RULESSECTION_HH_
#define CIRC_RULESSECTION_HH_


#include "FileFormat/Section.hh"
#include "../RulesAP.hh"


// Class predeclaration:
class Header;


class iRulesSection : public iSection
{
 public:
    iRulesSection( iRulesAP& ap ) : iap( ap ), rulesAmnt( 0 ) { } 
    virtual ~iRulesSection() { }
    virtual const char* GiveName() const { return "RULES"; }
    virtual void ReadSection( Header& );
 private:
    iRulesAP& iap;
    unsigned rulesAmnt;
};


class oRulesSection : public oSection, public iRulesAP
{
 public:
    oRulesSection( oRulesAP& ap ) : oap( &ap ), rulesAmnt( 0 ),
                                    rulesWritten( 0 ) { }
    oRulesSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ),
                                        rulesAmnt( 0 ), rulesWritten( 0 ) { }
    virtual ~oRulesSection() { }
    virtual const char* GiveName() const { return "RULES"; }

    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );

    virtual void lsts_StartRules( unsigned );
    virtual void lsts_Rule(lsts_index_t inputLSTS,
                           lsts_index_t transitionNumber);
    virtual void lsts_RuleDestinationTransitionNumber(lsts_index_t
                                                      transitionNumber);
    virtual void lsts_EndRules();

 private:
    oRulesAP* oap;
    unsigned rulesAmnt, rulesWritten;
};


#undef CIRC_RULESSECTION_HH_
#endif

