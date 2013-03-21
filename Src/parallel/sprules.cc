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

#include "sprules.hh"

#include <cctype>
#include <cstdlib>

using namespace std;

StatePropRules::StatePropRules(const std::vector<unsigned>&
                               componentNumberToIndexLookupTable):
    componentNumberToIndexLookup(componentNumberToIndexLookupTable),
    //immediateOTFVIChecks(true),
    resultSPNamesAmount(0),
    guardProcessNumber(0),
    hasOTFVIRules_(false)
{
/***OTFVI***
    resultSPContainer.separateOTFVI();
*/
}

/*
void StatePropRules::immediateOTFVIChecksActive(bool b)
{
    immediateOTFVIChecks = b;
}
*/


// ---------------------------------------------------------------------------
// Calculate resulting state propositions and return possible OTFVI bits
// ---------------------------------------------------------------------------
unsigned
StatePropRules::calculateStateProps(lsts_index_t destStateNumber,
                                    const vector<lsts_index_t>& stateNumbers)
{
    if(bytecode.empty()) return 0;

    unsigned retval = 0;

    for(unsigned i=0; i<bytecode.size(); ++i)
    {
        if(evaluateRule(bytecode[i], stateNumbers))
        {
            StatePropsContainer::StatePropsPtr spptr =
                resultSPContainer.getStateProps(destStateNumber);
            if(resultSPType[i] == SPNAME)
            {
                spptr.setProp(resultSPNumber[i]);
            }
            else if(resultSPType[i] == CUT)
            {
                spptr.setProp(resultSPNumber[i]);
                retval |= Cut_bit;
            }
            else
            {
                switch(resultSPType[i])
                {
                  case REJ:
                      retval |= Rej_bit;
                      break;

                  case DL_REJ:
                      retval |= DLRej_bit;
                      break;

                  case LL_REJ:
                      retval |= LLRej_bit;
                      break;

                  case INF_REJ:
                      break;

                  default:
                      check_claim(false,
                                  "An unexpected error ocurred in "
                                  "StatePropRules::calculateStateProps(). "
                                  "Please make a bug report.");
                }
            }
        }
    }

    return retval;
}


//===========================================================================
// Compilation and evaluation
//===========================================================================

// ---------------------------------------------------------------------------
// Values for byte-compiled commands
// ---------------------------------------------------------------------------
namespace
{
    const unsigned
    cb_Or = 0,     // pop two values, push their logical 'or'
    cb_And = 1,    // pop two values, push their logical 'and'
    cb_Negate = 2, // change truth value of top of stack
    cb_Push_true = 3,  // Push true to stack
    cb_Push_false = 4, // Push false to stack
    cb_Push_sp = 5;    // cb_Push_sp, input-LSTS no, state prop no
    //cb_Push_cut = 6,   // cb_Push_*, input-LSTS no
    //cb_Push_rej = 7,
    //cb_Push_dlrej = 8,
    //cb_Push_llrej = 9,
    //cb_Push_infrej = 10;
}


// ---------------------------------------------------------------------------
// Parse sticky stateprops to hide
// ---------------------------------------------------------------------------
void StatePropRules::ParseHide(const string& line)
{
    for(unsigned ind = 4; ind < line.size(); ++ind)
    {
        check_claim(line[ind] == StringDelim,
                    string
                    ("Syntax error in 'hide' command: Expecting \", found ")+
                    line[ind] + " instead.");
        ++ind;
        check_claim(line[ind] == '/',
                    "Error in 'hide' command: Only sticky propositions can "
                    "be hidden");
        unsigned ind2 = ind;
        while(line[ind2] != StringDelim)
        {
            ++ind2;
            check_claim(ind2 < line.size(),
                        "Syntax error in 'hide' command: Premature end of "
                        "string");
        }

        stickyPropsToHide.insert(line.substr(ind, ind2-ind));

        ind = ind2+1;
        check_claim(ind == line.size() || line[ind] == ',',
                    "Syntax error in 'hide' command: , expected after state "
                    "prop name.");
    }
}

