/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2002 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen, Timo Erkkilä, Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: StateNamesStore.cc: Src/FileFormats/LSTS_File
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: StateNamesStore.cc 1.12 Fri, 20 Sep 2002 20:04:50 +0300 timoe $";
// 
// Implementation of StateNamesStore class.
//

// $Log:$


#include "StateNamesStore.hh"
#include "error_handling.hh"
#include "LSTS_File/Header.hh"

#include <string>
using std::string;


StateNamesStore::StateNamesStore() :
    iStateNamesAP(),
    oStateNamesAP(),
    state_cnt( 0 )
{ }

bool
StateNamesStore::lsts_doWeWriteStateNames() { return state_cnt; }

void
StateNamesStore::lsts_WriteStateNames( iStateNamesAP& ap )
{
    for ( unsigned i = 0; i < state_names.size(); ++i )
    {
        if ( state_numbers[i] )
        { ap.lsts_StateName( state_numbers[i], state_names[i] ); }
    }
}

void
StateNamesStore::lsts_StartStateNames( Header& hd )
{ state_cnt = hd.GiveStateCnt(); }
    
void
StateNamesStore::lsts_StateName( lsts_index_t state_number,
                                 const string& name )
{
    state_numbers.push_back( state_number );
    state_names.push_back( name );
}

void
StateNamesStore::lsts_EndStateNames() { }

lsts_index_t
StateNamesStore::RenumberStateNames( Renumber<lsts_index_t>& renum )
{
    unsigned names_removed = 0;

    state_cnt = state_numbers.size();

    for ( unsigned i = 0; i < state_numbers.size(); ++i )
    {
        state_numbers[i] = renum.newNumber( state_numbers[i] ); 
        
        if ( !state_numbers[i] )
        {
            ++names_removed;
            --state_cnt;
        }
    }

    return names_removed;
}
