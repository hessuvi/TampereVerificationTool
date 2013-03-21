#include "SBreducer.hh"
#include "LSTS_File/oLSTS_File.hh"
#include "InputLSTSContainer.hh"

#include <map>
#include <algorithm>

SBreducer::SBreducer(const InputLSTSContainer& ilsts):
    actionNames(ilsts.getActionNamesStore()),
    stateProps(ilsts.getStateProps().getMaxVal()),
    accsets(ilsts.getHeaderData().GiveActionCnt()),
    inputHeader(ilsts.getHeaderData()),
    states(ilsts.getHeaderData().GiveStateCnt()+1)
{
    transitions.reserve(inputHeader.GiveTransitionCnt());

    const StatePropsContainer inputStateProps = ilsts.getStateProps();
    for(unsigned i = 1; i <= inputStateProps.getMaxStatePropNameNumber(); ++i)
        stateProps.getStatePropName(i) = inputStateProps.getStatePropName(i);

    initializeTransitions(ilsts);
    initializeBlocks(ilsts);
}

void SBreducer::initializeTransitions(const InputLSTSContainer& ilsts)
{
    for(unsigned stateNumber = 1; stateNumber < states.size(); ++stateNumber)
    {
        State* statePtr = &states[stateNumber];

        const InputLSTSContainer::State& inputState =
            ilsts.getState(stateNumber);

        for(InputLSTSContainer::tr_const_iterator tr =
                ilsts.tr_begin(inputState);
            tr != ilsts.tr_end(inputState); ++tr)
        {
            unsigned destStateNumber =
                ilsts.getStateNumber(tr->getDestinationState());
            transitions.push_back(Transition(tr->getTransitionNumber(),
                                             statePtr,
                                             &states[destStateNumber]));

        }
    }
}

namespace
{
    struct StateData
    {
        StatePropsContainer::StatePropsPtr stateProps;
        RO_AccSets accsets;
        bool divBit;

        StateData(StatePropsContainer::StatePropsPtr sp,
                  RO_AccSets as, bool db):
            stateProps(sp), accsets(as), divBit(db)
        {}

        bool operator<(const StateData& rhs) const
        {
            if(stateProps == rhs.stateProps)
            {
                if(accsets == rhs.accsets)
                {
                    return divBit < rhs.divBit;
                }
                else
                {
                    return accsets < rhs.accsets;
                }
            }
            else
            {
                return stateProps < rhs.stateProps;
            }
        }
    };
}

void SBreducer::initializeBlocks(const InputLSTSContainer& ilsts)
{
    typedef std::map<StateData, std::list<Block>::iterator> SDMap;

    SDMap stateDataMap;
    const StatePropsContainer inputStateProps = ilsts.getStateProps();
    unsigned accsetsStateNumberCounter = 0;

    for(unsigned stateNumber = 1; stateNumber < states.size(); ++stateNumber)
    {
        const InputLSTSContainer::State& inputState =
            ilsts.getState(stateNumber);

        StateData data(inputStateProps.getStateProps(stateNumber),
                       ilsts.getRO_AccSets(stateNumber),
                       inputState.getDivBit());

        SDMap::iterator iter = stateDataMap.find(data);

        if(iter == stateDataMap.end())
        {
            toBeProcessed.push_front(Block());
            std::list<Block>::iterator newBlock = toBeProcessed.begin();
            if(!data.stateProps.isEmpty())
            {
                newBlock->statePropositions =
                    stateProps.getStateProps(toBeProcessed.size());
                stateProps.assignStateProps(toBeProcessed.size(),
                                            data.stateProps);
            }
            if(!data.accsets.isEmpty())
            {
                newBlock->accsetsStateNumber = ++accsetsStateNumberCounter;
                accsets.addAccSets(accsetsStateNumberCounter, data.accsets);
            }
            if(data.divBit)
            {
                newBlock->setDivBit();
            }
            stateDataMap[data] = newBlock;
            states[stateNumber].block = newBlock;
        }
        else
        {
            states[stateNumber].block = iter->second;
        }
    }
}

void SBreducer::initializeStates()
{
    for(unsigned trIndex = 0; trIndex < transitions.size(); ++trIndex)
    {
        Transition& tr = transitions[trIndex];
        tr.endState->block->noninertTransitions.push_back(&tr);
    }

    for(unsigned stateIndex = 1; stateIndex < states.size(); ++stateIndex)
    {
        State& state = states[stateIndex];
        state.block->bottomStates.push_back(&state);
    }

    for(std::list<Block>::iterator iter = toBeProcessed.begin();
        iter != toBeProcessed.end(); ++iter)
    {
        std::sort(iter->noninertTransitions.begin(),
                  iter->noninertTransitions.end(),
                  Transition::actionCompare);
    }
}

