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

Contributor(s): Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: $Source: Src/lsts2ag/dfsiter.cc $
// FILE_DES: $Id: dfsiter.cc 1.3 Fri, 27 Feb 2004 16:36:58 +0200 warp $

// 
// 
// 

#include "dfsiter.hh"
#ifndef MAKEDEPEND
//#include <>
#endif
//#include ""

DFS_Iterator::DFS_Iterator( Automaton& lsts, lsts_index_t startState )
    : a_lsts(lsts),
      isInStack(lsts.cntStates()),
      isOld(lsts.cntStates())
{
    travelStack.reserve( lsts.cntStates()+1 );
    // Note: No more than one state can be in stack twice
    currentTransition.reserve( lsts.cntStates()+1 );

    recursiveCall( startState );
}
      
DFS_Iterator::~DFS_Iterator()
{
}

lsts_index_t
DFS_Iterator::operator *()
{
    return stackTop();
}

DFS_Iterator&
DFS_Iterator::operator ++()
{
    step();
    return *this;
}

bool
DFS_Iterator::isValid() const
{
    return (travelStack.size() > 0);
}

bool
DFS_Iterator::isLoop() const
{
    return isInStack[stackTop()];
}

bool
DFS_Iterator::isVisited() const
{
    return isVisited(stackTop());
}

bool
DFS_Iterator::isVisited(lsts_index_t state) const
{
    return (isInStack[state] || isOld[state]) ;
}


// ======================================================================
// Private methods:

lsts_index_t
DFS_Iterator::stackTop() const
{
    return travelStack.back();
}

void
DFS_Iterator::recursiveCall(lsts_index_t state)
{
    if( isValid() ) {
	isInStack.setBit( stackTop() );
    }

    travelStack.push_back( state );
    if( isVisited( state ) ) {
	currentTransition.push_back( a_lsts.endInvis( state ) );
    } else {
	currentTransition.push_back( a_lsts.beginInvis( state ) );
    }
}

void
DFS_Iterator::returnFromRecursion()
{
    for( pop_stack(); isValid() ; pop_stack() ) {
	++(currentTransition.back());
	if( currentTransition.back() != a_lsts.endInvis( stackTop() ) ) {
	    recursiveCall( destination( currentTransition.back() ) );
	    break;
	}
    }
}

void
DFS_Iterator::step()
{
    if( isValid() ) {
	if( currentTransition.back() == a_lsts.endInvis( stackTop() ) ) {
	    returnFromRecursion();
	} else {
	    recursiveCall( destination( currentTransition.back() ) );
	}
    }
}

void
DFS_Iterator::pop_stack()
{
    isOld.setBit( stackTop() );
    isInStack.setBit( stackTop(), false );
    travelStack.pop_back();
    currentTransition.pop_back();
}


// Local variables:
// mode: c++
// mode: font-lock
// compile-command: "g++ -W -Wall -pedantic -c dfsiter.cc"
// End:
