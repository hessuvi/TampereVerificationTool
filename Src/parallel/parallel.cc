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


#include "parallel.hh"
#include "writer.hh"
#include "error_handling.hh"

using namespace std;

inline void Parallel::CalculateStateProps(OutputLSTS::StatePointer state,
                                          const vector<lsts_index_t>& stateN)
{
    unsigned otfvi =
        parrules.getStatePropRules().calculateStateProps
        (oLSTS->getStateNumber(state), stateN);
    if(otfvi)
    {
        if(otfvi&StatePropRules::Cut_bit)
        { PruneState = true; }
        if(otfvi&StatePropRules::Rej_bit)
        { ReturnValue |= 2; }
        if(otfvi&StatePropRules::DLRej_bit)
        { oLSTS->SetDLRejFlag(state); }
        if(otfvi&StatePropRules::LLRej_bit)
        { oLSTS->SetLLRejFlag(state); }
    }
}


//===========================================================================
// Main parallel composition calculation routine
//===========================================================================
unsigned Parallel::Calculate(SSType useStubbornSets)
{
    calcStartTime = std::time(NULL);
    calcStartTime_ms = std::clock();

    // If OTFVI rules were specified we need to store rule number
    // information in each transition:
    if(parrules.getStatePropRules().hasOTFVIRules())
    {
        oLSTS->getTransitionsContainer().extraDataMaximumValue
            (Rules.getRulesAmnt()-1);
    }

    Rules.addTauRules();
    Rules.setGuardProcess(parrules.getStatePropRules().getGuardProcess());

    if(useStubbornSets != NONE)
        return CalculateWithStubbornSets(useStubbornSets);

// Initializations
//----------------
    OutputLSTS::StatePointer currState;

    // Create the initial state
    for(unsigned ind=0; ind<Rules.get_iLSTSAmnt(); ind++)
    {
        currStateN[ind] = iLSTS[ind]->GetInitialState();
    }
    currState = oLSTS->CreateInitialState(currStateN);
    stack.Push(currState);
    CalculateStateProps(currState, currStateN);
    if(CheckStatus(currState)) return ReturnValue;

// Parallel composition
//---------------------
    bool match;
    int res;

    // For each state in the stack
    while((res=GetStateFromStack()))
    {
        oLSTS->StartAddingTransitions();

        // Go through all the synchronization rules:
        for(unsigned rule=0; rule<Rules.getRulesAmnt(); ++rule)
        {
            const vector<Rule>& curRule = Rules.getRule(rule);
            match = true;
            newStateN = currStateN;

            // for each transition in the rule:
            for(unsigned ruleInd=0; ruleInd<curRule.size(); ++ruleInd)
            {
                // Check if the input state has the necessary transition.
                // If any of the input LSTS's doesn't have a transition
                // required by the rule, the rule is not active.
                const unsigned ilsts = curRule[ruleInd].iLSTS;
                const lsts_index_t trans = curRule[ruleInd].actionNumber;
                const lsts_index_t istate =
                    iLSTS[ilsts]->GetState(currStateN[ilsts], trans);
                if(istate == 0)
                {
                    match = false;
                    break;
                }
                newStateN[ilsts] = istate;
            }

            // If the rule is active, create new transitions and states to
            // the result:
            if(match)
                if(AddTransitions(rule))
                {
                    oLSTS->DoneAddTransition();
                    return ReturnValue;
                }
        }

        // Check for deadlock rejection:
        if(oLSTS->DoneAddTransition()==0) // If it's a deadlock
        {
            if(oLSTS->IsDLRej(oLSTS->GetCurrentState()))
            {
                oLSTS->setInterruptedData
                    ("Deadlock rejection triggered by state prop rules",
                     oLSTS->getStateNumber(oLSTS->GetCurrentState()),
                     Header::DL_REJ);
                return (ReturnValue |= 4);
            }
        }
    }

    return 0;
}

