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

#include "TvtCLP.hh"
#include "InputLSTSContainer.hh"
#include "LSTS_File/LSTS_Sections/specialStateProps.hh"

#include <vector>
#include <set>
#include <string>
#include <algorithm>

using namespace std;


class SBcompare
{
    InputLSTSContainer& lsts1;
    InputLSTSContainer& lsts2;

    struct StatePair
    {
        unsigned state1, state2;
        unsigned parentInd;
        StatePair(unsigned s1=0, unsigned s2=0, unsigned par=0):
            state1(s1), state2(s2), parentInd(par) {}
    };


//======================================================================
// Check that the alphabet and state proposition names are the same
// in the two LSTSs.
//======================================================================
    bool checkNames(vector<string>& names1, vector<string>& names2,
                    const string& nametype)
    {
        if(names1.size() != names2.size())
        {
            cout << "*** The LSTSs have different amount of " << nametype
                 << " names.\n"
                 << "(First LSTS has " << names1.size()
                 << " names and second LSTS has " << names2.size()
                 << " names.)" << endl;
            return false;
        }

        sort(names1.begin(), names1.end());
        sort(names2.begin(), names2.end());

        for(unsigned i = 0; i < names1.size(); ++i)
        {
            if(names1[i] != names2[i])
            {
                cout << "*** The LSTSs have different " << nametype
                     << " names.\n";
                if(names1[i] < names2[i])
                {
                    cout << "(First LSTS has name \"" << names1[i]
                         << "\" which the second LSTS doesn't.)" << endl;
                }
                else
                {
                    cout << "(Second LSTS has name \"" << names2[i]
                         << "\" which the first LSTS doesn't.)" << endl;
                }
                return false;
            }
        }

        return true;
    }

    bool checkAlphabets()
    {
        const ActionNamesStore& st1 = lsts1.getActionNamesStore();
        const ActionNamesStore& st2 = lsts2.getActionNamesStore();

        vector<string> names1, names2;
        names1.reserve(st1.size());
        names2.reserve(st2.size());

        for(unsigned i = 1; i <= st1.size(); ++i)
            names1.push_back(st1.GiveActionName(i));
        for(unsigned i = 1; i <= st2.size(); ++i)
            names2.push_back(st2.GiveActionName(i));

        return checkNames(names1, names2, "action");
    }

    bool checkStatePropNames()
    {
        const StatePropsContainer& sp1 = lsts1.getStateProps();
        const StatePropsContainer& sp2 = lsts2.getStateProps();

        vector<string> names1, names2;
        names1.reserve(sp1.getMaxStatePropNameNumber());
        names2.reserve(sp2.getMaxStatePropNameNumber());

        for(unsigned i = 1; i <= sp1.getMaxStatePropNameNumber(); ++i)
            names1.push_back(sp1.getStatePropName(i));
        for(unsigned i = 1; i <= sp2.getMaxStatePropNameNumber(); ++i)
            names2.push_back(sp2.getStatePropName(i));

        return checkNames(names1, names2, "state proposition");
    }


//======================================================================
// Check that the given state of the given LSTS is deterministic
//======================================================================
    bool checkDeterminism(InputLSTSContainer& lsts, unsigned stateNumber,
                          const string& lstsName)
    {
        const InputLSTSContainer::State& state = lsts.getState(stateNumber);
        InputLSTSContainer::tr_const_iterator tr1, tr2;

        for(tr1 = lsts.tr_begin(state); tr1 != lsts.tr_end(state);)
        {
            tr2 = tr1;
            ++tr1;

            if((tr2->getTransitionNumber() == 0 &&
                state.getStatePropsPtr() ==
                tr2->getDestinationState().getStatePropsPtr())
               ||
               (tr1 != lsts.tr_end(state) && tr2->equal(*tr1)))
            {
                cout << "*** The state " << stateNumber << " of the "
                     << lstsName << " LSTS is not deterministic.\n"
                    "Please determinize the LSTS before comparing." << endl;
                return false;
            }
        }
        return true;
    }


//======================================================================
// Printing functions
//======================================================================
    void printStateProps(const StatePropsContainer& stateprops,
                         StatePropsContainer::StatePropsPtr props)
    {
        for(unsigned prop = stateprops.firstProp(props); prop;)
        {
            cout << "\"" << stateprops.getStatePropName(prop) << "\"";
            prop = stateprops.nextProp(props);
            if(prop) cout << ", ";
        }
        cout << endl;
    }

