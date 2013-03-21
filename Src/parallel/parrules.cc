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

// FILE_DES: parrules.cc: Parallel
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: parrules.cc 1.12 Fri, 04 Mar 2005 16:13:04 +0200 warp $";
// 
// Rinnankytkijäsääntöjen lukijaluokan toteutus
//

// $Log:$

#include "parrules.hh"
#include "error_handling.hh"

#include <algorithm>
using std::map;
using std::string;
using std::sort;

// Rakentaja ja purkaja
//===========================================================================
ParRules::ParRules(InStream& is):
    indexCounter(0), sprules(componentNumberToIndexLookup),
    irules(is)
{
    irules.AddParComponentsReader(*this);
    irules.AddRulesReader(*this);
    irules.AddActionNamesReader(actionnamesstore);
    irules.AddStatePropRulesReader(sprules);

    irules.ReadFile();
}

ParRules::~ParRules()
{ }

// Palauttaa indeksin siihen tiedostonimeen, joka on sama kuin annetulla
// indeksillä oleva
//===========================================================================
unsigned ParRules::fileAppearance(const string& filename)
{
    /*
    map<unsigned,string>::iterator i;
    for(i=inputFiles.begin(); i!=inputFiles.end(); i++)
        if(i->second == filename) return i->first;
    */
    for(unsigned i = 1; i < inputFiles.size(); ++i)
        if(inputFiles[i] == filename) return i;
    return 0;
}

const string& ParRules::getFilename(unsigned ind)
{
    return inputFiles[ind];

    /*
    static const std::string empty;
    map<unsigned,string>::iterator i = inputFiles.find(ind);
    if(i == inputFiles.end()) return empty;
    */
    /*
    check_claim(i!=inputFiles.end(),
                valueToMessage("Request for input filename number ", ind,
                               " which doesn't exist."));
    */

    //return i->second;
}

const string& ParRules::getStickySPPrefix(unsigned ind)
{
    return stickySPPrefix[ind];
}


// Periytettyjen metodien toteutus:
//===========================================================================
// ParComponentsSection:
// --------------------
void ParRules::lsts_StartParComponents()
{}

void ParRules::lsts_Component(unsigned inputNumber, const string& filename,
                              const string& prefix)
{
    if(componentNumberToIndexLookup.size() <= inputNumber)
        componentNumberToIndexLookup.resize(inputNumber+1);
    componentNumberToIndexLookup[inputNumber] = ++indexCounter;

    if(inputFiles.size() <= indexCounter)
        inputFiles.resize(indexCounter+1);
    inputFiles[indexCounter] = filename;

    if(stickySPPrefix.size() <= indexCounter)
        stickySPPrefix.resize(indexCounter+1);
    stickySPPrefix[indexCounter] = prefix;
}

void ParRules::lsts_EndParComponents()
{}


// RulesSection:
// ------------
void ParRules::lsts_StartRules(unsigned amnt)
{
    ruleTable.resize(amnt);
    ruleIndex = 0;
}

void ParRules::lsts_Rule(lsts_index_t inputLSTS, lsts_index_t transitionNumber)
{
    ruleTable[ruleIndex].push_back
        (Rule(componentNumberToIndexLookup[inputLSTS]-1, transitionNumber));
}

void
ParRules::lsts_RuleDestinationTransitionNumber(lsts_index_t transitionNumber)
{
    sort(ruleTable[ruleIndex].begin(), ruleTable[ruleIndex].end());
    resultTable.push_back(transitionNumber);

    for(unsigned i=1; i<ruleTable[ruleIndex].size(); ++i)
    {
        check_claim(ruleTable[ruleIndex][i].iLSTS !=
                    ruleTable[ruleIndex][i-1].iLSTS,
                    irules.its().errorMsg("Can't synchronize process (",
                                          ruleTable[ruleIndex][i].iLSTS+1,
                                          ") with itself."));
    }

    ++ruleIndex;
}

void ParRules::lsts_EndRules() {}
