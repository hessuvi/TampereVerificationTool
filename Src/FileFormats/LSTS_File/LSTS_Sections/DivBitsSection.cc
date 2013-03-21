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

#include "DivBitsSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "LSTS_File/Header.hh"

#include "otfvi.hh"


void
iDivBitsSection::ReadSection( Header& header )
{
    iTokenStream& iTs = its();

    iTs.SetPunctuation( Interval::GiveSubrangePunctuator() );

    iap.lsts_StartDivBits( header );
    

    lsts_index_t state = 0;
    lsts_index_t inter_end = 0;

    while ( Interval::Read( iTs, state, inter_end ) )
    {
        if ( inter_end )
        {
            iap.lsts_DivBits( state, inter_end );
            continue;
        }

        iap.lsts_DivBit( state );
    }

    iap.lsts_EndDivBits();
}


bool
oDivBitsSection::doWeWriteSectionWithInit( Header& )
{ return oap->lsts_doWeWriteDivBits(); }

void
oDivBitsSection::WriteSection( Header& header )
{
    interv.EnableOutputIntervals( oap->lsts_isIntervalSearchOn() );

    lsts_StartDivBits( header );
    oap->lsts_WriteDivBits( *this );
    lsts_EndDivBits();
}

void
oDivBitsSection::lsts_StartDivBits( Header& ) { WriteBeginSection(); }

void
oDivBitsSection::lsts_DivBit( lsts_index_t state )
{ interv.Write( ots(), state ); }

void
oDivBitsSection::lsts_DivBits( lsts_index_t subrange_start,
                               lsts_index_t subrange_end )
{ Interval::WriteSubrange( ots(), subrange_start, subrange_end ); }

void
oDivBitsSection::lsts_EndDivBits()
{
    interv.WriteLast( ots() );
    ots().Endl();
    WriteEndSection();
}
