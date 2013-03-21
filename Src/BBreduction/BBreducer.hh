#ifndef BBREDUCER_HH
#define BBREDUCER_HH

#include "StateProps.hh"
#include "LSTS_File/ActionNamesStore.hh"
#include "LSTS_File/TransitionsAP.hh"

#include <vector>
#include <list>

class TransitionsContainer;

class BBreducer: public oTransitionsAP
{
 public:
    BBreducer(unsigned amountOfStates, unsigned initialStateNumber,
              const TransitionsContainer& inputTransitions,
              const StatePropsContainer& inputStateProps,
              const ActionNamesStore& actionNamesStore);

    void reduce(OutStream&, Header& inputHeader);



 private:
    const ActionNamesStore& actionNames;
    StatePropsContainer stateProps;

    struct State;
    struct Transition;

    struct Block
    {
        std::vector<State*> bottomStates;
        std::vector<State*> nonbottomStates;
        std::vector<Transition*> noninertTransitions;
        unsigned value;
        StatePropsContainer::StatePropsPtr statePropositions;

        Block(): value(0) {}
        bool flag() const { return value&1; }
        void setFlag() { value |= 1; }
        void unsetFlag() { value &= ~1U; }
        bool stable() const { return value&2; }
        void setStable() { value |= 2; }
        void unsetStable() { value &= ~2U; }
    };

    struct State
    {
        std::list<Block>::iterator block;
        std::vector<Transition*> inertTransitions;
        bool flag; // ***>***

        State(): flag(false) {} // ***>***
    };

    static const unsigned DIVERGENCE_ACTION = ~0U;

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
    // states[0] is a special "divergence state" where all divergences point.
    std::vector<State> states;
    //std::vector<bool> stateFlags; // ***<***
    std::vector<Transition> transitions;
    unsigned initialStateIndex, uniqueTransitionsCount;

    //bool getStateFlag(const State* state) const // ***<***
    //{ return stateFlags[state-&states[0]]; } // ***<***
    //void setStateFlag(const State* state) // ***<***
    //{ stateFlags[state-&states[0]] = true; } // ***<***
    //void unsetStateFlag(const State* state) // ***<***
    //{ stateFlags[state-&states[0]] = false; } // ***<***

    void initializeTransitions(const TransitionsContainer& inputTransitions);
    void initializeBlocks(const StatePropsContainer& inputStateProps);
    void initializeStates();
    void splitBlocks();
    bool splitBlock(std::list<Block>::iterator block);
    void saveLSTS(OutStream&, Header& inputHeader);


    virtual lsts_index_t lsts_numberOfTransitionsToWrite();
    virtual void lsts_WriteTransitions(iTransitionsAP& pipe);


    BBreducer(const BBreducer&);
    const BBreducer& operator=(const BBreducer&);
};

#endif
