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

// FILE_DES: DivBitsChecker.cc: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: DivBitsChecker.cc 1.9 Fri, 08 Feb 2002 14:14:18 +0200 timoe $";
// 
// The implementation of DivBitsChecker class.
//

// $Log:$


#include "DivBitsChecker.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/FileFormat.hh"
#include "Interval.hh"


// PUBLIC:


DivBitsChecker::DivBitsChecker( iDivBitsAP& ap,
                                FileFormat& ff ) :
    iDivBitsAP(), Checker( ff ),
    state_cnt( 0 ),

    AP( ap )
{ }

DivBitsChecker::~DivBitsChecker() { }


// PRIVATE:


void
DivBitsChecker::lsts_StartDivBits( Header& hd )
{
    check_isGiven( "State_cnt", hd.isStateCntGiven() );
    
    state_cnt = hd.GiveStateCnt();

    AP.lsts_StartDivBits( hd );
}

void
DivBitsChecker::lsts_EndDivBits()
{
    AP.lsts_EndDivBits();
}


void
DivBitsChecker::lsts_DivBit( lsts_index_t state )
{
    check_state( state, state_cnt, "state number" );
                 
    AP.lsts_DivBit( state );
}

void
DivBitsChecker::lsts_DivBits( lsts_index_t interv_start,
                              lsts_index_t interv_end )
{
    check_interval( "state", state_cnt, interv_start, interv_end );
    AP.lsts_DivBits( interv_start, interv_end );
}
