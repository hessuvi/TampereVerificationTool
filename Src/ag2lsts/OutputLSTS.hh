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

// FILE_DES: OutputLSTS.hh: Src/ag2lsts
// Nieminen Juha

// $Id: OutputLSTS.hh 1.12 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// AG->LSTS tulos-lsts-luokka
//
// OutputLSTS:
//     [Metodin nimi]: [Metodin lyhyt kuvaus]
//

// $Log:$

#ifdef CIRC_OUTPUTLSTS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_OUTPUTLSTS_HH_
#define ONCE_OUTPUTLSTS_HH_
#define CIRC_OUTPUTLSTS_HH_

#include "InputLSTS.hh"
#include "TransitionsContainer.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <vector>
#endif

// Luokan esittely:

class OutputLSTS
{
public:
    OutputLSTS(InputLSTS&);
    ~OutputLSTS();

    void calculateLSTS();

    void writeLSTSFile(OutStream&);

//========================================================================
private:
//========================================================================
// Tyypit
// ------
    typedef unsigned StateHandle;

    class State
    {
     public:
        inline State():
            stateprops(StatePropsContainer::getEmptyStatePropsPtr()) {}
        inline State(StatePropsContainer::StatePropsPtr sprops):
            stateprops(sprops) {}

        inline void setStatePropsPtr(StatePropsContainer::StatePropsPtr props)
        { stateprops = props; }
        inline StatePropsContainer::StatePropsPtr getStatePropsPtr()
        { return stateprops; }

     private:
        StatePropsContainer::StatePropsPtr stateprops;
    };

// Jäsenmuuttujat
// --------------
    InputLSTS& a_ilsts;
    std::vector<State> a_states;
    TransitionsContainer a_transitions;
    StateHandle a_initialState;

// Metodit
// -------
    inline StateHandle
    addNewState(StatePropsContainer::StatePropsPtr stateprops);
    inline bool transitionsEqualAccsets(const InputLSTS::State&);
    inline void addAllTransitions(const InputLSTS::State& state);
    void FillSPContainer(StatePropsContainer& spcont);

    // Kopiointiesto
    OutputLSTS (const OutputLSTS&);
    OutputLSTS& operator=(const OutputLSTS&);
};


#undef CIRC_OUTPUTLSTS_HH_
#endif

