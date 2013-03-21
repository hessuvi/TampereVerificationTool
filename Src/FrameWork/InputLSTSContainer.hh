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

Contributor(s): Juha Nieminen.
*/

// FILE_DES: InputLSTSContainer.hh: Src
// Nieminen Juha

// $Id: InputLSTSContainer.hh 1.36 Mon, 20 Sep 2004 18:15:42 +0300 warp $
// 
// Geneerinen syöte-lsts-varasto, joka on syötteen lukemisen jälkeen
//   read-only.
// Tämä luokka on tarkoitettu perittäväksi.
//
// InputLSTSContainer:
//     [Metodin nimi]: [Metodin lyhyt kuvaus]
//

// $Log:$

#ifdef CIRC_INPUTLSTSCONTAINER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_INPUTLSTSCONTAINER_HH_
#define ONCE_INPUTLSTSCONTAINER_HH_
#define CIRC_INPUTLSTSCONTAINER_HH_

#include "LSTS_File/iLSTS_File.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "LSTS_File/DivBitsAP.hh"
#include "LSTS_File/ActionNamesStore.hh"
#include "AccSets.hh"
#include "RO_AccSets.hh"
#include "StateProps.hh"

#ifndef MAKEDEPEND
#include <vector>
#include <algorithm>
#include <climits>
#endif

namespace ILC
{
    enum SupportedSections
    {
        ACCSETS = 1,
        DIVBITS = (1<<1),
        STATEPROPS = (1<<2),
        TRANSITIONS = (1<<3)
    };
}

// ------------------------------------------------------------------------
// InputLSTSContainer class:
// ------------------------------------------------------------------------
class InputLSTSContainer: public iTransitionsAP,
                          public iDivBitsAP,
                          public ActionNamesStore
{
    class TransitionsContainer;
    typedef unsigned TransitionsHandle;


 public:
    InputLSTSContainer(unsigned supportedSections,
/***OTFVI***
                       bool separateOTFVI=false,
*/
                       bool strictSorting=false);
    InputLSTSContainer(const InputLSTSContainer&);
    virtual ~InputLSTSContainer();

    // Main LSTS reading method:
    void readLSTSFile(InStream&);

    virtual void ChecksBeforeReadingLSTS(const iLSTS_File&);
    virtual void ChecksAfterReadingLSTS(const iLSTS_File&);

    // Subclass declarations:
    class State;
    class tr_const_iterator;
    class TransitionCompare;

    // State reading methods:
    inline const State& getInitialState() const;
    inline lsts_index_t getInitialStateNumber() const;
    inline lsts_index_t getStateNumber(const State& state) const;
    inline const State& getState(lsts_index_t stateNumber) const;

    // Acceptance sets reading methods:
    inline AccSets getAccSets(const State&) const;
    inline AccSets getAccSets(unsigned stateNumber) const;
    inline RO_AccSets getRO_AccSets(const State&) const;
    inline RO_AccSets getRO_AccSets(unsigned stateNumber) const;
    inline bool accSetsEmpty() const { return data->accsets.isEmpty(); }

    // LSTS header reading:
    inline const Header& getHeaderData() const;

    // Get the state propositions container:
    inline StatePropsContainer& getStateProps();
    inline const StatePropsContainer& getStateProps() const;

    // Get the ActionNamesStore instance:
    //inline ActionNamesStore& getActionNamesStore();
    inline const ActionNamesStore& getActionNamesStore() const;

    // Iterators to the transitions of the given state:
    inline tr_const_iterator tr_begin(const State&) const;
    inline tr_const_iterator tr_end(const State&) const;

    // Searches a transition with the given action number.
    // Returns tr_end() if the transitions was not found.
    // If there are more than one transition with the same action number,
    // it returns an iterator to the first one (the transitions are in
    // increasing order according to the action number).
    inline tr_const_iterator findTransition(const State&,
                                            lsts_index_t trNumber) const;

    // Get an instance of the transitions comparator class:
    inline TransitionCompare getTransitionCompare() const;


// Transition class
// ------------------------------------------------------------------------
    class Transition
    {
     public:
        inline Transition();
        inline Transition(lsts_index_t tn, const State& ds);

        // Transition data:
        inline lsts_index_t getTransitionNumber() const;
        inline const State& getDestinationState() const;

        // Comparison according to the action numbers:
        inline bool operator<(const Transition&) const;
        inline bool operator==(const Transition&) const;

        // Comparison according to action numbers and state propositions of
        // the destination states:
        inline bool
        lessThan(const Transition&, const InputLSTSContainer&) const;
        inline bool equal(const Transition&) const;

     private:
        lsts_index_t a_transitionNumber;
        const State* a_destinationState;
    };

    // Transition comparator for usage in the STL. It compares transitions
    // with their 'lessThan()' method.
    class TransitionCompare
    {
     public:
        inline TransitionCompare(const InputLSTSContainer& i): ilsts(&i) {}

        inline bool operator() (const Transition& tr1, const Transition& tr2)
        { return tr1.lessThan(tr2, *ilsts); }

        inline bool operator() (const Transition*const& tr1,
                                const Transition*const& tr2)
        { return tr1->lessThan(*tr2, *ilsts); }

     private:
        const InputLSTSContainer* ilsts;
    };


// State class
// ------------------------------------------------------------------------
    class State
    {
     public:
        inline State();
        inline ~State();

        inline bool getDivBit() const;
        inline StatePropsContainer::StatePropsPtr getStatePropsPtr() const;
        inline unsigned getTransitionsAmnt() const;

     //-------------------------------------------------------------
     protected:
        friend class InputLSTSContainer;
        TransitionsHandle getTransitionsHandle() const;
        void setTransitionsAmnt(unsigned amnt);
        void startTransitions(TransitionsContainer&);
        void addTransition(lsts_index_t trnumber, const State& dest,
                           TransitionsContainer&);
        void endTransitions(TransitionsContainer&,
                                   const InputLSTSContainer&);
        void setDivBit();

     private:
        TransitionsHandle trHandle;
        unsigned transitionsAmnt; // Highest bit = div
        StatePropsContainer::StatePropsPtr stateprops;
    };


// tr_const_iterator-luokka
// ------------------------------------------------------------------------
    class tr_const_iterator
    {
     public:
        inline tr_const_iterator(): transitions(0), trAmnt(0), index(0) {}

        inline const Transition& operator*() const;
        inline const Transition* operator->() const;
        inline tr_const_iterator& operator++();
        inline bool operator==(const tr_const_iterator&) const;
        inline bool operator!=(const tr_const_iterator&) const;

     //-------------------------------------------------------------
     private:
        friend class InputLSTSContainer;
        inline tr_const_iterator(const TransitionsContainer& transitionsCont,
                                 TransitionsHandle transitionsHandle,
                                 unsigned transitionsAmount,
                                 unsigned transitionsIndex=0):
            transitions(&transitionsCont), trHandle(transitionsHandle),
            trAmnt(transitionsAmount), index(transitionsIndex) {}

        const TransitionsContainer* transitions;
        TransitionsHandle trHandle;
        unsigned trAmnt, index;
    };


    void allowInterruptedLSTSReading();


//=========================================================================
 protected:
//=========================================================================
    // Transitions:
    virtual void lsts_StartTransitions(Header&);
    virtual void lsts_StartTransitionsFromState(lsts_index_t start_state);
    virtual void lsts_Transition(lsts_index_t sState,
                                 lsts_index_t eState,
                                 lsts_index_t trNumber);
    virtual void lsts_EndTransitionsFromState(lsts_index_t start_state);
    virtual void lsts_EndTransitions();

    // Div bits:
    virtual void lsts_StartDivBits(Header&);
    virtual void lsts_DivBit(lsts_index_t state_number);
    virtual void lsts_EndDivBits();

    // Action names
    virtual void lsts_EndActionNames();


//========================================================================
 private:
//========================================================================
    // Transitions container
    // ----------------------
    class TransitionsContainer
    {
     public:
        void setTransitionCount(lsts_index_t cnt);

        TransitionsHandle beginTransitions();
        void addTransition(lsts_index_t number, const State& dest);
        void endTransitions(const InputLSTSContainer&);

        inline const Transition& getTransition(TransitionsHandle handle,
                                               unsigned index) const;

        inline unsigned find(lsts_index_t trNumber,
                             TransitionsHandle handle,
                             unsigned trAmnt) const;

     private:
        std::vector<Transition> transitions;
        TransitionsHandle current;
        std::vector<Transition>::iterator currentGroupBegin, currentGroupEnd;
    };


    // Specialized RO_AccSetsContainer for sorted actions:
    // --------------------------------------------------
    class sortingRO_AccSetsContainer: public RO_AccSetsContainer
    {
        bool sort;
        const std::vector<lsts_index_t>* lookup;
        lsts_index_t maxVal;
        AccSets tmpAccSets;

     public:
        sortingRO_AccSetsContainer
        (bool strictSorting,
         const std::vector<lsts_index_t>* lookupTable):
            sort(strictSorting), lookup(lookupTable)
        {}

     protected:
        void lsts_StartAccSets(Header& h);
        void lsts_StartAccSetsOfState(lsts_index_t state);
        void lsts_StartSingleAccSet(lsts_index_t state);
        void lsts_AccSetAction(lsts_index_t state, lsts_index_t action);
        void lsts_EndSingleAccSet(lsts_index_t state);
        void lsts_EndAccSetsOfState(lsts_index_t state);
        void lsts_EndAccSets();
    };


    // Main data of the LSTS:
    // ---------------------
    struct Data
    {
        unsigned refCnt;
        TransitionsContainer transitions;
        std::vector<State> states;
        sortingRO_AccSetsContainer accsets;
        Header headerData;
        //ActionNamesStore actionnames;
        bool allowInterrupted;

        Data(bool strictSorting, const std::vector<lsts_index_t>* lookupTable):
            accsets(strictSorting, lookupTable),
            allowInterrupted(false)
        {}
    };

    Data* data;
    StatePropsContainer stateprops;

    unsigned sectionsToRead;
    lsts_index_t initialState;
/***OTFVI***
    bool separateOTFVI;
*/
    bool sorted;

    // Temporary data for action name sorting
    // --------------------------------------
    struct ActionName
    {
        unsigned number;
        std::string name;

        ActionName(unsigned num, const std::string& nam):
            number(num), name(nam) {}
        bool operator<(const ActionName& rhs) const { return name < rhs.name; }
    };

    std::vector<lsts_index_t> trNumberLookup;

    lsts_index_t size() const { return ActionNamesStore::size(); }


    // Reference counting:
    // ------------------
    inline void incRefCnt() { ++(data->refCnt); }
    inline void decRefCnt() { if((--(data->refCnt)) == 0) delete data; }

    // Disable assignment
    InputLSTSContainer& operator=(const InputLSTSContainer&);
};













