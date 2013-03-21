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

#include "LayoutSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "LSTS_File/Header.hh"


void
iLayoutSection::ReadSection( Header& header )
{ 
    iap.lsts_StartLayout( header );

    iTokenStream& tin = its();
    tin.SetPunctuation( "-" );
    lsts_index_t state_n;
    
    while ( tin.GetNumber( state_n ) )
    {
        int x; int y; tin >> x >> y;
        iap.lsts_StateLayout( state_n, x, y );
    }

    iap.lsts_EndLayout();
}


bool
oLayoutSection::doWeWriteSectionWithInit( Header& )
{ return oap->lsts_doWeWriteLayout(); }

void
oLayoutSection::WriteSection( Header& header )
{
    lsts_StartLayout( header );
    // Here we give control and the writer to the user:
    oap->lsts_WriteLayout( *this );
    lsts_EndLayout();
}

void
oLayoutSection::lsts_StateLayout( lsts_index_t state, int x, int y )
{ tout << state << x << y << endl; }

void
oLayoutSection::lsts_StartLayout( Header& ) { WriteBeginSection(); }
void
oLayoutSection::lsts_EndLayout() { WriteEndSection(); }
