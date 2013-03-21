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

Contributor(s): Juha Nieminen, Timo Erkkil‰.
*/

// FILE_DES: OutputLSTS.cc: Src/detlsts
// Nieminen Juha & Erkkil‰ Timo

static const char * const ModuleVersion=
  "Module version: $Id: OutputLSTS.cc 1.27 Fri, 27 Feb 2004 16:36:58 +0200 warp $";
// 
// Deterministisointiohjelman tulos-LSTS-tietorakenteen toteutus
//

// $Log:$

#include "OutputLSTS.hh"
#include "LSTS_File/oLSTS_File.hh"

#ifdef DETDEBUG
#include <iostream>
#endif

#include <algorithm>
#include <cstdlib>
using namespace std;



// Transition
//=========================================================================
inline OutputLSTS::Transition::Transition() :
    transitionNumber(0), destinationState(0)
{ }
inline OutputLSTS::Transition::Transition(lsts_index_t tn, const StateSet& ds):
    transitionNumber(tn), destinationState(&ds)
{ }

inline lsts_index_t
OutputLSTS::Transition::getTransitionNumber() const
{
    return transitionNumber;
}

inline const
OutputLSTS::StateSet&
OutputLSTS::Transition::getDestinationState()
    const
{
    return *destinationState;
}




// TransitionsContainer
//=========================================================================
inline OutputLSTS::TransitionsContainer::TransitionsContainer():
    current(0)
{ }

inline OutputLSTS::TransitionsHandle
OutputLSTS::TransitionsContainer::beginTransitions()
{
    return current;
}

inline void OutputLSTS::TransitionsContainer::addTransition(
    lsts_index_t number, const StateSet& dest)
{
    transitions.push_back(Transition(number, dest));
    current++;
}

inline OutputLSTS::Transition OutputLSTS::TransitionsContainer::getTransition(
    TransitionsHandle handle, unsigned index) const
{
    return transitions[handle+index];
}




// StateNumbersContainer
//=========================================================================
inline
OutputLSTS::StateNumbersContainer::StateNumbersContainer(unsigned
                                                         maxStateNumber):
    memory(maxStateNumber),
    currentPtr(MemoryEq::NULLPTR), readPtr(MemoryEq::NULLPTR)
{ }

inline void OutputLSTS::StateNumbersContainer::beginStates()
{
    stateNumbersBuf.clear();
}

inline void
OutputLSTS::StateNumbersContainer::addStateNumber(lsts_index_t number)
{
    stateNumbersBuf.push_back(number);
}

inline OutputLSTS::StateNumbersContainer::StateNumbersHandle
OutputLSTS::StateNumbersContainer::endStates()
{
    sort(stateNumbersBuf.begin(), stateNumbersBuf.end());
    currentPtr = memory.allocateItems(stateNumbersBuf.size());
    memory.putItems(currentPtr, stateNumbersBuf);

#ifdef DETDEBUG
    cout<<"StateNumbersContainer::endStates(): put (";
    for(unsigned i=0; i<stateNumbersBuf.size();)
    {
        cout << stateNumbersBuf[i];
        if((++i)<stateNumbersBuf.size()) cout<<",";
    }
    cout<<") to handle "<<currentPtr<<endl;
#endif

    return currentPtr;
}

lsts_index_t
OutputLSTS::StateNumbersContainer::getStateNumber(StateNumbersHandle handle,
                                                  unsigned index,
                                                  unsigned numbersAmnt)
{
    if(readPtr != handle)
    {
        readPtr = handle;
        readBuf.resize(numbersAmnt);
        memory.getItems(readPtr, readBuf);
#ifdef DETDEBUG
        cout<<"StateNumbersContainer::getSTateNumber(): got (";
        for(unsigned i=0; i<readBuf.size();)
        {
            cout << readBuf[i];
            if((++i)<readBuf.size()) cout<<",";
        }
        cout<<") from handle "<<handle<<endl;
#endif
    }
    return readBuf[index];
}

inline bool
OutputLSTS::StateNumbersContainer::differ(StateNumbersHandle first,
                                          StateNumbersHandle second,
                                          unsigned amount ) const
{
    return !memory.compare(first, second, amount);
}








// StateSet
//=========================================================================
// Staattiset muuttujat:
// --------------------
OutputLSTS::StateNumbersContainer*
  OutputLSTS::StateSet::sa_stateNumbersContainer;
OutputLSTS::TransitionsContainer
  OutputLSTS::StateSet::sa_transitionsContainer;
