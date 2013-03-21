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

// FILE_DES: 
// Nieminen Juha

// $Id: TransitionsContainer.hh 1.21 Thu, 21 Oct 2004 15:11:45 +0300 warp $
// 
// Geneerinen transitiosäiliöluokka
//

// $Log:$

#ifdef CIRC_TRANSITIONSCONTAINER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_TRANSITIONSCONTAINER_HH_
#define ONCE_TRANSITIONSCONTAINER_HH_
#define CIRC_TRANSITIONSCONTAINER_HH_

#include "MemoryStack.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "LSTS_File/Header.hh"
#include "error_handling.hh"

#include "LSTS_File/LSTS_Sections/TransitionsSection.hh"
#include "FileFormat/Lexical/OTokenStream.hh"

#ifndef MAKEDEPEND
#include <set>
#include <string>
#endif


class TransitionsContainer: public iTransitionsAP, public oTransitionsAP
{
 public:
    // ---------------------------------------------------------------------
    // Transition struct
    // ---------------------------------------------------------------------
    struct Transition
    {
        lsts_index_t transitionNumber;
        lsts_index_t destStateNumber;
        lsts_index_t extraData;

        inline Transition(lsts_index_t trn=0, lsts_index_t dsn=0,
                          lsts_index_t ed=0);
        /*
        inline Transition(lsts_index_t trn=0, lsts_index_t dsn=0);
        */

        inline bool operator<(const Transition&) const;
    };

    // ---------------------------------------------------------------------
    // Constructors
    // ---------------------------------------------------------------------
    inline TransitionsContainer(bool keepTransitionsSorted);

    inline TransitionsContainer(lsts_index_t maxTransitionNumber,
                                lsts_index_t maxStateNumber,
                                bool keepTransitionsSorted);

    inline ~TransitionsContainer();

    // ---------------------------------------------------------------------
    // Direct file writing
    // ---------------------------------------------------------------------
    void directFileWrite(const std::string& filename, bool force);

    // ---------------------------------------------------------------------
    // Adding transitions
    // ---------------------------------------------------------------------
    // This can be called if the class was built with the empty constructor:
    inline void setMaximumValues(lsts_index_t maxTransitionNumber,
                                 lsts_index_t maxStateNumber);

    inline void extraDataMaximumValue(lsts_index_t maxValue);
    inline void writeExtraDataToOutputFile();
    inline void readExtraDataFromInputFile();

    // Start adding transitions to a state. Must call only once for each
    // state number.
    inline void startAddingTransitionsToState(lsts_index_t stateNumber);

    // Add a transition to the state given above:
    inline void addTransitionToState(lsts_index_t transitionNumber,
                                     lsts_index_t destStateNumber);

    inline void addTransitionToState(lsts_index_t transitionNumber,
                                     lsts_index_t destStateNumber,
                                     lsts_index_t data);

    // End adding transition to the state. Must be called after calling
    // startAddingTransitionsToState() and addTrnsitionToState():
    inline void doneAddingTransitionsToState();

    // ---------------------------------------------------------------------
    // Reading
    // ---------------------------------------------------------------------
    // Get number of transition in the given state:
    inline unsigned numberOfTransitions(lsts_index_t stateNumber) const;
    // Get a transition from the given state. 'trIndex' must be smaller
    // than the number of transitions given above.
    inline Transition getTransition(lsts_index_t stateNumber,
                                    unsigned trIndex) const;

    inline unsigned totalNumberOfTransitions() const;

    // ---------------------------------------------------------------------
    // Transition deletion
    // ---------------------------------------------------------------------
    inline void deleteTransition(lsts_index_t stateNumber, unsigned trIndex,
                                 bool keepOrder = true);


//==========================================================================
 protected:
//==========================================================================
    virtual lsts_index_t lsts_numberOfTransitionsToWrite();
    virtual void lsts_WriteTransitions(iTransitionsAP&);

    virtual void lsts_StartTransitions(Header&);
    virtual void lsts_StartTransitionsFromState(lsts_index_t start_state);
    virtual void lsts_Transition(lsts_index_t start_state,
                                 lsts_index_t dest_state,
                                 lsts_index_t action);
    virtual void lsts_TransitionWithExtraData(lsts_index_t start_state,
                                              lsts_index_t dest_state,
                                              lsts_index_t action,
                                              const std::string& extra_data);
    virtual void lsts_EndTransitionsFromState(lsts_index_t start_state);
    virtual void lsts_EndTransitions();

//==========================================================================
 private:
//==========================================================================
    MemoryStack transitionNumbers;
    MemoryStack destStateNumbers;
    // Item n contains the number of transitions in state n:
    MemoryStack transitionsAmount;
    // Item n contains index to transitions of state n:
    MemoryStack transitionIndex;

