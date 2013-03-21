/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#include "AccSetsChecker.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/FileFormat.hh"

#include "Interval.hh"


// PUBLIC:

AccSetsChecker::AccSetsChecker( iAccSetsAP& ap, FileFormat& ff ) :
    iAccSetsAP(), Checker( ff ),
    state_cnt( 0 ),
    action_cnt( 0 ),
    exp_state( 0 ),

    AP( ap )
{ }

AccSetsChecker::~AccSetsChecker() { }


// PRIVATE:


void
AccSetsChecker::lsts_StartAccSets( Header& hd )
{
    check_isGiven( "State_cnt", hd.isStateCntGiven() );
    check_isGiven( "Action_cnt", hd.isActionCntGiven() );

    action_cnt = hd.GiveActionCnt();
    state_cnt = hd.GiveStateCnt();

    AP.lsts_StartAccSets( hd );
}

void
AccSetsChecker::lsts_StartAccSetsOfState( lsts_index_t state )
{
    check_state( state, state_cnt, "state number" );
    exp_state = state;

    AP.lsts_StartAccSetsOfState( state );
}
    
void
AccSetsChecker::lsts_StartSingleAccSet( lsts_index_t state )
{
    isStateSame( state, exp_state, "lsts_StartSingleAccSet" );

    AP.lsts_StartSingleAccSet( state );
}


void
AccSetsChecker::lsts_AccSetAction( lsts_index_t state, lsts_index_t action )
{
    isStateSame( state, exp_state, "lsts_AccSetAction" );
    check_visible_action( action, action_cnt, "action number" );

    AP.lsts_AccSetAction( state, action );
}

void
AccSetsChecker::lsts_AccSetActions( lsts_index_t state,
                                    lsts_index_t interv_start,
                                    lsts_index_t interv_end )
{
    check_interval( "action", action_cnt, interv_start, interv_end );
    AP.lsts_AccSetActions( state, interv_start, interv_end );
}
   
void
AccSetsChecker::lsts_EndSingleAccSet( lsts_index_t state )
{
    isStateSame( state, exp_state, "lsts_EndSingleAccSet" );

    AP.lsts_EndSingleAccSet( state );
}

void
AccSetsChecker::lsts_EndAccSetsOfState( lsts_index_t state )
{
    isStateSame( state, exp_state, "lsts_EndAccSetsOfState" );

    AP.lsts_EndAccSetsOfState( state );
}

void
AccSetsChecker::lsts_EndAccSets()
{
    AP.lsts_EndAccSets();
}


inline void
AccSetsChecker::isStateSame( lsts_index_t givenState, lsts_index_t expState,
                             const std::string& methodName )
{ 
    check_claim( givenState == expState,
                 ts().errorMsg(
                     Exs::catString( methodName + "(): bad state "
                                     "number (", givenState, ", " ) +
                     "should be ", expState, ")" ) );
}
