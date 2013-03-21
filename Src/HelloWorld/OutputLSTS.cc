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

// FILE_DES: OutputLSTS.cc: Src/HelloWorld
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: OutputLSTS.cc 1.1.2.3 Tue, 18 Feb 2003 21:58:37 +0200 timoe $";
// 
// The implementation of the OutputLSTS class of the 'hello´ program.
//

// $Log:$

#include "OutputLSTS.hh"
#include "LSTS_File/oLSTS_File.hh"

using namespace std;

OutputLSTS::OutputLSTS() :
    
    actions(),
    // true means: transitions are kept in increasing order:
    transitions( true )

{ }

OutputLSTS::~OutputLSTS() { }

// The main algorithm:
void
OutputLSTS::CalculateLSTS()
{
    // We give new names for these types just for extra clarity:
    typedef unsigned state_t;
    typedef lsts_index_t action_t;

    // Creating two states to our result LSTS:
    state_t initialState = 1;
    state_t endState = 2;
    
    // And one single action:
    action_t hello_idx = 1;
    string hello_name = "Hello World!";

    Header hd;
    hd.SetActionCnt( 1 );

    actions.lsts_StartActionNames( hd );
    actions.lsts_ActionName( hello_idx, hello_name );
    actions.lsts_EndActionNames();

    // And finally we add one transition from initialState
    // to the end state:
    transitions.startAddingTransitionsToState( initialState );
    transitions.addTransitionToState( hello_idx, endState );
    transitions.doneAddingTransitionsToState();
}

void OutputLSTS::WriteLSTS( OutStream& os )
{
    // Creating an instance of the LSTS-i/o-library:
    oLSTS_File olsts;

    // Giving it actions and transitions:
    olsts.AddActionNamesWriter( actions );
    olsts.AddTransitionsWriter( transitions );

    // Setting the proper header data:
    Header& hd = olsts.GiveHeader();

    hd.SetStateCnt( 2 );
    hd.SetInitialState( 1 );

    // Finally, writing the file:
    olsts.WriteFile( os );
}