    MemoryStack extraData;

    const bool keepSorted;

    lsts_index_t currentState;
    unsigned currentTrAmount, transitionsCount;
    std::set<Transition> tmpTrans;

    OutStream* ostr;
    OTokenStream* otstr;
    iTransitionsAP* trwriter;

    bool useExtraData, writeExtraData, readExtraData;

    //TransitionsContainer(const TransitionsContainer&);
    //TransitionsContainer& operator=(const TransitionsContainer&);

 public:
    inline unsigned getReservedBytes()
    {
        return
            transitionNumbers.usedMemory() +
            destStateNumbers.usedMemory() +
            transitionsAmount.usedMemory() +
            transitionIndex.usedMemory() +
            extraData.usedMemory();
    }
};


inline TransitionsContainer::Transition::Transition(lsts_index_t trn,
                                                    lsts_index_t dsn,
                                                    lsts_index_t ed):
    transitionNumber(trn), destStateNumber(dsn), extraData(ed)
{}

inline bool
TransitionsContainer::Transition::operator<(const Transition& rhs) const
{
    return
        transitionNumber == rhs.transitionNumber ?
        destStateNumber < rhs.destStateNumber :
        transitionNumber < rhs.transitionNumber;
}

//==========================================================================
// Rakentajat
//==========================================================================
inline TransitionsContainer::TransitionsContainer(bool keepTransitionsSorted):
    transitionsAmount(255),
    transitionIndex(255),
    keepSorted(keepTransitionsSorted),
    transitionsCount(0),
    ostr(0), otstr(0), trwriter(0),
    useExtraData(false), writeExtraData(false), readExtraData(false)
{}

inline TransitionsContainer::TransitionsContainer(lsts_index_t
                                                  maxTransitionNumber,
                                                  lsts_index_t maxStateNumber,
                                                  bool keepTransitionsSorted):
    transitionNumbers(maxTransitionNumber),
    destStateNumbers(maxStateNumber),
    transitionsAmount(255),
    transitionIndex(255),
    keepSorted(keepTransitionsSorted),
    transitionsCount(0),
    ostr(0), otstr(0), trwriter(0),
    useExtraData(false), writeExtraData(false), readExtraData(false)
{}

inline TransitionsContainer::~TransitionsContainer()
{
    if(trwriter) trwriter->lsts_EndTransitions();

    if(trwriter) delete trwriter;
    if(otstr) delete otstr;
    if(ostr) delete ostr;
}


inline void
TransitionsContainer::setMaximumValues(lsts_index_t maxTransitionNumber,
                                       lsts_index_t maxStateNumber)
{
    transitionNumbers.setMaximumValue(maxTransitionNumber);
    destStateNumbers.setMaximumValue(maxStateNumber);
}

inline void TransitionsContainer::extraDataMaximumValue(lsts_index_t maxValue)
{
    extraData.setMaximumValue(maxValue);
    useExtraData = true;
}

inline void TransitionsContainer::writeExtraDataToOutputFile()
{
    writeExtraData = true;
}

inline void TransitionsContainer::readExtraDataFromInputFile()
{
    readExtraData = true;
    useExtraData = true;
}


//==========================================================================
// Transitioiden lisäys
//==========================================================================
inline void
TransitionsContainer::startAddingTransitionsToState(lsts_index_t stateNumber)
{
    if(!trwriter)
    {
        unsigned trIndex = transitionNumbers.valuesAmount();
        transitionIndex.setValue(stateNumber, trIndex);
        currentTrAmount = 0;
    }
    currentState = stateNumber;
}

inline void
TransitionsContainer::addTransitionToState(lsts_index_t transitionNumber,
                                           lsts_index_t destStateNumber)
{
    if(keepSorted || trwriter)
    {
        tmpTrans.insert(Transition(transitionNumber, destStateNumber));
    }
    else
    {
        ++currentTrAmount;
        transitionNumbers.push(transitionNumber);
        destStateNumbers.push(destStateNumber);
    }
}

