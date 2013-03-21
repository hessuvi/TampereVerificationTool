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

#ifdef CIRC_RO_ACCSETS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_RO_ACCSETS_HH_
#define ONCE_RO_ACCSETS_HH_
#define CIRC_RO_ACCSETS_HH_

#include "AccSets.hh"
#include "LSTS_File/AccSetsAP.hh"
#include "LSTS_File/Header.hh"

#include "error_handling.hh"

#ifndef MAKEDEPEND
#include <vector>
#include <list>
#endif


class RO_AccSets;

//========================================================================
// RO_AccSetsContainer
//========================================================================
class RO_AccSetsContainer: public iAccSetsAP
{
 public:
    RO_AccSetsContainer();
    RO_AccSetsContainer(unsigned maxVal); // Action range

    ~RO_AccSetsContainer();

    inline void setMaxVal(unsigned maxVal); // Action range

    inline void addAccSets(unsigned stateNumber, const AccSets&);
    inline void addAccSets(unsigned stateNumber, const RO_AccSets&);

    // Returns the AccSets for the given state. If there's no such state,
    // returns an empty AccSets.
    inline AccSets getAccSets(unsigned stateNumber) const;
    inline RO_AccSets getRO_AccSets(unsigned stateNumber) const;

    inline bool isInitialized() const;
    inline bool isEmpty() const;

    inline unsigned getActionMaxVal() const { return actionMaxVal; }


    // Methods indented mainly for RO_AccSets:
    inline unsigned getSetIndex(unsigned stateNumber) const;
    inline unsigned getSetsAmount(unsigned stateNumber) const;
    inline BitVector getAccSet(unsigned setIndex) const;

//========================================================================
 protected:
//========================================================================
    // Methods for reading:
    virtual void lsts_StartAccSets(Header&);
    virtual void lsts_StartAccSetsOfState(lsts_index_t state);
    virtual void lsts_StartSingleAccSet(lsts_index_t state);
    virtual void lsts_AccSetAction(lsts_index_t state, lsts_index_t action);
    virtual void lsts_EndSingleAccSet(lsts_index_t state);
    virtual void lsts_EndAccSetsOfState(lsts_index_t state);
    virtual void lsts_EndAccSets();

    // Methods for writing:
    virtual bool lsts_doWeWriteAccSets();
    virtual void lsts_WriteAccSets(iAccSetsAP& pipe);

//========================================================================
 private:
//========================================================================
    struct SetsPtr
    {
        inline SetsPtr(unsigned ind=0, unsigned amnt=0):
            setsIndex(ind), setsAmnt(amnt) {}

        unsigned setsIndex;
        unsigned setsAmnt;
    };

    // Each item is the set of accsets of one state.
    // The index of the item is the number of the state.
    std::vector<SetsPtr> accsets;

    // Each item is a pointer to one set:
    std::vector<unsigned*> sets;

    // Each item is a block of memory. Each block of memory has the data
    // of several bitvectors:
    typedef std::list<std::vector<unsigned> > datachunk_t;
    datachunk_t bitDataChunks;

    // The data of how many bitvectors in each item:
    static const unsigned bitDataChunkSize = 256;

    inline unsigned* allocateBitDataChunk();
    inline void setupNewAccSets(unsigned stateNumber, unsigned setsAmount);
    inline void addDataChunk(const BitVector&);

    unsigned actionMaxVal;
    bool actionMaxValHasBeenSet;
    AccSets tmpaccsets;

    // Block copying:
    RO_AccSetsContainer(const RO_AccSetsContainer&);
    RO_AccSetsContainer& operator=(const RO_AccSetsContainer&);
};






//========================================================================
// RO_AccSets
//========================================================================
class RO_AccSets
{
public:
    class AccSetPtr;
    class action_const_iterator;

    inline AccSetPtr firstAccSet() const;
    inline AccSetPtr endAccSet() const;
    inline action_const_iterator begin(AccSetPtr ptr) const;
    inline action_const_iterator end(AccSetPtr ptr) const;

    inline bool isMember(unsigned actionNumber, const AccSetPtr& ptr) const;

    inline unsigned setsAmount() const;

    inline bool isEmpty() const;

    inline BitVector getAllUsedActionsAsBitVector() const;

    inline bool operator<(const RO_AccSets& rhs) const;
    inline bool operator==(const RO_AccSets& rhs) const;
    inline bool operator!=(const RO_AccSets& rhs) const;

    void writeAccSets(lsts_index_t stateNumber, iAccSetsAP& pipe);

