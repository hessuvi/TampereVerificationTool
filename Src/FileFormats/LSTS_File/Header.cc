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

// FILE_DES: Header.cc: FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

static const char * const ModuleVersion=
"Module version: $Id: Header.cc 1.8 Wed, 18 Aug 2004 19:30:57 +0300 timoe $";
// 
// The implementation of Header class.
//

// $Log: Header.cc,v $
// Revision 1.1  1999/08/16 04:40:58  timoe
// Initial revision
//

#include "Header.hh"
#include "error_handling.hh"
#include "FileFormat/Lexical/charconv.hh"


Header::Header() :

    state_cnt_given( false ),
    transition_cnt_given( false ),
    action_cnt_given( false ),
    state_prop_cnt_given( false ),

    initially_unstable_given( false ),
    interrupted_given( false ),
  
    state_cnt( 0 ),
    transition_cnt( 0 ),
    action_cnt( 0 ),
    elusive_action_cnt( 0 ),
    state_prop_cnt( 0 ),
  
    initially_unstable( false ),

    otf_error( NONE ),
    otf_error_state( 0 ),
    guard( 0 )

{ }

Header::~Header() { }


// The following methods all return true if the data member they refer
//   is given and false if it isn't.
// ##########################################################################
// 

#define isAttrGiven( m_name, var_name ) \
 bool Header::m_name() const { return var_name; }


isAttrGiven( isStateCntGiven, state_cnt_given )
isAttrGiven( isTransitionCntGiven, transition_cnt_given )
isAttrGiven( isActionCntGiven, action_cnt_given )
isAttrGiven( hasStatePropCnt, state_prop_cnt_given )

isAttrGiven( isInitiallyUnstableGiven, initially_unstable_given )

#define GiveAttr( ret_type, m_name, var_name ) \
 ret_type Header::m_name() const { return var_name; }

GiveAttr( bool, isInitiallyUnstable, initially_unstable )


#define GivePromise( ret_type, m_name, var_name ) \
 ret_type Header::m_name() const { return hasPromise( var_name ); }

GivePromise( bool, hasNoLongTauLoops, "no_long_tau_loops" )
GivePromise( bool, hasShallowTaus, "shallow_taus" )
GivePromise( bool, hasNoTaus, "no_taus" )
GivePromise( bool, isDeterministic, "deterministic" )
GivePromise( bool, isNormal, "normal" )


#define SetAttr( m_name, par_type, var_name, var_given_name ) \
 void Header::m_name( par_type x ) { var_name = x; var_given_name = true; }


SetAttr( SetStateCnt, lsts_index_t, state_cnt, state_cnt_given )
SetAttr( SetTransitionCnt, lsts_index_t, transition_cnt,
         transition_cnt_given )
SetAttr( SetActionCnt, lsts_index_t, action_cnt, action_cnt_given )
SetAttr( SetStatePropCnt, lsts_index_t, state_prop_cnt, state_prop_cnt_given )

SetAttr( SetInitiallyUnstable, bool, initially_unstable,
         initially_unstable_given )

#define SetPromise( m_name, var_name ) \
 void Header::m_name( bool b ) \
 { if ( b ) { AddPromise( var_name ); } else { RemovePromise( var_name ); } }

SetPromise( SetNoLongTauLoops, "no_long_tau_loops" )
SetPromise( SetShallowTaus, "shallow_taus" )
SetPromise( SetNoTaus, "no_taus" )
SetPromise( SetDeterministic, "deterministic" )
SetPromise( SetNormal, "normal" )

bool
Header::hasSemantics( const std::string& sem ) const
{
    std::string str = sem;
    ToUpperCase( str );

    for ( unsigned i = 0; i < semantics.size(); ++i )
    { if ( str == semantics[ i ] ) { return true; } }

    return false;
}

const std::vector<std::string>&
Header::GiveSemantics() const
{ return semantics; }

void
Header::AddSemantics( const std::string& new_semantics )
{
    std::string str = new_semantics;
    ToUpperCase( str );

    semantics.push_back( str );
}

bool
Header::hasPromise( const std::string& promise ) const
{
    std::string str = promise;
    ToUpperCase( str );

    for ( unsigned i = 0; i < promises.size(); ++i )
    { if ( str == promises[ i ] ) { return true; } }

    return false;
}

const std::vector<std::string>&
Header::GivePromises() const
{ return promises; }

void
Header::AddPromise( const std::string& new_promise )
{
    std::string str = new_promise;
    ToUpperCase( str );

    promises.push_back( str );
}

bool
Header::RemovePromise( const std::string& promise )
{
    if ( promises.empty() ) { return false; }

    std::string str = promise;
    ToUpperCase( str );
    
    for ( unsigned i = 0; i < promises.size(); ++i )
    {
        if ( str == promises[i] )
        {
            promises[i] = promises.back();
            promises.pop_back();
            return true;
        }
    }
    
    return false;
}
