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

#include "calculaterulesfile.hh"

#include "ParRulesFile/ParRulesFile.hh"
#include "ParRulesFile/ParRulesSections/StatePropRulesSection.hh"
#include "error_handling.hh"

using std::map;
using std::string;
using std::vector;



//========================================================================
// CalculateRulesFile::ReadActionNames
//========================================================================
CalculateRulesFile::ReadActionNames::ReadActionNames(InStream& is, Actions& d):
    instream(is), dest(d)
{
    iLSTS_File reader(is);
    reader.AddActionNamesReader(*this);
    reader.SetNoReaderAction(iLSTS_File::IGNORE);
    reader.ReadFile();
}

void CalculateRulesFile::ReadActionNames::lsts_StartActionNames(Header&) {}
void CalculateRulesFile::ReadActionNames::lsts_ActionName
(lsts_index_t action_number, const string& name)
{
    if(action_number > 0)
    {
        check_claim(dest.find(name) == dest.end(),
                    string("In ")+instream.GiveFilename()+
                    ": Action name '"+name+"' found twice.");

        dest[name] = action_number;
    }
}
void CalculateRulesFile::ReadActionNames::lsts_EndActionNames() {}


//========================================================================
// Rule generation
//========================================================================
namespace
{
    typedef std::map<std::string, unsigned> Actions;
    struct WildCardIndex
    {
        unsigned node, ifileNumber;
        Actions::const_iterator iAction;
    };

    vector<string> params;
    vector<bool> appeared;

    // Returns the number at pattern[index+1] and assigns index to point
    // to the character following the number:
    unsigned getNumber(const string& pattern, unsigned& index)
    {
        unsigned result = 0;
        while(++index < pattern.size() && isdigit(pattern[index]))
            result = result*10 + pattern[index]-'0';
        return result;
    }

    bool matchActionName(const string& pattern, const string& name)
    {
        bool check = false;
        unsigned pi = 0, ni = 0;
        while(pi < pattern.size())
        {
            if(ni == name.size()) return false;
            char pc = pattern[pi], nc = name[ni];

            if(pc == '<') check = true;

            if(check && pc == '$')
            {
                unsigned number = getNumber(pattern, pi);
                params[number].clear();
                while(/*nc != ',' &&*/ nc != '>')
                {
                    params[number] += nc;
                    if(++ni == name.size()) break;
                    nc = name[ni];
                }
            }
            else if(check && pc == '\\')
            {
                unsigned number = getNumber(pattern, pi);
                for(unsigned i = 0; i < params[number].size(); ++i)
                {
                    if(ni == name.size() ||
                       params[number][i] != name[ni]) return false;
                    ++ni;
                }
            }
            else
            {
                if(pc != nc) return false;

                ++pi; ++ni;
            }
        }

        return true;
    }

    string getReplacementString(const string& pattern)
    {
        string result;
        for(unsigned i = 0; i < pattern.size(); ++i)
        {
            char c = pattern[i];

            if(c == '\\')
            {
                unsigned number = getNumber(pattern, i);
                result += params[number];
                --i;
            }
            else
                result += c;
        }
        return result;
    }

    bool wildCardReplace(string& name, bool checkNumber)
    {
        bool check = false;
        for(unsigned j = 0; j < name.size(); ++j)
        {
            char c = name[j];
            if(c == '<') check = true;

            else if(check && c == '$')
            {
                unsigned newj = j;
                unsigned number = getNumber(name, newj);
                if(params.size() <= number)
                {
                    params.resize(number+1);
                    appeared.resize(number+1, false);
                }
                if(appeared[number]) name[j] = '\\';
                else
                {
                    if(checkNumber) return false;
                    appeared[number] = true;
                }
                j = newj;
            }
        }
        return true;
    }
}

bool CalculateRulesFile::ConvertWildCards(RawRule& rawRule)
{
    appeared.clear(); appeared.resize(params.size(), false);
    for(unsigned i = 0; i < rawRule.nodes.size(); ++i)
    {
        wildCardReplace(rawRule.nodes[i].actionName, false);
    }
    return wildCardReplace(rawRule.resActionName, true);
}

