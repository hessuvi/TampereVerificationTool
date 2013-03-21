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
#include "TransitionsContainer.hh"
#include "StateProps.hh"
#include "LSTS_File/oLSTS_File.hh"

#include <vector>
using std::vector;

/*
namespace
{
    template<typename itemType>
    inline void resetVector(vector<itemType>& v)
    {
        vector<itemType> dummy;
        v.swap(dummy);
    }
}
*/

class TauReduction
{
    InputLSTSContainer& ilsts;
    TransitionsContainer transitions;
    StatePropsContainer stateprops;

    struct StateInfo
    {
        unsigned n, minn, newStateNumber;
        InputLSTSContainer::tr_const_iterator currentChild;

        StateInfo(): n(0), newStateNumber(0) {}
    };

    vector<StateInfo> states;

    // This vector contains index values to 'states'. The index values are
    // grouped, and the group delimiter is ~0U. Each group of indices form
    // one destination state. The state number of a destination state
    // increases with each group, starting with 1 for the first group.
    vector<unsigned> newStates;

    vector<unsigned> Stack;
    vector<unsigned> StrongComponent;

    unsigned nodeIndex, newStateNumberCounter;

    void searchStronglyConnectedComponents(unsigned startingStateInd)
    {
        Stack.resize(1, startingStateInd);
        StrongComponent.resize(1, startingStateInd);

        while(Stack.size())
        {
            StateInfo& state = states[Stack.back()];
            const InputLSTSContainer::State& stateRef =
                ilsts.getState(Stack.back()+1);

            if(state.n == 0) // if unvisited
            {
                state.n = nodeIndex++;
                state.minn = state.n;
            }
            else // go to next child node
            {
                unsigned nextStateInd =
                    ilsts.getStateNumber
                    (state.currentChild->getDestinationState())-1;
                unsigned nextState_n = states[nextStateInd].n;
                if(nextState_n < state.minn)
                    state.minn = nextState_n;

                ++state.currentChild;
            }

            // Search next unvisited node (which is reached with an inv. tr.)
            bool allVisited = true;
            while(state.currentChild != ilsts.tr_end(stateRef) &&
                  state.currentChild->getTransitionNumber() == 0)
            {
                const InputLSTSContainer::State& nextStateRef =
                    state.currentChild->getDestinationState();
                unsigned nextStateInd =
                    ilsts.getStateNumber(nextStateRef)-1;

                // If it's an inv. tr. and the dest. state is unvisited:
                if(stateRef.getStatePropsPtr() ==
                   nextStateRef.getStatePropsPtr())
                {
                    unsigned nextState_n = states[nextStateInd].n;
                    if(nextState_n == 0)
                    {
                        Stack.push_back(nextStateInd);
                        StrongComponent.push_back(nextStateInd);
                        allVisited = false;
                        break;
                    }
                    else
                    {
                        if(nextState_n < state.minn)
                            state.minn = nextState_n;
                    }
                }
                ++state.currentChild;
            }

            // If all the child nodes have been visited, check for strongly
            // connected component:
            if(allVisited)
            {
                unsigned stateInd = Stack.back();
                Stack.pop_back();

                if(state.minn != state.n)
                {
                    state.n = state.minn;
                }
                else // we have a strongly connected component
                {
                    // Search the beginning of the scc:
                    unsigned scStartInd = StrongComponent.size();
                    unsigned compNodeInd;
                    do
                    {
                        compNodeInd = StrongComponent[--scStartInd];
                        states[compNodeInd].n = ~0U;
                    } while(compNodeInd != stateInd);

                    for(unsigned i=scStartInd; i<StrongComponent.size(); ++i)
                    {
                        states[StrongComponent[i]].newStateNumber =
                            newStateNumberCounter;
                        newStates.push_back(StrongComponent[i]);
                    }

                    newStates.push_back(~0U);
                    ++newStateNumberCounter;
                    StrongComponent.resize(scStartInd);
                }
            }
        } // while(Stack.size())
    }

