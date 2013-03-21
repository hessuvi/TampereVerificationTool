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

// FILE_DES: InputLSTS.cc: Src/detlsts
// Nieminen Juha

static const char * const ModuleVersion=
  "Module version: $Id: InputLSTS.cc 1.11 Fri, 27 Feb 2004 16:36:58 +0200 warp $";
// 
// Implementation of the input LSTS data structure of the determinization
// program.
//

// $Log:$

#include "InputLSTS.hh"

#ifndef MAKEDEPEND
#endif

// Constructor and destructor
//===========================================================================
InputLSTS::InputLSTS():
    InputLSTSContainer
      (ILC::ACCSETS | ILC::DIVBITS | ILC::STATEPROPS | ILC::TRANSITIONS),
    divBitsRead(false)
{
}

InputLSTS::~InputLSTS()
{
}


void InputLSTS::ChecksBeforeReadingLSTS(const iLSTS_File& ifile)
{
    warn_ifnot(ifile.GiveHeader().hasPromise( "no_taus" ), //hasNoTaus(),
               "LSTS may contain invisible transitions.");
}

void InputLSTS::lsts_Transition(lsts_index_t sState, lsts_index_t eState,
                                lsts_index_t trNumber)
{
    const StatePropsContainer& sprops = getStateProps();
    check_claim(trNumber!=0 ||
                sprops.getStateProps(sState) != sprops.getStateProps(eState),
                "Invisible transition(s) found in input.");

    InputLSTSContainer::lsts_Transition(sState, eState, trNumber);
}

void InputLSTS::lsts_StartDivBits(Header& hd)
{
    divBitsRead = true;
    return InputLSTSContainer::lsts_StartDivBits(hd);
}
