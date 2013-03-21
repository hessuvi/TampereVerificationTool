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

// FILE_DES: StatePropRulesSection.cc: ParrulesFile
// Nieminen Juha

static const char * const ModuleVersion=
"Module version: $Id: StatePropRulesSection.cc 1.2 Wed, 08 Sep 2004 18:43:46 +0300 timoe $";
// 
// StatePropRules-sektioluokan toteutus
//

// $Log: StatePropRulesSection.cc,v $
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


#include <cctype>
#include <string>
using std::string;

#include "StatePropRulesSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"


static const char string_separator_char = '\x01';


// Periytetyt metodit
//===========================================================================

// Luku
//-----

void
iStatePropRulesSection::ReadSection( Header& hd )
{
    ITokenStream& is = its();

    iap.lsts_StartStatePropRules( hd );

    string end_token = "end";

    while ( !is.peekIdentifier( end_token ) )
    {
        iap.lsts_StatePropRule( is.GetLine( ';', string_separator_char,
                                            remove_white_space ) );
    }
    
    // check_claim( 
    //             is.errorMsg( is.Peek(), "bad beginning of the rule" ) );
    
    iap.lsts_EndStatePropRules();
}


// Kirjoitus
//----------
bool
oStatePropRulesSection::doWeWriteSectionWithInit( Header& )
{
    return oap->lsts_doWeWriteStatePropRules();
}

void
oStatePropRulesSection::WriteSection( Header& )
{
    WriteBeginSection();
    oap->lsts_WriteStatePropRules( *this );
    WriteEndSection();
}

void
oStatePropRulesSection::lsts_StatePropRule( const string& rule )
{
    OTokenStream& os = ots();

    os.PutLine( rule, ';', string_separator_char );
    os.Endl();
}

void
oStatePropRulesSection::lsts_StartStatePropRules( Header& )
{ WriteBeginSection(); }

void
oStatePropRulesSection::lsts_EndStatePropRules()
{ WriteEndSection(); }
