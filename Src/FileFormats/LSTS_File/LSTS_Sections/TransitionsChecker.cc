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

#include "TransitionsChecker.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/FileFormat.hh"

using namespace std;


// PUBLIC:

TransitionsChecker::TransitionsChecker( iTransitionsAP& ia,
                                        FileFormat& ff ) :
    Checker( ff ), iTransitionsAP(),
    action_cnt( 0 ),
    state_cnt( 0 ),
    transition_cnt( 0 ),
    amount( 0 ),

    iap( ia ) { }

TransitionsChecker::~TransitionsChecker() { }


// PRIVATE:
void
TransitionsChecker::lsts_StartTransitions( Header& hd )
{
    check_isGiven( "State_cnt", hd.isStateCntGiven() );
    check_isGiven( "Action_cnt", hd.isActionCntGiven() );
    check_isGiven( "Transition_cnt", hd.isTransitionCntGiven() );
 
    action_cnt = hd.GiveActionCnt();
    state_cnt = hd.GiveStateCnt();
    transition_cnt = hd.GiveTransitionCnt();
    amount = 0;

    start_states_checked.resize( transition_cnt + 1 );

    for( unsigned i = 0; i < start_states_checked.size(); ++i )
    { start_states_checked[ i ] = false; }

    iap.lsts_StartTransitions( hd );
}

void
TransitionsChecker::lsts_EndTransitions()
{
   check_amount( amount, transition_cnt, "transitions", "Transition_cnt" );
   start_states_checked.clear();

   iap.lsts_EndTransitions();
}


void
TransitionsChecker::lsts_StartTransitionsFromState( lsts_index_t start_state )
{
    check_state( start_state, state_cnt, "state" );

    if ( start_state < start_states_checked.size() )
    {
        check_claim(  !start_states_checked[ start_state ],
                      ts().errorMsg(
                     "can't give transitions starting from state ",
                     start_state,
                     " in several different points of the section" ) );
      
        start_states_checked[ start_state ] = true;
    }

    iap.lsts_StartTransitionsFromState( start_state );
}

inline void
TransitionsChecker::CheckTransition( lsts_index_t /*start_state*/,
                                     lsts_index_t dest_state,
                                     lsts_index_t action )
{
    check_action( action, action_cnt, "action number" );
    check_state( dest_state, state_cnt, "destination state" );
    ++amount;
    
    check_claim( amount <= transition_cnt,
                 ts().errorMsg( "the amount of transitions "
                                "is greater than transition_cnt (",
                                transition_cnt, ")" ) );
}

void
TransitionsChecker::lsts_Transition( lsts_index_t start_state,
                                     lsts_index_t dest_state,
                                     lsts_index_t action )
{
    CheckTransition( start_state, dest_state, action );
    iap.lsts_Transition( start_state, dest_state, action );
}

void
TransitionsChecker::lsts_TransitionWithExtraData
( lsts_index_t start_state, lsts_index_t dest_state,
  lsts_index_t action, const std::string& extra_data )
{
    CheckTransition( start_state, dest_state, action );
    iap.lsts_TransitionWithExtraData( start_state, dest_state, action,
                                      extra_data );
}

void
TransitionsChecker::lsts_TransitionWithLayoutInfo
( lsts_index_t start_state, lsts_index_t dest_state,
  lsts_index_t action, const std::string& layout_info )
{
    CheckTransition( start_state, dest_state, action );
    iap.lsts_TransitionWithLayoutInfo( start_state, dest_state, action,
                                       layout_info );
}


void
TransitionsChecker::lsts_TransitionWithExtraDataAndLayoutInfo
( lsts_index_t start_state, lsts_index_t dest_state,
  lsts_index_t action, const std::string& extra_data,
  const std::string& layout_info )
{
    CheckTransition( start_state, dest_state, action );
    iap.lsts_TransitionWithExtraDataAndLayoutInfo( start_state, dest_state,
                                                   action, extra_data,
                                                   layout_info );
}


#ifdef RATE
void
TransitionsChecker::lsts_Transition( lsts_index_t start_state,
                                     lsts_index_t dest_state,
                                     lsts_index_t action,
                                     lsts_float_t fl )
{
    check_action( action, action_cnt, "action number" );
    check_state( dest_state, state_cnt, "destination state" );

    ++amount;

    check_claim( amount <= transition_cnt,
                 ts().errorMsg( "the amount of transitions "
                                "is greater than transition_cnt (",
                                transition_cnt,
                                ")" ) );

    iap.lsts_Transition( start_state, dest_state, action, fl );
}
#endif


void
TransitionsChecker::lsts_EndTransitionsFromState( lsts_index_t start_state )
{ iap.lsts_EndTransitionsFromState( start_state ); }
