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

// FILE_DES: parrules.hh: Parallel
// Juha Nieminen

// $Id: parrules.hh 1.14 Fri, 04 Mar 2005 16:13:04 +0200 warp $
// 
// Luokka, joka lukee rinnankytkentäsäännöt
//

// $Log:$

#ifdef CIRC_PARRULES_HH_
#error "Include recursion"
#endif

#ifndef ONCE_PARRULES_HH_
#define ONCE_PARRULES_HH_
#define CIRC_PARRULES_HH_

#include "ParRulesFile/ParComponentsAP.hh"
#include "ParRulesFile/RulesAP.hh"
#include "LSTS_File/ActionNamesStore.hh"
#include "ParRulesFile/ParRulesFile.hh"
#include "InOutStream.hh"

#include "sprules.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <map>
#include <string>
#include <vector>
#endif

// Luokan esittely:

class ParRules: public iParComponentsAP,
                public iRulesAP
{
public:
    //static const lsts_index_t NonParticipant = 0;

    ParRules(InStream&);
    ~ParRules();

    inline unsigned inputFilesAmnt() const { return inputFiles.size()-1; }
    unsigned fileAppearance(const std::string& filename);
    /* Palauttaa sen tiedostonimen indeksin, joka on täsmälleen saman niminen
       kuin annettu tiedostonimi. Jos ei ole olemassa, palauttaa 0.
     */
    const std::string& getFilename(unsigned ind);
    /* Antaa viitteen tiedostonimeen jonka indeksi on 'ind'.
     */

    const std::string& getStickySPPrefix(unsigned ind);

    struct Rule
    {
        lsts_index_t iLSTS, actionNumber;

        inline Rule(lsts_index_t i, lsts_index_t a):
            iLSTS(i), actionNumber(a) {}
        inline bool operator<(const Rule& rhs) const
        { return iLSTS < rhs.iLSTS; }
    };

    inline const std::vector<std::vector<Rule> >& getRuleTable() const
    { return ruleTable; }
    inline const std::vector<lsts_index_t>& getResultingActions() const
    { return resultTable; }

    inline ActionNamesStore& getActionNamesStore() { return actionnamesstore; }

    inline StatePropRules& getStatePropRules() { return sprules; }

//=========================================================================
private:
// Periytetyt metodit:
    void lsts_StartParComponents();
    void lsts_Component( unsigned inputNumber, const std::string& filename,
                         const std::string& prefix );
    void lsts_EndParComponents();

    void lsts_StartRules(unsigned amnt);
    void lsts_Rule(lsts_index_t inputLSTS, lsts_index_t transitionNumber);
    void lsts_RuleDestinationTransitionNumber(lsts_index_t transitionNumber);
    void lsts_EndRules();

    ActionNamesStore actionnamesstore;

    // Omat metodit ja jäsenmuuttujat:
    unsigned ruleIndex;
    std::string outputFilename;
    //std::map<unsigned,std::string> inputFiles;
    std::vector<std::string> inputFiles;
    std::vector<std::string> stickySPPrefix;

    std::vector<std::vector<Rule> > ruleTable;
    std::vector<lsts_index_t> resultTable;

    unsigned indexCounter;
    std::vector<unsigned> componentNumberToIndexLookup;

    StatePropRules sprules;

    iParrulesFile irules;

    // Kopiointiesto
    ParRules (const ParRules&);
    ParRules& operator=(const ParRules&);
};


#undef CIRC_PARRULES_HH_
#endif

