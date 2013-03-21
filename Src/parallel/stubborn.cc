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

using namespace std;


class StubbornSets
{
    // Data from Parallel
    typedef Parallel::Rule Rule;
    vector<InputLSTS*>& iLSTS;
    const Parallel::TransitionRules& Rules;

    // Table of enabled and disabled rules.
    // A value of RULE_IS_ENABLED means enabled. Any other value means
    // first process (ie. input LSTS) which disables the rule.
    vector<unsigned> disabledRules;

    static const unsigned RULE_IS_ENABLED = ~0U;

    // Lookup table processes -> rules they participate in
    struct ParticipatingRule
    {
        unsigned rule, indexInRule;

        ParticipatingRule(): rule(0),indexInRule(0) {}
        ParticipatingRule(unsigned r, unsigned i): rule(r),indexInRule(i) {}
    };

    vector<vector<ParticipatingRule> > processesInRules;

    // Search graph node
    struct Node
    {
        enum NodeType { PROCESS, PARCOMPRULE };

        NodeType type;
        vector<Node*> arcs;
        unsigned n, visitedInd;
        bool notGood;

        Node(NodeType t): type(t) {}
    };

    /*
      Explanation of the search graph idea:
      ------------------------------------
      * 'type' and 'arcs':
        The search graph consists of two types of nodes: One node for each
      process, and one node for each parallel composition rule. A process
      node has arcs to rule nodes and vice-versa.
        The arcs are built in the following way:
        - From a process node to every rule node which it is ready to
          participating in.
        - From a rule node of an enabled rule to all processes which
          participate in that rule.
        - From a rule node of a disabled rule to the first process which
          is not ready to participate in that rule (but is mentioned in it).

      * 'n' and 'visitedInd':
        The 'n' variable is used for the Tarjan algorithm, which is used for
      searching maximal strongly connected components. The 'visitedInd' is
      used for depth-first search: It contains the arc index which was last
      followed from this node (when we get back to this node, we increase
      this index until we find an arc which goes to an unvisited node or
      there are no more arcs left).
        When a strongly connected component is found, its enabled rules may or
      may not be an acceptable stubborn set. Whether this is so depends on
      the equivalence used.

      * 'notGood':
        There are three reasons for a strongly connected component for not
      being acceptable as a stubborn set:
        1. If it doesn't have any enabled rule. This is the only thing to
      check when deadlocks-preserving stubborn sets are used. However, it's
      not marked as 'notGood'.
        2. If we are using CSP or CFFD equivalence and the set contains rules
      which result in visible actions. If such set is found, its nodes are
      marked as 'notGood'.
        3. If it's possible to reach a 'notGood' strongly connected component
      from the one currently found.
        In order to make the third check, the 'notGood' bit flows upwards
      when performing the depth-first search, that is, when we go back in
      the search, we copy a 'notGood' bit to the parent node if it was true.
    */

    // Search graph
    vector<Node> processes;
    vector<Node> parCompRules;

    inline unsigned getProcessNodeIndex(Node* node)
    {
        return node - (&processes[0]);
    }
    inline unsigned getParCompRulesNodeIndex(Node* node)
    {
        return node - (&parCompRules[0]);
    }


    // Stacks for strongly connected components search
    vector<Node*> Stack;
    vector<Node*> StrongComponent;

    // The index number needed for the Tarjan algorithm:
    unsigned nodeIndex;

    // Used for a trick in CalculateStubbornRules()
    vector<unsigned> tempSet;

    // Fill the disabled rules table. Returns false if all rules are
    // disabled (is used as a quick termination test):
    bool FillDisabledTable(const vector<lsts_index_t>& currStateN)
    {
        bool retVal = false;

        for(unsigned rule=0; rule<Rules.getRulesAmnt(); ++rule)
        {
            const vector<Rule>& curRule = Rules.getRule(rule);
            bool match = true;
            unsigned ruleInd;

            for(ruleInd=0; ruleInd<curRule.size(); ++ruleInd)
            {
                const unsigned ilsts = curRule[ruleInd].iLSTS;
                const lsts_index_t trans = curRule[ruleInd].actionNumber;
                const lsts_index_t istate =
                    iLSTS[ilsts]->GetState(currStateN[ilsts], trans);
                if(istate == 0)
                {
                    match = false;
                    break;
                }
            }
            if(match)
            {
                disabledRules[rule] = RULE_IS_ENABLED;
                retVal = true;
            }
            else
            {
                disabledRules[rule] = curRule[ruleInd].iLSTS;
            }
        }

        return retVal;
    }