//===========================================================================
// Pop value from stack
//===========================================================================
bool Parallel::GetStateFromStack()
{
    OutputLSTS::StatePointer state;
    bool apu;
    while((apu=apustack.Peek(state)) || stack.Peek(state))
    {
        // If it was gray or black, set to black and get next.
        if(!oLSTS->IsWhite(state))
        {
            if(!apustack.Pop()) stack.Pop();
            oLSTS->SetBlack(state);
            continue;
        }

        // Set the current state we just popped in the output lsts container:
        oLSTS->SetCurrentState(state);
        oLSTS->GetCurrentStateNumbers(currStateN);

        // Set to gray if it was taken from the secondary stack, else black:
        if(apu)
            oLSTS->SetGray(state);
        else
        {
            if(oLSTS->IsLLRej(state))
            {
                stack.Pop();
                apustack.Push(state);
                oLSTS->SetGray(state);
            }
            else
                oLSTS->SetBlack(state);
        }

        return true; // was white
    }
    return false; // stack empty
}

// Check the status of the newly created state
//===========================================================================
inline bool Parallel::CheckStatus(OutputLSTS::StatePointer newState)
{
    // Check if it's a rejection state
    if(ReturnValue&2)
    {
        oLSTS->setInterruptedData
            ("Rejection state triggered by state prop rules",
             oLSTS->getStateNumber(newState),
             Header::REJ);
        return true;
    }
    // Check if it's a cut state
    if(PruneState)
    {
        PruneState = false;
        oLSTS->SetBlack(newState);
        ReturnValue |= 1;
    }
    return false;
}


// Add one transition
//===========================================================================
inline bool Parallel::AddTransition(lsts_index_t trName,
                                    bool guardProcessActive)
{
    OutputLSTS::StatePointer newState;

    unsigned newStateColor = oLSTS->AddTransition(trName, newStateN, newState);

    if(currentSSType == CFFD)
    {
        if(trName == 0 && newStateColor < 2)
        {
            // if tau, push onto auxiliary stack
            apustack.Push(newState);
        }
        else if(newStateColor == 0) //< 2)
        {
            stack.Push(newState);
        }

        // If action is tau and the destination state is gray, we have found
        // a tau-loop:
        return (trName == 0 && newStateColor == 2);
    }
    else
    {
        // If a new state was created, calculate its state propositions:
        if(newStateColor == 0)
        {
            CalculateStateProps(newState, newStateN);
            if(CheckStatus(newState)) return true;
        }

        // If the destination state was gray, we have found a ll_rej:
        if(newStateColor == 2 && guardProcessActive)
        {
            oLSTS->setInterruptedData
                ("Livelock rejection activated by state prop rules",
                 oLSTS->getStateNumber(newState),
                 Header::LL_REJ);
            ReturnValue |= 16;
            return true;
        }

        // If the guard process is active, push onto auxiliary stack
        if(newStateColor < 2 && guardProcessActive)
            apustack.Push(newState);
        else if(newStateColor == 0)
            stack.Push(newState);
    }

    return false;
}