void CalculateRulesFile::GenerateRules(const RawRule& rawRule)
{
    vector<WildCardIndex> indices;

    WildCardIndex tmp;
    for(unsigned i = 0; i < rawRule.nodes.size(); ++i)
    {
        if(!rawRule.nodes[i].actionName.empty())
        {
            tmp.node = i;
            tmp.ifileNumber = rawRule.nodes[i].inputFileNumber;
            tmp.iAction = inputActions[tmp.ifileNumber].begin();
            indices.push_back(tmp);
        }
    }

    if(indices.empty())
    {
        if(rawRule.resActionName.empty())
            rules.push_back(Rule(rawRule));
        return;
    }

    for(unsigned i = 0; i < indices.size();)
    {
        const Actions& a = inputActions[indices[i].ifileNumber];
        const string& pattern = rawRule.nodes[indices[i].node].actionName;
        while(indices[i].iAction != a.end() &&
              !matchActionName(pattern, indices[i].iAction->first))
        {
            ++(indices[i].iAction);
        }

        if(indices[i].iAction == a.end())
        {
            if(i == 0) return;
            else
            {
                indices[i].iAction = a.begin();
                --i;
                ++(indices[i].iAction);
            }
        }
        else ++i;
    }

    Rule rule(rawRule);
    while(true)
    {
        for(unsigned i = 0; i < indices.size(); ++i)
        {
            rule.nodes[indices[i].node].actionNumber =
                indices[i].iAction->second;
        }
        if(!rawRule.resActionName.empty())
        {
            string result = getReplacementString(rawRule.resActionName);
            Actions::const_iterator iter = destActions.find(result);
            if(iter != destActions.end())
            {
                rule.resActionNumber = iter->second;
            }
            else
            {
                rule.resActionNumber = destActions.size()+1;
                destActions[result] = rule.resActionNumber;
            }
        }
        rules.push_back(rule);

        unsigned curr = indices.size()-1;
        while(true)
        {
            const Actions& a = inputActions[indices[curr].ifileNumber];
            const string& pattern =
                rawRule.nodes[indices[curr].node].actionName;
            while(++(indices[curr].iAction) != a.end() &&
                  !matchActionName(pattern, indices[curr].iAction->first));

            if(indices[curr].iAction != a.end())
            {
                if(++curr == indices.size()) break;
            }
            else
            {
                if(curr == 0) return;
                indices[curr].iAction = a.begin();
                --curr;
            }
        }
    }
}


//========================================================================
// Rule parsing
//========================================================================
void CalculateRulesFile::ReadString(ITokenStream& is, string& dest)
{
    while(is.Peek().isString() ||
          (is.Peek().isIdentifier() &&
           !is.Peek().isIdentifier("begin")))
    {
        // Check if it's an alias:
        if(is.Peek().isIdentifier())
        {
            string alias = is.Get(TT::IDENTIFIER).stringData();
            map<string,string>::iterator iter =
                aliases.find(alias);
            check_claim(iter != aliases.end(),
                        string("Undeclared alias: ")+alias);
            dest += iter->second;
        }

        // If there's a string, append it to the current name:
        else //if(is.Peek().isString())
            dest += is.Get(TT::STRING).stringData();
    }
}

namespace
{
    bool hasWildCards(const string& name)
    {
        bool check = false;
        for(unsigned i=0; i<name.size(); ++i)
        {
            char c = name[i];
            if(c == '<') check = true;
            else if(check && c == '$') return true;
        }
        return false;
    }
}