    // Fills the lookup vector with an item for each input process.
    // Each item is a vector of type 'ParticipatingRule', which contains
    // an index to a rule which this process is participating and an index
    // inside the rule to where the process appears in it.
    // That is, the vector is a lookup table from process numbers to rules
    // where this process appears.
    void FillProcessesInRulesLookup()
    {
        for(unsigned rule=0; rule<Rules.getRulesAmnt(); ++rule)
        {
            const vector<Rule>& curRule = Rules.getRule(rule);
            for(unsigned ruleInd=0; ruleInd<curRule.size(); ++ruleInd)
            {
                processesInRules[curRule[ruleInd].iLSTS].
                    push_back(ParticipatingRule(rule, ruleInd));
            }
        }
    }

    // Arcs from process to rules it would be ready to participate in
    void CalculateArcsForProcessNode(unsigned pNumber,
                                     const vector<lsts_index_t>& currStateN)
    {
        vector<Node*>& arcs = processes[pNumber].arcs;
        arcs.clear();
        const vector<ParticipatingRule>& lookup = processesInRules[pNumber];
        for(unsigned j=0; j<lookup.size(); ++j)
        {
            const ParticipatingRule& rlookup = lookup[j];
            const Rule& rule =
                Rules.getRule(rlookup.rule)[rlookup.indexInRule];
            const lsts_index_t istate =
                iLSTS[rule.iLSTS]->GetState(currStateN[rule.iLSTS],
                                            rule.actionNumber);
            if(istate != 0)
            {
                arcs.push_back(&parCompRules[rlookup.rule]);
            }
        }

    }

    void CalculateArcsForParCompRulesNode(unsigned ruleNumber)
    {
        vector<Node*>& arcs = parCompRules[ruleNumber].arcs;
        arcs.clear();

        // Arcs from enabled rule to participating processes
        if(disabledRules[ruleNumber] == RULE_IS_ENABLED)
        {
            const vector<Rule>& rule = Rules.getRule(ruleNumber);
            for(unsigned j=0; j<rule.size(); ++j)
            {
                arcs.push_back(&processes[rule[j].iLSTS]);
            }
        }
        // Arc from disabled rule to first disabling process
        else
        {
            arcs.push_back(&processes[disabledRules[ruleNumber]]);
        }
    }

    void CalculateArcsForNode(Node* node,
                              const vector<lsts_index_t>& currStateN)
    {
        if(node->type == Node::PROCESS)
            CalculateArcsForProcessNode(getProcessNodeIndex(node), currStateN);
        else
            CalculateArcsForParCompRulesNode(getParCompRulesNodeIndex(node));
    }


    void markSetAsNotGood(unsigned scStartInd)
    {
        for(unsigned ind = scStartInd; ind < StrongComponent.size(); ++ind)
            StrongComponent[ind]->notGood = true;
    }


