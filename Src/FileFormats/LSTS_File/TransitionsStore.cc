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

// FILE_DES: TransitionsStore.cc: Src/FileFormats/LSTS_File
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: TransitionsStore.cc 1.23 Fri, 20 Sep 2002 20:04:50 +0300 timoe $";
// 
// Implementation of TransitionsStore class.
//

// $Log:$


#include <algorithm>

#include "TransitionsStore.hh"
#include "error_handling.hh"
#include "LSTS_File/Header.hh"
#include "removeDuplicates.hh"

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;


TransitionsStore::TransitionsStore() :

    iTransitionsAP(),
    oTransitionsAP(),
    i( 0 )

{ }

lsts_index_t
TransitionsStore::lsts_numberOfTransitionsToWrite()
{
    return transitions.size() / 3;
}

void
TransitionsStore::lsts_WriteTransitions( iTransitionsAP& ap )
{
    i = 0;
    
    while ( i < transitions.size() )
    {
        const lsts_index_t start_state = transitions[i];
        ap.lsts_StartTransitionsFromState( start_state );

        do
        {
            ap.lsts_Transition( transitions[i++],
                                transitions[i++],
                                transitions[i++] );
        
        } while ( i < transitions.size() && transitions[i] == start_state );
        
        ap.lsts_EndTransitionsFromState( start_state );
    }
    
}


void
TransitionsStore::lsts_StartTransitions( Header& hd )
{
    transitions.resize( 3 * hd.GiveTransitionCnt() );
    i = 0;
}

void
TransitionsStore::lsts_StartTransitionsFromState( lsts_index_t
                                                  /*start_state*/ ) { }

void
TransitionsStore::lsts_Transition( lsts_index_t start_state,
                                   lsts_index_t dest_state,
                                   lsts_index_t action )
{
    transitions[i++] = start_state;
    transitions[i++] = dest_state;
    transitions[i++] = action;
}

#ifdef RATE

#include <iostream>
using std:: cout;
using std::endl;

void
TransitionsStore::lsts_Transition( lsts_index_t ss, lsts_index_t ds,
                                   lsts_index_t a, lsts_float_t fl )
{ cout << ss << "-" << a << "->" << ds << " (" << fl << ") read." << endl; }
#endif

void
TransitionsStore::lsts_EndTransitionsFromState( lsts_index_t
                                                /*start_state*/ ) { }

void
TransitionsStore::lsts_EndTransitions()
{
    //cout << transitions.size() / 3 << " transitions read." << endl;
}



// Safe store class:


SafeTransitionsStore::SafeTransitionsStore() :

    iTransitionsAP(),
    oTransitionsAP(),
    initial_state( 0 ),
    greatest_state( 0 ),
    greatest_action( 0 )

{ }

lsts_index_t
SafeTransitionsStore::lsts_numberOfTransitionsToWrite()
{
    return GiveTransitionCnt();
}

void
SafeTransitionsStore::lsts_WriteTransitions( iTransitionsAP& ap )
{
    lsts_index_t i = 0;
    
    while ( i < transitions.size() )
    {
        const lsts_index_t start_state = transitions[i].s_state;
        ap.lsts_StartTransitionsFromState( start_state );

        do
        {
#ifdef RATE
            if ( transitions[ i ].fl_number != -1.1 )
            {
                ap.lsts_Transition( transitions[i].s_state,
                                    transitions[i].d_state,
                                    transitions[i].action,
                                    transitions[i].fl_number );
            }
            else
#endif
            ap.lsts_Transition( transitions[i].s_state,
                                transitions[i].d_state,
                                transitions[i].action );
            ++i;
        
        } while ( i < transitions.size() &&
                  transitions[i].s_state == start_state );
        
        ap.lsts_EndTransitionsFromState( start_state );
    }
    
}


void
SafeTransitionsStore::lsts_StartTransitions( Header& )
{
    greatest_state = 0;
    greatest_action = 0;
}

void
SafeTransitionsStore::lsts_StartTransitionsFromState( lsts_index_t ) { }


void
SafeTransitionsStore::lsts_Transition( lsts_index_t start_state,
                                       lsts_index_t dest_state,
                                       lsts_index_t action )
{
    checkIfGreatestState( start_state );
    checkIfGreatestState( dest_state );

    if ( action > greatest_action )
    {
        greatest_action = action;
    }

    transitions.push_back( Transition( start_state, dest_state, action ) );
}

#ifdef RATE
void
SafeTransitionsStore::lsts_Transition( lsts_index_t ss, lsts_index_t ds,
                                       lsts_index_t a, lsts_float_t fl )
{
    lsts_Transition( ss, ds, a );
    transitions.back().fl_number = fl;
    // cout << ss << "-" << a << "->" << ds << " (" << fl << ") read." << endl;
}
#endif

void
SafeTransitionsStore::lsts_EndTransitionsFromState( lsts_index_t ) { }