void SBreducer::reduce(OutStream& os)
{
    initializeStates();
    splitBlocks();
    saveLSTS(os);
}

void SBreducer::splitBlocks()
{
    std::vector<State*> statesToBeReset;

    // While there are blocks to be processed:
    while(!toBeProcessed.empty())
    {
        std::list<Block>::iterator block = toBeProcessed.begin();
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
                tr->startState->flag = true;
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
                    if(!BL[i]->bottomStates[stateIndex]->flag)
                    {
                        splits = true;
                        break;
                    }
                }

                // Split the marked block:
                if(splits)
                {
                    thisBlockWasSplit = (BL[i] == block);
                    splitBlock(BL[i]);
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
                statesToBeReset[i]->flag = false;
            }

            // If this block was split, we don't need to (and in fact can't)
            // continue checking its transitions:
            if(thisBlockWasSplit)
                break;
        } // for(unsigned trIndex = 0; ...

        // If this block was not split move this block to the list of
        // stable blocks:
        if(!thisBlockWasSplit)
        {
            block->setStable();
            stable.splice(stable.end(), toBeProcessed, block);
        }
    } // while(!toBeProcessed.empty())
}

void SBreducer::splitBlock(std::list<Block>::iterator block)
{
    // Create two new blocks:
    toBeProcessed.push_front(Block());
    std::list<Block>::iterator newBlock1 = toBeProcessed.begin();
    toBeProcessed.push_front(Block());
    std::list<Block>::iterator newBlock2 = toBeProcessed.begin();

    newBlock1->statePropositions = block->statePropositions;
    newBlock1->accsetsStateNumber = block->accsetsStateNumber;
    newBlock2->statePropositions = block->statePropositions;
    newBlock2->accsetsStateNumber = block->accsetsStateNumber;
    if(block->divBit()) { newBlock1->setDivBit(); newBlock2->setDivBit(); }

    // All bottom states with the flag set go to newBlock1, the other
    // bottom states go to newBlock2:
    for(unsigned i = 0; i < block->bottomStates.size(); ++i)
    {
        State* bottomState = block->bottomStates[i];
        if(bottomState->flag)
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
}

void SBreducer::saveLSTS(OutStream& os)
{
    // Create result stateprops:
    StatePropsContainer spCont(stateProps.getMaxVal());
    for(unsigned i = 1; i <= stateProps.getMaxStatePropNameNumber(); ++i)
        spCont.getStatePropName(i) = stateProps.getStatePropName(i);

    // Assign state numbers to the blocks and get divbits:
    bool hasDivBits = false;
    divBits.resize(stable.size()+1);
    unsigned stateCounter = 0;
    for(std::list<Block>::iterator iter = stable.begin();
        iter != stable.end(); ++iter)
    {
        ++stateCounter;
        if(iter->divBit()) hasDivBits = divBits[stateCounter] = true;
        iter->value = stateCounter;
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
        if(i == 0 || tr != transitions[i-1])
            ++uniqueTransitionsCount;
    }

    oLSTS_File ofile;

    ofile.GiveHeader() = inputHeader;
    ofile.GiveHeader().SetStateCnt(stateCounter);
    ofile.GiveHeader().SetInitialState
        (states[inputHeader.GiveInitialState()].block->value);

    ofile.AddTransitionsWriter(*this);
    ofile.AddStatePropsWriter(spCont);
    ofile.AddActionNamesWriter(actionNames);
    if(!accsets.isEmpty()) ofile.AddAccSetsWriter(*this);
    if(hasDivBits) ofile.AddDivBitsWriter(*this);
    ofile.WriteFile(os);
}

lsts_index_t SBreducer::lsts_numberOfTransitionsToWrite()
{
    return uniqueTransitionsCount;
}

void SBreducer::lsts_WriteTransitions(iTransitionsAP& pipe)
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

        pipe.lsts_Transition(stateNumber, tr.endState->block->value,
                             tr.action);
    }
    pipe.lsts_EndTransitionsFromState(stateNumber);
}

bool SBreducer::lsts_doWeWriteAccSets()
{
    return true;
}

void SBreducer::lsts_WriteAccSets(iAccSetsAP& pipe)
{
    for(std::list<Block>::iterator iter = stable.begin();
        iter != stable.end(); ++iter)
    {
        accsets.getRO_AccSets(iter->accsetsStateNumber).writeAccSets
            (iter->value, pipe);
    }
}

bool SBreducer::lsts_doWeWriteDivBits()
{
    return true;
}

void SBreducer::lsts_WriteDivBits(iDivBitsAP& pipe)
{
    for(unsigned stateNumber = 1; stateNumber < divBits.size(); ++stateNumber)
        if(divBits[stateNumber])
            pipe.lsts_DivBit(stateNumber);
}
