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
#include "StateProps.hh"

#include "LSTS_File/iLSTS_File.hh"
#include "ParRulesFile/ParRulesFile.hh"
#include "ParRulesFile/ParComponentsAP.hh"
#include "ParRulesFile/RulesAP.hh"
#include "ParRulesFile/StatePropRulesAP.hh"
#include "ParRulesFile/ParRulesSections/StatePropRulesSection.hh"
#include "LSTS_File/ActionNamesStore.hh"

#include <string>
#include <vector>
#include <map>

class RuleCreator: public iParComponentsAP, public oParComponentsAP,
                   public iRulesAP, public oRulesAP,
                   public iStatePropRulesAP, public oStatePropRulesAP
{
    typedef std::map<std::string, unsigned> StrIntMap_t;

    const std::string& testerFilename;

    StrIntMap_t testerActions;
    const StatePropsContainer& testerStateProps;
    ActionNamesStore rulesActions;

    struct ParComponent
    {
        std::string filename, prefix;
        ParComponent(const std::string& f, const std::string& p):
            filename(f), prefix(p)
        {}
        ParComponent() {}
    };

    struct RuleNode
    {
        unsigned lstsNumber, actionNumber;
        RuleNode(unsigned l, unsigned a): lstsNumber(l), actionNumber(a) {}
    };

    struct Rule
    {
        std::vector<RuleNode> nodes;
        unsigned resultActionNumber;
    };

    std::vector<ParComponent> parComponents;
    std::vector<Rule> rules;
    std::vector<std::string> statePropRules;

    unsigned currentRule;

    void lsts_StartParComponents() {}
    void lsts_Component(unsigned fileNumber, const std::string& filename,
                        const std::string& prefix)
    {
        if(parComponents.size() <= fileNumber)
            parComponents.resize(fileNumber+1);
        parComponents[fileNumber] =
            ParComponent(filename, prefix);
    }
    void lsts_EndParComponents() {}

    void lsts_StartRules(unsigned amnt)
    {
        rules.resize(amnt);
        currentRule = 0;
    }
    void lsts_Rule(unsigned fileNumber, unsigned actionNumber)
    {
        rules[currentRule].nodes.push_back(RuleNode(fileNumber, actionNumber));
    }
    void lsts_RuleDestinationTransitionNumber(unsigned actionNumber)
    {
        rules[currentRule++].resultActionNumber = actionNumber;
    }
    void lsts_EndRules() {}

    void lsts_StartStatePropRules(Header&) {}
    void lsts_StatePropRule(const std::string& rule)
    {
        statePropRules.push_back(rule);
    }
    void lsts_EndStatePropRules() {}



    bool lsts_doWeWriteParComponents() { return true; }

    void lsts_WriteParComponents(iParComponentsAP& out)
    {
        for(unsigned i = 1; i < parComponents.size(); ++i)
        {
            out.lsts_Component(i, parComponents[i].filename,
                               parComponents[i].prefix);
        }
        out.lsts_Component(parComponents.size(), testerFilename, "");
    }


    unsigned lsts_numberOfRulesToWrite()
    {
        return rules.size();
    }

    void lsts_WriteRules(iRulesAP& out)
    {
        for(unsigned i = 0; i < rules.size(); ++i)
        {
            for(unsigned j = 0; j < rules[i].nodes.size(); ++j)
            {
                out.lsts_Rule(rules[i].nodes[j].lstsNumber,
                              rules[i].nodes[j].actionNumber);
            }

            unsigned result = rules[i].resultActionNumber;
            if(result)
            {
                StrIntMap_t::const_iterator iter =
                    testerActions.find(rulesActions.GiveActionName(result));

                if(iter != testerActions.end())
                    out.lsts_Rule(parComponents.size(), iter->second);
            }

            out.lsts_RuleDestinationTransitionNumber(result);
        }
    }


    bool lsts_doWeWriteStatePropRules() { return true; }

    void writeSPRule(const std::string& name, const std::string& result,
                     iStatePropRulesAP& out)
    {
        out.lsts_StatePropRule
            (valueToMessage("", parComponents.size(),
                            std::string(".\"")+name+"\" -> "+result));
    }

    void lsts_WriteStatePropRules(iStatePropRulesAP& out)
    {
        for(unsigned i = 0; i < statePropRules.size(); ++i)
        {
            out.lsts_StatePropRule(statePropRules[i]);
        }
        for(unsigned i = 1;
            i <= testerStateProps.getMaxStatePropNameNumber(); ++i)
        {
            const std::string& propName =
                testerStateProps.getStatePropName(i);

            if(propName == "/rej") writeSPRule(propName,"rej", out);
            else if(propName == "/dl_rej") writeSPRule(propName,"dl_rej", out);
            else if(propName == "/ll_rej") writeSPRule(propName,"ll_rej", out);
        }
    }

 public:
    RuleCreator(InStream& rules, const std::string& filename,
                ActionNamesStore& actions,
                const StatePropsContainer& stateProps,
                OutStream& output):
        testerFilename(filename),
        testerStateProps(stateProps),
        rulesActions(true)
    {
        iParrulesFile rulesreader(rules);
        rulesreader.AddParComponentsReader(*this);
        rulesreader.AddRulesReader(*this);
        rulesreader.AddStatePropRulesReader(*this);
        rulesreader.AddActionNamesReader(rulesActions);
        rulesreader.ReadFile();

        for(unsigned i = 1; i <= actions.size(); ++i)
            testerActions[actions.GiveActionName(i)] = i;

        oParrulesFile writer;
        writer.AddParComponentsWriter(*this);
        writer.AddRulesWriter(*this);
        writer.AddStatePropRulesWriter(*this);
        writer.AddActionNamesWriter(rulesActions);
        writer.WriteFile(output);
    }
};


void CalculateNewRules(InStream& lsts, InStream& rules, OutStream& output)
{
    iLSTS_File lstsreader(lsts);
    ActionNamesStore testerActions(true);
    StatePropsContainer testerStateProps;

    lstsreader.AddActionNamesReader(testerActions);
    lstsreader.AddStatePropsReader(testerStateProps);
    lstsreader.SetNoReaderAction(iLSTS_File::IGNORE);
    lstsreader.ReadFile();

    RuleCreator(rules, lsts.GiveFilename(),
                testerActions, testerStateProps, output);
}

class CLP: public TvtCLP
{
 public:
    CLP(): TvtCLP("Adds rules for a given tester process into a given rules "
                  "file.")
    {}

    void help(const std::string& programName)
    {
        using std::cerr; using std::endl;
        printProgramDescription();
        cerr << endl << "Usage:\n " << programName
             << " [<options>] <tester lsts> <rules file> [<output rules file>]"
             << endl << endl;
        printOptionsHelp();
        printStdinStdoutNotice();
        printCopyrightNotice();
    }
};

int main(int argc, char* argv[])
{
#ifndef NO_EXCEPTIONS
  try {
#endif
    CLP clp;
    clp.setRequiredFilenamesCnt(2);
    if(!clp.parseCommandLine(argc, argv)) return 1;

    CalculateNewRules(clp.getInputStream(0), clp.getInputStream(1),
                      clp.getOutputStream(2));
#ifndef NO_EXCEPTIONS
  } catch(...) { return 1; }
#endif
    return 0;
}
