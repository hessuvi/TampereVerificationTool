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

// FILE_DES: outputlsts.hh: Parallel
// Juha Nieminen

// $Id: outputlsts.hh 1.19 Wed, 08 Sep 2004 18:43:46 +0300 timoe $
// 
// Rinnankytketyn LSTS-luokan esittely
//

// $Log:$

#ifdef CIRC_OUTPUTLSTS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_OUTPUTLSTS_HH_
#define ONCE_OUTPUTLSTS_HH_
#define CIRC_OUTPUTLSTS_HH_

#ifndef USE_ALTERNATIVE_STATESET
//#define USE_RADIX_TREE
#endif

#ifdef USE_RADIX_TREE
#include "RadixTree/RadixTree.hh"
#else
#define MEMORYVAR_ALLOW_FINDING
#ifdef USE_ALTERNATIVE_STATESET
#include "ASMemory.hh"
#else
#include "Memory.hh"
#endif
#endif

#include "TransitionsContainer.hh"
#include "LSTS_File/StateNamesAP.hh"

typedef unsigned lsts_index_t;

#ifndef MAKEDEPEND
#include <vector>
#include <string>
#endif

// Luokan esittely:

class OutputLSTS: public oStateNamesAP
{
public:
    typedef unsigned StatePointer;

    OutputLSTS(lsts_index_t ilstsAmnt,
               const std::vector<lsts_index_t>& ilstsStateAmnts,
               lsts_index_t maxOutputTransitionNumber
#ifdef USE_ALTERNATIVE_STATESET
               , int bii, int bik, int kib, int bis
#endif
               );
    /* Alustetaan rinnankytkett‰vien LSTS:ien m‰‰r‰ll‰, kunkin LSTS:n
       tilojen m‰‰r‰ll‰ ja tuloksen transitioiden maksimiarvolla
     */

    StatePointer CreateInitialState(const std::vector<lsts_index_t>&
                                    stateNumbers);
    /* Luo alkutilan ja palauttaa osoittimen siihen.
     */

    inline void SetCurrentState(StatePointer st);
    inline StatePointer GetCurrentState();
    inline void StartAddingTransitions();
    /* T‰ll‰ kerrotaan luokalle, mit‰ tilaa halutaan k‰sitell‰
       AddTransition():illa.
     */

    void GetCurrentStateNumbers(std::vector<lsts_index_t>& StateNumbers) const;

    inline void setExtraData(unsigned data);

    unsigned AddTransition(
        lsts_index_t transitionNumber,
        const std::vector<lsts_index_t>& destStateNumbers,
        StatePointer& newState);
    /* Lis‰t‰‰n tapahtuma SetCurrentState():lla annettuun tilaan ja
       luodaan uusi tila nimill‰ 'destStateNumbers'.
       Mik‰li 'destStateNumbers':illa oli jo olemassa tila, palauttaa
       1 (valkoinen), 2 (harmaa) tai 3 (musta), muuten 0.
       Tilan kaikki tapahtumat t‰ytyy lis‰t‰ kerralla.
     */

    unsigned DoneAddTransition();
    /* Kutsuttava sitten kun nykyiseen tilaan ei en‰‰ lis‰t‰ tapahtumia.
       T‰m‰n j‰lkeen ei kyseiseen tilaan voi en‰‰ lis‰t‰ tapahtumia.
       Palauttaa nykyiseen tilaan lis‰ttyjen transitioiden m‰‰r‰n.
     */

    // V‰ri-informaatio:
    inline bool IsWhite(StatePointer st) const;
    inline bool IsGray(StatePointer st) const;
    inline bool IsBlack(StatePointer st) const;
    inline void SetGray(StatePointer st);
    inline void SetBlack(StatePointer st);

    // Datojen lukumetodit:
    inline lsts_index_t StateCnt() const;
    inline lsts_index_t TransitionCnt() const;
    inline TransitionsContainer& getTransitionsContainer();
    inline lsts_index_t ActionCnt() const;

    // Erikoisflagien asetus:
    //inline void SetPruneFlag(StatePointer st);
    inline void SetDLRejFlag(StatePointer st);
    inline bool IsDLRej(StatePointer st) const;
    inline void SetLLRejFlag(StatePointer st);
    inline bool IsLLRej(StatePointer st) const;

    inline lsts_index_t InitialState() const;


    inline lsts_index_t getStateNumber(StatePointer st) const;


    inline void setInterruptedData(const std::string& msg,
                                   lsts_index_t state,
                                   Header::OTF_ERROR errorType,
                                   bool echoMsg=true)
    {
        interruptedState = state;
        interruptedErrorType = errorType;
        isInterrupted = true;
        if(echoMsg) write_message(msg);
    }
    inline bool hasBeenInterrupted() { return isInterrupted; }
    inline lsts_index_t getInterruptedState() { return interruptedState; }
    inline Header::OTF_ERROR getInterruptedErrorType()
    { return interruptedErrorType; }

    ~OutputLSTS();


//-------------------------------------------------------------------------
private:
//-------------------------------------------------------------------------

#ifdef USE_RADIX_TREE
    class RadixTreeMemory
    {
        unsigned keySize;
        RadixTree radixTree;
        std::vector<unsigned> valuesBits;
        unsigned valOffset;