inline void
TransitionsContainer::addTransitionToState(lsts_index_t transitionNumber,
                                           lsts_index_t destStateNumber,
                                           lsts_index_t data)
{
    if(keepSorted || trwriter)
    {
        tmpTrans.insert(Transition(transitionNumber, destStateNumber, data));
    }
    else
    {
        ++currentTrAmount;
        transitionNumbers.push(transitionNumber);
        destStateNumbers.push(destStateNumber);
        extraData.push(data);
    }
}

inline void TransitionsContainer::doneAddingTransitionsToState()
{
    if(trwriter)
    {
        transitionsCount += tmpTrans.size();
        trwriter->lsts_StartTransitionsFromState(currentState);
        for(std::set<Transition>::iterator i = tmpTrans.begin();
            i != tmpTrans.end(); ++i)
        {
            if(writeExtraData)
                trwriter->lsts_TransitionWithExtraData
                    (currentState,
                     i->destStateNumber,
                     i->transitionNumber,
                     unsignedToString(i->extraData));
            else
                trwriter->lsts_Transition(currentState,
                                          i->destStateNumber,
                                          i->transitionNumber);
        }
        trwriter->lsts_EndTransitionsFromState(currentState);
        tmpTrans.clear();
    }
    else if(keepSorted)
    {
        transitionsCount += tmpTrans.size();
        transitionsAmount.setValue(currentState, tmpTrans.size());
        for(std::set<Transition>::iterator i = tmpTrans.begin();
            i != tmpTrans.end(); ++i)
        {
            transitionNumbers.push(i->transitionNumber);
            destStateNumbers.push(i->destStateNumber);
            if(useExtraData) extraData.push(i->extraData);
        }
        tmpTrans.clear();
    }
    else
    {
        transitionsCount += currentTrAmount;
        transitionsAmount.setValue(currentState, currentTrAmount);
    }
}

//==========================================================================
// Transitioiden luku
//==========================================================================
inline unsigned
TransitionsContainer::numberOfTransitions(lsts_index_t stateNumber) const
{
#ifdef DEBUG
    check_claim(!trwriter,
                "In Transitionscontainer::numberOfTransitions(): "
                "Class is write-only in direct write mode.");
#endif

    return transitionsAmount.getValue(stateNumber);
}

inline TransitionsContainer::Transition
TransitionsContainer::getTransition(lsts_index_t stateNumber,
                                    unsigned trIndex) const
{
#ifdef DEBUG
    check_claim(!trwriter,
                "In Transitionscontainer::getTransition(): "
                "Class is write-only in direct write mode.");
#endif

    unsigned ind = transitionIndex.getValue(stateNumber)+trIndex;

    if(useExtraData)
        return Transition(transitionNumbers.getValue(ind),
                          destStateNumbers.getValue(ind),
                          extraData.getValue(ind));
    else
        return Transition(transitionNumbers.getValue(ind),
                          destStateNumbers.getValue(ind));
}

inline unsigned TransitionsContainer::totalNumberOfTransitions() const
{
    return transitionsCount;
}

//==========================================================================
// Transition poisto
//==========================================================================
inline void
TransitionsContainer::deleteTransition(lsts_index_t stateNumber,
                                       unsigned trIndex, bool keepOrder)
{
#ifdef DEBUG
    check_claim(!trwriter,
                "In Transitionscontainer::deleteTransition(): "
                "Class is write-only in direct write mode.");
#endif

    unsigned trAmount = numberOfTransitions(stateNumber);
    check_claim(trAmount > 0 && trIndex < trAmount,
                "In TransitionsContainer::deleteTransition(): "
                "Invalid parameters.");
    unsigned startInd = transitionIndex.getValue(stateNumber);
    unsigned ind = startInd+trIndex;
    unsigned lastInd = startInd+trAmount-1;
    if(keepOrder)
    {
        for(unsigned i=ind; i<lastInd; ++i)
        {
            transitionNumbers.setValue(i, transitionNumbers.getValue(i+1));
            destStateNumbers.setValue(i, destStateNumbers.getValue(i+1));
            if(useExtraData)
                extraData.setValue(i, extraData.getValue(i+1));
        }
    }
    else
    {
        transitionNumbers.setValue(ind, transitionNumbers.getValue(lastInd));
        destStateNumbers.setValue(ind, destStateNumbers.getValue(lastInd));
        if(useExtraData)
            extraData.setValue(ind, extraData.getValue(lastInd));
    }
    transitionsAmount.setValue(stateNumber, trAmount-1);
    --transitionsCount;
}


#undef CIRC_TRANSITIONSCONTAINER_HH_
#endif
