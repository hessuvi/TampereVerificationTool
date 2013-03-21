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

// FILE_DES: InputLSTS.hh: Src/detlsts
// Nieminen Juha

// $Id: InputLSTS.hh 1.10 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// Deterministisointiohjelman syöte-LSTS-tietorakenne
//
// InputLSTS:
//     [Metodin nimi]: [Metodin lyhyt kuvaus]
//

// $Log:$

#ifdef CIRC_INPUTLSTS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_INPUTLSTS_HH_
#define ONCE_INPUTLSTS_HH_
#define CIRC_INPUTLSTS_HH_

#include "AccSets.hh"
#include "StateProps.hh"
#include "InputLSTSContainer.hh"

#ifndef MAKEDEPEND
#endif

// Luokan esittely:

class InputLSTS: public InputLSTSContainer
{
public:
    InputLSTS();
    ~InputLSTS();

    // Header info:
    inline unsigned getActionCount()
    { return getHeaderData().GiveActionCnt(); }
    inline unsigned getStatePropCount()
    { return getHeaderData().GiveStatePropCnt(); }
    inline unsigned getInitiallyUnstable()
    { return getHeaderData().isInitiallyUnstable(); }

    inline bool divBitsSectionRead() { return divBitsRead; }


//=========================================================================
private:
//=========================================================================
    // Muuttujat
    // ---------
    bool divBitsRead;

    // Metodit
    // -------
    void ChecksBeforeReadingLSTS(const iLSTS_File&);
    void lsts_Transition(lsts_index_t sState, lsts_index_t eState,
                         lsts_index_t trNumber);
    void lsts_StartDivBits(Header&);

    // Kopiointiesto
    InputLSTS (const InputLSTS&);
    InputLSTS& operator=(const InputLSTS&);
};

#undef CIRC_INPUTLSTS_HH_
#endif
