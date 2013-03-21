/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// The command line parser of TVT:
//#include "TvtCLP.hh" 

// The user-made class that writes the created LSTS:
//#include "OutputLSTS.hh" 

// typedef is here in case we might want to add some extra options
// to this program in the future:
//typedef TvtCLP HelloWorldClp;

//#include "FileFormat/Lexical/oTokenStream.hh"

//static    char* c = "jflksjafdaskfjalksjfkl";

#include <string>

int
main( int argc, char* argv[] )
{
    std::string c = "jflksjafdaskfjalksjfkl";


    return 0;

    // const char* const description = "  Says hello to the world.";
#ifndef NO_EXCEPTIONS
    try {
#endif
        //        HelloWorldClp options( description );

        /*        if ( !options.parseCommandLine( argc, argv ) )
        {
            return 1;
            }*/

        //    OutputLSTS olsts;
        //olsts.CalculateLSTS();
        //olsts.WriteLSTS( options.getOutputStream() );
#ifndef NO_EXCEPTIONS
    } catch( ... ) { return 1; }
#endif

    return 0;
}