// ---------------------------------------------------------------------------
// Return which keyword the string contains:
// ---------------------------------------------------------------------------
StatePropRules::OTFVIKeyword
StatePropRules::whichKeyword(const string& keyword)
{
    static const char* OTFVIKeywordStrings[] =
    { /*"cut",*/ "rej", "dl_rej", "ll_rej", "inf_rej"
    };
    static const OTFVIKeyword codes[] =
    { /*CUT,*/ REJ, DL_REJ, LL_REJ, INF_REJ
    };

    for(unsigned i=0; i<4; ++i)
        if(keyword == OTFVIKeywordStrings[i])
            return codes[i];
    return NOKEYWORD;
}


namespace
{
// ---------------------------------------------------------------------------
// Make error message for the rule parser:
// ---------------------------------------------------------------------------
    inline string MakeErrMsg(const string& msg,
                             vector<string>::size_type which)
    {
        return valueToMessage("In state prop rule ", which, string(": ")+msg);
    }

// ---------------------------------------------------------------------------
// Issue error for premature rule ending:
// ---------------------------------------------------------------------------
    inline void CheckEndOfRule(const string& rule, string::size_type ind,
                               vector<string>::size_type which)
    {
        check_claim(ind!=rule.size(),
                    MakeErrMsg("Syntax error at end of rule: "
                               "Premature end of rule.", which));
    }

// ---------------------------------------------------------------------------
// Compare str1 (starting from 'ind1') and str2 up to length of str2 (if str1
// is shorter than str2, they are not equal)
// ---------------------------------------------------------------------------
    inline bool StrComp(const string& str1, string::size_type ind1,
                        const char* str2)
    {
        for(unsigned ind2 = 0; str2[ind2]; ++ind1, ++ind2)
        {
            if(ind1 == str1.size()) return false;
            if(str1[ind1] != str2[ind2]) return false;
        }
        return true;
    }
}

// ---------------------------------------------------------------------------
// Expect string or keyword
// (actually just code repetition avoiding for CheckSyntax())
// ---------------------------------------------------------------------------
StatePropRules::OTFVIKeyword
StatePropRules::expectSPNameOrKeyword(const string& rule,
                                      StrVec::size_type which,
                                      string::size_type& ind,
                                      const string& after)
{
    CheckEndOfRule(rule, ind, which);
    check_claim(rule[ind]==StringDelim || isalpha(rule[ind]),
                MakeErrMsg(string("Syntax error at '")+rule[ind]+
                           "': Expecting string or keyword", which));

    if(rule[ind] == StringDelim)
    {
        // Expect string
        string::size_type start_ind = ind+1;
        do
        {
            ++ind;
            CheckEndOfRule(rule, ind, which);
        } while(rule[ind] != StringDelim);
        check_claim(ind > start_ind,
                    MakeErrMsg(string("Empty state prop name after '")+after+
                               "'", which));
        ++ind;
        return SPNAME;
    }
    else
    {
        // Expect keyword
        string keyword;
        OTFVIKeyword kwtype;

        while(ind<rule.size() && (isalpha(rule[ind]) || rule[ind]=='_'))
        {
            keyword += rule[ind++];
        }
        kwtype = whichKeyword(keyword);
        check_claim(kwtype != NOKEYWORD,
                    MakeErrMsg(string("Invalid keyword after '")+after+"'",
                               which));
        return kwtype;
    }
}


