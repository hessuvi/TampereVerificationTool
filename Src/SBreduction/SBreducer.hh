#ifndef SBREDUCER_HH
#define SBREDUCER_HH

#include "StateProps.hh"
#include "RO_AccSets.hh"
#include "LSTS_File/ActionNamesStore.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "LSTS_File/AccSetsAP.hh"
#include "LSTS_File/DivBitsAP.hh"

#include <vector>
#include <list>

class InputLSTSContainer;

class SBreducer: public oTransitionsAP, public oAccSetsAP, public oDivBitsAP
{
 public:
    SBreducer(const InputLSTSContainer&);

    void reduce(OutStream&);



 private:
    ActionNamesStore actionNames;
    StatePropsContainer stateProps;
    RO_AccSetsContainer accsets;
    Header inputHeader;
    std::vector<bool> divBits;

    struct State;
    struct Transition;

    struct Block
    {
        std::vector<State*> bottomStates;
        std::vector<Transition*> noninertTransitions;
        unsigned value;
        StatePropsContainer::StatePropsPtr statePropositions;
        unsigned accsetsStateNumber;

        Block(): value(0), accsetsStateNumber(0) {}
        bool flag() const { return value&1; }
        void setFlag() { value |= 1; }
        void unsetFlag() { value &= ~1U; }
        bool stable() const { return value&2; }
        void setStable() { value |= 2; }
        void unsetStable() { value &= ~2U; }
        bool divBit() const { return value&4; }
        void setDivBit() { value |= 4; }
    };

    struct State
    {
        std::list<Block>::iterator block;
        bool flag;

        State(): flag(false) {}
    };

    struct Transition
    {
        unsigned action;
        State* startState;
        State* endState;

        Transition(unsigned a=0, State* s=0, State* e=0):
            action(a), startState(s), endState(e) {}

        // NOTE: These operators are only valid when the algorithm has been
        // completed and all blocks have been numbered (in their 'value'):
        bool operator<(const Transition& rhs) const
        {
            const unsigned stateNumber1 = startState->block->value;
            const unsigned stateNumber2 = rhs.startState->block->value;
            if(stateNumber1 == stateNumber2)
            {
                if(action == rhs.action)
                    return endState->block->value < rhs.endState->block->value;
                else
                    return action < rhs.action;
            }
            else
            {
                return stateNumber1 < stateNumber2;
            }
        }

        bool operator==(const Transition& rhs) const
        {
            return
                endState->block->value == rhs.endState->block->value &&
                action == rhs.action &&
                startState->block->value == rhs.startState->block->value;
        }

        bool operator!=(const Transition& rhs) const
        {
            return !operator==(rhs);
        }

        static bool actionCompare(const Transition* lhs,
                                  const Transition* rhs)
        {
            return lhs->action < rhs->action;
        }
    };


    // List of blocks to be processed and stable blocks:
    std::list<Block> toBeProcessed;
    std::list<Block> stable;

    // Auxiliary container used when calculating the stableness of a block:
    std::vector<std::list<Block>::iterator> BL;

    // The 'states' and 'transitions' vectors are initialized in the
    // constructor and their size does not change during the execution
    // of the algorithm (reduce()).
    std::vector<State> states;
    std::vector<Transition> transitions;
    unsigned uniqueTransitionsCount;

    void initializeTransitions(const InputLSTSContainer& ilsts);
    void initializeBlocks(const InputLSTSContainer& ilsts);
    void initializeStates();
    void splitBlocks();
    void splitBlock(std::list<Block>::iterator block);
    void saveLSTS(OutStream&);


    virtual lsts_index_t lsts_numberOfTransitionsToWrite();
    virtual void lsts_WriteTransitions(iTransitionsAP& pipe);

    virtual bool lsts_doWeWriteAccSets();
    virtual void lsts_WriteAccSets(iAccSetsAP& pipe);

    virtual bool lsts_doWeWriteDivBits();
    virtual void lsts_WriteDivBits(iDivBitsAP& pipe);


    SBreducer(const SBreducer&);
    const SBreducer& operator=(const SBreducer&);
};

#endif