    bool searchStubbornSet(Node* startingNode,
                           vector<unsigned>& ruleNumbers,
                           const vector<lsts_index_t>& currStateN,
                           Parallel::SSType sstype)
    {
        // This is the stack for depth-first-search:
        Stack.clear();
        // This is a similar stack, but nodes are not popped out of it
        // when they are popped from 'Stack'. This means that the extra
        // nodes on top of it (compared to 'Stack') will form the strongly
        // connected component:
        StrongComponent.clear();

        Stack.push_back(startingNode);
        StrongComponent.push_back(startingNode);

        // Perform depth-first search:
        while(Stack.size())
        {
            // Take node from the top of the stack:
            Node* node = Stack.back();

            // If node is unvisited, calculate its arcs, assign index
            // number (Tarjan algorithm) and mark next successor node to
            // visit as the first arc:
            if(node->n == 0)
            {
                CalculateArcsForNode(node, currStateN);
                node->n = nodeIndex++;
                node->visitedInd = 0;
            }
            // Else move to the next successor node
            else
            {
                node->notGood =
                    node->notGood || node->arcs[node->visitedInd]->notGood;
                ++(node->visitedInd);
            }

            // Search next unvisited successor node and put it onto the stack
            bool allVisited = true;
            while(node->visitedInd < node->arcs.size())
            {
                Node* nextNode = node->arcs[node->visitedInd];
                if(nextNode->n == 0) // if not already visited
                {
                    Stack.push_back(nextNode);
                    StrongComponent.push_back(nextNode);
                    allVisited = false;
                    break;
                }
                node->notGood = node->notGood || nextNode->notGood;
                ++(node->visitedInd);
            }

            // If all successor nodes have already been visited, check for
            // strongly connected component (using the Tarjan algorithm) and
            // return it if it has an enabled rule
            if(allVisited)
            {
                Stack.pop_back();

                // Get the minimum n in this and successor nodes:
                unsigned minn = node->n;
                for(unsigned i=0; i<node->arcs.size(); ++i)
                {
                    unsigned arcnoden = node->arcs[i]->n;
                    if(arcnoden < minn) minn = arcnoden;
                }

                // If n in the current node is different from the minimum,
                // then this is not a strongly connected component. Set n
                // in current node to the found minimum:
                if(minn != node->n)
                {
                    node->n = minn;
                }
                // If it was the same, then we have found a strongly connected
                // component:
                else
                {
                    // Search the current node in the StrongComponent stack;
                    // the strong component is a the top of it starting from
                    // the current node:
                    unsigned scStartInd = StrongComponent.size();
                    Node* compNode;
                    do
                    {
                        compNode = StrongComponent[--scStartInd];
                        compNode->n = ~0U;
                    } while(compNode != node);

                    // If the nodes in the set have the 'notGood' bit set,
                    // then just skip
                    if(!node->notGood)
                    {
                        // Check that it has at least one enabled rule and
                        // put all the enabled rules in the return vector:
                        bool hasEnabledRule = false;
                        bool hasVisibleResult = false;
                        for(unsigned i = scStartInd;
                            i < StrongComponent.size(); ++i)
                        {
                            compNode = StrongComponent[i];
                            if(compNode->type == Node::PARCOMPRULE)
                            {
                                unsigned ruleInd =
                                    getParCompRulesNodeIndex(compNode);
                                if(disabledRules[ruleInd] == RULE_IS_ENABLED)
                                {
                                    ruleNumbers.push_back(ruleInd);
                                    hasEnabledRule = true;
                                    if(Rules.getResult(ruleInd) != 0)
                                    {
                                        hasVisibleResult = true;
                                    }
                                }
                            }
                        }
                        if(hasEnabledRule)
                        {
                            // For DEADLOCKS it's enough that it just has
                            // an enabled rule. For CSP and CFFD all the
                            // enabled rules must be invisible:
                            if(sstype==Parallel::DEADLOCKS ||
                               !hasVisibleResult)
                            {
                                return true;
                            }
                            // If it had visible rules, mark set as notGood:
                            markSetAsNotGood(scStartInd);
                        }

                        ruleNumbers.clear();
                    } // if(!node->notGood)
                    else
                    {
                        markSetAsNotGood(scStartInd);
                    }

                    // Remove the set from StrongComponent:
                    StrongComponent.resize(scStartInd);
                } // else (if(minn != node->n))
            } // if(allVisited)
        } // while(Stack.size())

        // No proper set was found:
        return false;
    }