    class action_const_iterator
    {
     public:
        inline action_const_iterator():
            accset(0), actionIndex(0), actionMaxVal(0) {}

        inline unsigned operator*() const;
        inline action_const_iterator& operator++();
        inline bool operator==(const action_const_iterator&) const;
        inline bool operator!=(const action_const_iterator&) const;

     private:
        friend class RO_AccSets;
        inline action_const_iterator(AccSetPtr accset,
                                     unsigned actionMaxVal,
                                     unsigned index=0);

        inline void searchNextAction();

        const BitVector accset;
        unsigned actionIndex;
        unsigned actionMaxVal;
    };

    class AccSetPtr
    {
     public:
        inline AccSetPtr(): container(0), index(0) {}
        inline BitVector operator*() const
        { return container->getAccSet(index); }
        inline AccSetPtr& operator++() { ++index; return *this; }
        inline bool operator==(const AccSetPtr& rhs) const
        { return container==rhs.container && index==rhs.index; }
        inline bool operator!=(const AccSetPtr& rhs) const
        { return index!=rhs.index || container!=rhs.container; }

     private:
        friend class RO_AccSets;
        inline AccSetPtr(const RO_AccSetsContainer* cont, unsigned ptrIndex):
            container(cont), index(ptrIndex) {}

        const RO_AccSetsContainer* container;
        unsigned index;
    };

    inline RO_AccSets(const RO_AccSetsContainer*, unsigned stateNumber);
    inline ~RO_AccSets();

//========================================================================
private:
//========================================================================
    const RO_AccSetsContainer* container;
    unsigned stateNumber;

    // Disable copying
    //RO_AccSets (const RO_AccSets&);
    //RO_AccSets& operator=(const RO_AccSets&);
};






//========================================================================
// Implementations
//========================================================================

// AccSetsContainer
//========================================================================

inline bool RO_AccSetsContainer::isInitialized() const
{
    return actionMaxValHasBeenSet;
}

inline bool RO_AccSetsContainer::isEmpty() const
{
    return accsets.size() <= 1;
}

inline void RO_AccSetsContainer::setMaxVal(unsigned maxVal)
{
    check_claim(!actionMaxValHasBeenSet,
                "In RO_AccSetsContainer::setMaxVal(): "
                "Attempt to set action max value more than once.");

    actionMaxVal = maxVal;
    actionMaxValHasBeenSet = true;
    addAccSets(0, AccSets(actionMaxVal));
}

// Adding sets:
// -----------
inline unsigned* RO_AccSetsContainer::allocateBitDataChunk()
{
    unsigned dataSize = BitVector::bufferSize(actionMaxVal);
    unsigned vecSize = dataSize*bitDataChunkSize;

    // If this is the first call or the current chunk is full, allocate
    // a new chunk:
    if(bitDataChunks.empty() || bitDataChunks.back().size() == vecSize)
    {
        bitDataChunks.resize(bitDataChunks.size()+1);
        bitDataChunks.back().reserve(vecSize);
    }

    // Allocate space for one bit vector:
    std::vector<unsigned>& vec = bitDataChunks.back();
    unsigned endInd = vec.size();
    vec.resize(vec.size()+dataSize);
    unsigned* ptr = &(vec[endInd]);
    return ptr;
}

inline void
RO_AccSetsContainer::setupNewAccSets(unsigned stateNumber, unsigned setsAmount)
{
    check_claim(stateNumber >= accsets.size() ||
                (accsets[stateNumber].setsIndex == 0 &&
                 accsets[stateNumber].setsAmnt == 0),
                valueToMessage("In RO_AccSetsContainer::addAccSets(): "
                               "Trying to set accsets of state ", stateNumber,
                               " twice."));

    // If accsets is not big enough, increase:
    if(stateNumber >= accsets.size())
    {
        accsets.resize(stateNumber+1);
    }

    // Set the values for the accsets corresponding to stateNumber:
    // - Index to sets:
    accsets[stateNumber].setsIndex = sets.size();
    // - Amount of sets:
    accsets[stateNumber].setsAmnt = setsAmount;
}

inline void RO_AccSetsContainer::addDataChunk(const BitVector& bv)
{
    // Allocate space for a bit vector data:
    unsigned* chunk = allocateBitDataChunk();
    sets.push_back(chunk);
    // Use the constructor of BitVector to copy the bit data from the
    // given BitVector bv to the space allocated above:
    BitVector(bv, chunk);
}