// ---------------------------------------------------------------------------
// Check the syntax correctness of the rule
// ---------------------------------------------------------------------------
bool StatePropRules::CheckSyntax(string& rule, StrVec::size_type which)
{
    check_claim(!rule.empty(), MakeErrMsg("Empty rule.", which));

    if(rule.substr(0,4) == "hide")
    {
        ParseHide(rule);
        return false;
    }

    bool hadOnlyOneInputLSTSNumber = true;
    unsigned inputLSTSNumber = 0;

    int parenthesisCnt = 0;

    string::size_type ind = 0;
    while(true)
    {
        // If '('
        if(rule[ind] == '(')
        {
            ++parenthesisCnt;
            ++ind;
            CheckEndOfRule(rule, ind, which);
            continue;
        }

        // If '!'
        if(rule[ind] == '!')
        {
            ++ind;
            CheckEndOfRule(rule, ind, which);
            continue;
        }

        // Expect true/false
        if(StrComp(rule, ind, "true")) ind += 4;
        else if(StrComp(rule, ind, "false")) ind += 5;
        else
        {
            // Expect number
            check_claim(isdigit(rule[ind]),
                        MakeErrMsg(string("Syntax error at '")+rule[ind]+
                                   "': Expecting number.", which));
            unsigned inputNumber = atoi(rule.c_str()+ind);

            check_claim(inputNumber > 0,
                        MakeErrMsg("Component number number must be > 0",
                                   which));

            if(inputLSTSNumber == 0) inputLSTSNumber = inputNumber;
            else if(inputLSTSNumber != inputNumber)
                hadOnlyOneInputLSTSNumber = false;

            while(++ind < rule.size() && isdigit(rule[ind]));

            // Expect '.'
            CheckEndOfRule(rule, ind, which);
            check_claim(rule[ind]=='.',
                        MakeErrMsg(string("Syntax error at '")+rule[ind]+
                                   "': Expecting '.'", which));
            ++ind;

            // Expect string or keyword
            check_claim
                (expectSPNameOrKeyword(rule, which, ind,
                                       valueToMessage("", inputNumber, ".")) ==
                 SPNAME,
                 MakeErrMsg("OTFVI keywords are not supported anymore "
                            "as input propositions.", which));
            CheckEndOfRule(rule, ind, which);
        }

        // if ')'
        while(rule[ind] == ')')
        {
            check_claim(--parenthesisCnt >= 0,
                        MakeErrMsg("Unmatched ')'", which));
            ++ind;
            CheckEndOfRule(rule, ind, which);
        }

        // Check for '->'
        if(rule[ind] == '-')
        {
            ++ind;
            check_claim(ind<rule.size() && rule[ind]=='>',
                        MakeErrMsg("Syntax error after '-' "
                                   "(expecting '->')", which));
            rule[ind-1] = RuleEndMark;
            rule.erase(ind, 1);
            break;
        }

        // Expect operator
        check_claim(rule[ind]=='&' || rule[ind]=='|',
                    MakeErrMsg(string("Syntax error at '")+rule[ind]+
                               "': Expecting operator (& or |).", which));
        ++ind;
        CheckEndOfRule(rule, ind, which);
    }

    check_claim(parenthesisCnt == 0, MakeErrMsg("Unmatched '('", which));

    // Check for string or keyword after '->'
    OTFVIKeyword kwtype = expectSPNameOrKeyword(rule, which, ind, "->");

    // Check for '!'
    /*
    if(ind < rule.size())
    {
        check_claim(rule[ind] == '!',
                    MakeErrMsg(string("Unexpected character '")+rule[ind]+
                               "' after rule result.", which));
        check_claim(++ind = rule.size(),
                    MakeErrMsg("Unexpected character(s) at end of rule.",
                               which));
    */
        check_claim(ind == rule.size(),
                    MakeErrMsg("Unexpected character(s) at end of rule.",
                               which));

        string kwtypeName;
        switch(kwtype)
        {
          //case SPNAME: kwtypeName = "State prop name"; break;
          //case LL_REJ: kwtypeName = "Livelock rejection"; break;
          case INF_REJ: kwtypeName = "Infinity rejection"; break;
          default: kwtypeName = "";
        }
        check_claim(kwtypeName == "",
                    MakeErrMsg(kwtypeName+" is not yet supported as OTFVI "
                               "checking command.", which));

        if(kwtype == LL_REJ)
        {
            check_claim(guardProcessNumber == 0,
                        MakeErrMsg("Only one ll_rej rule can be specified.",
                                   which));
            check_claim(inputLSTSNumber > 0,
                        MakeErrMsg("One input component is required for a "
                                   "ll_rej rule.", which));
            check_claim(hadOnlyOneInputLSTSNumber,
                        MakeErrMsg("Only one input component can be used in "
                                   "a ll_rej rule.", which));

            guardProcessNumber = componentNumberToIndexLookup[inputLSTSNumber];
        }

    //}

    return true;
}

