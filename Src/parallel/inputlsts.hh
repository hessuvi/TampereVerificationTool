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

// FILE_DES: inputlsts.hh: Parallel
// Juha Nieminen

// $Id: inputlsts.hh 1.13 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// Rinnankytkij‰n syˆte-LSTS-luokan esittely
//
// Vaatimukset:
// - Tilat on numeroitu 1 - STATE_CNT
// - Tapahtumat, jotka l‰htev‰t samasta tilasta, ovat per‰kk‰in
//   syˆtetiedostossa
//

// $Log:$

#ifdef CIRC_INPUTLSTS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_INPUTLSTS_HH_
#define ONCE_INPUTLSTS_HH_
#define CIRC_INPUTLSTS_HH_

//#include "InputLSTSContainer.hh"

//typedef InputLSTSContainer InputLSTS;


#include "InputLSTSContainer.hh"

//typedef unsigned lsts_index_t;

// Luokan esittely:

class InputLSTS: public InputLSTSContainer
{
 public:
    inline InputLSTS(InStream& input);

    inline lsts_index_t GetState(lsts_index_t iState,
                                 lsts_index_t transitionNumber);
    /* Palauttaa tilan numeron, johon p‰‰st‰‰n tilasta 'iState' seuraamalla
       tapahtumaa 'transitionNumber'. Mik‰li tilasta 'iState' ei l‰hde
       tapahtumaa 'transitionNumber', palauttaa 0:n (ts. metodi toimii
       samalla sen testerin‰).
     */

    inline lsts_index_t GetNextState();
    /* Palauttaa seuraavan tilan numeron, johon p‰‰st‰‰n samasta tilasta
       seuraamalla samannimist‰ tapahtumaa jotka annettiin GetState():lle.
       Mik‰li saman nimist‰ tapahtumaa ei en‰‰ ole, palauttaa 0:n.
     */

/***OTFVI***
    inline bool IsPruneState(lsts_index_t st) const;
    inline bool IsRejectionState(lsts_index_t st) const;
    inline bool IsDeadlockRejectState(lsts_index_t st) const;
    inline bool IsLivelockRejectState(lsts_index_t st) const;
    inline bool IsInfinityRejectState(lsts_index_t st) const;
*/

    inline lsts_index_t GetInitialState() const
    { return getInitialStateNumber(); }
    inline lsts_index_t GetStateAmount() const
    { return getHeaderData().GiveStateCnt(); }

//===========================================================================
 private:
//===========================================================================
    tr_const_iterator currentTransition, current_tr_end;
/*    
    // Omat metodit

    struct Transition
    {
        lsts_index_t destState, number;
        inline bool operator<(const Transition& rhs) const
        { return number < rhs.number; }
    };

    class State
    {
    public:
        State();
        ~State();
        lsts_index_t getDestState(lsts_index_t trInd);
        lsts_index_t getTransitionNumber(lsts_index_t trInd);
        void SetTransitionsPtr(Transition*);
        void AddTransition(lsts_index_t destState, lsts_index_t number);
        lsts_index_t TransitionAmount();
        bool ExistTransitions();
        void Sort();

        inline bool IsPruneState() { return (Flags&1) != 0; }
        inline bool IsRejectionState() { return (Flags&2) != 0; }
        inline bool IsDeadlockRejectState() { return (Flags&4) != 0; }
        inline bool IsLivelockRejectState() { return (Flags&8) != 0; }
        inline bool IsInfinityRejectState() { return (Flags&16) != 0; }

        inline void SetPruneBit() { Flags &= 1; }
        inline void SetRejectionBit() { Flags &= 2; }
        inline void SetDeadlockRejectBit() { Flags &= 4; }
        inline void SetLivelockRejectBit() { Flags &= 8; }
        inline void SetInfinityRejectBit() { Flags &= 16; }

     private:
        Transition* transitions;
        unsigned transitionAmnt;
        unsigned Flags;
    };

    State* LSTS; // LSTS-taulukko, jossa stateAmount kpl State:ja.
    Transition* Transitions; // Transitiotaulukko
    unsigned transitionsPtr;
    lsts_index_t stateAmount, transitionAmount;
    lsts_index_t initialState, currentState, currentTransition;
    bool thisIsACopy;

// Periytetyt metodit
    SectionState StartReadingHeaderSection();
    void EndReadingHeaderSection();

    SectionState StartReadingTransitionsSection();
    void ReadStartTransitionsFromState(lsts_index_t startState);
    void ReadTransition(lsts_index_t, lsts_index_t fState,
                        lsts_index_t transitionNumber);
    void ReadEndTransitionsFromState(lsts_index_t);

    SectionState StartReadingActionNamesSection();

    bool isOTFVI_Separate();
    void ReadCutState(lsts_index_t state);
    void ReadRejectState(lsts_index_t state);
    void ReadDeadlockRejectState(lsts_index_t state);
    void ReadLivelockRejectState(lsts_index_t state);
    void ReadInfinityRejectState(lsts_index_t state);

    // Kopiointiesto
    //InputLSTS(const InputLSTS&);
    InputLSTS& operator=(const InputLSTS&);
*/
};


inline InputLSTS::InputLSTS(InStream& input):
    InputLSTSContainer(ILC::STATEPROPS | ILC::TRANSITIONS)
{
    readLSTSFile(input);
}

inline lsts_index_t InputLSTS::GetState(lsts_index_t iState,
                                        lsts_index_t transitionNumber)
{
    const State& currentState = getState(iState);
    currentTransition = findTransition(currentState,
                                       transitionNumber);
    current_tr_end = tr_end(currentState);

    if(currentTransition == current_tr_end) return 0;

    return getStateNumber(currentTransition->getDestinationState());
}

inline lsts_index_t InputLSTS::GetNextState()
{
    unsigned trNumber = currentTransition->getTransitionNumber();
    ++currentTransition;

    if(currentTransition == current_tr_end ||
       currentTransition->getTransitionNumber() != trNumber)
        return 0;

    return getStateNumber(currentTransition->getDestinationState());
}


/***OTFVI***
inline bool InputLSTS::IsPruneState(lsts_index_t st) const
{ return getStateProps().getOTFVI(st).isCutState(); }
inline bool InputLSTS::IsRejectionState(lsts_index_t st) const
{ return getStateProps().getOTFVI(st).isRejectState(); }
inline bool InputLSTS::IsDeadlockRejectState(lsts_index_t st) const
{ return getStateProps().getOTFVI(st).isDeadlockRejectState(); }
inline bool InputLSTS::IsLivelockRejectState(lsts_index_t st) const
{ return getStateProps().getOTFVI(st).isLivelockRejectState(); }
inline bool InputLSTS::IsInfinityRejectState(lsts_index_t st) const
{ return getStateProps().getOTFVI(st).isInfinityRejectState(); }
*/

#undef CIRC_INPUTLSTS_HH_
#endif