struct StackState
{
    inline StackState( lsts_index_t st, lsts_index_t i ) :
        state_n( st ), tr_idx( i ) { }
    
    lsts_index_t state_n; lsts_index_t tr_idx;
};

void
SafeTransitionsStore::lsts_EndTransitions()
{
    std::sort( transitions.begin(), transitions.end() );
}

lsts_index_t
SafeTransitionsStore::RemoveDuplicateTransitions()
{
    return RemoveDuplicates( transitions );
}

lsts_index_t
SafeTransitionsStore::RemoveUnreachablePart( lsts_index_t i_state )
{
    check_claim( i_state,
                 "SafeTransitionsStore::RemoveUnreachablePart(): initial "
                 "state can't be zero" );
    initial_state = i_state;

    if ( !greatest_state )
    {
        renumber.SetOldMaxNumber( i_state );
        renumber.AddNumber( i_state );
        return 0;
    }
    renumber.SetOldMaxNumber( greatest_state );


    const lsts_index_t FOUND = ~0;
    vector<lsts_index_t> trs_of_states( greatest_state + 1, FOUND );
    // Notice: slot in index zero is left unused.
    // FOUND means: a) the state doesn't have transitions at all or
    //              b) the state has been found

    {
        lsts_index_t j = 0;
        for( lsts_index_t i = 0; i < transitions.size(); ++i )
        {
            if ( transitions[ i ].s_state != j )
            {
                // Writing down the start index of transitions of
                // each of the states.
                j = transitions[ i ].s_state;
                trs_of_states[ j ] = i;
            }
        }
    }

    // Depth-first-search:
    vector<StackState> DFSstack;

    const StackState init_state( i_state, trs_of_states[ i_state ] );
    DFSstack.push_back( init_state );
    trs_of_states[ i_state ] = FOUND;
    
    while ( DFSstack.size() )
    {
        lsts_index_t tr_idx = DFSstack.back().tr_idx;
        
        // Is the current state finished?
        if ( tr_idx >= transitions.size() ||
             DFSstack.back().state_n != transitions[ tr_idx ].s_state )
        {
            DFSstack.pop_back();
            continue;
        }

        const lsts_index_t next_state_n = transitions[ tr_idx ].d_state;
        DFSstack.back().tr_idx++;
        tr_idx = trs_of_states[ next_state_n ];

        if ( tr_idx != FOUND )
        {
            const StackState foundState( next_state_n, tr_idx );
            DFSstack.push_back( foundState );
            // cout << "1 <= " << next_state_n << " <= " <<
            //    trs_of_states.size() - 1 << endl;
            trs_of_states[ next_state_n ] = FOUND;
        }

    }
    
    // Removal of unreached states and transitions:
    
    lsts_index_t i = 0;
    lsts_index_t j = 0;

    while ( j < transitions.size() )
    {
        if ( trs_of_states[ transitions[ j ].s_state ] == FOUND )
        {
            transitions[ i ] = transitions[ j ];
            ++i;
        }
        ++j;
    }


    lsts_index_t removedTrs = 0;

    if ( j != i )
    {
        for ( ; j != i; --j )
        {
            transitions.pop_back();
            ++removedTrs;
        }

        // Finding the greatest state number again:

        greatest_state = transitions.back().s_state;

        for ( j = 0; j < transitions.size(); ++j )
        {
            checkIfGreatestState( transitions[ j ].d_state );
        }

    }
    
    return removedTrs;
}


bool
SafeTransitionsStore::RenumberStates( const Renumber<lsts_index_t>& ren )
{
    check_claim( initial_state,
                 "SafeTransitionsStore::RenumberStates(): "
                 "no initial state given" );
    
    // Setting initial_state always to number 1:
    renumber.AddNumber( initial_state );
       
    for ( unsigned i = 0; i < transitions.size(); ++i )
    {
        renumber.AddNumber( transitions[ i ].s_state );
        renumber.AddNumber( transitions[ i ].d_state );
    }

    // Giving new state numbers to the transitions:
    for ( unsigned i = 0; i < transitions.size(); ++i )
    {
        transitions[ i ].s_state =
            renumber.newNumber( transitions[ i ].s_state );
        transitions[ i ].d_state =
            renumber.newNumber( transitions[ i ].d_state );
        transitions[ i ].action =
            ren.newNumber( transitions[ i ].action );
    }

    std::sort( transitions.begin(), transitions.end() );

    return renumber.hasNumberingChanged();
}



inline void
SafeTransitionsStore::checkIfGreatestState( lsts_index_t state )
{
    if ( state > greatest_state )
    {
        greatest_state = state;
    }
}

lsts_index_t
SafeTransitionsStore::GiveGreatestState() const
{ return greatest_state; }

lsts_index_t
SafeTransitionsStore::GiveGreatestAction() const
{ return greatest_action; }

lsts_index_t
SafeTransitionsStore::GiveTransitionCnt() const
{ return transitions.size(); }