bool OutputLSTS::StateSet::sa_isNewAction = true;

OutputLSTS::TransitionSet::iterator
  OutputLSTS::StateSet::sa_currentInputTransition;

// Metodit
// -------
inline OutputLSTS::StateSet::StateSet():
    a_stateNumbersAmount(0),
    a_transitions(TransitionsContainer::NULL_TRANSITIONS_HANDLE),
    a_transitionsAmount(0),
    next(NULL_STATESETPTR)
{}

inline OutputLSTS::StateSet::StateSet(lsts_index_t stateNumber):
    a_stateNumbersAmount(0),
    a_transitions(TransitionsContainer::NULL_TRANSITIONS_HANDLE),
    a_transitionsAmount(0),
    next(NULL_STATESETPTR)
{
    sa_stateNumbersContainer->beginStates();
    sa_stateNumbersContainer->addStateNumber(stateNumber);
    a_stateNumbers = sa_stateNumbersContainer->endStates();
    a_stateNumbersAmount++;
}


inline bool
OutputLSTS::StateSet::operator==( const StateSet& second ) const
{
    return a_stateNumbersAmount == second.a_stateNumbersAmount &&
	!sa_stateNumbersContainer->differ(
            a_stateNumbers, second.a_stateNumbers, a_stateNumbersAmount );
}

inline void OutputLSTS::StateSet::addState(
    const InputLSTS::Transition* tr, const InputLSTS& ilsts)
{
    if(a_stateNumbersAmount == 0)
    {
        sa_stateNumbersContainer->beginStates();
    }
    sa_stateNumbersContainer->addStateNumber
        (ilsts.getStateNumber(tr->getDestinationState()));

    a_stateNumbersAmount++;
}

inline void OutputLSTS::StateSet::endStates()
{
    a_stateNumbers = sa_stateNumbersContainer->endStates();
}

inline void OutputLSTS::StateSet::addTransition(StateSet& destState,
                                                lsts_index_t trNumber)
{
    if(a_transitionsAmount == 0)
    {
        a_transitions = sa_transitionsContainer.beginTransitions();
    }
    sa_transitionsContainer.addTransition(trNumber, destState);
    a_transitionsAmount++;
}

void OutputLSTS::StateSet::initTransitions(InputLSTS& ilsts,
                                           TransitionSet& inputTransitions)
{
    inputTransitions.clear();

    for (unsigned i = 0; i < a_stateNumbersAmount; i++ )
    {
        const InputLSTS::State& iState = ilsts.getState(getStateNumber(i));
        InputLSTS::tr_const_iterator trIt = ilsts.tr_begin(iState);

        while (trIt != ilsts.tr_end(iState))
        {
#ifdef DETDEBUG
            cout<<"initTransitions: Inserting ("
                <<ilsts.getStateNumber(trIt->getDestinationState())<<","
                <<trIt->getTransitionNumber()<<")"<<endl;
#endif
            inputTransitions.insert(inputTransitions.begin(), &(*trIt));
            ++trIt;
        }
    }

#ifdef DETDEBUG
    cout<<"initTransitions: Done inserting, inputTransitions: ";
    for(TransitionSet::iterator iter = inputTransitions.begin();
        iter != inputTransitions.end(); ++iter)
        cout<<"("<<ilsts.getStateNumber((*iter)->getDestinationState())<<","
            <<(*iter)->getTransitionNumber()<<")";
    cout<<endl;
#endif

    sa_currentInputTransition = inputTransitions.begin();
}


const InputLSTS::Transition*
OutputLSTS::StateSet::nextTransition(TransitionSet& inputTransitions)
{
    TransitionSet::const_iterator trIt = sa_currentInputTransition;
    ++trIt;

    sa_isNewAction = (trIt == inputTransitions.end() ||
                      !(*sa_currentInputTransition)->equal(*(*trIt)));

    return *sa_currentInputTransition++;
}



//===========================================================================
//===========================================================================

// Konstruktori ja destruktori
//===========================================================================
OutputLSTS::OutputLSTS(InputLSTS& ilsts, bool useMaxSize, double maxSize):
    a_ilsts(ilsts),
    a_hashTable(HASH_TABLE_SIZE, NULL_STATESETPTR),
    a_stateNumbersContainer(ilsts.getHeaderData().GiveStateCnt()),
    a_inputTransitions(ilsts.getTransitionCompare()),
    useMaxSize(useMaxSize),
    lstsMaxSize(unsigned(maxSize*ilsts.getHeaderData().GiveStateCnt()))
{
    StateSet::sa_stateNumbersContainer = &a_stateNumbersContainer;
}

