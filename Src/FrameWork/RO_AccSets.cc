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

#include "RO_AccSets.hh"

//========================================================================
// Constructors and destructor
//========================================================================
RO_AccSetsContainer::RO_AccSetsContainer():
    actionMaxVal(0), actionMaxValHasBeenSet(false)
{
}

RO_AccSetsContainer::RO_AccSetsContainer(unsigned maxVal):
    actionMaxVal(maxVal), actionMaxValHasBeenSet(true)
{
    addAccSets(0, AccSets(actionMaxVal));
}

RO_AccSetsContainer::~RO_AccSetsContainer()
{}


//========================================================================
// Methods for LSTS-reading:
//========================================================================
void RO_AccSetsContainer::lsts_StartAccSets(Header& header)
{
    actionMaxVal = header.GiveActionCnt();
    actionMaxValHasBeenSet = true;
    addAccSets(0, AccSets(actionMaxVal));
}

void RO_AccSetsContainer::lsts_StartAccSetsOfState(lsts_index_t)
{
    tmpaccsets = AccSets(actionMaxVal);
}

void RO_AccSetsContainer::lsts_StartSingleAccSet(lsts_index_t)
{
    tmpaccsets.createNewAccSet();
}

void RO_AccSetsContainer::lsts_AccSetAction(lsts_index_t,
                                            lsts_index_t action)
{
    tmpaccsets.addActionToNewAccSet(action);
}

void RO_AccSetsContainer::lsts_EndSingleAccSet(lsts_index_t)
{
    tmpaccsets.addNewAccSet();
}

void RO_AccSetsContainer::lsts_EndAccSetsOfState(lsts_index_t state)
{
    addAccSets(state, tmpaccsets);
}

void RO_AccSetsContainer::lsts_EndAccSets() {}


//========================================================================
// Methods for LSTS-writing
//========================================================================
void RO_AccSets::writeAccSets(lsts_index_t sNumber, iAccSetsAP& pipe)
{
    if(!isEmpty())
    {
        pipe.lsts_StartAccSetsOfState(sNumber);
        for(AccSetPtr ptr = firstAccSet(); ptr != endAccSet(); ++ptr)
        {
            pipe.lsts_StartSingleAccSet(sNumber);
            for(action_const_iterator action = begin(ptr);
                action != end(ptr); ++action)
            {
                pipe.lsts_AccSetAction(sNumber, *action);
            }
            pipe.lsts_EndSingleAccSet(sNumber);
        }
        pipe.lsts_EndAccSetsOfState(sNumber);
    }
}


bool RO_AccSetsContainer::lsts_doWeWriteAccSets()
{
    return actionMaxValHasBeenSet && !isEmpty();
}

void RO_AccSetsContainer::lsts_WriteAccSets(iAccSetsAP& pipe)
{
    if(!actionMaxValHasBeenSet) return;

    for(unsigned state=1; state<accsets.size(); state++)
    {
        getRO_AccSets(state).writeAccSets(state, pipe);
    }
}