// Calculate all the combinations of input transitions which match the
// given rule.
//===========================================================================
bool Parallel::AddTransitions(unsigned tRInd)
{
    const vector<Rule>& curRule = Rules.getRule(tRInd);
    const lsts_index_t trName = Rules.getResult(tRInd);

    bool guardProcessActive =
        oLSTS->IsLLRej(oLSTS->GetCurrentState()) &&
        Rules.guardProcessNotParticipating(tRInd);

    bool retVal = false;

    // If we are in CFFD mode and a there's a tau transition starting from
    // the current state, the current state has to be colored gray because
    // it can be a part of a tau loop:
    if((currentSSType == CFFD && trName == 0))
    {
        // If the current state was taken from the main stack it will be
        // black and it will need to be pushed onto the auxiliary stack
        // so that it will be eventually colored black again:
        if(oLSTS->IsBlack(oLSTS->GetCurrentState()))
            apustack.Push(oLSTS->GetCurrentState());

        oLSTS->SetGray(oLSTS->GetCurrentState());
    }

    if(parrules.getStatePropRules().hasOTFVIRules())
        oLSTS->setExtraData(tRInd);

    while(true)
    {
        lsts_index_t ilsts = curRule[0].iLSTS;
        lsts_index_t trans = curRule[0].actionNumber;
        // The first transition of the current state of the first LSTS:
        lsts_index_t istate = newStateN[ilsts] =
            iLSTS[ilsts]->GetState(currStateN[ilsts], trans);
        do
        {
            // Add a transition
            if(AddTransition(trName, guardProcessActive))
            {
                if(currentSSType == CFFD)
                    retVal = true;
                else
                    return true;
            }
            // The next transition of the current state of the first LSTS:
            istate = newStateN[ilsts] = iLSTS[ilsts]->GetNextState();
        } while(istate); // For as long as there are transitions

        unsigned ruleInd = 0;
        while(true)
        {
            // If all the input transitions have been handled, end:
            if(++ruleInd == curRule.size()) return retVal;

            // The next input LSTS which participates:
            ilsts = curRule[ruleInd].iLSTS;
            trans = curRule[ruleInd].actionNumber;

            // The next transition of that LSTS:
            istate = newStateN[ilsts] = iLSTS[ilsts]->GetNextState();
            // If there was such transition, continue:
            if(istate) break;
            // Else get the first transition and search the next
            // participating LSTS:
            else
            {
                istate = newStateN[ilsts] =
                    iLSTS[ilsts]->GetState(currStateN[ilsts], trans);
            }
        }
    }
}

//===========================================================================
// Write the result
//===========================================================================
void Parallel::Write()
{
    calcEndTime = std::time(NULL);
    calcEndTime_ms = std::clock();
    Writer(outputStream, *oLSTS, parrules,
           parrules.getStatePropRules().getGuardProcess());
}


//===========================================================================
// Constructor and destructor
//===========================================================================
Parallel::Parallel(ParRules& parru, OutStream& os,
                   bool printInfo
#ifdef USE_ALTERNATIVE_STATESET
                   , int bii, int kib, int bis, int expectedStates
#endif
                   ):
    outputStream(os),
    ReturnValue(0),
    PruneState(false),
    parrules(parru),
    verbose(printInfo),
    totalStartTime(std::time(NULL)), calcStartTime(-1), calcEndTime(-1),
    totalStartTime_ms(std::clock()), calcStartTime_ms(-1), calcEndTime_ms(-1),
    currentSSType(NONE)
{
    // Create the input LSTS's:
    iLSTS.resize(parrules.inputFilesAmnt(), 0);

    Rules.initialize(parru);
    is.resize(Rules.get_iLSTSAmnt());

    vector<lsts_index_t> lstsSizes(Rules.get_iLSTSAmnt(), 0);

    for(unsigned i=0; i<Rules.get_iLSTSAmnt(); i++)
    {
        const std::string& filename = parrules.getFilename(i+1);
        if(!filename.empty())
        {
            unsigned prev = parrules.fileAppearance(filename);

            if(prev<i+1) // If identical file name, copy
            {
                iLSTS[i] = new InputLSTS(*iLSTS[prev-1]);
                outputStream.AddInStream(is[prev-1]);
            }
            else // else open it and create a new InputLSTS
            {
                is[i].OpenInputFile(parrules.getFilename(i+1));
                iLSTS[i] = new InputLSTS(is[i]);
                outputStream.AddInStream(is[i]);
                is[i].CloseFile();
            }

            // Get the amount of states in the input LSTS
            lstsSizes[i] = iLSTS[i]->GetStateAmount();

            // Give the state propositions of the input LSTS to the rules
            // class:
            parrules.getStatePropRules().addStatePropsContainer
                (i+1, iLSTS[i]->getStateProps(),
                 parrules.getStickySPPrefix(i+1));
        }
    }
    parrules.getStatePropRules().doneAddingStatePropsContainers();

    // Calculate the maximum action number of the result
    lsts_index_t MaxVal=0, Val;
    for(unsigned i=0; i<Rules.getRulesAmnt(); i++)
    {
        Val = Rules.getResult(i);
        if(Val>MaxVal) MaxVal=Val;
    }



#ifdef USE_ALTERNATIVE_STATESET
    std::vector<unsigned> itemGroupBits;
    MemoryVar::requiredBits(lstsSizes, itemGroupBits, 1);
    int totalbits = 0;
    for(unsigned i=0; i<itemGroupBits.size(); ++i)
        totalbits += itemGroupBits[i];

    cerr << "Bits per state: " << totalbits << endl;

    int defbii, defbik, defkib, defbis, bik;
    if(expectedStates <= 0)
        calc_params_from_mem(totalbits, 10000000,
                             defbii, defbik, defkib, defbis);
    else
        ava_guess(totalbits, expectedStates, defbii, defbik, defkib, defbis);

    if(bii == -1)
    {
        //bii = ((totalbits/4)/2)*4+totalbits%4;
        bii = defbii;
        bik = defbik;
    }
    else
    {
        bik = totalbits-bii; if(bik<8) bik = 8;
    }
    check_claim(bii <= totalbits,
                valueToMessage("Given bii (", bii, ") is too large. Abort."));

    if(kib == -1) kib = defkib; //kib = 7;
    if(bis == -1) bis = defbis; //bis = 10;

    cerr << "Using values: bii("<<bii<<"), bik("<<bik<<"), kib("<<kib
         <<"), bis("<<bis<<")"<<endl<<endl;;

    oLSTS = new OutputLSTS(Rules.get_iLSTSAmnt(), lstsSizes, MaxVal,
                           bii, bik, kib, bis);
#else

    // Create the result LSTS
    oLSTS = new OutputLSTS(Rules.get_iLSTSAmnt(), lstsSizes, MaxVal);
#endif


    // Helper vectors
    currStateN.resize(Rules.get_iLSTSAmnt());
    newStateN.resize(Rules.get_iLSTSAmnt());
}

