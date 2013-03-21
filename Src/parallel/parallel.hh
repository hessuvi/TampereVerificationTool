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

#ifdef CIRC_PARALLEL_HH_
#error "Include recursion"
#endif

#ifndef ONCE_PARALLEL_HH_
#define ONCE_PARALLEL_HH_
#define CIRC_PARALLEL_HH_

#include "parrules.hh"
#include "inputlsts.hh"
#include "outputlsts.hh"
#include "stack.hh"
#include "InOutStream.hh"

#include <ctime>

// Class declaration:

class Parallel
{
public:
    Parallel(ParRules&, OutStream& os, bool printInfo
#ifdef USE_ALTERNATIVE_STATESET
             , int bii, int kib, int bis, int expectedStates
#endif
             );
    ~Parallel();

    enum SSType { NONE, DEADLOCKS, CSP, CFFD };

    unsigned Calculate(SSType useStubbornSets = NONE);
    /* Return value (currently obsolete):
       Bit 1: Cut state found
           2: Rejection state found
           3: Deadlock rejection state found
           4: Livelock rejection state found
     */

    void Write();

    lsts_index_t GetStateCnt();
    lsts_index_t GetTransitionCnt();

    inline void setDirectTrWriteFile(const std::string& filename, bool force)
    {
        oLSTS->getTransitionsContainer().directFileWrite(filename, force);
    }



//==========================================================================
private:
//==========================================================================
    OutStream& outputStream;

    typedef ParRules::Rule Rule;

    friend class StubbornSets;

    class TransitionRules
    {
     public:

        inline const std::vector<Rule>& getRule(unsigned ruleNumber) const
        { return transitionRules[ruleNumber]; }

        inline lsts_index_t getResult(unsigned ruleNumber) const
        { return resultingActions[ruleNumber]; }

        inline unsigned getRulesAmnt() const { return transitionRules.size(); }

        inline unsigned get_iLSTSAmnt() const { return ilstsAmnt; }

        inline void initialize(const ParRules& pr)
        {
            transitionRules = pr.getRuleTable();
            resultingActions = pr.getResultingActions();
            ilstsAmnt = pr.inputFilesAmnt();
        }

        void addTauRules()
        {
            for(unsigned i=0; i<get_iLSTSAmnt(); ++i)
            {
                transitionRules.push_back(std::vector<Rule>(1, Rule(i, 0)));
                resultingActions.push_back(0);
            }
        }

        void setGuardProcess(unsigned processNumber);

        inline bool guardProcessNotParticipating(unsigned ruleNumber) const
        { return gpNotPart[ruleNumber]; }

     private:
        std::vector<std::vector<Rule> > transitionRules;
        std::vector<lsts_index_t> resultingActions;
        std::vector<bool> gpNotPart;
        lsts_index_t ilstsAmnt;
    } Rules;

    std::vector<InStream> is;

    std::vector<InputLSTS*> iLSTS; // Input LSTS's
    OutputLSTS* oLSTS; // Result
    Stack<OutputLSTS::StatePointer> stack; // Main stack
    Stack<OutputLSTS::StatePointer> apustack; // Auxiliary stack
    std::vector<lsts_index_t> currStateN; // Numbers of the current state
    std::vector<lsts_index_t> newStateN; // Numbers of the new state

    unsigned ReturnValue; // Return value of Calculate()

    bool PruneState; // Cut state flag

    ParRules& parrules;

    bool verbose;
    std::time_t totalStartTime, calcStartTime, calcEndTime;
    std::clock_t totalStartTime_ms, calcStartTime_ms, calcEndTime_ms;

    bool GetStateFromStack();

    bool AddTransition(lsts_index_t,bool);
    bool AddTransitions(unsigned);

    bool CheckStatus(OutputLSTS::StatePointer);

    void CalculateStateProps(OutputLSTS::StatePointer state,
                             const std::vector<lsts_index_t>& stateN);


    unsigned CalculateWithStubbornSets(SSType ss_type);

    SSType currentSSType;

    // Disable copying:
    Parallel (const Parallel&);
    Parallel& operator=(const Parallel&);
};


#undef CIRC_PARALLEL_HH_
#endif

