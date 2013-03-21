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

#include "StateProps.hh"

//==========================================================================
// Constructor and destructor
//==========================================================================
StatePropsContainer::StatePropsContainer(unsigned maxVal,
                                         bool sortSPNames):
    emptyBitVector(maxVal?maxVal:1),
    maxPropVal(maxVal?maxVal:1),
/***OTFVI***
    readSeparateOTFVI(false),
*/
    statepropnames(1),
    sortNames(sortSPNames)
{}

StatePropsContainer::~StatePropsContainer() {}

void StatePropsContainer::setMaxVal(unsigned maxVal)
{
    maxPropVal = maxVal ? maxVal : 1;
    emptyBitVector = BitVector(maxPropVal);
}


//=========================================================================
// LSTS reading
//=========================================================================

void StatePropsContainer::lsts_StartStateProps(Header& header)
{
    setMaxVal(header.GiveStatePropCnt());
}

void
StatePropsContainer::lsts_StartPropStates(const std::string& statePropName)
{
    if(sortNames)
    {
        tempStateNumbers.resize(tempStateNumbers.size()+1);
        tempSPData.push_back(SPData(statePropName, &tempStateNumbers.back()));
    }
    else
    {
        statepropnames.push_back(statePropName);
    }
}

void StatePropsContainer::lsts_PropState(lsts_index_t stateNumber)
{
    if(sortNames)
    {
        tempStateNumbers.back().push_back(stateNumber);
    }
    else
    {
        if(sets.size()<=stateNumber) sets.resize(stateNumber+1, maxPropVal);
        StatePropsPtr ptr(&sets[stateNumber]);
        ptr.setProp(statepropnames.size()-1);
    }
}

/***OTFVI***
void StatePropsContainer::lsts_CutState(lsts_index_t state_number)
{
    if(!readSeparateOTFVI)
    {
        iStatePropsAP::lsts_CutState(state_number);
        return;
    }

    if(otfvi.size()<=state_number) otfvi.resize(state_number+1);
    otfvi[state_number].setCutState();
}

void StatePropsContainer::lsts_RejectState(lsts_index_t state_number)
{
    if(!readSeparateOTFVI)
    {
        iStatePropsAP::lsts_RejectState(state_number);
        return;
    }

    if(otfvi.size()<=state_number) otfvi.resize(state_number+1);
    otfvi[state_number].setRejectState();
}

void StatePropsContainer::lsts_DeadlockRejectState(lsts_index_t state_number)
{
    if(!readSeparateOTFVI)
    {
        iStatePropsAP::lsts_DeadlockRejectState(state_number);
        return;
    }

    if(otfvi.size()<=state_number) otfvi.resize(state_number+1);
    otfvi[state_number].setDeadlockRejectState();
}

void StatePropsContainer::lsts_LivelockRejectState(lsts_index_t state_number)
{
    if(!readSeparateOTFVI)
    {
        iStatePropsAP::lsts_LivelockRejectState(state_number);
        return;
    }

    if(otfvi.size()<=state_number) otfvi.resize(state_number+1);
    otfvi[state_number].setLivelockRejectState();
}

void StatePropsContainer::lsts_InfinityRejectState(lsts_index_t state_number)
{
    if(!readSeparateOTFVI)
    {
        iStatePropsAP::lsts_InfinityRejectState(state_number);
        return;
    }

    if(otfvi.size()<=state_number) otfvi.resize(state_number+1);
    otfvi[state_number].setInfinityRejectState();
}
*/

void StatePropsContainer::lsts_EndPropStates(const std::string&)
{}

void StatePropsContainer::lsts_EndStateProps()
{
    if(sortNames)
    {
        std::sort(tempSPData.begin(), tempSPData.end());
        sortNames = false;
        for(unsigned i=0; i<tempSPData.size(); ++i)
        {
            SPData& data = tempSPData[i];
            lsts_StartPropStates(data.spname);
            for(unsigned j=0; j<data.states->size(); ++j)
                lsts_PropState((*data.states)[j]);
            //lsts_EndPropStates(data.spname);
        }
        sortNames = true;
        tempSPData.clear();
        tempStateNumbers.clear();
    }
}


//=========================================================================
// LSTS writing
//=========================================================================

lsts_index_t StatePropsContainer::lsts_numberOfStatePropsToWrite()
{
    return calculateStatePropCnt();
}

void StatePropsContainer::lsts_WriteStateProps(iStatePropsAP& pipe)
{
    using std::vector;
    using std::map;
    using std::string;

    typedef map<string, vector<lsts_index_t> > statelists_t;

/***OTFVI***
    vector<lsts_index_t> cutStates;
    vector<lsts_index_t> rejStates;
    vector<lsts_index_t> dlrejStates;
    vector<lsts_index_t> llrejStates;
    vector<lsts_index_t> infrejStates;
*/

    statelists_t statesLists;

    for(unsigned i=1; i<statepropnames.size(); ++i)
        if(!statepropnames[i].empty())
            statesLists[statepropnames[i]];
    for(unsigned state=1; state<sets.size(); ++state)
    {
        StatePropsPtr ptr(&sets[state]);
        for(unsigned prop = firstProp(ptr); prop; prop = nextProp(ptr))
        {
            statesLists[statepropnames[prop]].push_back(state);
        }
    }
/***OTFVI***
    for(unsigned state=1; state<otfvi.size(); ++state)
    {
        if(otfvi[state].isCutState())
            cutStates.push_back(state);
        if(otfvi[state].isRejectState())
            rejStates.push_back(state);
        if(otfvi[state].isDeadlockRejectState())
            dlrejStates.push_back(state);
        if(otfvi[state].isLivelockRejectState())
            llrejStates.push_back(state);
        if(otfvi[state].isInfinityRejectState())
            infrejStates.push_back(state);
    }

#define SP_MakePipeCall(InterfaceName, VecName) \
  for(unsigned i=0; i<VecName.size(); ++i) \
    pipe.lsts_##InterfaceName##State(VecName[i]);
  // pipe.lsts_Start##InterfaceName##States();
  // pipe.lsts_End##InterfaceName##States()
  // LK:n rajapinta muuttunut 19.7.01 --Timo

    if(readSeparateOTFVI) // Vanhan rajapinnan jäännöksiä.. 14.11.01 --Timo
    {
        SP_MakePipeCall(Cut, cutStates);
        SP_MakePipeCall(Reject, rejStates);
        SP_MakePipeCall(DeadlockReject, dlrejStates);
        SP_MakePipeCall(LivelockReject, llrejStates);
        SP_MakePipeCall(InfinityReject, infrejStates);
    }
*/

    for(statelists_t::iterator iter = statesLists.begin();
        iter != statesLists.end(); ++iter)
    {
        pipe.lsts_StartPropStates(iter->first);
        for(unsigned i=0; i<iter->second.size(); ++i)
            pipe.lsts_PropState(iter->second[i]);
        pipe.lsts_EndPropStates(iter->first);
    }
}
