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

// FILE_DES: $Source: Src/lsts2ag/automaton.hh $
// FILE_DES: $Id: automaton.hh 1.11 Fri, 27 Feb 2004 16:36:58 +0200 warp $


#ifdef CIRC_AUTOMATON_HH
#error Recursive include
#endif
#ifndef ONCE_AUTOMATON_HH
#define ONCE_AUTOMATON_HH
#define CIRC_AUTOMATON_HH

#ifndef MAKEDEPEND
#include <vector>
#include <algorithm>
using namespace std;
#endif
#include "config.hh"
#include "StateProps.hh"
#include "AccSets.hh"
#include "LSTS_File/ActionNamesStore.hh"
#include "LSTS_File/DivBitsAP.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "TvtCLP.hh"

// #include "LSTS_File/StatePropNamesStore.hh"  OBSOLETE! --Timo 5.7.01


typedef vector<bool> BoolVector;

class Transition
{
 public:
    Transition( lsts_index_t act, lsts_index_t dest)
	:a_action(act), a_destination( dest ) {};
    ~Transition() {};

    lsts_index_t action() { return a_action; };
    lsts_index_t destination() { return a_destination; };

    bool operator == ( const Transition& other ) const {
	return a_action == other.a_action
	    && a_destination == other.a_destination; }

    bool operator < ( const Transition& other ) const {
	if( a_action == other.a_action ) {
	    return (a_destination < other.a_destination) ;
	} else {
	    return (a_action < other.a_action);
	}
    };

 private:
    lsts_index_t a_action;
    lsts_index_t a_destination;
};


class InvisibleTransition
{
 public:
    InvisibleTransition( lsts_index_t dest)
	:a_destination( dest ) {};
    ~InvisibleTransition() {};

    lsts_index_t action() { return TAU_ACTION; };
    lsts_index_t destination() { return a_destination; };

 private:
    lsts_index_t a_destination;
};

typedef vector<Transition> TransitionContainer;
typedef vector<InvisibleTransition> InvisibleContainer;
typedef InvisibleContainer::iterator InvisIterator;
typedef TransitionContainer::iterator VisibleIterator;

typedef vector<lsts_index_t> IndexVector;
typedef vector<lsts_index_t> StateNameVector;

template <class ITER>
lsts_index_t action( ITER& tr ) {
    return tr->action();
}

template <class ITER>
lsts_index_t destination( ITER& tr ) {
    return tr->destination();
}


class State
{
 public:
    State() {};
    ~State() {};

    lsts_index_t agStateNumber() const {
	return a_AG_state; };

    void setAgStateNumber( lsts_index_t num ) {
	a_AG_state = num; };
	

    InvisIterator beginInvis() {
	return a_internal.begin(); };
    InvisIterator endInvis() {
	return a_internal.end(); };

    void removeInvis() {
	vector<InvisibleTransition> empty;
	a_internal.swap(empty); };
	
    
    VisibleIterator beginVis() {
	return a_visible.begin(); };
    VisibleIterator endVis() {
	return a_visible.end(); };

    VisibleIterator beginAGTrans() {
	return a_AG_trans.begin(); };
    VisibleIterator endAGTrans() {
	return a_AG_trans.end(); };

    void addTransitionsToAG( VisibleIterator start, VisibleIterator end);
    // Precondition: Transitions to be inserted should be sorted.

    void addInvisble( lsts_index_t to ) {
	a_internal.push_back( InvisibleTransition( to ) ); };
	
    void addVisible( lsts_index_t act, lsts_index_t to) {
	a_visible.push_back( Transition( act, to ) ); };

    void sortVisible() {
	sort( a_visible.begin(),a_visible.end()); };

 private:
    lsts_index_t        a_AG_state;
    TransitionContainer a_visible;
    InvisibleContainer  a_internal;
    TransitionContainer a_AG_trans;
    
};

typedef vector<State> StateContainer;
class iLSTS_File;
class oLSTS_File;



class Automaton: public iTransitionsAP,
                 public iDivBitsAP,
                 public oTransitionsAP,
                 public oAccSetsAP,
		 public oDivBitsAP
{
 public:
    Automaton();
    ~Automaton();

    void setupForRead( iLSTS_File& );
    //void setupForWrite( oLSTS_File& );
    void WriteLSTS(  oLSTS_File&, TvtCLP& );
    // Graph access
    lsts_index_t cntStates() const;
    lsts_index_t initialState() const;

    InvisIterator beginInvis( lsts_index_t fromState );
    InvisIterator endInvis( lsts_index_t fromState );

    VisibleIterator beginVis( lsts_index_t fromState );
    VisibleIterator endVis( lsts_index_t fromState );

    AccSets& getAccsets( lsts_index_t state );

    StatePropsContainer::StatePropsPtr
    getStateProps( lsts_index_t state );

    // Acceptance graph construction
    lsts_index_t firstReserveState() const;
    lsts_index_t nextReserveState( lsts_index_t current ) const;
    bool getDivBit( lsts_index_t state ) const;
    void setDivBit( lsts_index_t state, bool value = true );
    bool isStable( lsts_index_t state ) const;

    void removeInvisTransitions( lsts_index_t state );
    void copyTransitions( lsts_index_t fromState,
			  lsts_index_t toState );
    void addAccSet( VisibleIterator start,
		    VisibleIterator end,
		    lsts_index_t toState );

    void copyAccSetsAndDivBit( lsts_index_t fromState,
			       lsts_index_t toState );

    virtual void lsts_StartTransitions( Header& );
    
    virtual void lsts_StartTransitionsFromState( lsts_index_t start_state );
    
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action );
    
    virtual void lsts_EndTransitionsFromState( lsts_index_t start_state );
    
    virtual void lsts_EndTransitions();

    virtual lsts_index_t lsts_numberOfTransitionsToWrite();
    virtual void lsts_WriteTransitions( iTransitionsAP& pipe );

    // virtual bool lsts_separateOTFVI(); OBSOLETE! --Timo 5.7.01

    //virtual bool lsts_doWeWriteStateProps();

    //virtual void lsts_WriteStateProps( StatePropsAP& pipe );

    virtual bool lsts_doWeWriteAccSets();
  
    virtual void lsts_WriteAccSets( iAccSetsAP& pipe );

    virtual void lsts_StartDivBits( Header& );
    
    virtual void lsts_DivBit( lsts_index_t state_number );
    
    virtual void lsts_EndDivBits();

    virtual bool lsts_doWeWriteDivBits();
    virtual void lsts_WriteDivBits( iDivBitsAP& pipe );

 private:

    bool isInvisible( lsts_index_t from,
		      lsts_index_t action,
		      lsts_index_t to );
    void writeTransitions( lsts_index_t stateIdx, iTransitionsAP& pipe);

    lsts_index_t  renumber();
    lsts_index_t  countAGTransitions();

    lsts_index_t        a_initialState;
    StatePropsContainer a_statePropositions; // Pi ja val
    AccSetsContainer    a_accsets;
    ActionNamesStore    a_alphabet;
    //    StatePropNamesStore a_statepropname_store; OBSOLETE! --Timo 5.7.01
    BoolVector          a_divergence;
    BoolVector          a_stability;
  
    BoolVector          a_AG_State;
    StateContainer      a_states;
    lsts_index_t        a_AG_stateCnt;
    lsts_index_t        a_AG_transCnt;
    
};

void constructAG( Automaton& );


#undef CIRC_AUTOMATON_HH
#endif  // ONCE_AUTOMATON_HH
// Local variables:
// mode: c++
// mode: font-lock
// compile-command: "gmake"
// End:
