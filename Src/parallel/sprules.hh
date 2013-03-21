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

#ifdef CIRC_SPRULES_HH_
#error "Include recursion"
#endif

#ifndef ONCE_SPRULES_HH_
#define ONCE_SPRULES_HH_
#define CIRC_SPRULES_HH_


#include "ParRulesFile/StatePropRulesAP.hh"
#include "StateProps.hh"

#include <vector>
#include <string>
#include <map>
#include <set>

class StatePropRules: public iStatePropRulesAP
{
 public:
    StatePropRules(const std::vector<unsigned>&
                   componentNumberToIndexLookupTable);

    //void immediateOTFVIChecksActive(bool);

    void addStatePropsContainer(unsigned fileno,
                                const StatePropsContainer& sp,
                                const std::string& stickySPPrefix);
    void doneAddingStatePropsContainers();


    static const unsigned
    Cut_bit = 1, Rej_bit = 2, DLRej_bit = 4, LLRej_bit = 8;

    unsigned calculateStateProps(lsts_index_t destStateNumber,
                                 const std::vector<lsts_index_t>&stateNumbers);

/***OTFVI***
    inline void setCut(unsigned stateNumber);
    inline void setRej(unsigned stateNumber);
    inline void setDLRej(unsigned stateNumber);
    inline void setLLRej(unsigned stateNumber);
*/

    inline StatePropsContainer& getStatePropsContainer()
    { return resultSPContainer; }


    inline unsigned getGuardProcess() { return guardProcessNumber; }

    inline bool hasOTFVIRules() { return hasOTFVIRules_; }

    inline bool empty() { return bytecode.empty(); }

//===========================================================================
 private:
//===========================================================================
    // Types and constants:
    // -------------------

    enum OTFVIKeyword { NOKEYWORD, SPNAME, CUT, REJ, DL_REJ, LL_REJ, INF_REJ };
    static const char StringDelim = 1;
    static const char RuleEndMark = 2;

    typedef std::vector<std::string> StrVec;
    typedef std::vector<const StatePropsContainer*> SPPtrVec;
    typedef std::map<std::string, unsigned> SPNameMap;


    // Member variables:
    // ----------------

    const std::vector<unsigned>& componentNumberToIndexLookup;

    //bool immediateOTFVIChecks;

    StrVec sprules; // Raw state prop rule strings
    SPPtrVec stateprops; // Pointers to input state prop containers

    // Map: state prop name -> state prop number
    // for each input LSTS (needed for the compilation process):
    std::vector<SPNameMap> iSPNames;

    // Each rule has a correspondent item in the following vectors
    // (ie. the size of these vectors is the amount of rules):
    std::vector<std::vector<unsigned> > bytecode;
    std::vector<OTFVIKeyword> resultSPType;
    //std::vector<bool> resultImmediate;
    std::vector<unsigned> resultSPNumber;
    unsigned ruleBeingCompiled; // Rule being currently compiled (for errmsgs)

    // Resulting state props:
    StatePropsContainer resultSPContainer;
    unsigned resultSPNamesAmount;

    // Evaluation stack:
    std::vector<bool> evalStack;

    // Number of the input LSTS which the llrej rule refers to:
    unsigned guardProcessNumber;

    // Sticky propositions to hide:
    std::set<std::string> stickyPropsToHide;

    bool hasOTFVIRules_;

    // Member functions:
    // ----------------

    // Functions for the parsing and compilation stage:
    void ParseHide(const std::string&);
    OTFVIKeyword whichKeyword(const std::string& keyword);
    OTFVIKeyword expectSPNameOrKeyword(const std::string& rule,
                                       StrVec::size_type which,
                                       std::string::size_type& ind,
                                       const std::string& after);

    bool CheckSyntax(std::string& rule, StrVec::size_type which);
    void BuildResultingStateProps();
    void BuildInputSPNameMaps();
    void CompileRules();
    void CompileSubstring(const std::string& rule,
                          std::string::size_type sind,
                          std::string::size_type eind,
                          std::vector<unsigned>& dest);

    // Functions for rule evaluation:
    bool evaluateRule(const std::vector<unsigned>& rulebc,
                      const std::vector<lsts_index_t>& stateNumbers);

    // Functions for rules file reading (inherited):
    virtual void lsts_StartStatePropRules(class Header&);
    virtual void lsts_StatePropRule(const std::string& rule);
    virtual void lsts_EndStatePropRules();
};


/***OTFVI***
inline void StatePropRules::setCut(unsigned stateNumber)
{
    resultSPContainer.getOTFVI(stateNumber).setCutState();
}

inline void StatePropRules::setRej(unsigned stateNumber)
{
    resultSPContainer.getOTFVI(stateNumber).setRejectState();
}

inline void StatePropRules::setDLRej(unsigned stateNumber)
{
    resultSPContainer.getOTFVI(stateNumber).setDeadlockRejectState();
}

inline void StatePropRules::setLLRej(unsigned stateNumber)
{
    resultSPContainer.getOTFVI(stateNumber).setLivelockRejectState();
}
*/

#undef CIRC_SPRULES_HH_
#endif