// ---------------------------------------------------------------------------
// Fills the resulting state props data containers
// ---------------------------------------------------------------------------
void StatePropRules::BuildResultingStateProps()
{
    // We can trust the correctness of the rule at this point as it has been
    // checked by CheckSyntax(), so no obsolete correctness checks are needed

    typedef map<string, unsigned> NameMap;
    NameMap spNames;
    for(StrVec::size_type i=0; i<sprules.size(); ++i)
    {
        const string& rule = sprules[i];
        string::size_type sind = 0;
        while(rule[sind++] != RuleEndMark);
        string::size_type eind = rule.size(); // sind;
        //while(eind<rule.size() && rule[eind]!='!') ++eind;

        // If result is a state prop name:
        if(rule[sind] == StringDelim)
        {
            ++sind; --eind;

            if(rule[sind]=='%')
                resultSPType.push_back(CUT);
            else
                resultSPType.push_back(SPNAME);

            //resultImmediate.push_back(false);
            string name = rule.substr(sind, eind-sind);
            NameMap::iterator iter = spNames.find(name);
            if(iter != spNames.end())
            {
                resultSPNumber.push_back(iter->second);
            }
            else
            {
                ++resultSPNamesAmount;
                spNames[name] = resultSPNamesAmount;
                resultSPNumber.push_back(resultSPNamesAmount);
                resultSPContainer.getStatePropName(resultSPNamesAmount) = name;
            }
        }
        // If result is an OTFVI:
        else
        {
            resultSPType.push_back(whichKeyword(rule.substr(sind, eind-sind)));
            //resultImmediate.push_back(eind != rule.size());
            resultSPNumber.push_back(0);
            hasOTFVIRules_ = true;
        }
    }

    resultSPContainer.setMaxVal(resultSPNamesAmount);
}

