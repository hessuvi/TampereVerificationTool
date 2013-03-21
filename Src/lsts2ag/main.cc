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

Contributor(s): Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: $Source: Src/lsts2ag/main.cc $
// FILE_DES: $Id: main.cc 1.12 Fri, 27 Feb 2004 16:36:58 +0200 warp $

#include "automaton.hh"
#include "LSTS_File/iLSTS_File.hh"
#include "LSTS_File/oLSTS_File.hh"
#include "TvtCLP.hh"

typedef TvtCLP Lsts2agClp;

int main(int argc, char* argv[])
{
    const char* const description =
        "  Makes the CFFD equivalent acceptance graph from the given LSTS\n"
        "(not necessarily deterministic).";

#ifndef NO_EXCEPTIONS
    try {
#endif
        Lsts2agClp options(description);
        if ( ! options.parseCommandLine(argc, argv))
	    return 1;
	iLSTS_File lstsInChan( options.getInputStream() );
	Automaton lsts;
	lsts.setupForRead( lstsInChan );
	lstsInChan.ReadFile();

	constructAG( lsts );

	oLSTS_File lstsOutChan;
	lsts.WriteLSTS( lstsOutChan, options );
	//	cerr << "hep" << endl;
	//lstsOutChan.WriteFile( options.getOutputStream() );
#ifndef NO_EXCEPTIONS
    } catch(...) { return 1; }
#endif
    return 0;
}
