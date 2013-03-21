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

// FILE_DES: InputLSTS.hh: Src/ag2lsts
// Nieminen Juha

// $Id: InputLSTS.hh 1.7 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// AG->LSTS:n syöte-lsts
//

// $Log:$

#ifdef CIRC_INPUTLSTS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_INPUTLSTS_HH_
#define ONCE_INPUTLSTS_HH_
#define CIRC_INPUTLSTS_HH_

#include "InputLSTSContainer.hh"

// Luokan esittely:

typedef InputLSTSContainer InputLSTS;

/*
class InputLSTS: public InputLSTSContainer
{
 protected:
    inline void lsts_Transition(lsts_index_t sState,
                                lsts_index_t eState,
                                lsts_index_t trNumber)
    {
        check_claim(trNumber != 0,
                    "Input LSTS contains tau transitions. Not supported.");
        InputLSTSContainer::lsts_Transition(sState, eState, trNumber);
    }
};
*/

#undef CIRC_INPUTLSTS_HH_
#endif

