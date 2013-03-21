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

// FILE_DES: OutputLSTS.hh: Src/HelloWorld
// Timo Erkkilä

// $Id: OutputLSTS.hh 1.1.1.3 Tue, 18 Feb 2003 21:58:37 +0200 timoe $
// 
// This class writes the result LSTS of the program 'hello´.

// $Log:$

#ifndef OUTPUTLSTS_HH
#define OUTPUTLSTS_HH

#include "TransitionsContainer.hh"
#include "LSTS_File/ActionNamesStore.hh"


// A class declaration:

class OutputLSTS
{
 public:
    OutputLSTS();
    ~OutputLSTS();

    void CalculateLSTS();
    void WriteLSTS( OutStream& );

 private:
    // A trick to prevent any use of the copy constructor and
    // assignment operator of this class:
    OutputLSTS ( const OutputLSTS& );
    OutputLSTS& operator=( const OutputLSTS& );

    ActionNamesStore actions;
    TransitionsContainer transitions;
};


#endif
