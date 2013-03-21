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

// FILE_DES: RulesSection.cc: ParrulesFile
// Nieminen Juha

static const char * const ModuleVersion=
  "Module version: $Id: RulesSection.cc 1.3 Wed, 08 Sep 2004 18:43:46 +0300 timoe $";
// 
// Rules-sektioluokan toteutus
//

// $Log: RulesSection.cc,v $
// Revision 1.4  1999/09/14 14:06:56  warp
// *** empty log message ***
//
// Revision 1.3  1999/09/13 09:51:08  warp
// *** empty log message ***
//
// Revision 1.2  1999/09/08 10:53:16  warp
// *** empty log message ***
//
// Revision 1.1  1999/08/12 13:22:18  warp
// Initial revision
//


#include "RulesSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "error_handling.hh"

using namespace std;


// Periytetyt metodit
//===========================================================================

// Luku
//-----

void
iRulesSection::ReadSection( Header& )
{
    ITokenStream& is = its();

    is.SetPunctuation( "(#)#,#->#=" );


    is.Get(TT::IDENTIFIER,"Amount");
    is.Get(TT::PUNCTUATION,"=");

    rulesAmnt = is.Get( TT::NUMBER ).numberData();


    iap.lsts_StartRules( rulesAmnt );


    lsts_index_t n1, n2;

    for(unsigned i=0;i<rulesAmnt;i++)
    {
        while(1)
        {
            const TT::Token* token = &is.Get();
            check_claim(token->isPunctuation() &&
                        (token->stringData()=="(" ||
                         token->stringData()=="->"),
                        is.errorMsg(
                            *token, "Syntax error. '(' or '->' expected."));

            if(token->stringData()=="->") break;
            
            n1 = is.Get( TT::NUMBER ).numberData();
            
            is.Get(TT::PUNCTUATION,",");

            n2 = is.Get( TT::NUMBER ).numberData();

            is.Get(TT::PUNCTUATION,")");

            iap.lsts_Rule(n1,n2);
        }


        n1 = is.Get(TT::NUMBER).numberData();

        iap.lsts_RuleDestinationTransitionNumber(n1);
    }


    iap.lsts_EndRules();
}


// Kirjoitus
//----------
bool oRulesSection::doWeWriteSectionWithInit( Header& )
{
    rulesAmnt = oap->lsts_numberOfRulesToWrite();
    return rulesAmnt;
}

void oRulesSection::WriteSection( Header& )
{
    WriteBeginSection();

    
    ots().PutIdentifier("Amount");
    ots().PutPunctuation("=");
    ots().PutNumber( rulesAmnt );
    ots().Endl();

    rulesWritten = 0;


    oap->lsts_WriteRules( *this );

    warn_ifnot(rulesWritten == rulesAmnt,
               valueToMessage("Rules amount set to ", rulesAmnt,
                              valueToMessage(" but ", rulesWritten,
                                             " rules has been written.")));

    WriteEndSection();
}


void oRulesSection::lsts_Rule( lsts_index_t inputLSTS,
                               lsts_index_t transitionNumber )
{
    OTokenStream& os = ots();

    os.PutPunctuation("(");
  
    os.spaceOff();
    os.PutNumber(inputLSTS);
    os.PutPunctuation(",");
    os.PutNumber(transitionNumber);
    os.PutPunctuation(")");
    os.spaceOn();
}

void oRulesSection::lsts_RuleDestinationTransitionNumber( lsts_index_t
                                                          transitionNumber)
{
    ots().PutPunctuation("->");
    ots().PutNumber(transitionNumber);
    ots().Endl();

    rulesWritten++;
}

void
oRulesSection::lsts_StartRules( unsigned )
{ WriteBeginSection(); }

void
oRulesSection::lsts_EndRules()
{ WriteEndSection(); }
