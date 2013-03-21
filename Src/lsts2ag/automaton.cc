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

// FILE_DES: $Source: Src/lsts2ag/automaton.cc $
// FILE_DES: $Id: automaton.cc 1.13 Fri, 27 Feb 2004 16:36:58 +0200 warp $

// 
// 
// 

#include "automaton.hh"
#ifndef MAKEDEPEND
#endif
#include "dfsiter.hh"
#include "LSTS_File/iLSTS_File.hh"
#include "LSTS_File/oLSTS_File.hh"


void State::addTransitionsToAG( VisibleIterator start, VisibleIterator end)
{
    for( VisibleIterator cur = start; cur != end ; ++cur ) {
	VisibleIterator place = lower_bound( a_AG_trans.begin(),
					     a_AG_trans.end(),
					     *cur );
	if( place == a_AG_trans.end() || *cur < *place  ) {
	    a_AG_trans.insert( place, *cur );
	}
    }
}



Automaton::Automaton()
{
}

Automaton::~Automaton()
{
}

void Automaton::lsts_StartDivBits( Header&) {}
void Automaton::lsts_DivBit( lsts_index_t state) {
    a_divergence[state-1] = true;
}
void Automaton::lsts_EndDivBits( ) {}
bool Automaton::lsts_doWeWriteDivBits() { return true; } 
void Automaton::lsts_WriteDivBits( iDivBitsAP& pipe ) {
    // pipe.lsts_StartDivBits( cntStates() );
    for ( lsts_index_t idx = 0 ; idx < cntStates() ; ++idx ) {
	if ( a_AG_State[idx] && a_divergence[idx] ) {
	    pipe.lsts_DivBit( a_states[idx].agStateNumber() );
	    }
	}
    // pipe.lsts_EndDivBits();
}

Header head;

void
Automaton::setupForRead( iLSTS_File& input )
{
    head = input.GiveHeader();
    
    a_divergence.resize(0);
    a_divergence.insert(a_divergence.end(), head.GiveStateCnt(), false );
    a_stability.resize(0);
    a_stability.insert(a_stability.end(), head.GiveStateCnt(), true );
    a_AG_State.resize(0);
    a_AG_State.insert( a_AG_State.end(), head.GiveStateCnt(), false );
    a_states.resize(head.GiveStateCnt());
    a_initialState = head.GiveInitialState()-1;
    a_AG_State[a_initialState]=true;
    a_accsets.setMaxVal( head.GiveActionCnt() );
    
    input.AddActionNamesReader( a_alphabet );
    input.AddDivBitsReader( *this );
    input.AddAccSetsReader( a_accsets );


    // input.AddStatePropNamesReader( a_statepropname_store ); OBSOLETE! --Timo 5.7.01


    input.AddStatePropsReader( a_statePropositions );
    input.AddTransitionsReader( *this );
}

void
Automaton::WriteLSTS( oLSTS_File& output,  TvtCLP& options )
{
    a_AG_stateCnt = renumber();
    a_AG_transCnt = countAGTransitions();

    //StatePropsContainer tempContainer( a_AG_stateCnt ); // hmm?
    StatePropsContainer tempContainer( a_statePropositions.getMaxVal() );
    for(unsigned i = 1; i<= a_statePropositions.getMaxStatePropNameNumber();++i)
    {
	tempContainer.getStatePropName(i) = 
	    a_statePropositions.getStatePropName(i);
    }
    for( lsts_index_t  idx = 0; idx < cntStates(); ++idx )
    {
	StatePropsContainer::StatePropsPtr currStateProp =
		a_statePropositions.getStateProps( idx+1 );
	if ( a_AG_State[idx] ) {
	    lsts_index_t new_number = a_states[idx].agStateNumber();
	    tempContainer.assignStateProps( new_number , currStateProp );
	}
    }

    head.SetStateCnt( a_AG_stateCnt );
    head.SetTransitionCnt( a_AG_transCnt );
    head.SetNoTaus( true );
    head.SetInitialState( a_states[initialState()].agStateNumber() );
    head.SetInitiallyUnstable( !a_stability[initialState()] );
    output.AddActionNamesWriter( a_alphabet );
    output.AddDivBitsWriter( *this );
    output.AddAccSetsWriter( *this );

    // output.AddStatePropNamesWriter( a_statepropname_store ); OBSOLETE! --Timo 5.7.01

    output.AddStatePropsWriter( tempContainer );
    output.AddTransitionsWriter( *this );

    output.GiveHeader() = head;
    output.WriteFile( options.getOutputStream() );
}

