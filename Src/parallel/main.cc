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

// FILE_DES: main.cc: Parallel
// Juha Nieminen

//static const char * const ModuleVersion=
//  "Module version: $Id: main.cc 1.23 Fri, 27 Feb 2004 16:36:58 +0200 warp $";
// 
// Rinnankytkij‰n p‰‰ohjelma
//

// $Log:$

#include "TvtCLP.hh"
#include "parallel.hh"
#include "parrules.hh"

#include <iostream>
#include <cstdlib>

using namespace std;

class CLP: public TvtCLP
{
    static const char* const description;

    //bool otfvi_lsts, otfvi_rules;
    bool printInfo;
    string troutFilename;

    Parallel::SSType ss_type;

#ifdef USE_ALTERNATIVE_STATESET
    int bii, kib, bis, expectedStates;
#endif

 public:
    CLP():
        TvtCLP(description), //otfvi_lsts(true), otfvi_rules(true),
        printInfo(false), ss_type(Parallel::NONE)
#ifdef USE_ALTERNATIVE_STATESET
        , bii(-1), kib(-1), bis(-1), expectedStates(-1)
#endif
    {
        /*
        setOptionHandler("-otfvi_checks=", &CLP::otfviHandler, false,
                         "Sets OTFVI checking mode. Possible values for "
                         "<mode> are: none, lstsonly, rulesonly, both. "
                         "The default is 'both'.", "<mode>", false);
        */

        setOptionHandler("-trout=", &CLP::troutHandler, false,
                         "Specifies an output file for transitions. If used, "
                         "transitions are written to it as they are created, "
                         "thus freeing memory for state information.",
                         "<filename>", false);

        setOptionHandler("-info", &CLP::infoHandler, true,
                         "Prints extra information about memory usage and "
                         "calculation time.");

        setOptionHandler("-use-ss=", &CLP::useSSHandler, false,
                         "Use the stubborn sets method to produce a reduced "
                         "LSTS which is equivalent to the regular parallel "
                         "composition according to the given equivalence. "
                         "Valid values for <type> are: deadlocks, csp, cffd",
                         "<type>", false);

#ifdef USE_ALTERNATIVE_STATESET
        setOptionHandler("-states=", &CLP::statesHandler, false,
                         "Tells the program the expected amount of states "
                         "in the result in order to set the parameters of "
                         "the data structure.", "<n>", false);

        setOptionHandler("-sizes=", &CLP::sizesHandler, false,
                         "Sets data sizes for state set structure. "
                         "Parameters are optional (but commas are mandatory). "
                         "Examples: --sizes=12,5,20 --sizes=7,,25 "
                         "--sizes=10,,", "<bii>,<kib>,<bis>", false);
#endif
    }

    /*
    bool otfviHandler(const string& param)
    {
        string mode = param.substr(15);

        if(mode == "none"){ otfvi_lsts = otfvi_rules = false; }
        else if(mode == "lstsonly"){ otfvi_lsts = true; otfvi_rules = false; }
        else if(mode == "rulesonly"){ otfvi_lsts = false; otfvi_rules = true; }
        else if(mode == "both"){ otfvi_lsts = otfvi_rules = true; }
        else
        {
            cerr << "Invalid parameter to --otfvi_checks: " << mode << endl;
            return false;
        }
        return true;
    }
    */

    bool troutHandler(const string& param)
    {
        troutFilename = param.substr(8);
        if(troutFilename.empty() || troutFilename=="-")
        {
            cerr << "Invalid file name for --trout." << endl;
            return false;
        }
        return true;
    }

    bool infoHandler(const string&)
    {
        printInfo = true;
        return true;
    }

    bool useSSHandler(const string& param)
    {
        string type = param.substr(9);
        if(type=="deadlocks") ss_type = Parallel::DEADLOCKS;
        else if(type=="csp") ss_type = Parallel::CSP;
        else if(type=="cffd") ss_type = Parallel::CFFD;
        else
        { cerr<<"Invalid value in parameter: "<<param<<endl; return false; }
        return true;
    }

#ifdef USE_ALTERNATIVE_STATESET
    bool statesHandler(const string& param)
    {
        expectedStates = atoi(param.substr(9).c_str());
        if(expectedStates <= 0)
        {
            cerr << "Invalid value in parameter: " << param << endl;
            return false;
        }
        return true;
    }

    bool sizesHandler(const string& param)
    {
        string vals = param.substr(8);
        if(vals[0] != ',')
        {
            bii = atoi(vals.c_str());
            if(bii <= 0)
            {
                cerr << "Invalid value for 'bii' in --sizes" << endl;
                return false;
            }
        }
        unsigned ind = 0;
        while(ind < vals.size() && vals[ind++] != ',');
        if(ind == vals.size()) return true;
        vals.erase(0, ind);

        if(vals[0] != ',')
        {
            kib = atoi(vals.c_str());
            if(kib <= 0)
            {
                cerr << "Invalid value for 'kib' in --sizes" << endl;
                return false;
            }
        }
        ind = 0;
        while(ind < vals.size() && vals[ind++] != ',');
        if(ind == vals.size()) return true;
        vals.erase(0, ind);

        if(vals[0] != ',')
        {
            bis = atoi(vals.c_str());
            if(bis <= 0)
            {
                cerr << "Invalid value for 'bis' in --sizes" << endl;
                return false;
            }
        }

        return true;
    }
#endif

    void help(const string& programName)
    {
        printProgramDescription();
        cerr << endl << "Usage:" << endl << " " << programName
             << " [<options>] <parrulesfile> [<outputfile>]" << endl << endl;
        printOptionsHelp();
        printStdinStdoutNotice();
        printCopyrightNotice();
    }

    //inline bool otfviLSTS() const { return otfvi_lsts; }
    //inline bool otfviRules() const { return otfvi_rules; }

    inline bool getInfoFlag() const { return printInfo; }
    inline Parallel::SSType getSSType() const { return ss_type; }

    inline const string& getTroutFilename() const { return troutFilename; }

#ifdef USE_ALTERNATIVE_STATESET
    inline int getbii() const { return bii; }
    inline int getkib() const { return kib; }
    inline int getbis() const { return bis; }
    inline int getExpectedStates() const { return expectedStates; }
#endif
};

const char* const CLP::description=
"  Calculates the parallel composition of the LSTS files given in the rules\n"
"file using the composition rules in the same file.";

//#include <unistd.h>

// Komentorivin j‰sennys
//===========================================================================
int main(int argc, char* argv[])
{
    //cerr << "(main): "; sbrk(0);

#ifndef NO_EXCEPTIONS
    try {
#endif
        CLP cline;
        if(!cline.parseCommandLine(argc, argv))
            return 1;

        ParRules rules(cline.getInputStream());

        Parallel par(rules, cline.getOutputStream(),
                     //cline.otfviLSTS(), cline.otfviRules(),
                     cline.getInfoFlag()
#ifdef USE_ALTERNATIVE_STATESET
                     , cline.getbii(), cline.getkib(), cline.getbis(),
                     cline.getExpectedStates()
#endif
                     );

        if(!cline.getTroutFilename().empty())
            par.setDirectTrWriteFile(cline.getTroutFilename(),
                                     cline.getForce());

        par.Calculate(cline.getSSType());

#ifdef PARDEBUG
        write_message(valueToMessage
                      ("Number of states: ",
                       par.GetStateCnt(),
                       valueToMessage("\n Number of transitions: ",
                                      par.GetTransitionCnt(), "")));
#endif

        par.Write();
#ifndef NO_EXCEPTIONS
    } catch(...) { return 1; }
#endif

    return 0;
}
