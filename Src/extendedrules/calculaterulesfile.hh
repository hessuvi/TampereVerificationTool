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

#ifdef CIRC_CRULESFILE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_CRULESFILE_HH_
#define ONCE_CRULESFILE_HH_
#define CIRC_CRULESFILE_HH_


#include "erulesclp.hh"

#include "LSTS_File/ActionNamesAP.hh"
#include "ParRulesFile/StatePropRulesStore.hh"
#include "ParRulesFile/ParComponentsAP.hh"
#include "ParRulesFile/RulesAP.hh"
#include "LSTS_File/iLSTS_File.hh"

#include <map>
#include <string>
#include <vector>

class CalculateRulesFile: public oParComponentsAP,
                          public oRulesAP,
                          public oActionNamesAP
{
    typedef std::map<std::string, unsigned> Actions;

    struct RawRuleNode
    {
        unsigned inputFileNumber, actionNumber;
        std::string actionName;
    };

    struct RawRule
    {
        std::vector<RawRuleNode> nodes;
        std::string resActionName;
        unsigned resActionNumber;
    };

    struct RuleNode
    {
        unsigned inputFileNumber, actionNumber;

        RuleNode() {}
        RuleNode(const RawRuleNode& raw):
            inputFileNumber(raw.inputFileNumber),
            actionNumber(raw.actionNumber) {}
    };

    struct Rule
    {
        std::vector<RuleNode> nodes;
        unsigned resActionNumber;

        Rule() {}
        Rule(const RawRule& raw):
            nodes(raw.nodes.size()), resActionNumber(raw.resActionNumber)
        {
            for(unsigned i=0; i<raw.nodes.size(); ++i)
                nodes[i] = RuleNode(raw.nodes[i]);
        }
    };

    ERulesCLP& clp;
    std::vector<Rule> rules;
    std::vector<Actions> inputActions;
    Actions destActions;
    std::vector<std::string> inputFilenames;
    //std::vector<unsigned> inputNumberToProcessNumberLookup;
    std::vector<std::string> stickySPPrefix;
    std::map<std::string, std::string> aliases;

    bool proprulesRead;
    StatePropRulesStore proprules;

    typedef std::map<std::string, unsigned> StrIntMap;
    StrIntMap parameterNames;
    unsigned paramNameCount;


    // Class for reading the action names section from input LSTS:
    // ----------------------------------------------------------
    class ReadActionNames: public iActionNamesAP
    {
     public:
        ReadActionNames(InStream& is, Actions& d);

     private:
        InStream& instream;
        Actions& dest;

        void lsts_StartActionNames(Header&);
        void lsts_ActionName(lsts_index_t, const std::string&);
        void lsts_EndActionNames();
    };


    void ReadString(ITokenStream& is, std::string& dest);

    // Parse one rule line:
    // -------------------
    bool ConvertWildCards(RawRule& rawRule);
    void GenerateRules(const RawRule& rawRule);
    void ParseRule(ITokenStream& is);

    // Read the extended rlules file:
    // -----------------------------
    void ParseExtendedRulesFile();

    // ParComponents section:
    // ----------------------
    virtual bool lsts_doWeWriteParComponents();
    virtual void lsts_WriteParComponents(iParComponentsAP& pipe);

    // Rules section:
    // --------------
    virtual lsts_index_t lsts_numberOfRulesToWrite();
    virtual void lsts_WriteRules(iRulesAP& pipe);

    // Action names section:
    // ---------------------
    std::vector<std::string> sortedActionNames;
    lsts_index_t lsts_numberOfActionNamesToWrite();
    void lsts_WriteActionNames(iActionNamesAP& pipe);



//======================================================================
 public:
//======================================================================
    CalculateRulesFile(ERulesCLP& c);
};

#undef CIRC_CRULESFILE_HH_
#endif
