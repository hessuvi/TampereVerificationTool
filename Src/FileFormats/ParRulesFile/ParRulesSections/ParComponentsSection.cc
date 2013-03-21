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

// FILE_DES: ParComponentsSection.cc: ParrulesFile
// Nieminen Juha

static const char * const ModuleVersion=
  "Module version: $Id: ParComponentsSection.cc 1.2 Wed, 08 Sep 2004 18:43:46 +0300 timoe $";
// 
// ParComponents-sektioluokan toteutus
//

// $Log: ParComponentsSection.cc,v $
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


#include "ParComponentsSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"


using namespace std;


// Periytetyt metodit
//===========================================================================

// Luku
//-----

void
iParComponentsSection::ReadSection( Header& )
{
    iap.lsts_StartParComponents();

    ITokenStream& is = its();
    is.SetPunctuation( "=#<#>" );

    unsigned n;

    while ( is.GetNumber( n ) )
    {
        is.Get( TT::PUNCTUATION, "=" );
        
        string pre;

        if ( is.Peek().isPunctuator( "<" ) )
        {
            is.Get();
            pre = is.Get( TT::STRING ).stringData();
            is.Get( TT::PUNCTUATOR, ">" );
        }
            
        iap.lsts_Component( n, is.Get( TT::STRING ).stringData(), pre );
    }


    iap.lsts_EndParComponents();
}


// Kirjoitus
// ---------
bool oParComponentsSection::doWeWriteSectionWithInit( Header& )
{
    return oap->lsts_doWeWriteParComponents();
}

void oParComponentsSection::WriteSection( Header& )
{
    WriteBeginSection();
    //filenameCnt=1;
    //std::cout <<  GiveoAP() << "###" << GiveWriter() << std::endl;
    oap->lsts_WriteParComponents( *this );
    WriteEndSection();
}


void oParComponentsSection::lsts_Component( unsigned filenameCnt,
                                            const string& filename,
                                            const string& prefix )
{
    oTokenStream& o = ots();

    o.PutNumber( filenameCnt );
    o.PutPunctuation( "=" );

    if ( prefix.size() )
    {
        o.PutPunctuator( "<" );
        o.spaceOff();
        o.PutString( prefix );
        o.PutPunctuator( ">" );
        o.spaceOn();
    }

    o.PutString( filename );
    o.Endl();

    //filenameCnt++;
}

void
oParComponentsSection::lsts_StartParComponents()
{ WriteBeginSection(); }

void
oParComponentsSection::lsts_EndParComponents()
{ WriteEndSection(); }
