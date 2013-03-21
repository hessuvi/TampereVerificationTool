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

// FILE_DES: StatePropsStore.cc: Src/FileFormats/LSTS_File
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: StatePropsStore.cc 1.15 Fri, 20 Sep 2002 20:04:50 +0300 timoe $";
// 
// Implementation of StatePropsStore class.
//

// $Log:$


#include <algorithm>

#include "StatePropsStore.hh"
#include "error_handling.hh"
#include "FileFormat/Lexical/constants.hh"
#include "removeDuplicates.hh"
#include <iostream>

using std::string;


StatePropsStore::StatePropsStore() : iStatePropsAP(), oStatePropsAP() { }

lsts_index_t
StatePropsStore::lsts_numberOfStatePropsToWrite()
{ return state_props.size(); }

void
StatePropsStore::lsts_WriteStateProps( iStatePropsAP& ap )
{
    /*
    ap.lsts_StartCutStates();

    ap.lsts_CutState( 5 );
    ap.lsts_CutState( 2 );
    ap.lsts_CutState( 1 );

    ap.lsts_EndCutStates();


    ap.lsts_StartRejectStates();

    ap.lsts_RejectState( 1 );
    ap.lsts_RejectState( 2 );
    ap.lsts_RejectState( 1 );

    ap.lsts_EndRejectStates();
    */

    for ( props_t::size_type p = 0; p < state_props.size(); ++p )
    {
        const string prop_name = state_props[ p ].name;
        
        ap.lsts_StartPropStates( prop_name );
        
        for ( lsts_vec_size_t s = 0; s < state_props[ p ].states.size(); ++s )
        {
            //    cout << "So(" << state_props[ p ].states[ s ] << ")" << endl;
            ap.lsts_PropState( state_props[ p ].states[ s ] );
        }
        
        ap.lsts_EndPropStates( prop_name );
    }
    
}


void
StatePropsStore::lsts_StartStateProps( Header& )
{ state_props.clear(); }

void
StatePropsStore::lsts_StartPropStates( const std::string& prop_name )
{ state_props.push_back( StateProp( prop_name ) ); }

void
StatePropsStore::lsts_PropState( lsts_index_t state )
{ state_props.back().states.push_back( state ); }

void
StatePropsStore::lsts_EndPropStates( const string& )
{
    std::vector<lsts_index_t>& vec = state_props.back().states;

    if ( vec.size() > 1 )
    { std::sort( vec.begin(), vec.end() ); }
}

void
StatePropsStore::lsts_EndStateProps() { }


lsts_index_t
StatePropsStore::RemoveDuplicatePropStates()
{
    lsts_index_t removed_amount = 0;
    
    for ( props_t::size_type i = 0; i < state_props.size(); ++i )
    {
        if ( state_props[ i ].states.size() > 1 )
        {
            removed_amount += RemoveDuplicates( state_props[ i ].states );
        }
    }

    return removed_amount;
}

lsts_index_t
StatePropsStore::RenumberPropStates( Renumber<lsts_index_t>& states_renum )
{
    lsts_index_t removed_amount = 0;
    
    for ( props_t::size_type i = 0; i < state_props.size(); ++i )
    {
        removed_amount +=
            states_renum.RenumberVector( state_props[ i ].states );
    }

    return removed_amount;
}
