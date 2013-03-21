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

#include "TransitionsContainer.hh"

#include <cstdlib>


//========================================================================
// Direct file writing setup
//========================================================================
void TransitionsContainer::directFileWrite(const std::string& filename,
                                           bool force)
{
    check_claim(!trwriter,
                "In TransitionsContainer::directFileWrite(): "
                "Attempted to set output file more than once.");

    ostr = new OutStream(filename, force);
    otstr = new OTokenStream(*ostr);
    trwriter = new oTransitionsSection(*otstr);

    SetIncludeFilename(filename);

    Header dumb;
    trwriter->lsts_StartTransitions(dumb);
}

//==========================================================================
// LSTS writing
//==========================================================================
lsts_index_t TransitionsContainer::lsts_numberOfTransitionsToWrite()
{
    return transitionsCount;
}

void TransitionsContainer::lsts_WriteTransitions(iTransitionsAP& pipe)
{
    for(unsigned i=1; i<transitionsAmount.valuesAmount(); i++)
    {
        unsigned transAmnt = numberOfTransitions(i);
        if(transAmnt == 0) continue;
        pipe.lsts_StartTransitionsFromState(i);
        for(unsigned j=0; j<transAmnt; j++)
        {
            Transition tr = getTransition(i, j);
            if(writeExtraData)
                pipe.lsts_TransitionWithExtraData
                    (i, tr.destStateNumber,
                     tr.transitionNumber,
                     unsignedToString(tr.extraData));
            else
                pipe.lsts_Transition(i, tr.destStateNumber,
                                     tr.transitionNumber);
        }
        pipe.lsts_EndTransitionsFromState(i);
    }
}

//==========================================================================
// LSTS reading
//==========================================================================
void TransitionsContainer::lsts_StartTransitions(Header& header)
{
    setMaximumValues(header.GiveTransitionCnt(), header.GiveStateCnt());
}

void
TransitionsContainer::lsts_StartTransitionsFromState(lsts_index_t start_state)
{
    startAddingTransitionsToState(start_state);
}

void TransitionsContainer::lsts_Transition(lsts_index_t,
                                           lsts_index_t dest_state,
                                           lsts_index_t action)
{
    addTransitionToState(action, dest_state);
}

void TransitionsContainer::lsts_TransitionWithExtraData
(lsts_index_t,
 lsts_index_t dest_state,
 lsts_index_t action,
 const std::string& extra_data)
{
    if(readExtraData)
        addTransitionToState(action, dest_state,
                             std::atoi(extra_data.c_str()));
    else
        addTransitionToState(action, dest_state);
}

void TransitionsContainer::lsts_EndTransitionsFromState(lsts_index_t)
{
    doneAddingTransitionsToState();
}

void TransitionsContainer::lsts_EndTransitions() {}