    void reduceStronglyConnectedComponents()
    {
        nodeIndex = 1;
        newStateNumberCounter = 1;

        /*
        for(unsigned i=0; i<states.size(); ++i)
        {
            if(states[i].n == 0)
                searchStronglyConnectedComponents(i);
        }
        */

        std::vector<bool> visited(ilsts.getHeaderData().GiveStateCnt(), false);
        std::vector<unsigned> stateNumberStack;
        stateNumberStack.push_back(ilsts.getHeaderData().GiveInitialState()-1);

        while(!stateNumberStack.empty())
        {
            unsigned stateNumber = stateNumberStack.back();
            stateNumberStack.pop_back();
            visited[stateNumber] = true;

            if(states[stateNumber].n == 0)
                searchStronglyConnectedComponents(stateNumber);

            const InputLSTSContainer::State& iState =
                ilsts.getState(stateNumber+1);
            for(InputLSTSContainer::tr_const_iterator tr =
                    ilsts.tr_begin(iState);
                tr != ilsts.tr_end(iState); ++tr)
            {
                unsigned destStateNumber =
                    ilsts.getStateNumber(tr->getDestinationState())-1;
                if(!visited[destStateNumber])
                    stateNumberStack.push_back(destStateNumber);
            }
        }

        const StatePropsContainer& istateprops = ilsts.getStateProps();

        unsigned newStateNumber = 1;
        for(unsigned i=0; i<newStates.size(); ++i)
        {
            stateprops.assignStateProps
                (newStateNumber, istateprops.getStateProps(newStates[i]+1));

            transitions.startAddingTransitionsToState(newStateNumber);
            for(; newStates[i] != ~0U; ++i)
            {
                const InputLSTSContainer::State& iState =
                    ilsts.getState(newStates[i]+1);

                for(InputLSTSContainer::tr_const_iterator trIter =
                        ilsts.tr_begin(iState);
                    trIter != ilsts.tr_end(iState); ++trIter)
                {
                    unsigned destStateInd =
                        ilsts.getStateNumber(trIter->getDestinationState())-1;
                    transitions.addTransitionToState
                        (trIter->getTransitionNumber(),
                         states[destStateInd].newStateNumber);
                }
            }
            transitions.doneAddingTransitionsToState();

            ++newStateNumber;
        }
    }


 public:
    TauReduction(InputLSTSContainer& i):
        ilsts(i),
        transitions(i.getHeaderData().GiveActionCnt(),
                    i.getHeaderData().GiveStateCnt(), true),
        stateprops(i.getHeaderData().GiveStatePropCnt()),
        states(i.getHeaderData().GiveStateCnt())
    {
        const StatePropsContainer& istateprops = ilsts.getStateProps();
        for(unsigned i = istateprops.getMaxStatePropNameNumber(); i > 0; --i)
        {
            stateprops.getStatePropName(i) = istateprops.getStatePropName(i);
        }

        for(unsigned i=0; i<states.size(); ++i)
        {
            states[i].currentChild = ilsts.tr_begin(ilsts.getState(i+1));
        }
    }

    void reduce()
    {
        reduceStronglyConnectedComponents();
    }

    void writeToFile(OutStream& os)
    {
        oLSTS_File ofile;

        ofile.GiveHeader() = ilsts.getHeaderData();
        ofile.GiveHeader().SetStateCnt(newStateNumberCounter-1);
        ofile.GiveHeader().SetInitialState
            (states[ilsts.getInitialStateNumber()-1].newStateNumber);

        ofile.AddTransitionsWriter(transitions);
        ofile.AddStatePropsWriter(stateprops);
        ofile.AddActionNamesWriter(ilsts.getActionNamesStore());

        ofile.WriteFile(os);
    }

    const TransitionsContainer& getTransitions() const { return transitions; }
    const StatePropsContainer& getStateProps() const { return stateprops; }
    unsigned getStatesAmount() const { return newStateNumberCounter-1; }
    unsigned getInitialStateNumber() const
    { return states[ilsts.getInitialStateNumber()-1].newStateNumber; }
};