    void printAccSets(const RO_AccSets& accsets)
    {
        const ActionNamesStore& actnames = lsts1.getActionNamesStore();

        cout << "{";
        for(RO_AccSets::AccSetPtr accset = accsets.firstAccSet();
            accset != accsets.endAccSet(); ++accset)
        {
            cout << "{";
            for(RO_AccSets::action_const_iterator action=accsets.begin(accset);
                action != accsets.end(accset);)
            {
                cout << "\"" << actnames.GiveActionName(*action) << "\"";
                if((++action) != accsets.end(accset)) cout << ", ";
            }
            cout << "}";
        }
        cout << "}" << endl;
    }

    void printStatePropsDiff(const StatePropsContainer& stateprops,
                             StatePropsContainer::StatePropsPtr props1,
                             StatePropsContainer::StatePropsPtr props2)
    {
        vector<unsigned> p1, p2;
        for(unsigned prop = stateprops.firstProp(props1);
            prop; prop = stateprops.nextProp(props1))
            p1.push_back(prop);
        for(unsigned prop = stateprops.firstProp(props2);
            prop; prop = stateprops.nextProp(props2))
            p2.push_back(prop);

        bool printingFirst = true;
        for(unsigned ind1=0, ind2=0; ind1<p1.size() || ind2<p2.size();)
        {
            if(ind1<p1.size() && ind2<p2.size() && p1[ind1] == p2[ind2])
            {
                ++ind1; ++ind2;
                continue;
            }
            if(!printingFirst) cout << " ";

            string spname;
            if(ind1==p1.size() || (ind2<p2.size() && p2[ind2] < p1[ind1]))
            {
                spname = stateprops.getStatePropName(p2[ind2]);
                cout << "+";
                ++ind2;
            }
            else
            {
                spname = stateprops.getStatePropName(p1[ind1]);
                cout << "-";
                ++ind1;
            }
            SP::StatePropType spt = SP::GiveStatePropType(spname);
            if(spt == SP::COMMON)
                cout << "\"" << spname << "\"";
            else
                cout << SP::SpecialPropIds[spt];

            printingFirst = false;
        }
    }

    void printTransitions(const InputLSTSContainer& ilsts,
                          const InputLSTSContainer::State& state)
    {
        const ActionNamesStore& actnames = ilsts.getActionNamesStore();

        for(InputLSTSContainer::tr_const_iterator iter = ilsts.tr_begin(state);
            iter != ilsts.tr_end(state);)
        {
            if(iter->getTransitionNumber() == 0)
                cout << "tau";
            else
                cout << "\""
                     << actnames.GiveActionName(iter->getTransitionNumber())
                     << "\"";
            if(state.getStatePropsPtr() !=
               iter->getDestinationState().getStatePropsPtr())
            {
                cout << "(";
                printStatePropsDiff
                    (ilsts.getStateProps(),
                     state.getStatePropsPtr(),
                     iter->getDestinationState().getStatePropsPtr());
                cout << ")";
            }
            if((++iter) != ilsts.tr_end(state)) cout << ", ";
        }
        cout << endl;
    }

    struct Transition
    {
        lsts_index_t number;
        StatePropsContainer::StatePropsPtr iprops, eprops;

        Transition(): number(0) {}
        Transition(lsts_index_t n,
                   StatePropsContainer::StatePropsPtr p1,
                   StatePropsContainer::StatePropsPtr p2):
            number(n), iprops(p1), eprops(p2)
        {}
    };

