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

#include "InputLSTSContainer.hh"


//=========================================================================
// Constructors, destructor and assignment
//=========================================================================
InputLSTSContainer::InputLSTSContainer(unsigned supportedSections,
/***OTFVI***
                                       bool separateOTFVI,
*/
                                       bool strictSorting):

    ActionNamesStore( strictSorting ),

    stateprops(0, strictSorting),
    sectionsToRead(supportedSections),
/***OTFVI***
    separateOTFVI(separateOTFVI),
*/
    sorted(strictSorting)
{
    data = new Data(sorted, &trNumberLookup);
    data->refCnt = 1;
}

InputLSTSContainer::InputLSTSContainer(const InputLSTSContainer& cpy):
    iTransitionsAP(), iDivBitsAP(), ActionNamesStore(cpy),
    data(cpy.data),
    stateprops(cpy.stateprops),
    initialState(cpy.initialState),
/***OTFVI***
    separateOTFVI(cpy.separateOTFVI),
*/
    sorted(cpy.sorted)
{
    incRefCnt();
}

InputLSTSContainer::~InputLSTSContainer()
{
    decRefCnt();
}

InputLSTSContainer&
InputLSTSContainer::operator=(const InputLSTSContainer& cpy)
{
    decRefCnt();
    data = cpy.data;
    stateprops = cpy.stateprops;
    initialState = cpy.initialState;
/***OTFVI***
    separateOTFVI = cpy.separateOTFVI;
*/
    incRefCnt();
    return *this;
}

void InputLSTSContainer::allowInterruptedLSTSReading()
{
    data->allowInterrupted = true;
}


//=========================================================================
// Transitions container implementations
//=========================================================================
// setTransitionCount
void
InputLSTSContainer::TransitionsContainer::setTransitionCount(lsts_index_t cnt)
{
    transitions.resize(cnt);
    current = 0;
    currentGroupBegin = currentGroupEnd = transitions.begin();
}

// beginTransitions
InputLSTSContainer::TransitionsHandle
InputLSTSContainer::TransitionsContainer::beginTransitions()
{
    currentGroupBegin = currentGroupEnd;
    return current;
}

// addTransition
void InputLSTSContainer::TransitionsContainer::addTransition(
    lsts_index_t number, const State& dest)
{
    transitions[current] = Transition(number, dest);
    current++;
    currentGroupEnd++;
}

// endTransitions
void InputLSTSContainer::TransitionsContainer::endTransitions
(const InputLSTSContainer& ilsts)
{
    std::sort(currentGroupBegin, currentGroupEnd,
              ilsts.getTransitionCompare());
}



//=========================================================================
// State class implementations
//=========================================================================
InputLSTSContainer::TransitionsHandle
InputLSTSContainer::State::getTransitionsHandle() const
{
    return trHandle;
}

void InputLSTSContainer::State::setTransitionsAmnt(unsigned amnt)
{
    transitionsAmnt =
        (transitionsAmnt & (1 << (sizeof(unsigned)*CHAR_BIT-1))) | amnt;
}

void
InputLSTSContainer::State::startTransitions(TransitionsContainer& trCont)
{
    trHandle = trCont.beginTransitions();
    setTransitionsAmnt(0);
}

void
InputLSTSContainer::State::addTransition(lsts_index_t trnumber,
                                         const State& dest,
                                         TransitionsContainer& trCont)
{
    trCont.addTransition(trnumber, dest);
    setTransitionsAmnt(getTransitionsAmnt()+1);
}

void
InputLSTSContainer::State::endTransitions(TransitionsContainer& trCont,
                                          const InputLSTSContainer& ilsts)
{
    trCont.endTransitions(ilsts);
}

void InputLSTSContainer::State::setDivBit()
{
    transitionsAmnt |= (1 << (sizeof(unsigned)*CHAR_BIT-1));
}



//=========================================================================
// sortingRO_AccSetsContainer implementations
//=========================================================================
void InputLSTSContainer::sortingRO_AccSetsContainer::lsts_StartAccSets
(Header& h)
{
    if(sort) maxVal = h.GiveActionCnt();
    RO_AccSetsContainer::lsts_StartAccSets(h);
}

void InputLSTSContainer::sortingRO_AccSetsContainer::lsts_StartAccSetsOfState
(lsts_index_t state)
{
    if(sort) tmpAccSets = AccSets(maxVal);
    else RO_AccSetsContainer::lsts_StartAccSetsOfState(state);
}

void InputLSTSContainer::sortingRO_AccSetsContainer::lsts_StartSingleAccSet
(lsts_index_t state)
{
    if(sort) tmpAccSets.createNewAccSet();
    else RO_AccSetsContainer::lsts_StartSingleAccSet(state);
}

void InputLSTSContainer::sortingRO_AccSetsContainer::lsts_AccSetAction
(lsts_index_t state, lsts_index_t action)
{
    if(sort) tmpAccSets.addActionToNewAccSet((*lookup)[action]);
    else RO_AccSetsContainer::lsts_AccSetAction(state, action);
}

