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

// FILE_DES: StatePropsChecker.cc: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: StatePropsChecker.cc 1.15 Wed, 18 Aug 2004 19:30:57 +0300 timoe $";
// 
// The implementation of StatePropsChecker class.
//

// $Log:$


#include "StatePropsChecker.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/FileFormat.hh"
#include "specialStateProps.hh"

using std::string;


// PUBLIC:
#define DELETE_STRING_SET  delete string_set; string_set = 0

#define CREATE_STRING_SET  if( string_set ){ DELETE_STRING_SET; }\
                               string_set = new StringSet

StatePropsChecker::StatePropsChecker( iStatePropsAP& ap, FileFormat& ff ) :
    
    iStatePropsAP(), Checker( ff ),
    state_cnt( 0 ),
    state_prop_cnt( 0 ),

    actual_number( 0 ),

    cut_states_passed ( 0 ),
    rej_states_passed( 0 ),
    dl_rej_states_passed( 0 ),
    ll_rej_states_passed( 0 ),
    inf_rej_states_passed( 0 ),

    special_props_passed( SP::SIZE ),

    AP( ap ),

    string_set( 0 )
{ }

StatePropsChecker::~StatePropsChecker() { DELETE_STRING_SET; }


#define check_interval( m_prop ) \
  check_state( subrange_start, state_cnt,\
               "the beginning of the " m_prop "state interval" ); \
  check_state( subrange_end, state_cnt, \
               "the end of the " m_prop "state interval" )


// PRIVATE:
    

void
StatePropsChecker::lsts_StartStateProps( Header& hd )
{
    check_isGiven( "State_prop_cnt", hd.hasStatePropCnt() );
    check_isGiven( "State_cnt", hd.isStateCntGiven() );
    
    state_prop_cnt = hd.GiveStatePropCnt();
    state_cnt = hd.GiveStateCnt();
    actual_number = 0;

    cut_states_passed = 0;
    rej_states_passed = 0;
    dl_rej_states_passed = 0;
    ll_rej_states_passed = 0;
    inf_rej_states_passed = 0;

    CREATE_STRING_SET;
    
    AP.lsts_StartStateProps( hd );
}

void
StatePropsChecker::lsts_EndStateProps()
{
    actual_number +=
        cut_states_passed +
        rej_states_passed +
        dl_rej_states_passed +
        ll_rej_states_passed +
        inf_rej_states_passed;

    check_amount( actual_number, state_prop_cnt, "state propositions",
                  "State_prop_cnt" );

    DELETE_STRING_SET;
    
    AP.lsts_EndStateProps();
}

inline void
StatePropsChecker::AddActualNumber()
{
    ++actual_number;
    check_claim( actual_number <= state_prop_cnt,
                 ts().errorMsg( "the amount of state propositions "
                                "is greater than State_prop_cnt (",
                                state_prop_cnt,
                                ")" ) );
}

void
StatePropsChecker::lsts_StartPropStates( const string& prop_name )
{
    check_claim( prop_name.size(), "an empty proposition name" );
    checkBeginProp( prop_name, "lsts_StartPropStates" );
    AddActualNumber();

    AP.lsts_StartPropStates( prop_name );
}

void
StatePropsChecker::lsts_PropState( lsts_index_t state )
{
    check_state( state, state_cnt, "state number" );

    AP.lsts_PropState( state );
}

void
StatePropsChecker::lsts_PropStates( lsts_index_t subrange_start,
                                    lsts_index_t subrange_end )
{
    check_interval( "" );
    AP.lsts_PropStates( subrange_start, subrange_end );
}

void
StatePropsChecker::lsts_EndPropStates( const string& prop_name )
{
    checkEndProp( prop_name, "lsts_EndPropStates" );

    AP.lsts_EndPropStates( prop_name );
}


void
StatePropsChecker::checkBeginProp( const string& name, const string& m_name )
{
    check_claim( curr_prop.empty(),
                 ts().errorMsg( m_name + "( '" + name + "' ): "
                 "the delivering of state data of '" ) +
                   curr_prop + "' has not ended" );

    check_claim( string_set->add( name ),
                 ts().errorMsg( name,
                 "the second occurence of the same proposition" ) );

    curr_prop = name;
}

void
StatePropsChecker::checkEndProp( const string& name, const string& m_name )
{
    check_claim( curr_prop.size(),
                 ts().errorMsg( m_name + "( '" + name + "' ): "
                 "stray call, lsts_Start..() must be called first" ) );

    check_claim( name == curr_prop,
                 ts().errorMsg( m_name + "( '" + name + "' ): "
                 "the delivering of state data of '" ) +
                 curr_prop + "' has not ended" );

    curr_prop.erase();
}


#define SpecialProp( m_name, name_str, var ) \
void \
StatePropsChecker::m_name( lsts_index_t state ) \
{ \
    check_state( state, state_cnt, name_str " state number" ); \
    AP.m_name( state ); \
    var##_states_passed = 1; \
} \
void \
StatePropsChecker::m_name##s( lsts_index_t subrange_start, \
                              lsts_index_t subrange_end ) \
{ \
    check_interval( name_str " " ); \
    AP.m_name##s( subrange_start, subrange_end ); \
    var##_states_passed = 1; \
}


SpecialProp( lsts_CutState, "cut", cut )
SpecialProp( lsts_RejectState, "reject", rej )
SpecialProp( lsts_DeadlockRejectState, "deadlock reject", dl_rej )
SpecialProp( lsts_LivelockRejectState, "livelock reject", ll_rej )
SpecialProp( lsts_InfinityRejectState, "inifinity reject", inf_rej )


void
StatePropsChecker::CheckOutSpecialProps( SP::StatePropType type,
                                         const string& name )
{
    bool& passed_already = special_props_passed[ type ].value;
    
    check_claim( !passed_already,
                 ts().errorMsg(
                   name.substr( SP::prefix_length,
                                name.size() - SP::prefix_length -
                                SP::postfix_length ),
                 "the second occurence of the same special proposition" ) );

    passed_already = true;

    AP.CheckOutSpecialProps( type, name );
}
