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

Contributor(s): Jacobus Geldenhuys.
*/

#include "OutputLSTS.hh"
#include "TvtCLP.hh"

int
main(int argc, char* argv[])
{
	const char* const description =
		"  Converts an acceptance graph to a tester process.";

#ifndef NO_EXCEPTIONS
    try {
#endif
	TvtCLP clp(description);
	if (!clp.parseCommandLine(argc, argv)) { return 1; }

	InputLSTS ilsts(ILC::ACCSETS | ILC::DIVBITS | ILC::STATEPROPS | ILC::TRANSITIONS);
	ilsts.readLSTSFile(clp.getInputStream());

	OutputLSTS olsts(ilsts);
	olsts.calculateTP();
	olsts.writeLSTSFile(clp.getOutputStream());

#ifndef NO_EXCEPTIONS
   } catch(...) { return 1; }
#endif

	return 0;
}

/* vim: set tabstop=4 shiftwidth=4: */