lsts_index_t
Automaton::renumber()
{
    lsts_index_t nextNum = 0; 
    for ( lsts_index_t  idx = 0; idx < cntStates(); ++idx ) {
	if ( a_AG_State[idx] ) {
	    a_states[idx].setAgStateNumber( ++nextNum );
	    }
	}
    return nextNum;
}

lsts_index_t
Automaton::countAGTransitions() 
{
    lsts_index_t cnt = 0;
    for ( lsts_index_t  idx = 0; idx < cntStates(); ++idx ) {
	if ( a_AG_State[idx] ) {
	    cnt += (a_states[idx].endAGTrans() - a_states[idx].beginAGTrans());
	    }
	}
    return cnt;
}

lsts_index_t
Automaton::cntStates() const
{
    return a_states.size();
}

lsts_index_t
Automaton::initialState() const
{
    return a_initialState;
}

InvisIterator
Automaton::beginInvis( lsts_index_t fromState )
{
    return a_states[fromState].beginInvis();
}

InvisIterator
Automaton::endInvis( lsts_index_t fromState )
{
    return a_states[fromState].endInvis();
}

VisibleIterator
Automaton::beginVis( lsts_index_t fromState )
{
    return a_states[fromState].beginVis();
}

VisibleIterator
Automaton::endVis( lsts_index_t fromState )
{
    return a_states[fromState].endVis();
}


lsts_index_t
Automaton::firstReserveState() const
{
    lsts_index_t current = 0;
    if( a_AG_State[current] ) return current;
    else return nextReserveState( current );
}

lsts_index_t
Automaton::nextReserveState( lsts_index_t current ) const
{
    for ( ++current ; current < cntStates() ; ++current ) {
	if( a_AG_State[current] ) {
	    return current;
	}
    }
    return cntStates();
}

bool
Automaton::getDivBit( lsts_index_t state ) const
{
    return a_divergence[state];
}

void
Automaton::setDivBit( lsts_index_t state, bool value )
{
    a_divergence[state] = value;
}

bool
Automaton::isStable( lsts_index_t state ) const
{
    return a_stability[state];
}


void
Automaton::removeInvisTransitions( lsts_index_t state )
{
    a_states[state].removeInvis();
}

void
Automaton::copyTransitions( lsts_index_t fromState,
			    lsts_index_t toState )
{
    a_states[toState].addTransitionsToAG( a_states[fromState].beginVis(),
					  a_states[fromState].endVis());
    if( fromState != toState ) {
	a_states[toState].addTransitionsToAG(
	    a_states[fromState].beginAGTrans(),
	    a_states[fromState].endAGTrans() );
	}
}

AccSets&
Automaton::getAccsets( lsts_index_t state )
{
    return a_accsets.getAccSets( state + 1 );
}

StatePropsContainer::StatePropsPtr
Automaton::getStateProps( lsts_index_t state )
{
 if( ! a_statePropositions.isInitialized() ) {
     return a_statePropositions.getEmptyStatePropsPtr();
     }
 else {
     return a_statePropositions.getStateProps( state + 1 );
     }
}

void
Automaton::addAccSet( VisibleIterator start,
		      VisibleIterator end,
		      lsts_index_t toState )
{
    AccSets& to=getAccsets( toState );
    to.createNewAccSet();
    for ( VisibleIterator cur = start; cur != end; ++cur ) {
	to.addActionToNewAccSet( action( cur ) );
    }
    to.addNewAccSet();
}

void
Automaton::copyAccSetsAndDivBit( lsts_index_t fromState,
				 lsts_index_t toState )
{
    if(  fromState != toState ) {
	bool bitVal = getDivBit(fromState) || getDivBit( toState );
	setDivBit( toState, bitVal );
	AccSets& to=getAccsets( toState );
	AccSets& from=getAccsets( fromState );
	to.addAccSets( from );
    }
}


void
Automaton::lsts_StartTransitions( Header& )
{
}
    
void
Automaton::lsts_StartTransitionsFromState( lsts_index_t )
{
}

