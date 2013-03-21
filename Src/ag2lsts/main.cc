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

#include "InputLSTS.hh"
#include "OutputLSTS.hh"
#include "TvtCLP.hh"

typedef TvtCLP AG2LSTSClp;

int main(int argc, char* argv[])
{
    const char* const description=
        "  Converts an acceptance graph to an LSTS.";

#ifndef NO_EXCEPTIONS
    try {
#endif
        AG2LSTSClp options(description);
        if(!options.parseCommandLine(argc, argv))
            return 1;

        InputLSTS ilsts
            (ILC::ACCSETS | ILC::DIVBITS | ILC::STATEPROPS | ILC::TRANSITIONS);
        ilsts.readLSTSFile(options.getInputStream());

        OutputLSTS olsts(ilsts);
        olsts.calculateLSTS();
        olsts.writeLSTSFile(options.getOutputStream());
#ifndef NO_EXCEPTIONS
    } catch(...) { return 1; }
#endif
    return 0;
}