        RadixTreeMemory(const RadixTreeMemory&);
        RadixTreeMemory& operator=(const RadixTreeMemory&);

     public:
        typedef std::vector<unsigned char> dataContainer_t;

        RadixTreeMemory(const std::vector<lsts_index_t>& ilstsStateAmnts,
                        unsigned valueOffset);
        ~RadixTreeMemory();

        unsigned getStateNumber(unsigned loc_i) const
        { return radixTree.readData(loc_i); }

        unsigned getPointer(unsigned p) const { return p; }

        void putItems(dataContainer_t& dest,
                      const std::vector<lsts_index_t>& data) const;
        bool addPackedData(const dataContainer_t& key, unsigned data,
                           unsigned& loc_i);
        void getItems(unsigned loc_i, std::vector<lsts_index_t>& dest) const;

        unsigned getAllocatedMemoryAmount() const;
    };

    typedef RadixTreeMemory MemoryVar;

    RadixTreeMemory memory;
#else
    MemoryVar memory; // Tulos-LSTS:n tilojen tilanumerot
#endif

    MemoryVar::dataContainer_t packedDataBuffer;

    lsts_index_t stateCnt;
    std::vector<bool> stateColors;
    std::vector<bool> dlrejBits;
    std::vector<bool> llrejBits;


    StatePointer currentState;
    lsts_index_t ParallelLSTSAmnt; // Syˆte-lsts:ien m‰‰r‰
    lsts_index_t actionCnt;
    lsts_index_t transitionCount;

    TransitionsContainer trCont;

    bool isInterrupted;
    lsts_index_t interruptedState;
    Header::OTF_ERROR interruptedErrorType;

    unsigned extraData;

    StatePointer CreateNewState();

    bool StateAlreadyExists(const MemoryVar::dataContainer_t&, StatePointer&);

    inline unsigned getStateColor(StatePointer st) const;

    virtual bool lsts_doWeWriteStateNames();
    virtual void lsts_WriteStateNames(iStateNamesAP& pipe);

    // Kopiointiesto
    OutputLSTS (const OutputLSTS&);
    OutputLSTS& operator=(const OutputLSTS&);


 public:
    inline unsigned getReservedBytesForStates()
    {
        return
            memory.getAllocatedMemoryAmount() +
            stateColors.size()/8 +
            dlrejBits.size()/8 +
            llrejBits.size()/8;
    }
};

// Inline-metodien toteutukset
inline lsts_index_t OutputLSTS::getStateNumber(StatePointer st) const
{
#ifdef USE_RADIX_TREE
    return memory.getStateNumber(st);
#else
    return st+1;
#endif
}

inline unsigned OutputLSTS::getStateColor(StatePointer st) const
{
    unsigned i = getStateNumber(st)-1;
    return unsigned(stateColors[i*2])+2*unsigned(stateColors[i*2+1]);
}

inline void OutputLSTS::setExtraData(unsigned data)
{
    extraData = data;
}

inline bool OutputLSTS::IsWhite(StatePointer st) const
{ return getStateColor(st) == 0; }

inline bool OutputLSTS::IsGray(StatePointer st) const
{ return getStateColor(st) == 1; }

inline bool OutputLSTS::IsBlack(StatePointer st) const
{ return stateColors[(getStateNumber(st)-1)*2+1]; }

inline void OutputLSTS::SetGray(StatePointer st)
{
    unsigned i = getStateNumber(st)-1;
    stateColors[i*2] = true; stateColors[i*2+1] = false;
}

inline void OutputLSTS::SetBlack(StatePointer st)
{ stateColors[(getStateNumber(st)-1)*2+1] = true; }

inline void OutputLSTS::SetDLRejFlag(StatePointer st)
{ dlrejBits[getStateNumber(st)-1] = true; }

inline bool OutputLSTS::IsDLRej(StatePointer st) const
{ return dlrejBits[getStateNumber(st)-1]; }

inline void OutputLSTS::SetLLRejFlag(StatePointer st)
{ llrejBits[getStateNumber(st)-1] = true; }

inline bool OutputLSTS::IsLLRej(StatePointer st) const
{ return llrejBits[getStateNumber(st)-1]; }


inline lsts_index_t OutputLSTS::StateCnt() const
{ return stateCnt; }
inline lsts_index_t OutputLSTS::TransitionCnt() const
{ return transitionCount; }
inline TransitionsContainer& OutputLSTS::getTransitionsContainer()
{ return trCont; }
inline lsts_index_t OutputLSTS::ActionCnt() const
{ return actionCnt; }
inline lsts_index_t OutputLSTS::InitialState() const
{ return 1; }

inline void OutputLSTS::SetCurrentState(StatePointer st)
{
    currentState = st;
}
inline OutputLSTS::StatePointer OutputLSTS::GetCurrentState()
{
    return currentState;
}

inline void OutputLSTS::StartAddingTransitions()
{
    trCont.startAddingTransitionsToState(getStateNumber(currentState));
}

#undef CIRC_OUTPUTLSTS_HH_
#endif