Parallel::~Parallel()
{
    using namespace std;
    if(verbose)
    {
        time_t totalEndTime = time(NULL);
        long totalTime = long(difftime(totalEndTime, totalStartTime));
        long calcTime = long(difftime(calcEndTime, calcStartTime));
        clock_t totalEndTime_ms = clock();
        if(calcEndTime < 0) calcEndTime = totalEndTime;
        cerr << endl << "Total time: " << totalTime << " s";
        if(totalTime < 30*60)
            cerr << " (" <<
                ((totalEndTime_ms-totalStartTime_ms)/(CLOCKS_PER_SEC/1000))
                 << " ms)";
        cerr << endl << "Calculation time: " << calcTime << " s";
        if(calcTime < 30*60)
            cerr << " (" <<
                ((calcEndTime_ms-calcStartTime_ms)/(CLOCKS_PER_SEC/1000))
                 << " ms)";
        cerr << endl << endl;
    }

#ifndef USE_ALTERNATIVE_STATESET
    if(verbose)
        cerr << "Bytes for states: " << oLSTS->getReservedBytesForStates()
             << endl
             << "Bytes for transitions: "
             << oLSTS->getTransitionsContainer().getReservedBytes()
             << endl;
#endif

    for(unsigned i=0; i<Rules.get_iLSTSAmnt(); i++)
        delete iLSTS[i];
    delete oLSTS;
}

lsts_index_t Parallel::GetStateCnt() { return oLSTS->StateCnt(); }
lsts_index_t Parallel::GetTransitionCnt() { return oLSTS->TransitionCnt(); }


void Parallel::TransitionRules::setGuardProcess(unsigned processNumber)
{
    gpNotPart.clear();
    gpNotPart.resize(transitionRules.size(), true);

    for(unsigned i=0; i<transitionRules.size(); ++i)
    {
        for(unsigned j=0; j<transitionRules[i].size(); ++j)
        {
            unsigned iLSTS = transitionRules[i][j].iLSTS+1;
            if(processNumber < iLSTS) break;
            if(processNumber == iLSTS)
            {
                gpNotPart[i] = false;
                break;
            }
        }
    }
}