void CalculateRulesFile::ParseRule(ITokenStream& is)
{
    RawRule rule;
    string punct;
    bool skipRule = false;

    // Expect '(' or '->':
    while((punct=is.Get(TT::PUNCTUATION,"(#->").stringData()) != "->")
    {
        // Read input file number:
        RawRuleNode node;
        node.inputFileNumber = is.Get(TT::NUMBER).numberData();
        check_claim(node.inputFileNumber > 0,
                    is.errorMsg
                    ("Invalid input file number (",
                     node.inputFileNumber, ")."));

        is.Get(TT::PUNCTUATION, ",");

        if(node.inputFileNumber >= inputActions.size() ||
           inputFilenames[node.inputFileNumber]=="")
        {
            if(is.Peek().isString() || is.Peek().isIdentifier())
            {
                string dummy;
                ReadString(is, dummy);
            }
            else
                is.Get();
        }
        else
        {
            // Read action name or number:
            if(is.Peek().isString() || is.Peek().isIdentifier())
            {
                string name;
                ReadString(is, name);

                check_claim(name.size(),
                            is.errorMsg("Invalid action name (empty)."));

                if(hasWildCards(name))
                {
                    node.actionName = name;
                }
                else
                {
                    const Actions &acts = inputActions[node.inputFileNumber];

                    Actions::const_iterator iter = acts.find(name);
                    if(iter == acts.end())
                    {
                        if(clp.noError())
                        {
                            //warn_ifnot(false, errmsg);
                            skipRule = true;
                        }
                        else
                        {
                            check_claim
                                (false,
                                 is.errorMsg
                                 (string("Action '")+name+
                                  "' not found in "+
                                  inputFilenames[node.inputFileNumber]));
                        }
                    }
                    else
                    {
                        node.actionNumber = iter->second;
                    }
                }
            }
            else if(is.Peek().isNumber())
            {
                node.actionNumber = is.Get(TT::NUMBER).numberData();
            }
            else
            {
                check_claim(false,
                            is.errorMsg(is.Get(),
                                        "expecting string or number."));
            }

            rule.nodes.push_back(node);
        }

        is.Get(TT::PUNCTUATION, ")");

    }

    if(rule.nodes.empty()) skipRule = true;

    // Read resulting action name:
    if(is.Peek().isString() || is.Peek().isIdentifier())
    {
        string name;
        ReadString(is, name);
        if(!skipRule)
        {
            if(hasWildCards(name))
            {
                rule.resActionName = name;
            }
            else
            {
                Actions::const_iterator iter = destActions.find(name);
                if(iter != destActions.end())
                {
                    rule.resActionNumber = iter->second;
                }
                else
                {
                    rule.resActionNumber = destActions.size()+1;
                    destActions[name] = rule.resActionNumber;
                }
            }
        }
    }
    else if(is.Peek().isNumber())
    {
        rule.resActionNumber = is.Get(TT::NUMBER).numberData();
    }
    else
        check_claim(false,
                    is.errorMsg("expecting string or number."));

    if(!skipRule && ConvertWildCards(rule))
    {
        GenerateRules(rule);
    }
}

#include <iostream>
using std::cout;
using std::endl;