    void printPath(const vector<StatePair>& stateQueue, unsigned pathEnd)
    {
        cout << "\n*** Path:\n";
        if(pathEnd == 0)
        {
            cout << "At initial states." << endl;
            return;
        }

        vector<Transition> path;
        while(pathEnd != 0)
        {
            unsigned i = stateQueue[pathEnd].parentInd;

            StatePair state = stateQueue[i];
            StatePair destState = stateQueue[pathEnd];
            const InputLSTSContainer::State& state1 =
                lsts1.getState(state.state1);
            const InputLSTSContainer::State& state2 =
                lsts2.getState(state.state2);
            InputLSTSContainer::tr_const_iterator iter1, iter2;
            for(iter1 = lsts1.tr_begin(state1),
                    iter2 = lsts2.tr_begin(state2);
                iter1 != lsts1.tr_end(state1); ++iter1, ++iter2)
            {
                if(destState.state1 ==
                   lsts1.getStateNumber(iter1->getDestinationState()) &&
                   destState.state2 ==
                   lsts2.getStateNumber(iter2->getDestinationState()))
                {
                    path.push_back
                        (Transition
                         (iter1->getTransitionNumber(),
                          state1.getStatePropsPtr(),
                          iter1->getDestinationState().getStatePropsPtr()));
                    break;
                }
            }
            check_claim(iter1 != lsts1.tr_end(state1),
                        "Internal error happened at SBcompare::printPath(). "
                        "Please make a full bug report.");

            pathEnd = i;
        }

        const ActionNamesStore& actnames = lsts1.getActionNamesStore();
        for(unsigned i = path.size(); i != 0;)
        {
            cout << "\"" << actnames.GiveActionName(path[--i].number) << "\"";
            if(path[i].iprops != path[i].eprops)
            {
                cout << "(";
                printStatePropsDiff(lsts1.getStateProps(),
                                    path[i].iprops, path[i].eprops);
                cout << ")";
            }
            if(i) cout << ", ";
        }
        cout << endl;
    }


//======================================================================
// Check that the given states of the LSTS's are identical
//======================================================================
    bool checkEquality(unsigned stateNumber1, unsigned stateNumber2)
    {
        if(!checkDeterminism(lsts1, stateNumber1, "first") ||
           !checkDeterminism(lsts2, stateNumber2, "second"))
            return false;

        const InputLSTSContainer::State& state1 = lsts1.getState(stateNumber1);
        const InputLSTSContainer::State& state2 = lsts2.getState(stateNumber2);

        if(state1.getStatePropsPtr() != state2.getStatePropsPtr())
        {
            cout << "*** The LSTSs are not equivalent:\n"
                 << "State " << stateNumber1 << " of the first LSTS and state "
                 << stateNumber2 << " of the second LSTS have\n"
                    "different state propositions.\n";
            cout << "*  First LSTS: ";
            printStateProps(lsts2.getStateProps(), state1.getStatePropsPtr());
            cout << "* Second LSTS: ";
            printStateProps(lsts2.getStateProps(), state2.getStatePropsPtr());
            return false;
        }

        if(state1.getDivBit() != state2.getDivBit())
        {
            cout << "*** The LSTSs are not equivalent:\n"
                 << "State " << stateNumber1 << " of the first LSTS has the "
                "divergence bit " << (state1.getDivBit() ? "set" : "unset")
                 << ", while state " << stateNumber2 << "\nof the second LSTS "
                "has the divergence bit "
                 << (state2.getDivBit() ? "set" : "unset") << "." << endl;
            return false;
        }

        if(lsts1.getRO_AccSets(state1) != lsts2.getRO_AccSets(state2))
        {
            cout << "*** The LSTSs are not equivalent:\n"
                 << "State " << stateNumber1 << " of the first LSTS and state "
                 << stateNumber2 << " of the second LSTS have\n"
                    "different acceptance sets.\n";
            cout << "*  First LSTS: ";
            printAccSets(lsts1.getRO_AccSets(state1));
            cout << "* Second LSTS: ";
            printAccSets(lsts2.getRO_AccSets(state2));
            return false;
        }

        for(InputLSTSContainer::tr_const_iterator
                iter1 = lsts1.tr_begin(state1),
                iter2 = lsts2.tr_begin(state2);
            iter1 != lsts1.tr_end(state1) || iter2 != lsts2.tr_end(state2);
            ++iter1, ++iter2)
        {
            bool tr1ended = (iter1 == lsts1.tr_end(state1));
            bool tr2ended = (iter2 == lsts2.tr_end(state2));
            if(tr1ended || tr2ended)
            {
                cout << "*** The LSTSs are not equivalent:\n"
                     << "State " << stateNumber1
                     << " of the first LSTS and state "
                     << stateNumber2 << " of the second LSTS have\ndifferent "
                    "amount of transitions (" << state1.getTransitionsAmnt()
                     << " and " << state2.getTransitionsAmnt() << ").\n"
                    "Transitions:\n";
                cout << "*  First LSTS: "; printTransitions(lsts1, state1);
                cout << "* Second LSTS: "; printTransitions(lsts2, state2);
                return false;
            }

            if(!iter1->equal(*iter2))
                // This comparison works because the LSTSs are strictly
                // sorted and they have identical action and stateprop
                // names at this point.
            {
                cout << "*** The LSTSs are not equivalent:\n"
                     << "State " << stateNumber1
                     << " of the first LSTS and state "
                     << stateNumber2 << " of the second LSTS have\n"
                    "different transitions.\n"
                    "Transitions:\n";
                cout << "*  First LSTS: "; printTransitions(lsts1, state1);
                cout << "* Second LSTS: "; printTransitions(lsts2, state2);
                return false;
            }
        }

        return true;
    }



//======================================================================
// Main comparison function.
//======================================================================
 public:
    bool compare()
    {
        if(lsts1.getHeaderData().isInitiallyUnstable() !=
           lsts2.getHeaderData().isInitiallyUnstable())
        {
            cout << "*** The LSTSs are not equivalent:\n"
                 << "The first LSTS has initially unstable set to "
                 << (lsts1.getHeaderData().isInitiallyUnstable() ?
                     "true" : "false")
                 << " while\nthe second LSTS has it set to "
                 << (lsts2.getHeaderData().isInitiallyUnstable() ?
                     "true" : "false") << endl;
            return false;
        }

        if(!checkAlphabets()) return false;
        if(!checkStatePropNames()) return false;

        vector<set<unsigned> >
            stateSimulations(lsts1.getHeaderData().GiveStateCnt()+1);

        vector<StatePair> stateQueue;
        stateQueue.push_back(StatePair(lsts1.getInitialStateNumber(),
                                       lsts2.getInitialStateNumber()));

        unsigned queueTail = 0;

        while(queueTail < stateQueue.size())
        {
            StatePair state = stateQueue[queueTail];

            if(stateSimulations[state.state1].insert(state.state2).second ==
               true)
            {
                if(!checkEquality(state.state1, state.state2))
                {
                    printPath(stateQueue, queueTail);
                    return false;
                }

                const InputLSTSContainer::State& state1 =
                    lsts1.getState(state.state1);
                const InputLSTSContainer::State& state2 =
                    lsts2.getState(state.state2);
                InputLSTSContainer::tr_const_iterator iter1, iter2;
                for(iter1 = lsts1.tr_begin(state1),
                        iter2 = lsts2.tr_begin(state2);
                    iter1 != lsts1.tr_end(state1); ++iter1, ++iter2)
                {
                    stateQueue.push_back
                        (StatePair
                         (lsts1.getStateNumber(iter1->getDestinationState()),
                          lsts2.getStateNumber(iter2->getDestinationState()),
                          queueTail));
                }
            }
            ++queueTail;
        } // while

        return true;
    }