inline void
RO_AccSetsContainer::addAccSets(unsigned stateNumber, const AccSets& as)
{
    setupNewAccSets(stateNumber, as.setsAmount());

    // Copy the data from the given AccSets to sets and bitDataChunks:
    for(AccSets::AccSetPtr ptr = as.firstAccSet();
        ptr != as.endAccSet(); ++ptr) // go through all the sets
    {
        addDataChunk(*ptr);
    }
}

inline void
RO_AccSetsContainer::addAccSets(unsigned stateNumber, const RO_AccSets& as)
{
    setupNewAccSets(stateNumber, as.setsAmount());

    // Copy the data from the given RO_AccSets to sets and bitDataChunks:
    for(RO_AccSets::AccSetPtr ptr = as.firstAccSet();
        ptr != as.endAccSet(); ++ptr) // go through all the sets
    {
        addDataChunk(*ptr);
    }
}

// Reading sets:
// ------------
inline AccSets RO_AccSetsContainer::getAccSets(unsigned stateNumber) const
{
    check_claim(actionMaxValHasBeenSet,
                "In RO_AccSetsContainer::getAccSets(): "
                "Action max value has not been initialized.");

    if(stateNumber >= accsets.size()) return AccSets();

    const SetsPtr sInd = accsets[stateNumber];

    return AccSets(actionMaxVal,
                   sets.begin()+sInd.setsIndex,
                   sets.begin()+(sInd.setsIndex+sInd.setsAmnt));
}

inline
RO_AccSets RO_AccSetsContainer::getRO_AccSets(unsigned stateNumber) const
{
    check_claim(actionMaxValHasBeenSet,
                "In RO_AccSetsContainer::getRO_AccSets(): "
                "Action max value has not been initialized.");

    return RO_AccSets(this, stateNumber);
}

// Methods intended for RO_AccSets:
inline unsigned RO_AccSetsContainer::getSetsAmount(unsigned stateNumber) const
{
    return accsets.size()>stateNumber ? accsets[stateNumber].setsAmnt : 0;
}
inline unsigned RO_AccSetsContainer::getSetIndex(unsigned stateNumber) const
{
    return accsets.size()>stateNumber ? accsets[stateNumber].setsIndex : 0;
}

inline BitVector RO_AccSetsContainer::getAccSet(unsigned setsIndex) const
{
    return BitVector(actionMaxVal, sets[setsIndex]);
}




// AccSets
//========================================================================

// action_const_iterator
// ---------------------
inline unsigned RO_AccSets::action_const_iterator::operator*() const
{
#ifdef DEBUG
    if(actionIndex == actionMaxVal)
        check_claim(false,
                    "In AccSets::action_const_iterator::operator*(): "
                    "Tried to get value of end(), which should not be done.");
    check_claim(actionIndex < actionMaxVal,
                "In AccSets::action_const_iterator::operator*(): "
                "Tried to get the value of an iterator that is larger than "
                "the end() value, which should not be done.");
#endif

    return actionIndex+1;
}

inline void RO_AccSets::action_const_iterator::searchNextAction()
{
    for(; actionIndex<actionMaxVal; actionIndex++)
    {
        if(accset[actionIndex]) return;
    }
}

inline RO_AccSets::action_const_iterator&
RO_AccSets::action_const_iterator::operator++()
{
#ifdef DEBUG
    warn_ifnot(actionIndex < actionMaxVal,
               "In AccSets::action_const_iterator::operator++(): "
               "Applied ++ to an iterator pointing to end().");
#endif

    ++actionIndex;
    searchNextAction();
    return *this;
}

inline bool
RO_AccSets::action_const_iterator::operator==(const action_const_iterator& rhs)
    const
{
    return actionIndex==rhs.actionIndex && accset==rhs.accset;
}

inline bool
RO_AccSets::action_const_iterator::operator!=(const action_const_iterator& rhs)
    const
{
    return actionIndex!=rhs.actionIndex || !(accset==rhs.accset);
}

inline
RO_AccSets::action_const_iterator::action_const_iterator
(AccSetPtr acceptanceSet, unsigned actionMaximumValue, unsigned index):
    accset(*acceptanceSet), actionIndex(index),
    actionMaxVal(actionMaximumValue)
{
    searchNextAction();
}



// Constructor
//========================================================================
inline RO_AccSets::RO_AccSets(const RO_AccSetsContainer* cont,
                              unsigned sn):
    container(cont), stateNumber(sn)
{}

inline RO_AccSets::~RO_AccSets() {}