OutputLSTS::~OutputLSTS()
{
}


// P‰‰algoritmi
//===========================================================================
void OutputLSTS::subsetConstruction()
{
#ifdef DETDEBUG
    cout<<"*** Calculating result"<<endl;
#endif

    readInitialState();

    while( ! allStatesHandled() )
    {
	StateSet& currentState = nextUnhandledState();

#ifdef DETDEBUG
        cout<<"Handling state "; PrintStateSet(currentState);
        cout<<endl;
#endif

        currentState.initTransitions(a_ilsts, a_inputTransitions);

	while (!currentState.endTransitions(a_inputTransitions))
	{
	    StateSet newState;
            const InputLSTS::Transition* transition;
	    do
	    {
                transition = currentState.nextTransition(a_inputTransitions);
#ifdef DETDEBUG
                cout<<"Got from nextTransition(): ("
                    <<a_ilsts.getStateNumber(transition->getDestinationState())
                    <<","<<transition->getTransitionNumber()<<")"<<endl;
#endif
		newState.addState(transition, a_ilsts);

	    } while (!currentState.newAction());

            newState.endStates();

	    StateSet& addedState = addState(newState);
            currentState.addTransition(addedState,
                                       transition->getTransitionNumber());
        }
    }
}


// ReadInitialState() builds the root of OutputLSTS.
//===========================================================================
void OutputLSTS::readInitialState()
{
    lsts_index_t inStNumber =
        a_ilsts.getStateNumber(a_ilsts.getInitialState());

    StateSet initialState(inStNumber);

    addState(initialState);
}

// AddState() checks whether the state that is attempted to add here exists
//   already or not; in positive case it simply does nothing, in negative
//   case it adds the new state to the OutputLSTS.
//===========================================================================
OutputLSTS::StateSet& OutputLSTS::addState(StateSet& newState)
{
#ifdef DETDEBUG
    cout<<"addState:"<<endl;
    PrintStateSet(newState);
    cout<<endl;
#endif

    // >>>>>>>>>> Hash-avaimen laskeminen **********
    unsigned int hashKey =
        newState.stateNumbersAmount() *
        newState.stateNumbersAmount() * newState.getStateNumber(0);
    if ( newState.stateNumbersAmount() >= 2 )
    {
	hashKey += newState.getStateNumber(1);
    }
    if ( newState.stateNumbersAmount() >= 3 )
    {
	hashKey += newState.getStateNumber(2);
    }
    hashKey %= HASH_TABLE_SIZE;

    // ********* Hash-avaimen laskeminen <<<<<<<<<<<

    StateSetPtr st = a_hashTable[hashKey];

    for(StateSetPtr ptr = st; ptr != NULL_STATESETPTR;
        ptr = ptr->nextState())
    {
        if(*ptr == newState)
        {
#ifdef DETDEBUG
            cout<<"(State already existed)"<<endl;
#endif
            return *ptr; // On jo olemassa
        }
    }
#ifdef DETDEBUG
    cout<<"(New state)"<<endl;
#endif

    a_states.push_back(newState);
    StateSetPtr newst = &a_states.back();
    a_unhandledStates.push_back(newst);
    newst->nextState(a_hashTable[hashKey]);
    a_hashTable[hashKey] = newst;

    if(useMaxSize && a_states.size() > lstsMaxSize)
    {
        write_message("Resulting LSTS size exceeds given size limit.");
#ifdef NO_EXCEPTIONS
        exit(2);
#else
        throw(2);
#endif
    }

    return *newst;
}

// Removes and returns the last state set from unhandledStates vector.
//===========================================================================
OutputLSTS::StateSet& OutputLSTS::nextUnhandledState()
{
    StateSet& state = *a_unhandledStates.back();
    a_unhandledStates.pop_back();
    return state;
}


//===========================================================================
// LSTS Writing
//===========================================================================
void OutputLSTS::CalculateStateNumbers()
{
    for(unsigned i=0; i<a_states.size(); i++)
    {
        a_states[i].setStateNumber(i+1);
    }
}

void OutputLSTS::FillSPContainer(StatePropsContainer& spcont)
{
    const StatePropsContainer& ispcont = a_ilsts.getStateProps();

    if(ispcont.isEmpty()) return;

    for(unsigned i=1; i<=ispcont.getMaxStatePropNameNumber(); ++i)
        spcont.getStatePropName(i) = ispcont.getStatePropName(i);

    for(unsigned i=0; i<a_states.size(); ++i)
    {
        lsts_index_t iStateNumber = a_states[i].getStateNumber(0);
        const InputLSTS::State& iState = a_ilsts.getState(iStateNumber);
        spcont.getStateProps(i+1).unionAndAssign(iState.getStatePropsPtr());
    }
}