    SBcompare(InputLSTSContainer& il1, InputLSTSContainer& il2):
        lsts1(il1), lsts2(il2)
    {}
};

//======================================================================
// Command-line parser specialization
// (we need this in order to print the proper syntax line in the help)
//======================================================================
class SBCLP: public TvtCLP
{
    bool quiet_;

    bool qHandler(const string&)
    {
        quiet_ = true;
        return true;
    }

 public:
    SBCLP():
        TvtCLP
    ("Compares strong bisimilarity of two deterministic LSTSs.\n"
     "(Note: The return value of the program will be 0 if the LSTS are\n"
     "equivalent and 2 if they aren't. A return value of 1 is returned\n"
     "when an error occurs (eg. file not found).)"),
        quiet_(false)
    {
        setOptionHandler("q", &SBCLP::qHandler, true,
                         "Don't print anything if the LSTSs are equivalent "
                         "(the return value of the program will be 0; if "
                         "the LSTSs are not equivalent, the difference is "
                         "printed and the return value is 2).");
    }

    void help(const string& programName)
    {
        printProgramDescription();

        cerr << "\nUsage:\n"
             << " " << programName
             << " [<options>] <input file 1> <input file 2>\n\n";

        printOptionsHelp();
        printStdinStdoutNotice();
        printCopyrightNotice();
    }

    bool quiet() const { return quiet_; }
};

//======================================================================
// main()
//======================================================================
int main(int argc, char* argv[])
{
    unsigned supported =
        ILC::ACCSETS | ILC::DIVBITS | ILC::STATEPROPS | ILC::TRANSITIONS;

#ifndef NO_EXCEPTIONS
  try {
#endif
    SBCLP clp;
    clp.setRequiredFilenamesCnt(2, 2);

    if(!clp.parseCommandLine(argc, argv)) return 1;

    InputLSTSContainer
        lsts1(supported, true),
        lsts2(supported, true);
    lsts1.readLSTSFile(clp.getInputStream(0));
    lsts2.readLSTSFile(clp.getInputStream(1));

    SBcompare SB(lsts1, lsts2);

    if(!SB.compare()) return 2;

    if(!clp.quiet())
        cout << "The LSTSs are strongly bisimilar." << endl;

#ifndef NO_EXCEPTIONS
  } catch(...) { return 1; }
#endif
    return 0;
}