    void ResetSearchGraph()
    {
        for(unsigned i=0; i<processes.size(); ++i)
        { processes[i].n = 0; processes[i].notGood = false; }
        for(unsigned i=0; i<parCompRules.size(); ++i)
        { parCompRules[i].n = 0; parCompRules[i].notGood = false; }
    }


//------------------------------------------------------------------------
 public:
//------------------------------------------------------------------------
    StubbornSets(vector<InputLSTS*>& I, const Parallel::TransitionRules& R):
        iLSTS(I), Rules(R),
        disabledRules(Rules.getRulesAmnt()),
        processesInRules(Rules.get_iLSTSAmnt()),
        processes(Rules.get_iLSTSAmnt(), Node::PROCESS),
        parCompRules(Rules.getRulesAmnt(), Node::PARCOMPRULE)
    {
        FillProcessesInRulesLookup();
    }



    // Adds all enabled visible rules and rules reachable from them:
    bool AddAllVisibleRules(vector<unsigned>& ruleNumbers,
                            const vector<lsts_index_t>& currStateN,
                            bool calculateArcs = true)
    {
        ResetSearchGraph();
        Stack.clear();

        // Mark rules already in the set so that they are not added again:
        for(unsigned i=0; i<ruleNumbers.size(); ++i)
            parCompRules[ruleNumbers[i]].notGood = true;

        // Search and add the visible rules and rules reachable from them:
        bool setHasInvisibleRule = false;
        for(unsigned i=0; i<disabledRules.size(); ++i)
        {
            if(parCompRules[i].n == 0 && Rules.getResult(i) != 0)
            {
                Stack.push_back(&parCompRules[i]);
                Stack.back()->n = ~0U;
                while(Stack.size())
                {
                    Node* node = Stack.back(); Stack.pop_back();
                    if(node->type == Node::PARCOMPRULE)
                    {
                        unsigned ruleInd = getParCompRulesNodeIndex(node);
                        if(disabledRules[ruleInd] == RULE_IS_ENABLED &&
                           !node->notGood)
                        {
                            ruleNumbers.push_back(ruleInd);
                            if(Rules.getResult(ruleInd) == 0)
                                setHasInvisibleRule = true;
                        }
                    }

                    if(calculateArcs) CalculateArcsForNode(node, currStateN);

                    for(unsigned j = 0; j < node->arcs.size(); ++j)
                    {
                        if(node->arcs[j]->n == 0)
                        {
                            Stack.push_back(node->arcs[j]);
                            Stack.back()->n = ~0U;
                        }
                    }
                }
            }
        } // for(unsigned i=0; i<disabledRules.size(); ++i)

        return setHasInvisibleRule;
    }

    // Return value:
    // In CSP and CFFD modes: true if the set contains all visible rules,
    //   false if it contains only invisible rules or no rules at all.
    // In DEADLOCKS mode: The return value is insignificant (always false).
    bool CalculateStubbornRules(vector<unsigned>& ruleNumbers,
                                const vector<lsts_index_t>& currStateN,
                                Parallel::SSType sstype)
    {
        ruleNumbers.clear();
        if(!FillDisabledTable(currStateN)) return false;


// Search strongly connected component with enabled rules
// ------------------------------------------------------

        // Initialize search graph:
        ResetSearchGraph();

        // The index number needed for the Tarjan algorithm:
        nodeIndex = 1;

        // Search starting from enabled (and unvisited) rule nodes:
        for(unsigned i=0; i<parCompRules.size(); ++i)
        {
            if(parCompRules[i].n == 0)// && disabledRules[i]==RULE_IS_ENABLED)
            {
                if(searchStubbornSet(&parCompRules[i],
                                     ruleNumbers, currStateN, sstype))
                {
                    return false;
                    // Note: This does not mean failure. See the comment
                    // at the beginning of the method.
                }
            }
        }

        // If DEADLOCKS is used, then a set just has to be found. Thus if
        // we get here with DEADLOCKS, it's a bug:
        check_claim(sstype >= Parallel::CSP,
                    "An unexpected error occurred at "
                    "StubbornSets::CalculateStubbornRules() (DEADLOCKS "
                    "search gave no result)");


// If there is no component with only invisible rules, CSP and CFFD must
// use all visible rules
// ---------------------------------------------------------------------

        bool setHasInvisibleRule =
            AddAllVisibleRules(ruleNumbers, currStateN, false);


// CSP and CFFD must maintain stable failures:
// ------------------------------------------

        // If the set has no invisible rules, check if any enabled rule is
        // invisible:
        if(!setHasInvisibleRule)
        {
            unsigned invisibleRule;
            for(invisibleRule=0; invisibleRule<disabledRules.size();
                ++invisibleRule)
            {
                // Note: No need to check parCompRules[invisibleRule].n == 0
                // because we would not be here at all if any of them
                // had it set to ~0U previously (setHasInvisibleRule would
                // be true in that case)
                if(disabledRules[invisibleRule] == RULE_IS_ENABLED &&
                   Rules.getResult(invisibleRule) == 0)
                {
                    break;
                }
            }

            if(invisibleRule < disabledRules.size()) // there is one
            {
                // Add to the set the strongly connected component containing
                // this rule or a component reachable from this one which
                // has an enabled invisible rule:
                // (Trick: This can be easily achieved by calling
                // searchStubbornSet() in DEADLOCKS mode.)
                nodeIndex = 1;
                tempSet.clear();
                check_claim(searchStubbornSet(&parCompRules[invisibleRule],
                                              tempSet, currStateN,
                                              Parallel::DEADLOCKS),
                            "An unexpected error occurred at "
                            "StubbornSets::CalculateStubbornRules() "
                            "(searching invisible component failed)");

                for(unsigned i=0; i<tempSet.size(); ++i)
                {
                    ruleNumbers.push_back(tempSet[i]);
                }
            }
        } // if(!setHasInvisibleRule)

        return true;
    }
};


