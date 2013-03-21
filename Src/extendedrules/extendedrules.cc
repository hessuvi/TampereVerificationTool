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

#include "erulesclp.hh"
#include "calculaterulesfile.hh"

#include "ParRulesFile/ParRulesFile.hh"
#include "ParRulesFile/ParRulesSections/StatePropRulesSection.hh"
#include "error_handling.hh"


using std::string;
using std::vector;

class CalculateERulesFile: public iParComponentsAP,
                           public iRulesAP
{
    class ActionNames: public iActionNamesAP
    {
        vector<string> names;

     public:
        void lsts_StartActionNames(Header& h)
        {
            names.resize(h.GiveActionCnt());
        }
        void lsts_ActionName(lsts_index_t action_number,
                             const std::string& action_name)
        {
            names[action_number-1] = action_name;
        }
        void lsts_EndActionNames() {}

        const string& getName(unsigned actionNumber) const
        {
            return names[actionNumber-1];
        }
        unsigned size() const { return names.size()+1; }
    };

    vector<ActionNames> iActionNames;
    vector<string> iFileNames;
    ActionNames resActionNames;
    StatePropRulesStore propRules;
    OTokenStream os;
    iParrulesFile rules;

    virtual void lsts_StartParComponents() {}

    virtual void lsts_Component( unsigned filenameCnt,
                                 const std::string& filename,
                                 const std::string& prefix)
    {
        os.PutNumber(filenameCnt);
        os.PutPunctuation("=");
        if(prefix.size())
        {
            os.PutPunctuation("<");
            os.PutString(prefix);
            os.PutPunctuation(">");
        }
        os.PutString(filename);
        os.Endl();

        if(iActionNames.size() < filenameCnt)
        {
            iActionNames.resize(filenameCnt);
            iFileNames.resize(filenameCnt);
        }

        iFileNames[filenameCnt-1] = filename;
        InStream is(filename);
        iLSTS_File reader(is);
        reader.AddActionNamesReader(iActionNames[filenameCnt-1]);
        reader.SetNoReaderAction(iLSTS_File::IGNORE);
        reader.ReadFile();
    }

    virtual void lsts_EndParComponents() { os.Endl(); }

    virtual void lsts_StartRules(unsigned) {}

    virtual void lsts_Rule(lsts_index_t inputLSTS,
                           lsts_index_t transitionNumber)
    {
        check_claim(transitionNumber <= iActionNames[inputLSTS-1].size(),
                    rules.its().errorMsg
                    (iFileNames[inputLSTS-1]+" has no action number ",
                     transitionNumber, "."));

        os.PutPunctuation("(");
        os.PutNumber(inputLSTS);
        os.PutPunctuation(",");
        os.PutString
            (iActionNames[inputLSTS-1].getName(transitionNumber));
        os.PutPunctuation(")");
    }

    virtual void lsts_RuleDestinationTransitionNumber(lsts_index_t
                                                      transitionNumber)
    {
        os.spaceOn();
        os.PutPunctuation("->");
        os.spaceOff();
        if(transitionNumber == 0)
            os.PutNumber(0);
        else
            os.PutString(resActionNames.getName(transitionNumber));
        os.Endl();
    }

    virtual void lsts_EndRules() {}


 public:
    CalculateERulesFile(ERulesCLP& clp):
        os(clp.getOutputStream()),
        rules(clp.getInputStream())
    {
        os.spaceOff();

        rules.AddActionNamesReader(resActionNames);
        rules.AddParComponentsReader(*this);
        rules.AddRulesReader(*this);
        rules.AddStatePropRulesReader(propRules);
        rules.ReadFile();

        if(propRules.lsts_doWeWriteStatePropRules())
        {
            Header dummy;

            oStatePropRulesSection spwriter(os);
            spwriter.lsts_StartStatePropRules(dummy);
            propRules.lsts_WriteStatePropRules(spwriter);
            spwriter.lsts_EndStatePropRules();
            os.Endl();
        }
    }
};


int main(int argc, char* argv[])
{
#ifndef NO_EXCEPTIONS
  try {
#endif
    ERulesCLP clp("Converts extended rulesfile to regular parallel "
                  "composition rules file.");
    if(!clp.parseCommandLine(argc, argv)) return 1;

    if(clp.reverseConv())
        CalculateERulesFile tmp(clp);
    else
        CalculateRulesFile tmp(clp);

#ifndef NO_EXCEPTIONS
  } catch(...) { return 1; }
#endif
    return 0;
}