// Read the extended rlules file:
// -----------------------------
void CalculateRulesFile::ParseExtendedRulesFile()
{
    // Initialize stream:
    ITokenStream is(clp.getInputStream());
    is.SetPunctuation("=#<#>#(#)#,#->");

    // Read the input file names and action names from them:
    //unsigned processNumberCounter = 1;
    while(is.Peek().isNumber())
    {
        unsigned ilsts = is.Get().numberData();
        check_claim(ilsts > 0,
                    is.errorMsg("Input number must be > 0."));
        if(inputActions.size() <= ilsts)
        {
            inputActions.resize(ilsts+1);
            inputFilenames.resize(ilsts+1);
            //inputNumberToProcessNumberLookup.resize(ilsts+1);
            stickySPPrefix.resize(ilsts+1);
        }
        is.Get(TT::PUNCTUATION, "=");

        if(is.Peek().isPunctuation("<"))
        {
            is.Get();
            stickySPPrefix[ilsts] = is.Get(TT::STRING).stringData();
            is.Get(TT::PUNCTUATION, ">");
        }

        inputFilenames[ilsts] = is.Get(TT::STRING).stringData();
        //inputNumberToProcessNumberLookup[ilsts] = processNumberCounter++;
        InStream input(inputFilenames[ilsts]);
        ReadActionNames(input, inputActions[ilsts]);

    }
    check_claim(inputActions.size() > 0,
                string("No input files given in ")+
                clp.getInputFilename(0));

    // Read aliases:
    while(is.Peek().isIdentifier())
    {
        string alias = is.Get(TT::IDENTIFIER).stringData();
        check_claim(aliases.find(alias) == aliases.end(),
                    is.errorMsg(string("Redefinition of alias '")+
                                alias+"'"));
        is.Get(TT::PUNCTUATION, "=");
        aliases[alias] = is.Get(TT::STRING).stringData();
    }

    // Read rules:
    while(!is.Peek().isEndOfFile())
    {
        if(is.Peek().isIdentifier("begin"))
        {
            is.Get();
            is.Get(TT::IDENTIFIER, "state_prop_rules");
            proprulesRead = true;
            iStatePropRulesSection reader(proprules); //, false);
            Header header;
            reader.Read(is, header);
            is.Get(TT::IDENTIFIER, "end");
            is.Get(TT::IDENTIFIER, "state_prop_rules");
            continue;
        }

        ParseRule(is);
    }
}

//========================================================================
// Rule writing
//========================================================================
// ParComponents section:
// ----------------------
bool CalculateRulesFile::lsts_doWeWriteParComponents() { return true; }
void CalculateRulesFile::lsts_WriteParComponents(iParComponentsAP& pipe)
{
    for(unsigned i=1; i<inputFilenames.size(); ++i)
    {
        /*
        if(inputNumberToProcessNumberLookup[i])
            pipe.lsts_Component(inputNumberToProcessNumberLookup[i],
                                inputFilenames[i], stickySPPrefix[i]);
        */
        if(!inputFilenames[i].empty())
            pipe.lsts_Component(i, inputFilenames[i], stickySPPrefix[i]);
    }
}

// Rules section:
// --------------
lsts_index_t CalculateRulesFile::lsts_numberOfRulesToWrite()
{ return rules.size(); }

void CalculateRulesFile::lsts_WriteRules(iRulesAP& pipe)
{
    for(unsigned i=0; i<rules.size(); ++i)
    {
        for(unsigned j=0; j<rules[i].nodes.size(); ++j)
        {
            /*
            pipe.lsts_Rule(inputNumberToProcessNumberLookup
                           [rules[i].nodes[j].inputFileNumber],
                           rules[i].nodes[j].actionNumber);
            */
            pipe.lsts_Rule(rules[i].nodes[j].inputFileNumber,
                           rules[i].nodes[j].actionNumber);
        }
        pipe.lsts_RuleDestinationTransitionNumber
            (rules[i].resActionNumber);
    }
}

// Action names section:
// ---------------------
lsts_index_t CalculateRulesFile::lsts_numberOfActionNamesToWrite()
{
    sortedActionNames.resize(destActions.size()+1);
    for(Actions::iterator i=destActions.begin();
        i!=destActions.end(); ++i)
    {
        sortedActionNames[i->second] = i->first;
    }
    return destActions.size();
}

void CalculateRulesFile::lsts_WriteActionNames(iActionNamesAP& pipe)
{
    for(unsigned i=1; i<sortedActionNames.size(); ++i)
    {
        pipe.lsts_ActionName(i, sortedActionNames[i]);
    }
}



CalculateRulesFile::CalculateRulesFile(ERulesCLP& c):
    clp(c), proprulesRead(false), paramNameCount(0)
{
    ParseExtendedRulesFile();

    oParrulesFile rules;
    rules.AddParComponentsWriter(*this);
    rules.AddRulesWriter(*this);
    rules.AddActionNamesWriter(*this);
    if(proprulesRead) rules.AddStatePropRulesWriter(proprules);
    rules.WriteFile(clp.getOutputStream());
}