// ---------------------------------------------------------------------------
// Fills the iSPNames data structure. This is used by the compilation process.
// ---------------------------------------------------------------------------
void StatePropRules::BuildInputSPNameMaps()
{
    iSPNames.resize(stateprops.size());
    for(unsigned i=1; i<stateprops.size(); ++i)
    {
        if(stateprops[i])
        {
            const StatePropsContainer& cont = *(stateprops[i]);
            for(unsigned j=1; j<=cont.getMaxStatePropNameNumber(); ++j)
            {
                iSPNames[i][cont.getStatePropName(j)] = j;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Tries to optimize the compiled code for speed
// (Currently just removes multiple negations)
// ---------------------------------------------------------------------------
static void Optimize(vector<unsigned>& bytecode)
{
    // Remove multiple negations:
    for(vector<unsigned>::iterator i = bytecode.begin(); i != bytecode.end();)
    {
        if(i+1!=bytecode.end() && *i==cb_Negate && *(i+1)==cb_Negate)
        {
            i = bytecode.erase(i, i+2);
        }
        else
        {
            if(*i == cb_Push_sp) i+=3;
            //else if(*i >= cb_Push_cut) i+=2;
            else ++i;
        }
    }
}

// ---------------------------------------------------------------------------
// Parse and compile rules
// ---------------------------------------------------------------------------
void StatePropRules::CompileRules()
{
    // We can trust the correctness of the rule at this point as it has been
    // checked by CheckSyntax(), so no obsolete correctness checks are needed

    BuildResultingStateProps();

    BuildInputSPNameMaps();

    for(StrVec::size_type i=0; i<sprules.size(); ++i)
    {
        ruleBeingCompiled = i+1;
        const string& rule = sprules[i];
        string::size_type eind = 0;
        while(rule[eind+1] != RuleEndMark) ++eind;
        bytecode.resize(bytecode.size()+1);
        CompileSubstring(rule, 0, eind, bytecode.back());
        Optimize(bytecode.back());
    }

    // Free unused memory:
    iSPNames.clear();
    StrVec dummy;
    sprules.swap(dummy);
}

// ---------------------------------------------------------------------------
// Returns true if the section is completely enclosed by a pair of parentheses
// ---------------------------------------------------------------------------
namespace
{
    inline bool completelyEnclosed(const string& rule,
                                   string::size_type sind,
                                   string::size_type eind)
    {
        if(rule[sind]=='(' && rule[eind]==')')
        {
            unsigned i, cnt=1;
            for(i=sind+1; i<eind; ++i)
            {
                if(rule[i]=='(') ++cnt;
                else if(rule[i]==')') --cnt;
                if(cnt==0) break;
            }
            if(i==eind) return true;
        }
        return false;
    }
}


// ---------------------------------------------------------------------------
// Core compilation routine. Compiles given section of the rule.
// The function is recursive.
// ---------------------------------------------------------------------------
void StatePropRules::CompileSubstring(const string& rule,
                                      string::size_type sind,
                                      string::size_type eind,
                                      vector<unsigned>& dest)
{
    // We can trust the correctness of the rule at this point as it has been
    // checked by CheckSyntax(), so no obsolete correctness checks are needed

    // Skip enclosing parentheses:
    if(completelyEnclosed(rule, sind, eind))
    {
        CompileSubstring(rule, sind+1, eind-1, dest);
        return;
    }

    // Compile '!' before enclosing parentheses:
    if(rule[sind] == '!' && completelyEnclosed(rule, sind+1, eind))
    {
        CompileSubstring(rule, sind+2, eind-1, dest);
        dest.push_back(cb_Negate);
        return;
    }

    // Compile operators | and &:
    static const char* const Ops = "|&";
    for(unsigned op = 0; op < 2; ++op)
    {
        unsigned cnt = 0;
        for(unsigned i = eind; i > sind; --i)
        {
            if(rule[i] == ')') ++cnt;
            else if(rule[i] == '(') --cnt;
            if(cnt==0 && rule[i]==Ops[op])
            {
                CompileSubstring(rule, sind, i-1, dest);
                CompileSubstring(rule, i+1, eind, dest);
                dest.push_back(op);
                return;
            }
        }
    }

    // At this stage we have an operand alone (possibly with a '!').

    // Compile the '!' if there is one:
    if(rule[sind] == '!')
    {
        CompileSubstring(rule, sind+1, eind, dest);
        dest.push_back(cb_Negate);
        return;
    }

    // Check if true/false
    if(StrComp(rule, sind, "true"))
    {
        dest.push_back(cb_Push_true);
    }
    else if(StrComp(rule, sind, "false"))
    {
        dest.push_back(cb_Push_false);
    }
    else
    {
        // Compile the operand:
        unsigned componentNumber = atoi(rule.c_str()+sind);
        unsigned iLSTSNumber = componentNumberToIndexLookup[componentNumber];
        while(rule[sind++] != '.');
        //if(rule[sind] == StringDelim) // if it's a state prop name
        //{
            string name = rule.substr(sind+1, eind-sind-1);
            SPNameMap::const_iterator iter = iSPNames[iLSTSNumber].find(name);
            check_claim(iter != iSPNames[iLSTSNumber].end(),
                        MakeErrMsg
                        (valueToMessage
                         (string("Couldn't find state proposition \"")+name+
                          "\" in input LSTS ", componentNumber,
                          " as required by this rule."), ruleBeingCompiled));
            dest.push_back(cb_Push_sp);
            dest.push_back(iLSTSNumber);
            dest.push_back(iter->second);
        //}
        /*
        else // if it's a keyword
        {
            switch(whichKeyword(rule.substr(sind, eind-sind+1)))
            {
              case CUT: dest.push_back(cb_Push_cut); break;
              case REJ: dest.push_back(cb_Push_rej); break;
              case DL_REJ: dest.push_back(cb_Push_dlrej); break;
              case LL_REJ: dest.push_back(cb_Push_llrej); break;
              case INF_REJ: dest.push_back(cb_Push_infrej); break;
              default:
                  check_claim(false,
                              "An unexpected error happened while compiling "
                              "state proposition rules. Please make a bug "
                              "report.");
            }
            dest.push_back(iLSTSNumber);
        }
        */
    }
}


// ---------------------------------------------------------------------------
// Rule evaluation
// ---------------
// stateNumbers is 0-based (ie. state number of lsts 1 is at stateNumbers[0])
// ---------------------------------------------------------------------------
bool StatePropRules::evaluateRule(const vector<unsigned>& rulebc,
                                  const vector<lsts_index_t>& stateNumbers)
{
    evalStack.clear();
    for(vector<unsigned>::const_iterator i = rulebc.begin();
        i != rulebc.end(); ++i)
    {
        switch(*i)
        {
          case cb_Or:
              {
                  bool top = evalStack.back();
                  evalStack.pop_back();
                  evalStack.back() = evalStack.back() || top;
                  break;
              }

          case cb_And:
              {
                  bool top = evalStack.back();
                  evalStack.pop_back();
                  evalStack.back() = evalStack.back() && top;
                  break;
              }

          case cb_Negate:
              evalStack.back() = !evalStack.back();
              break;

          case cb_Push_true:
              evalStack.push_back(true);
              break;

          case cb_Push_false:
              evalStack.push_back(false);
              break;

          case cb_Push_sp:
              {
                  unsigned ilsts = *(++i);
                  unsigned propNumber = *(++i);
                  evalStack.push_back
                      (stateprops[ilsts]->
                       getStateProps
                       (stateNumbers[ilsts-1]).isPropSet(propNumber));
                  break;
              }

          /*
          case cb_Push_cut:
              {
                  unsigned ilsts = *(++i);
                  evalStack.push_back
                      (stateprops[ilsts]->
                       getOTFVI(stateNumbers[ilsts-1]).isCutState());
                  break;
              }

          case cb_Push_rej:
              {
                  unsigned ilsts = *(++i);
                  evalStack.push_back
                      (stateprops[ilsts]->
                       getOTFVI(stateNumbers[ilsts-1]).isRejectState());
                  break;
              }

          case cb_Push_dlrej:
              {
                  unsigned ilsts = *(++i);
                  evalStack.push_back
                      (stateprops[ilsts]->
                       getOTFVI(stateNumbers[ilsts-1]).isDeadlockRejectState());
                  break;
              }

          case cb_Push_llrej:
              {
                  unsigned ilsts = *(++i);
                  evalStack.push_back
                      (stateprops[ilsts]->
                       getOTFVI(stateNumbers[ilsts-1]).isLivelockRejectState());
                  break;
              }

          case cb_Push_infrej:
              {
                  unsigned ilsts = *(++i);
                  evalStack.push_back
                      (stateprops[ilsts]->
                       getOTFVI(stateNumbers[ilsts-1]).isInfinityRejectState());
                  break;
              }
          */
        }
    }
    return evalStack.back();
}


//===========================================================================
// File reading
//===========================================================================
void StatePropRules::addStatePropsContainer(unsigned fileno,
                                            const StatePropsContainer& sp,
                                            const std::string& stickySPPrefix)
{
    if(stateprops.size() <= fileno) stateprops.resize(fileno+1);

    stateprops[fileno] = &sp;

    // Add automatically rules for sticky stateprops and cut stateprops:
    for(unsigned spnameind = 1;
        spnameind <= sp.getMaxStatePropNameNumber(); ++spnameind)
    {
        const string& spname = sp.getStatePropName(spnameind);

        if((spname[0] == '/' &&
            stickyPropsToHide.find(spname) == stickyPropsToHide.end()) ||
           spname[0] == '%')
        {
            string rule = valueToMessage("", fileno, ".");
            rule += StringDelim; rule += spname; rule += StringDelim;
            rule += RuleEndMark;
            rule += StringDelim;
            rule += spname[0];
            rule += stickySPPrefix;
            rule += spname.substr(1);
            rule += StringDelim;
            sprules.push_back(rule);
        }
    }
}

void StatePropRules::doneAddingStatePropsContainers()
{
    if(!sprules.empty()) CompileRules();
}


void StatePropRules::lsts_StartStatePropRules(Header&) {}

void StatePropRules::lsts_StatePropRule(const string& rule)
{
    string cpy = rule;
    if(CheckSyntax(cpy, sprules.size()+1))
        sprules.push_back(cpy);
}

void StatePropRules::lsts_EndStatePropRules() {}
