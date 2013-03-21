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

Contributor(s): Juha Nieminen, Timo Erkkilä.
*/

#include "InputLSTS.hh"
#include "OutputLSTS.hh"
#include "TvtCLP.hh"

#ifndef MAKEDEPEND
#include <cstdlib>
using namespace std;
#endif

class DetCLP: public TvtCLP
{
    bool given;
    double maxSize;
    static const char* const description;

 public:
    DetCLP(): TvtCLP(description), given(false)
    {
        setOptionHandler("m", &DetCLP::maxSizeHandler, false,
                         "If the result will be larger than <n> times the "
                         "number of states in the input, just stop. The "
                         "return value of the program in this case is 2. "
                         "<n> can be a float.", "<n>", false);
    }

    bool maxSizeWasGiven() { return given; }
    double getMaxSize() { return maxSize; }

 private:
    bool maxSizeHandler(const string& param)
    {
        if(param.size() < 3)
        {
            cerr << "Float number missing after: -m" << endl;
            return false;
        }
        maxSize = atof(param.substr(2).c_str());
        given = true;
        return true;
    }
};

const char* const DetCLP::description=
"Determinizes the given acceptance graph.";

int main(int argc, char* argv[])
{
#ifndef NO_EXCEPTIONS
    try {
#endif
        DetCLP options;
        if(!options.parseCommandLine(argc, argv))
            return 1;

        InputLSTS ilsts;
        ilsts.readLSTSFile(options.getInputStream());
        OutputLSTS olsts(ilsts, options.maxSizeWasGiven(),
                         options.getMaxSize());
        olsts.subsetConstruction();
        olsts.WriteLSTS(options.getOutputStream());
#ifndef NO_EXCEPTIONS
    }
    catch(int r) { return r; }
    catch(...) { return 1; }
#endif
    return 0;
}
