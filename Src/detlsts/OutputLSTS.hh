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

Contributor(s): Juha Nieminen, Timo Erkkilä.
*/


#ifdef CIRC_OUTPUTLSTS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_OUTPUTLSTS_HH_
#define ONCE_OUTPUTLSTS_HH_
#define CIRC_OUTPUTLSTS_HH_

#include "InputLSTS.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "LSTS_File/AccSetsAP.hh"
#include "LSTS_File/DivBitsAP.hh"
#include "Memory.hh"
#include "TVTvec.hh"

#ifndef MAKEDEPEND
#include <deque>
#include <set>
#include <vector>
#endif

// Class declaration:

class OutputLSTS: public oTransitionsAP,
                  public oAccSetsAP,
                  public oDivBitsAP
{
public:

    OutputLSTS(InputLSTS&, bool useMaxSize, double maxSize);
    ~OutputLSTS();

    void subsetConstruction();
    void WriteLSTS(OutStream& os);


//========================================================================
private:
//========================================================================

// Types
//========================================================================
    class StateSet;

    typedef unsigned TransitionsHandle;
    typedef StateSet* StateSetPtr;
#define NULL_STATESETPTR StateSetPtr(0)
#define HASH_TABLE_SIZE 65536

// Result transition class
// -----------------------------------------------------------------------
    class Transition
    {

     public:

        inline Transition();
        inline Transition(lsts_index_t tn, const StateSet& ds);

        inline lsts_index_t getTransitionNumber() const;
        inline const StateSet& getDestinationState() const;

        //inline bool noTransition() const;

     private:
        lsts_index_t transitionNumber;
        const StateSet* destinationState;
    };

// Container for state numbers (each result state consists of a group of
// state numbers)
// -----------------------------------------------------------------------
    class StateNumbersContainer
    {
     public:
        typedef MemoryEq::Pointer StateNumbersHandle;

        inline StateNumbersContainer(unsigned maxStateNumber);

	inline void beginStates();
	inline void addStateNumber( lsts_index_t number );
	inline StateNumbersHandle endStates();

	lsts_index_t getStateNumber(StateNumbersHandle handle,
                                    unsigned index,
                                    unsigned numbersAmnt);

	inline bool differ( StateNumbersHandle first,
			    StateNumbersHandle second,
			    unsigned amount ) const;

     private:
        MemoryEq memory;
	std::vector<lsts_index_t> stateNumbersBuf;
	std::vector<lsts_index_t> readBuf;
	MemoryEq::Pointer currentPtr, readPtr;
    };


// Container for result transitions
// -----------------------------------------------------------------------
    class TransitionsContainer
    {
     public:
        static const TransitionsHandle NULL_TRANSITIONS_HANDLE = ~0;

        TransitionsContainer();

        inline TransitionsHandle beginTransitions();
        inline void addTransition(lsts_index_t trNumber,
                                  const StateSet& dest);

        inline Transition getTransition( TransitionsHandle handle,
					 unsigned index ) const;

        inline unsigned transitionsAmount() const
        { return transitions.size(); }

     private:
        TVTvec<Transition> transitions;
        TransitionsHandle current;
    };


// Result state
// -----------------------------------------------------------------------
    typedef std::set<const InputLSTS::Transition*,
        InputLSTS::TransitionCompare> TransitionSet;

    class StateSet
    {
     public:
	inline StateSet();
	inline StateSet(lsts_index_t stateNumber);

        void initTransitions(InputLSTS&, TransitionSet& inputTransitions);
	inline bool endTransitions(TransitionSet& inputTransitions) const
        { return sa_currentInputTransition == inputTransitions.end(); }

  	const InputLSTS::Transition* nextTransition(TransitionSet&);

	inline void addState(const InputLSTS::Transition*, const InputLSTS&);
        inline void endStates();

        inline bool newAction() const { return sa_isNewAction; }

	inline void addTransition(StateSet& destState, lsts_index_t trNumber);

	inline bool operator==(const StateSet& second) const;

        inline StateSetPtr nextState() const { return next; }
        inline void nextState(StateSetPtr n) { next = n; }

        inline unsigned stateNumbersAmount() const
        { return a_stateNumbersAmount; }
        inline lsts_index_t getStateNumber(unsigned index) const
        {
            return
                sa_stateNumbersContainer->getStateNumber(a_stateNumbers,
                                                         index,
                                                         stateNumbersAmount());
        }

        // The following two methods are a "hack" to preserve space.
        // (When writing the LSTS to the output file, the states need to
        // be numbered. Since 'next' is not needed at this stage as a
        // pointer, it's recycled as an integer state number.)
        inline void setStateNumber(unsigned number)
        { state_number = number; }
        inline unsigned getStateNumber() const { return state_number; }

        static inline const TransitionsContainer& getTransitionsContainer()
        { return sa_transitionsContainer; }

        inline TransitionsHandle getTransitionsHandle()
        { return a_transitions; }
        inline unsigned getTransitionsAmount() { return a_transitionsAmount; }

    private:
        // State numbers container
        friend class OutputLSTS;
        static StateNumbersContainer* sa_stateNumbersContainer;

        // Result transitions container
        static TransitionsContainer sa_transitionsContainer;

        // The state numbers if this state
	StateNumbersContainer::StateNumbersHandle a_stateNumbers;
	unsigned a_stateNumbersAmount;

        // The result transitions of this state
	TransitionsHandle a_transitions;
	unsigned a_transitionsAmount;

        // Is the next action different from the current one:
	static bool sa_isNewAction;

        // Next-pointer for the hash table:
	StateSetPtr next;
        unsigned    state_number;

        static TransitionSet::iterator sa_currentInputTransition;
    };


// Methods
//========================================================================
    void readInitialState();

    StateSet& addState(StateSet& newState);

    inline bool allStatesHandled() const { return a_unhandledStates.empty(); }

    StateSet& nextUnhandledState();

    void CalculateStateNumbers();

// Writing methods
    void FillSPContainer(StatePropsContainer& spcont);

    lsts_index_t lsts_numberOfTransitionsToWrite();
    void lsts_WriteTransitions(iTransitionsAP& pipe);

    bool lsts_doWeWriteAccSets();
    void lsts_WriteAccSets(iAccSetsAP& pipe);

    bool lsts_doWeWriteDivBits();
    void lsts_WriteDivBits(iDivBitsAP& pipe);


// Member variables
//========================================================================
    InputLSTS& a_ilsts;

    std::deque<StateSet> a_states; // 0 = first state, 1 = second state, ...

    std::vector<StateSetPtr> a_unhandledStates;
    std::vector<StateSetPtr> a_hashTable;

    StateNumbersContainer a_stateNumbersContainer;

    // The input LSTS transitions of the state
    TransitionSet a_inputTransitions;

    bool useMaxSize;
    unsigned lstsMaxSize;

    // Disable copying:
    OutputLSTS (const OutputLSTS&);
    OutputLSTS& operator=(const OutputLSTS&);

#ifdef DETDEBUG
    void PrintStateSet(const StateSet& stateset)
    {
        for(unsigned i=0; i<stateset.stateNumbersAmount();)
        {
            std::cout << stateset.getStateNumber(i);
            if((++i)<stateset.stateNumbersAmount()) std::cout << ",";
        }
    }
#endif
};



#undef CIRC_OUTPUTLSTS_HH_
#endif