// Reading methods
//========================================================================
inline RO_AccSets::AccSetPtr RO_AccSets::firstAccSet() const
{
    return AccSetPtr(container, container->getSetIndex(stateNumber));
}

inline RO_AccSets::AccSetPtr RO_AccSets::endAccSet() const
{
    return AccSetPtr(container,
                     container->getSetIndex(stateNumber)+
                     container->getSetsAmount(stateNumber));
}

inline RO_AccSets::action_const_iterator RO_AccSets::begin(AccSetPtr ptr) const
{
#ifdef DEBUG
    check_claim(ptr != endAccSet(),
                "In RO_AccSets::begin(): "
                "Tried to get action_const_iterator from an endAccSet() "
                "iterator.");
#endif

    return action_const_iterator(ptr, container->getActionMaxVal());
}

inline RO_AccSets::action_const_iterator RO_AccSets::end(AccSetPtr ptr) const
{
#ifdef DEBUG
    check_claim(ptr != endAccSet(),
                "In RO_AccSets::end(): "
                "Tried to get action_const_iterator from an endAccSet() "
                "iterator.");
#endif

    return action_const_iterator(ptr, container->getActionMaxVal(),
                                 container->getActionMaxVal());
}


inline bool
RO_AccSets::isMember(unsigned actionNumber, const AccSetPtr& ptr) const
{
#ifdef DEBUG
    check_claim(actionNumber > 0 &&
                actionNumber <= container->getActionMaxVal(),
                valueToMessage("In RO_AccSets::isMember(): "
                               "Parameter (actionNumber=", actionNumber,
                               ") is not in the valid range."));
    check_claim(ptr != endAccSet(),
                "In RO_AccSets::isMember(): "
                "Tried to use an endAccSet() iterator.");
#endif

    return (*ptr)[actionNumber-1];
}

inline bool RO_AccSets::isEmpty() const
{
    return container->getSetsAmount(stateNumber) == 0;
}

inline unsigned RO_AccSets::setsAmount() const
{
    return container->getSetsAmount(stateNumber);
}

inline BitVector RO_AccSets::getAllUsedActionsAsBitVector() const
{
    BitVector actions(container->getActionMaxVal());
    for(AccSetPtr iter = firstAccSet(); iter != endAccSet(); ++iter)
    {
        actions |= *iter;
    }
    return actions;
}

inline bool RO_AccSets::operator<(const RO_AccSets& rhs) const
{
#ifdef DEBUG
    check_claim(container->getActionMaxVal() ==
                rhs.container->getActionMaxVal(),
                "In AccSets::operator<(): "
                "actionMaxVal of parameter is not the same as in *this.");
#endif

    if(container->getSetsAmount(stateNumber) !=
       rhs.container->getSetsAmount(rhs.stateNumber))
        return container->getSetsAmount(stateNumber) <
            rhs.container->getSetsAmount(rhs.stateNumber);

    unsigned endInd = container->getSetIndex(stateNumber)+
        container->getSetsAmount(stateNumber);
    for(unsigned ind = container->getSetIndex(stateNumber),
            rhsind = rhs.container->getSetIndex(rhs.stateNumber);
        ind < endInd; ind++, rhsind++)
    {
        BitVector set1 = container->getAccSet(ind);
        BitVector set2 = rhs.container->getAccSet(rhsind);
        if(set1 < set2) return true;
        if(set2 < set1) return false;
    }
    return false;
}

inline bool RO_AccSets::operator==(const RO_AccSets& rhs) const
{
#ifdef DEBUG
    check_claim(container->getActionMaxVal() ==
                rhs.container->getActionMaxVal(),
                "In AccSets::operator<(): "
                "actionMaxVal of parameter is not the same as in *this.");
#endif

    if(container->getSetsAmount(stateNumber) !=
       rhs.container->getSetsAmount(rhs.stateNumber))
        return false;

    unsigned endInd = container->getSetIndex(stateNumber)+
        container->getSetsAmount(stateNumber);
    for(unsigned ind = container->getSetIndex(stateNumber),
            rhsind = rhs.container->getSetIndex(rhs.stateNumber);
        ind < endInd; ind++, rhsind++)
    {
        BitVector set1 = container->getAccSet(ind);
        BitVector set2 = rhs.container->getAccSet(rhsind);
        if(set1 != set2) return false;
    }
    return true;
}

inline bool RO_AccSets::operator!=(const RO_AccSets& rhs) const
{
    return !((*this) == rhs);
}


#undef CIRC_RO_ACCSETS_HH_
#endif