//========================================================================
// Inline implementations:
//========================================================================

// Transition
//------------------------------------------------------------------------
// Constructors
inline InputLSTSContainer::Transition::Transition():
    a_transitionNumber(~0), a_destinationState(0)
{}

inline InputLSTSContainer::Transition::Transition(lsts_index_t tn,
                                                  const State& ds):
    a_transitionNumber(tn), a_destinationState(&ds)
{}

// getTransitionNumber
inline lsts_index_t InputLSTSContainer::Transition::getTransitionNumber() const
{
    return a_transitionNumber;
}

// getDestinationState
inline const InputLSTSContainer::State&
InputLSTSContainer::Transition::getDestinationState() const
{
    return *a_destinationState;
}

// operator<
inline bool
InputLSTSContainer::Transition::operator<(const Transition& tr) const
{
    return a_transitionNumber < tr.a_transitionNumber;
}

// operator==
inline bool
InputLSTSContainer::Transition::operator==(const Transition& tr) const
{
    return a_transitionNumber == tr.a_transitionNumber;
}

// lessThan
inline bool
InputLSTSContainer::Transition::lessThan(const Transition& tr,
                                         const InputLSTSContainer& ilsts) const
{
    return
        (a_transitionNumber < tr.a_transitionNumber) ? true :
        (a_transitionNumber > tr.a_transitionNumber) ? false :

        (a_destinationState->getStatePropsPtr() <
         tr.a_destinationState->getStatePropsPtr()) ? true :
        (tr.a_destinationState->getStatePropsPtr() <
         a_destinationState->getStatePropsPtr()) ? false :

        ilsts.getStateNumber(*a_destinationState) <
        ilsts.getStateNumber(*tr.a_destinationState);
}

// equal
inline bool InputLSTSContainer::Transition::equal(const Transition& tr) const
{
    return
        a_transitionNumber == tr.a_transitionNumber &&
        a_destinationState->getStatePropsPtr() ==
        tr.a_destinationState->getStatePropsPtr();
}









// TransitionsContainer
//------------------------------------------------------------------------
// getTransition
inline const InputLSTSContainer::Transition&
InputLSTSContainer::TransitionsContainer::getTransition(
    TransitionsHandle handle, unsigned index) const
{
    return transitions[handle+index];
}

// find
inline unsigned
InputLSTSContainer::TransitionsContainer::find(lsts_index_t trNumber,
                                               TransitionsHandle handle,
                                               unsigned trAmnt) const
{
    unsigned min = 0, max = trAmnt;
    while(min < max)
    {
        unsigned mid = (min+max)/2;
        if(transitions[mid+handle].getTransitionNumber() < trNumber)
            min = mid+1;
        else
            max = mid;
    }
    return
        min == trAmnt ? trAmnt :
        (transitions[min+handle].getTransitionNumber() == trNumber ?
         min : trAmnt);
}








// tr_const_iterator
//------------------------------------------------------------------------
// Operator*
inline const InputLSTSContainer::Transition&
InputLSTSContainer::tr_const_iterator::operator*() const
{
    static const Transition nullTransition;
    if(index >= trAmnt) return nullTransition;
    return transitions->getTransition(trHandle, index);
}

