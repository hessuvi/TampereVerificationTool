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

#include "TauReductionClass.cc"

int main(int argc, char* argv[])
{
#ifndef NO_EXCEPTIONS
  try {
#endif
    TvtCLP clp
        ("Reduces the amount of invisible transitions of the given LSTS\n"
         "preserving CFFD equivalence.");

    if(!clp.parseCommandLine(argc, argv)) return 1;

    InputLSTSContainer ilsts(ILC::STATEPROPS | ILC::TRANSITIONS);
    ilsts.readLSTSFile(clp.getInputStream());

    TauReduction reduction(ilsts);
    reduction.reduce();
    reduction.writeToFile(clp.getOutputStream());

#ifndef NO_EXCEPTIONS
  } catch(...) { return 1; }
#endif
    return 0;
}