void OutputLSTS::WriteLSTS(OutStream& os)
{
    CalculateStateNumbers();

    oLSTS_File ofile;

    StatePropsContainer spcont(a_ilsts.getStateProps().getMaxVal());
    FillSPContainer(spcont);
    ofile.AddStatePropsWriter(spcont);

    ofile.AddTransitionsWriter(*this);
    ofile.AddAccSetsWriter(*this);
    ofile.AddDivBitsWriter(*this);
    ofile.AddActionNamesWriter(a_ilsts.getActionNamesStore());

    Header headerData = a_ilsts.getHeaderData();
    headerData.SetStateCnt(a_states.size());
    headerData.SetInitialState(1);
    headerData.SetNoTaus(true);
    headerData.SetInitiallyUnstable(a_ilsts.getInitiallyUnstable());
    headerData.SetDeterministic(true);

    ofile.GiveHeader() = headerData;

    ofile.WriteFile(os);
}

//===========================================================================
// Kirjoitusmetodit
//===========================================================================
// Transitions
// -----------
lsts_index_t OutputLSTS::lsts_numberOfTransitionsToWrite()
{
    return StateSet::getTransitionsContainer().transitionsAmount();
}

void OutputLSTS::lsts_WriteTransitions(iTransitionsAP& pipe)
{
    const TransitionsContainer& trCont = StateSet::getTransitionsContainer();

    for(unsigned i=0; i<a_states.size(); i++)
    {
        pipe.lsts_StartTransitionsFromState(i+1);
        TransitionsHandle trHandle = a_states[i].getTransitionsHandle();
        unsigned trAmnt = a_states[i].getTransitionsAmount();
        for(unsigned j=0; j<trAmnt; j++)
        {
            const Transition tr = trCont.getTransition(trHandle, j);
            pipe.lsts_Transition(i+1,
                                 tr.getDestinationState().getStateNumber(),
                                 tr.getTransitionNumber());
        }
        pipe.lsts_EndTransitionsFromState(i+1);
    }
}

// AccSets
// -------
bool OutputLSTS::lsts_doWeWriteAccSets()
{
    return !a_ilsts.accSetsEmpty();
}

void OutputLSTS::lsts_WriteAccSets(iAccSetsAP& pipe)
{
    // Jokaista yhdistelm‰tilaa kohti
    for(unsigned i=0; i<a_states.size(); i++)
    {
        // luetaan syˆtetilan accsetit yhteen
        lsts_index_t iStateNumber = a_states[i].getStateNumber(0);
        AccSets accsets = a_ilsts.getAccSets(iStateNumber);
        for(unsigned j = 1; j < a_states[i].stateNumbersAmount(); j++)
        {
            iStateNumber = a_states[i].getStateNumber(j);
            accsets.addAccSets(a_ilsts.getAccSets(iStateNumber));
        }
        // ja tulostetaan yhdistelm‰
        if(!accsets.isEmpty())
        {
            pipe.lsts_StartAccSetsOfState(i+1);
            for(AccSets::AccSetPtr ptr = accsets.firstAccSet();
                ptr != accsets.endAccSet(); ptr++)
            {
                pipe.lsts_StartSingleAccSet(i+1);
                for(AccSets::action_const_iterator action = accsets.begin(ptr);
                    action != accsets.end(ptr);
                    ++action)
                {
                    pipe.lsts_AccSetAction(i+1, *action);
                }
                pipe.lsts_EndSingleAccSet(i+1);
            }
            pipe.lsts_EndAccSetsOfState(i+1);
        }
    }
}

bool OutputLSTS::lsts_doWeWriteDivBits()
{
    return a_ilsts.divBitsSectionRead();
}

void OutputLSTS::lsts_WriteDivBits(iDivBitsAP& pipe)
{
    for(unsigned i=0; i<a_states.size(); i++)
    {
        bool divBit = false;
        for(unsigned j = 0; j < a_states[i].stateNumbersAmount(); j++)
        {
            lsts_index_t iStateNumber = a_states[i].getStateNumber(j);
            const InputLSTS::State& iState = a_ilsts.getState(iStateNumber);
            if(iState.getDivBit()) { divBit = true; break; }
        }
        if(divBit)
            pipe.lsts_DivBit(i+1);
    }
}