// Operator->
inline const InputLSTSContainer::Transition*
InputLSTSContainer::tr_const_iterator::operator->() const
{
    static const Transition nullTransition;
    if(index >= trAmnt) return &nullTransition;
    return &transitions->getTransition(trHandle, index);
}

// Operator++
inline InputLSTSContainer::tr_const_iterator&
InputLSTSContainer::tr_const_iterator::operator++()
{
    index++;
    return *this;
}

// Operator==
inline bool
InputLSTSContainer::tr_const_iterator::operator==(const tr_const_iterator& i)
    const
{
    return index == i.index && trHandle == i.trHandle;
}

// Operator!=
inline bool
InputLSTSContainer::tr_const_iterator::operator!=(const tr_const_iterator& i)
    const
{
    return index != i.index || trHandle != i.trHandle;
}








// State
//------------------------------------------------------------------------
// Rakenaja ja purkaja
inline InputLSTSContainer::State::State():
    transitionsAmnt(0)
{}
inline InputLSTSContainer::State::~State()
{}

// getDivBit
inline bool InputLSTSContainer::State::getDivBit() const
{
    return (transitionsAmnt & (1 << (sizeof(unsigned)*CHAR_BIT-1))) != 0;
}

// getStatePropsPtr
inline StatePropsContainer::StatePropsPtr
InputLSTSContainer::State::getStatePropsPtr() const
{
    return stateprops;
}

// getTransitionsAmnt
inline unsigned InputLSTSContainer::State::getTransitionsAmnt() const
{
    return transitionsAmnt & (~(1 << (sizeof(unsigned)*CHAR_BIT-1)));
}




// InputLSTSContainer
//------------------------------------------------------------------------

// getInitialState
inline const InputLSTSContainer::State&
InputLSTSContainer::getInitialState() const
{ return data->states[initialState]; }

inline lsts_index_t InputLSTSContainer::getInitialStateNumber() const
{
    return initialState;
}

// getHeaderData
inline const Header& InputLSTSContainer::getHeaderData() const
{
    return data->headerData;
}

// getStateProps
inline StatePropsContainer& InputLSTSContainer::getStateProps()
{ return stateprops; }

inline const StatePropsContainer& InputLSTSContainer::getStateProps() const
{ return stateprops; }

// getActionNamesStore
//inline ActionNamesStore& InputLSTSContainer::getActionNamesStore()
//{ return *this; /*data->actionnames;*/ }

inline const ActionNamesStore& InputLSTSContainer::getActionNamesStore() const
{ return *this; /*data->actionnames;*/ }

// getStateNumber
inline lsts_index_t InputLSTSContainer::getStateNumber(const State& st) const
{ return (&st)-(&(data->states[0])); }

// getState
inline const InputLSTSContainer::State&
InputLSTSContainer::getState(lsts_index_t sNumber) const
{ return data->states[sNumber]; }

// AccSets
inline AccSets InputLSTSContainer::getAccSets(const State& state) const
{
    return data->accsets.getAccSets(getStateNumber(state));
}
inline AccSets InputLSTSContainer::getAccSets(unsigned stateNumber) const
{
    return data->accsets.getAccSets(stateNumber);
}
inline RO_AccSets InputLSTSContainer::getRO_AccSets(const State& state) const
{
    return data->accsets.getRO_AccSets(getStateNumber(state));
}
inline RO_AccSets InputLSTSContainer::getRO_AccSets(unsigned stateNumber) const
{
    return data->accsets.getRO_AccSets(stateNumber);
}

// tr_begin
inline InputLSTSContainer::tr_const_iterator
InputLSTSContainer::tr_begin(const State& st) const
{
    return tr_const_iterator(data->transitions,
                             st.getTransitionsHandle(),
                             st.getTransitionsAmnt());
}

// tr_end
inline InputLSTSContainer::tr_const_iterator
InputLSTSContainer::tr_end(const State& st) const
{
    return tr_const_iterator(data->transitions,
                             st.getTransitionsHandle(),
                             st.getTransitionsAmnt(),
                             st.getTransitionsAmnt());
}

// findTransition
inline InputLSTSContainer::tr_const_iterator
InputLSTSContainer::findTransition(const State& st, lsts_index_t trNumber)
    const
{
    return tr_const_iterator(data->transitions,
                             st.getTransitionsHandle(),
                             st.getTransitionsAmnt(),
                             data->transitions.find(trNumber,
                                                    st.getTransitionsHandle(),
                                                    st.getTransitionsAmnt()));
}

// getTransitionCompare
inline InputLSTSContainer::TransitionCompare
InputLSTSContainer::getTransitionCompare() const
{ return TransitionCompare(*this); }



#undef CIRC_INPUTLSTSCONTAINER_HH_
#endif