//--------------------------------------------------------------------------
// Parallel composition using stubborn sets
//--------------------------------------------------------------------------
unsigned Parallel::CalculateWithStubbornSets(SSType sstype)
{
    check_claim(parrules.getStatePropRules().empty(),
                "State proposition rules are not supported with stubborn sets.");

// Initialization
//---------------
    currentSSType = sstype;

    StubbornSets stubborn(iLSTS, Rules);
    vector<unsigned> ruleNumbers;

    OutputLSTS::StatePointer currState;

    // Create the initial state
    for(unsigned ind=0; ind<Rules.get_iLSTSAmnt(); ind++)
    {
        currStateN[ind] = iLSTS[ind]->GetInitialState();
    }
    currState = oLSTS->CreateInitialState(currStateN);
    stack.Push(currState);

// Parallel composition
//---------------------
    int res;

    // Create transitions (and new states) in depth-first order:
    while((res=GetStateFromStack()))
    {
        oLSTS->StartAddingTransitions();

        // Calculate the stubborn set for the current state:
        bool setHasVisibleRules =
            stubborn.CalculateStubbornRules(ruleNumbers, currStateN, sstype);

        // Create transitions for all the rules in the set:
        for(unsigned rule=0; rule<ruleNumbers.size(); ++rule)
        {
            const vector<Rule>& curRule = Rules.getRule(ruleNumbers[rule]);
            newStateN = currStateN;

            for(unsigned ruleInd=0; ruleInd<curRule.size(); ++ruleInd)
            {
                const unsigned ilsts = curRule[ruleInd].iLSTS;
                const lsts_index_t trans = curRule[ruleInd].actionNumber;
                const lsts_index_t istate =
                    iLSTS[ilsts]->GetState(currStateN[ilsts], trans);

                newStateN[ilsts] = istate;
            }

            if(AddTransitions(ruleNumbers[rule]))
            {
                if(sstype == CFFD) // we have found a tau loop
                {
                    if(!setHasVisibleRules)
                    {
                        stubborn.AddAllVisibleRules(ruleNumbers, currStateN);
                        setHasVisibleRules = true;
                    }
                }
                else // the calculation was just interrupted
                {
                    oLSTS->DoneAddTransition();
                    return ReturnValue;
                }
            }
        }

        oLSTS->DoneAddTransition();
        /*
        // Check if it was a deadlock reject:
        if(oLSTS->DoneAddTransition()==0) // If no transitions were created
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
        */
    }

    return 0;
}