bool
Automaton::isInvisible( lsts_index_t from,
			lsts_index_t action,
			lsts_index_t to ) {
    return
      ( action == TAU_ACTION )
      && ( getStateProps(from) == getStateProps(to) );
}

   
void
Automaton::lsts_Transition( lsts_index_t start_state,
			    lsts_index_t dest_state,
			    lsts_index_t action )
{
    --start_state;
    --dest_state;
    if ( isInvisible( start_state, action, dest_state ) ) {
	a_states[start_state].addInvisble(dest_state);
	}
    else {
	a_states[start_state].addVisible(action,dest_state);
	a_AG_State[dest_state] = true;
	}
    if( action == TAU_ACTION ) {
	a_stability[start_state] = false;
	}
}
    
void
Automaton::lsts_EndTransitionsFromState( lsts_index_t state)
{
    --state;
    a_states[state].sortVisible();
}
    
void
Automaton::lsts_EndTransitions()
{
}

lsts_index_t
Automaton::lsts_numberOfTransitionsToWrite()
{
     return a_AG_transCnt;
}

void
Automaton::writeTransitions( lsts_index_t state, iTransitionsAP& pipe ) {
    lsts_index_t source = a_states[state].agStateNumber();
    VisibleIterator start = a_states[state].beginAGTrans();
    VisibleIterator end = a_states[state].endAGTrans();
    pipe.lsts_StartTransitionsFromState( source );

    for( VisibleIterator cur = start; cur != end; ++cur ) {
	lsts_index_t dest = a_states[destination(cur)].agStateNumber();
	pipe.lsts_Transition( source, dest, action(cur) );
	}
    pipe.lsts_EndTransitionsFromState( source );
}		  

void
Automaton::lsts_WriteTransitions( iTransitionsAP& pipe ) {
    // pipe.lsts_StartTransitions( a_AG_transCnt );
    for ( lsts_index_t  idx = 0; idx < cntStates(); ++idx ) {
	if ( a_AG_State[idx] ) {
	    writeTransitions( idx, pipe );
	    }
	}
    // pipe.lsts_EndTransitions();
}


/*  OBSOLETE! --Timo 5.7.01
bool
Automaton::lsts_separateOTFVI()
{
    return false; 
}

*/


/*bool
Automaton::lsts_doWeWriteStateProps()
{
     return a_statePropositions.isInitialized();
}

void
Automaton::lsts_WriteStateProps( StatePropsAP& pipe )
{
    // pipe.lsts_StartStateProps( StatePropsContainer vaatii metodin);
    for ( lsts_index_t  idx = 0; idx < cntStates(); ++idx ) {
	if ( a_AG_State[idx] ) {
	    a_statePropositions.writeStateProps(
		getStateProps( idx ),
		a_states[idx].agStateNumber(),
		pipe
		);
	    }
	}
    // pipe.lsts_EndStateProps();
}*/


bool
Automaton::lsts_doWeWriteAccSets()
{
     return true;
}
  
void
Automaton::lsts_WriteAccSets( iAccSetsAP& pipe )
{
     // pipe.lsts_StartAccSets( Johonkin tarvii lisätä );
     for ( lsts_index_t  idx = 0; idx < cntStates(); ++idx ) {
	 if ( a_AG_State[idx] ) {
	    getAccsets( idx ).writeAccSets(
		a_states[idx].agStateNumber(),
		pipe
		);
	    }
	}
     // pipe.lsts_EndAccSets();
}


void constructAG( Automaton& lsts )
{
    for ( lsts_index_t start = lsts.firstReserveState() ;
	  start < lsts.cntStates() ;
	  start = lsts.nextReserveState( start ) ) {
	for ( DFS_Iterator end( lsts, start ); end.isValid(); ++end ) {
	    if  ( end.isLoop() ) {
		lsts.setDivBit( start ); }
	    else if ( ! end.isVisited() ) {
		lsts.copyAccSetsAndDivBit( *end, start );
		if  ( lsts.isStable( *end ) ) {
		    lsts.addAccSet( lsts.beginVis( *end ),
				    lsts.endVis( *end ),
				    start ); }
		lsts.copyTransitions( *end, start );
	    }
	}
	lsts.removeInvisTransitions( start );
    }
}
		    

// Local variables:
// mode: c++
// mode: font-lock
// compile-command: "gmake automaton.o"
// End:
