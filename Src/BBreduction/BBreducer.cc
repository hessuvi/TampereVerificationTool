#include "BBreducer.hh"
#include "TransitionsContainer.hh"
#include "LSTS_File/oLSTS_File.hh"

#include <map>
#include <algorithm>

/*
  The lines marked with // ***<*** implement the state flags as a vector<bool>
  which in theory should take less memory than having a 'bool flag' in
  'State' (implemented in the // ***>*** lines). However, tests show that
  it does not save memory almost at all (in some cases it even increases
  slightly the memory usage!) and make the program a small percent slower.
 */

BBreducer::BBreducer(unsigned amountOfStates, unsigned initialStateNumber,
                     const TransitionsContainer& inputTransitions,
                     const StatePropsContainer& inputStateProps,
                     const ActionNamesStore& actionNamesStore):
    actionNames(actionNamesStore),
    stateProps(inputStateProps.getMaxVal()),
    states(amountOfStates+1),
    //stateFlags(amountOfStates+1, false),  // ***<***
    initialStateIndex(initialStateNumber)
{
    transitions.reserve(inputTransitions.totalNumberOfTransitions());

    for(unsigned i = 1; i <= inputStateProps.getMaxStatePropNameNumber(); ++i)
        stateProps.getStatePropName(i) = inputStateProps.getStatePropName(i);

    initializeTransitions(inputTransitions);
    initializeBlocks(inputStateProps);
}

void BBreducer::initializeTransitions(const TransitionsContainer&
                                      inputTransitions)
{
    /*
      Note: The branching bisimilarity reduction algorithm requires that
      if there's an inert transition s->s' then s' must be located before s
      in Block::nonbottomStates. TauReduction automatically creates its
      result fulfilling this requirement (because of its depth-first search
      algorithm). Thus the states in 'inputTransitions' are already in the
      correct order and they can be used as they are.
     */

    for(unsigned stateNumber = 1; stateNumber < states.size(); ++stateNumber)
    {
        State* statePtr = &states[stateNumber];

        const unsigned trAmount =
            inputTransitions.numberOfTransitions(stateNumber);

        for(unsigned trIndex = 0; trIndex < trAmount; ++trIndex)
        {
            TransitionsContainer::Transition tr =
                inputTransitions.getTransition(stateNumber, trIndex);
            if(tr.transitionNumber == 0 && tr.destStateNumber == stateNumber)
            {
                transitions.push_back(Transition(DIVERGENCE_ACTION,
                                                 statePtr, &states[0]));
            }
            else
            {
                transitions.push_back(Transition(tr.transitionNumber,
                                                 statePtr,
                                                 &states[tr.destStateNumber]));
            }
        }
    }
}

void BBreducer::initializeBlocks(const StatePropsContainer& inputStateProps)
{
    typedef
        std::map<StatePropsContainer::StatePropsPtr,
        std::list<Block>::iterator>
        SPMap;

    SPMap statePropMap;

    for(unsigned stateNumber = 1; stateNumber < states.size(); ++stateNumber)
    {
        StatePropsContainer::StatePropsPtr sp =
            inputStateProps.getStateProps(stateNumber);

        SPMap::iterator iter = statePropMap.find(sp);

        if(iter == statePropMap.end())
        {
            toBeProcessed.push_front(Block());
            std::list<Block>::iterator newBlock = toBeProcessed.begin();
            if(!sp.isEmpty())
            {
                newBlock->statePropositions =
                    stateProps.getStateProps(toBeProcessed.size());
                stateProps.assignStateProps(toBeProcessed.size(), sp);
            }
            statePropMap[sp] = newBlock;
            states[stateNumber].block = newBlock;
        }
        else
        {
            states[stateNumber].block = iter->second;
        }
    }

    toBeProcessed.push_front(Block());
    states[0].block = toBeProcessed.begin();
}

void BBreducer::initializeStates()
{
    for(unsigned trIndex = 0; trIndex < transitions.size(); ++trIndex)
    {
        Transition& tr = transitions[trIndex];

        if(tr.action == 0 && tr.startState->block == tr.endState->block)
        {
            tr.startState->inertTransitions.push_back(&tr);
            check_claim(tr.startState-&states[0] > tr.endState-&states[0],
                        "In BBreducer::initializeStates(): Order of inert "
                        "transition end states is incorrect. Please make "
                        "a full bug report.");
        }
        else
        {
            tr.endState->block->noninertTransitions.push_back(&tr);
        }
    }

    for(unsigned stateIndex = 0; stateIndex < states.size(); ++stateIndex)
    {
        State& state = states[stateIndex];
        if(state.inertTransitions.empty())
        {
            state.block->bottomStates.push_back(&state);
        }
        else
        {
            state.block->nonbottomStates.push_back(&state);
        }
    }

    for(std::list<Block>::iterator iter = toBeProcessed.begin();
        iter != toBeProcessed.end(); ++iter)
    {
        std::sort(iter->noninertTransitions.begin(),
                  iter->noninertTransitions.end(),
                  Transition::actionCompare);
    }
}

void BBreducer::reduce(OutStream& os, Header& inputHeader)
{
    initializeStates();
    splitBlocks();
    saveLSTS(os, inputHeader);
}

void BBreducer::splitBlocks()
{
    std::vector<State*> statesToBeReset;

    // While there are blocks to be processed:
    while(!toBeProcessed.empty())
    {
        std::list<Block>::iterator block = toBeProcessed.begin();
        bool bottomStateWasCreated = false;
        bool thisBlockWasSplit = false;

        // Scan all the  non-inert transition groups which end in this block
        // with the same action:
        for(unsigned trIndex = 0; trIndex < block->noninertTransitions.size();)
        {
            // A block B' splits a block B (they can be the same block) iff:
            // 1) there's a non-inert transition from B to B',
            // 2) for some state r in B and r' in B': r-action->r', and
            // 3) there is a bottom state s of B such that for no
            //    s' in B': s-action->s'

            BL.clear();
            statesToBeReset.clear();

            const Transition* tr = block->noninertTransitions[trIndex];
            const unsigned action = tr->action;
            // For each group of transitions with the same action, mark
            // starting states and blocks (ie. check requirements 1 and 2):
            do
            {
                tr->startState->flag = true; // ***>***
                //setStateFlag(tr->startState); // ***<***
                statesToBeReset.push_back(tr->startState);

                std::list<Block>::iterator startBlock = tr->startState->block;
                if(!startBlock->flag())
                {
                    startBlock->setFlag();
                    BL.push_back(startBlock);
                }

                if(++trIndex == block->noninertTransitions.size())
                    break;
                tr = block->noninertTransitions[trIndex];
            }
            while(action == tr->action);

            // For each marked block, check if this block splits it:
            for(unsigned i = 0; i < BL.size(); ++i)
            {
                BL[i]->unsetFlag();

                bool splits = false;
                for(unsigned stateIndex = 0;
                    stateIndex < BL[i]->bottomStates.size(); ++stateIndex)
                {
                    // Requirement 3: If for some bottom state the flag
                    // is not set, that means no transition from that
                    // state leads to the current block with 'action':
                    if(!BL[i]->bottomStates[stateIndex]->flag) // ***>***
                 //if(!getStateFlag(BL[i]->bottomStates[stateIndex]))// ***<***
                    {
                        splits = true;
                        break;
                    }
                }

                // Split the marked block:
                if(splits)
                {
                    thisBlockWasSplit = (BL[i] == block);
                    bottomStateWasCreated |= splitBlock(BL[i]);
                    // Note: splitBlock() deletes the block BL[i];
                    // thus 'block' must not be used anymore if
                    // BL[i] == block (because it does not exist anymore).

                    if(thisBlockWasSplit)
                    {
                        // If the block itself was split, there's no need
                        // to continue checking the BL blocks
                        for(unsigned j = i+1; j < BL.size(); ++j)
                            BL[j]->unsetFlag();
                        break;
                    }
                }
            }

            // Reset the flags:
            for(unsigned i = 0; i < statesToBeReset.size(); ++i)
            {
                statesToBeReset[i]->flag = false; // ***>***
                //unsetStateFlag(statesToBeReset[i]); // ***<***
                statesToBeReset[i]->block->unsetFlag();
            }

            // If this block was split, we don't need to (and in fact can't)
            // continue checking its transitions:
            if(thisBlockWasSplit)
                break;
        } // for(unsigned trIndex = 0; ...

        // If this block was not split and a new bottom state was not
        // created, move this block to the list of stable blocks:
        if(!thisBlockWasSplit && !bottomStateWasCreated)
        {
            block->setStable();
            stable.splice(stable.end(), toBeProcessed, block);
        }
    } // while(!toBeProcessed.empty())
}

