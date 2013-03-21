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

#include "AccSets.hh"

//========================================================================
// Constructors and destructor
//========================================================================
AccSetsContainer::AccSetsContainer():
    actionMaxVal(0), actionMaxValHasBeenSet(false)
{
    accsets.resize(1);
}

AccSetsContainer::AccSetsContainer(unsigned maxVal):
    actionMaxVal(maxVal), actionMaxValHasBeenSet(true)
{
    accsets.push_back(AccSets(maxVal));
}

AccSetsContainer::~AccSetsContainer()
{}



//========================================================================
// LSTS reading
//========================================================================
void AccSetsContainer::lsts_StartAccSets(Header& header)
{
    actionMaxVal = header.GiveActionCnt();
    actionMaxValHasBeenSet = true;
    accsets[0].setMaxVal(actionMaxVal);
}

void AccSetsContainer::lsts_StartAccSetsOfState(lsts_index_t state)
{
    if(accsets.size() < state+1)
    {
        accsets.resize(state+1, AccSets(actionMaxVal));
    }
}

void AccSetsContainer::lsts_StartSingleAccSet(lsts_index_t state)
{
    accsets[state].createNewAccSet();
}

void AccSetsContainer::lsts_AccSetAction(lsts_index_t state,
                                                lsts_index_t action)
{
    accsets[state].addActionToNewAccSet(action);
}

void AccSetsContainer::lsts_EndSingleAccSet(lsts_index_t state)
{
    accsets[state].addNewAccSet();
}

void AccSetsContainer::lsts_EndAccSetsOfState(lsts_index_t) {}
void AccSetsContainer::lsts_EndAccSets() {}



//========================================================================
// LSTS writing
//========================================================================
void AccSets::writeAccSets(lsts_index_t stateNumber, iAccSetsAP& pipe)
{
    if(!isEmpty())
    {
        pipe.lsts_StartAccSetsOfState(stateNumber);
        for(AccSetPtr ptr = firstAccSet(); ptr != endAccSet(); ++ptr)
        {
            pipe.lsts_StartSingleAccSet(stateNumber);
            for(action_const_iterator action = begin(ptr);
                action != end(ptr); ++action)
            {
                pipe.lsts_AccSetAction(stateNumber, *action);
            }
            pipe.lsts_EndSingleAccSet(stateNumber);
        }
        pipe.lsts_EndAccSetsOfState(stateNumber);
    }
}

// Methods for LSTS-writing
// ------------------------
bool AccSetsContainer::lsts_doWeWriteAccSets()
{
    return actionMaxValHasBeenSet && !isEmpty();
}

void AccSetsContainer::lsts_WriteAccSets(iAccSetsAP& pipe)
{
    if(!actionMaxValHasBeenSet) return;

    for(AccSetsCont_t::size_type state=1; state<accsets.size(); ++state)
    {
        accsets[state].writeAccSets(state, pipe);
    }
}