void InputLSTSContainer::sortingRO_AccSetsContainer::lsts_EndSingleAccSet
(lsts_index_t state)
{
    if(sort) tmpAccSets.addNewAccSet();
    else RO_AccSetsContainer::lsts_EndSingleAccSet(state);
}

void InputLSTSContainer::sortingRO_AccSetsContainer::lsts_EndAccSetsOfState
(lsts_index_t state)
{
    if(sort)
    {
        tmpAccSets.doneAddingAccSets();
        addAccSets(state, tmpAccSets);
    }
    else RO_AccSetsContainer::lsts_EndAccSetsOfState(state);
}

void InputLSTSContainer::sortingRO_AccSetsContainer::lsts_EndAccSets()
{
    if(sort) tmpAccSets = AccSets();
    else RO_AccSetsContainer::lsts_EndAccSets();
}



//=========================================================================
// LSTS reading methods
//=========================================================================
void InputLSTSContainer::ChecksBeforeReadingLSTS(const iLSTS_File&) {}
void InputLSTSContainer::ChecksAfterReadingLSTS(const iLSTS_File&) {}

void InputLSTSContainer::readLSTSFile(InStream& is)
{
    // Initialize reader:
    iLSTS_File ilsts(is, true, data->allowInterrupted);

    data->headerData = ilsts.GiveHeader();

    if(sectionsToRead&ILC::DIVBITS) ilsts.AddDivBitsReader(*this);
    if(sectionsToRead&ILC::ACCSETS) ilsts.AddAccSetsReader(data->accsets);
    if(sectionsToRead&ILC::STATEPROPS) ilsts.AddStatePropsReader(stateprops);
    if(sectionsToRead&ILC::TRANSITIONS) ilsts.AddTransitionsReader(*this);
    ilsts.AddActionNamesReader(*this);

    // Initialize data structures:
    data->states.resize(data->headerData.GiveStateCnt()+1);
    initialState = data->headerData.GiveInitialState();

    ChecksBeforeReadingLSTS(ilsts);

/***OTFVI***
    if(separateOTFVI) stateprops.separateOTFVI();
*/

    // Read:
    ilsts.ReadFile();

    // Check for stateprops:
    if(stateprops.isInitialized())
    {
        for(unsigned i=1; i<data->states.size(); i++)
        {
            data->states[i].stateprops = stateprops.getStateProps(i);
        }
    }
    else
        stateprops.setMaxVal(data->headerData.GiveStatePropCnt());

    if(!data->accsets.isInitialized())
    {
        data->accsets.setMaxVal(data->headerData.GiveActionCnt());
    }

    ChecksAfterReadingLSTS(ilsts);
}



// Transitions
// -----------
void InputLSTSContainer::lsts_StartTransitions(Header& header)
{
    data->transitions.setTransitionCount(header.GiveTransitionCnt());
}

void
InputLSTSContainer::lsts_StartTransitionsFromState(lsts_index_t start_state)
{
    data->states[start_state].startTransitions(data->transitions);
}

void InputLSTSContainer::lsts_Transition(lsts_index_t sState,
                                         lsts_index_t eState,
                                         lsts_index_t trNumber)
{
    if(sorted) trNumber = trNumberLookup[trNumber];
    data->states[sState].addTransition(trNumber, data->states[eState],
                                       data->transitions);
}

void
InputLSTSContainer::lsts_EndTransitionsFromState(lsts_index_t start_state)
{
    data->states[start_state].endTransitions(data->transitions, *this);
}

void InputLSTSContainer::lsts_EndTransitions()
{
    if(sorted)
    {
        std::vector<lsts_index_t> tempVector;
        trNumberLookup.swap(tempVector); // hack to delete the vector
    }
}


// DivBits
// -------
void InputLSTSContainer::lsts_StartDivBits(Header&){}

void InputLSTSContainer::lsts_DivBit(lsts_index_t state_number)
{
    data->states[state_number].setDivBit();
}

void InputLSTSContainer::lsts_EndDivBits() {}


// Action names (specialized for sorting)
// --------------------------------------
void InputLSTSContainer::lsts_EndActionNames()
{
    ActionNamesStore::lsts_EndActionNames();

    if(sorted)
    {
        std::vector<ActionName> names;
        names.reserve(size());
        for(unsigned i = 1; i <= size(); ++i)
            names.push_back(ActionName(i, GiveActionName(i)));
        sort(names.begin(), names.end());
        trNumberLookup.resize(names.size()+1);
        trNumberLookup[0] = 0;

        ActionNamesStore::clear(); // Timo : 22.10.04
        for(unsigned i = 0; i < names.size(); ++i)
        {
            trNumberLookup[names[i].number] = i+1;
            // Timo : 22.10.04
            //const_cast<std::string&>( GiveActionName(i+1) ) = names[i].name;
            ActionNamesStore::addAction( i + 1, names[i].name );
        }
    }
}