bool BBreducer::splitBlock(std::list<Block>::iterator block)
{
    bool bottomStateWasCreated = false;

    // Create two new blocks:
    toBeProcessed.push_front(Block());
    std::list<Block>::iterator newBlock1 = toBeProcessed.begin();
    toBeProcessed.push_front(Block());
    std::list<Block>::iterator newBlock2 = toBeProcessed.begin();

    newBlock1->statePropositions = block->statePropositions;
    newBlock2->statePropositions = block->statePropositions;

    // All bottom states with the flag set go to newBlock1, the other
    // bottom states go to newBlock2:
    for(unsigned i = 0; i < block->bottomStates.size(); ++i)
    {
        State* bottomState = block->bottomStates[i];
        if(bottomState->flag) // ***>***
        //if(getStateFlag(bottomState)) // ***<***
        {
            newBlock1->bottomStates.push_back(bottomState);
            bottomState->block = newBlock1;
        }
        else
        {
            newBlock2->bottomStates.push_back(bottomState);
            bottomState->block = newBlock2;
        }
    }

    // All non-bottom states with the flag unset and without inert
    // transitions going to newBlock1 go to newBlock2, the other non-bottom
    // states go to newBlock1:
    for(unsigned i = 0; i < block->nonbottomStates.size(); ++i)
    {
        State* nonbottomState = block->nonbottomStates[i];
        if(!nonbottomState->flag) // ***>***
        //if(!getStateFlag(nonbottomState)) // ***<***
        {
            bool inertTransitionDoesNotLeadToB1 = true;
            for(unsigned j=0; j<nonbottomState->inertTransitions.size(); ++j)
            {
                const State* endState =
                    nonbottomState->inertTransitions[j]->endState;
                if(endState->block == newBlock1)
                {
                    inertTransitionDoesNotLeadToB1 = false;
                    break;
                }
            }
            if(inertTransitionDoesNotLeadToB1)
            {
                // Put the state in newBlock2:
                newBlock2->nonbottomStates.push_back(nonbottomState);
                nonbottomState->block = newBlock2;
                continue;
            }
        }

        // Put the state in newBlock1:
        std::vector<Transition*> newInertTransitions;
        for(unsigned j = 0; j < nonbottomState->inertTransitions.size(); ++j)
        {
            Transition* transition = nonbottomState->inertTransitions[j];
            if(transition->endState->block != newBlock2)
            {
                newInertTransitions.push_back(transition);
            }
            else
            {
                newBlock2->noninertTransitions.push_back(transition);
            }
        }

        nonbottomState->inertTransitions.swap(newInertTransitions);
        nonbottomState->block = newBlock1;
        if(nonbottomState->inertTransitions.empty())
        {
            newBlock1->bottomStates.push_back(nonbottomState);
            // If a non-bottom state became a bottom state, the entire
            // stable list gets invalidated:
            for(std::list<Block>::iterator iter = stable.begin();
                iter != stable.end(); ++iter)
            {
                iter->unsetStable();
            }
            toBeProcessed.splice(toBeProcessed.end(), stable);
            bottomStateWasCreated = true;
        }
        else
        {
            newBlock1->nonbottomStates.push_back(nonbottomState);
        }
    }

    // Distribute the incoming non-inert transitions among the two blocks:
    for(unsigned i = 0; i < block->noninertTransitions.size(); ++i)
    {
        Transition* noninertTransition = block->noninertTransitions[i];
        if(noninertTransition->endState->block == newBlock1)
            newBlock1->noninertTransitions.push_back(noninertTransition);
        else
            newBlock2->noninertTransitions.push_back(noninertTransition);
    }

    // Remove the splitted block:
    if(block->stable())
        stable.erase(block);
    else
        toBeProcessed.erase(block);

    return bottomStateWasCreated;
}

void BBreducer::saveLSTS(OutStream& os, Header& inputHeader)
{
    // Remove the "divergence block" from the list:
    toBeProcessed.splice(toBeProcessed.end(), stable, states[0].block);
    states[0].block->value = ~0U;

    // Create result stateprops:
    StatePropsContainer spCont(stateProps.getMaxVal());
    for(unsigned i = 1; i <= stateProps.getMaxStatePropNameNumber(); ++i)
        spCont.getStatePropName(i) = stateProps.getStatePropName(i);

    // Assign state numbers to the blocks:
    unsigned stateCounter = 0;
    for(std::list<Block>::iterator iter = stable.begin();
        iter != stable.end(); ++iter)
    {
        iter->value = ++stateCounter;
        if(!iter->statePropositions.isEmpty())
            spCont.assignStateProps(stateCounter, iter->statePropositions);
    }

    // Sort transitions so that they can be written properly:
    std::sort(transitions.begin(), transitions.end());

    // Count unique transitions:
    uniqueTransitionsCount = 0;
    for(unsigned i = 0; i < transitions.size(); ++i)
    {
        const Transition& tr = transitions[i];
        if((tr.action != 0 || tr.startState->block != tr.endState->block) &&
           (i == 0 || tr != transitions[i-1]))
            ++uniqueTransitionsCount;
    }

    oLSTS_File ofile;

    ofile.GiveHeader() = inputHeader;
    ofile.GiveHeader().SetStateCnt(stateCounter);
    ofile.GiveHeader().SetInitialState(states[initialStateIndex].block->value);

    ofile.AddTransitionsWriter(*this);
    ofile.AddStatePropsWriter(spCont);
    ofile.AddActionNamesWriter(actionNames);
    ofile.WriteFile(os);
}

lsts_index_t BBreducer::lsts_numberOfTransitionsToWrite()
{
    return uniqueTransitionsCount;
}

void BBreducer::lsts_WriteTransitions(iTransitionsAP& pipe)
{
    unsigned stateNumber = 0;
    for(unsigned i = 0; i < transitions.size(); ++i)
    {
        const Transition& tr = transitions[i];

        if(i > 0 && tr == transitions[i-1]) continue;

        if(tr.startState->block->value != stateNumber)
        {
            if(stateNumber > 0)
                pipe.lsts_EndTransitionsFromState(stateNumber);
            stateNumber = tr.startState->block->value;
            pipe.lsts_StartTransitionsFromState(stateNumber);
        }

        if(tr.action != 0 || tr.startState->block != tr.endState->block)
        {
            if(tr.action == DIVERGENCE_ACTION)
                pipe.lsts_Transition(stateNumber, stateNumber, 0);
            else
                pipe.lsts_Transition(stateNumber, tr.endState->block->value,
                                     tr.action);
        }
    }
    pipe.lsts_EndTransitionsFromState(stateNumber);
}
