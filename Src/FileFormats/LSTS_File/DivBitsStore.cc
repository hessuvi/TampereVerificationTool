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

// FILE_DES: DivBitsStore.cc: Src/FileFormats/LSTS_File
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: DivBitsStore.cc 1.11 Tue, 08 Oct 2002 02:18:05 +0300 timoe $";
// 
// Implementation of DivBitsStore class.
//

// $Log:$


#include <algorithm>

#include "DivBitsStore.hh"
#include "error_handling.hh"
#include "removeDuplicates.hh"
#include <iostream>

using namespace std;


DivBitsStore::DivBitsStore() :

    iDivBitsAP(),
    oDivBitsAP()

{ }


bool
DivBitsStore::lsts_doWeWriteDivBits()
{
    return states.size();
}

#define WRITE  ;
//for ( unsigned i = 0; i < states.size(); ++i ) { cout << "#" << states[ i ]; } cout << endl


void
DivBitsStore::lsts_WriteDivBits( iDivBitsAP& ap )
{
    //cout << "Size: " << states.size() << endl;
    for ( unsigned i = 0; i < states.size(); ++i )
    {
        //  cout << "O: " << states[ i ] << endl;
        ap.lsts_DivBit( states[ i ] );
    }
}

void
DivBitsStore::lsts_StartDivBits( Header& )
{ states.clear(); }
    
void
DivBitsStore::lsts_DivBit( lsts_index_t state )
{
    // cout << state << endl;
    states.push_back( state );
}

void
DivBitsStore::lsts_EndDivBits()
{
     std::sort( states.begin(), states.end() );
     WRITE;
}

bool
DivBitsStore::divBitOn( lsts_index_t state ) const
{ return std::binary_search( states.begin(), states.end(), state ); }

lsts_index_t
DivBitsStore::RemoveDuplicateDivBits()
{
    //    cout << "hello" << endl;
    WRITE;
    return RemoveDuplicates( states );
}

lsts_index_t
DivBitsStore::RenumberDivBits( Renumber<lsts_index_t>& renum )
{
    //    cout << "hello2" << endl;
    WRITE;
    return renum.RenumberVector( states );
}
