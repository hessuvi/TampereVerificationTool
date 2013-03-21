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

#ifdef CIRC_STATEPROPS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATEPROPS_HH_
#define ONCE_STATEPROPS_HH_
#define CIRC_STATEPROPS_HH_

#include "BitVector.hh"
#include "LSTS_File/StatePropsAP.hh"
#include "LSTS_File/Header.hh"

#include "error_handling.hh"

#include <deque>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// Class declaration:

class StatePropsContainer: public iStatePropsAP, public oStatePropsAP
{
 public:
    class StatePropsPtr
    {
     public:
        inline StatePropsPtr(): ptr(0) {}

        inline bool operator<(const StatePropsPtr&) const;
        inline bool operator==(const StatePropsPtr&) const;
        inline bool operator!=(const StatePropsPtr&) const;
        inline void setProp(unsigned propNumber);
        inline void unsetProp(unsigned propNumber);
        inline bool isPropSet(unsigned propNumber) const;
        inline bool isEmpty() const;
        inline void unionAndAssign(const StatePropsPtr& src);

        //inline StatePropsPtr& operator=(const StatePropsPtr& rhs);

     private:
        friend class StatePropsContainer;
        explicit inline StatePropsPtr(BitVector* bvptr): ptr(bvptr) {}
        inline void operator=(BitVector* p) { ptr=p; }
        inline BitVector* get() { return ptr; }
        inline const BitVector* get() const { return ptr; }
        BitVector* ptr;
    };

/***OTFVI***
// This class has become obsolete due to changes on state propositions
// handling. All code related to this begins with ***OTFVI***
    class OTFVI
    {
     public:
        inline OTFVI(): bits(0) {}
        inline unsigned getAllBitsAsUnsigned() const; // (msb)ILDRC(lsb)

        inline bool isCutState() const;
        inline void setCutState();
        inline void unsetCutState();

        inline bool isRejectState() const;
        inline void setRejectState();
        inline void unsetRejectState();

        inline bool isDeadlockRejectState() const;
        inline void setDeadlockRejectState();
        inline void unsetDeadlockRejectState();

        inline bool isLivelockRejectState() const;
        inline void setLivelockRejectState();
        inline void unsetLivelockRejectState();

        inline bool isInfinityRejectState() const;
        inline void setInfinityRejectState();
        inline void unsetInfinityRejectState();

     private:
        unsigned bits;
    };
*/

    StatePropsContainer(unsigned maxVal=0, bool sortSPNames=false);
    ~StatePropsContainer();

    void setMaxVal(unsigned maxVal);
    inline unsigned getMaxVal() const { return maxPropVal; }

/***OTFVI***
    inline void separateOTFVI(bool yes=true) { readSeparateOTFVI = yes; }
*/

    inline StatePropsPtr getStateProps(unsigned stateNumber);
    inline const StatePropsPtr getStateProps(unsigned stateNumber) const;

    inline StatePropsPtr assignStateProps(unsigned stateNumber,
                                          const StatePropsPtr& src);

/***OTFVI***
    inline OTFVI& getOTFVI(unsigned stateNumber);
    inline const OTFVI& getOTFVI(unsigned stateNumber) const;
*/

    inline std::string& getStatePropName(unsigned propNumber);
    inline const std::string& getStatePropName(unsigned propNumber) const;
    inline unsigned getMaxStatePropNameNumber() const;

    static inline StatePropsPtr getEmptyStatePropsPtr();

    inline unsigned getStatePropSetsAmount() const;

    inline bool isInitialized() const;
    inline bool isEmpty() const;
    inline bool allSetsAreEmpty() const;

    inline unsigned firstProp(StatePropsPtr handle) const;
    inline unsigned nextProp(StatePropsPtr handle) const;

    inline unsigned calculateStatePropCnt() const;

//==========================================================================
//==========================================================================
 protected:
    // Methods for LSTS-reading:
    virtual void lsts_StartStateProps(Header&);

/***OTFVI***
    virtual void lsts_CutState(lsts_index_t state_number);
    virtual void lsts_RejectState(lsts_index_t state_number);
    virtual void lsts_DeadlockRejectState(lsts_index_t state_number);
    virtual void lsts_LivelockRejectState(lsts_index_t state_number);
    virtual void lsts_InfinityRejectState(lsts_index_t state_number);
*/

    virtual void lsts_StartPropStates(const std::string& statePropName);
    virtual void lsts_PropState(lsts_index_t state_number);
    virtual void lsts_EndPropStates(const std::string& statePropName);
    virtual void lsts_EndStateProps();

    // Methods for LSTS-writing:
    virtual lsts_index_t lsts_numberOfStatePropsToWrite();
    virtual void lsts_WriteStateProps(iStatePropsAP& pipe);

 private:
    std::deque<BitVector> sets;

    BitVector emptyBitVector;

/***OTFVI***
    std::deque<OTFVI> otfvi;
    bool readSeparateOTFVI;
*/

    unsigned maxPropVal;
    mutable unsigned propIndex;

    std::deque<std::string> statepropnames;

/***OTFVI***
    OTFVI emptyotfvi;
*/

    bool sortNames;

    // Temporary data structures for sorting
    // -------------------------------------
    struct SPData
    {
        std::string spname;
        std::deque<lsts_index_t>* states;
        SPData(const std::string& name, std::deque<lsts_index_t>* sptr):
            spname(name), states(sptr)
        {}
        bool operator<(const SPData& rhs) const { return spname < rhs.spname; }
    };
    std::deque<std::deque<lsts_index_t> > tempStateNumbers;
    std::deque<SPData> tempSPData;

    // Disable copying
    //StatePropsContainer (const StatePropsContainer&);
    //StatePropsContainer& operator=(const StatePropsContainer&);
};

// Implementations
//==========================================================================

/***OTFVI***
// OTFVI
// -----
// (msb)ILDRC(lsb)
inline unsigned StatePropsContainer::OTFVI::getAllBitsAsUnsigned() const
{ return bits; }

inline bool StatePropsContainer::OTFVI::isCutState() const
{ return (bits&1) != 0; }
inline void StatePropsContainer::OTFVI::setCutState()
{ bits |= 1; }
inline void StatePropsContainer::OTFVI::unsetCutState()
{ bits &= ~1; }

inline bool StatePropsContainer::OTFVI::isRejectState() const
{ return (bits&2) != 0; }
inline void StatePropsContainer::OTFVI::setRejectState()
{ bits |= 2; }
inline void StatePropsContainer::OTFVI::unsetRejectState()
{ bits &= ~2; }

inline bool StatePropsContainer::OTFVI::isDeadlockRejectState() const
{ return (bits&4) != 0; }
inline void StatePropsContainer::OTFVI::setDeadlockRejectState()
{ bits |= 4; }
inline void StatePropsContainer::OTFVI::unsetDeadlockRejectState()
{ bits &= ~4; }

inline bool StatePropsContainer::OTFVI::isLivelockRejectState() const
{ return (bits&8) != 0; }
inline void StatePropsContainer::OTFVI::setLivelockRejectState()
{ bits |= 8; }
inline void StatePropsContainer::OTFVI::unsetLivelockRejectState()
{ bits &= ~8; }

inline bool StatePropsContainer::OTFVI::isInfinityRejectState() const
{ return (bits&16) != 0; }
inline void StatePropsContainer::OTFVI::setInfinityRejectState()
{ bits |= 16; }
inline void StatePropsContainer::OTFVI::unsetInfinityRejectState()
{ bits &= ~16; }
*/



inline bool StatePropsContainer::isInitialized() const
{
    return maxPropVal != 0;
}

inline bool StatePropsContainer::isEmpty() const
{
    return sets.size() == 0;
}

inline bool StatePropsContainer::allSetsAreEmpty() const
{
    for(unsigned i=0; i<sets.size(); ++i)
        if(!sets[i].isEmpty()) return false;

/***OTFVI***
    for(unsigned i=0; i<otfvi.size(); ++i)
        if(otfvi[i].getAllBitsAsUnsigned()) return false;
*/

    return true;
}

inline StatePropsContainer::StatePropsPtr
StatePropsContainer::getEmptyStatePropsPtr()
{
    return StatePropsPtr();
}

// Proposition set creation and setting
//==========================================================================
inline StatePropsContainer::StatePropsPtr
StatePropsContainer::getStateProps(unsigned stateNumber)
{
    check_claim(maxPropVal > 0,
                "In StatePropsContainer::getStateProps(): "
                "Attempt to create state props without initializing state "
                "prop max value.");

    if(sets.size() <= stateNumber)
        sets.resize(stateNumber+1, emptyBitVector);

    return StatePropsPtr(&sets[stateNumber]);
}

inline const StatePropsContainer::StatePropsPtr
StatePropsContainer::getStateProps(unsigned stateNumber) const
{
    check_claim(maxPropVal > 0,
                "In StatePropsContainer::getStateProps(): "
                "Attempt to create state props without initializing state "
                "prop max value.");

    if(sets.size() <= stateNumber) return StatePropsPtr();

    return StatePropsPtr(const_cast<BitVector*>(&sets[stateNumber]));
}

inline StatePropsContainer::StatePropsPtr
StatePropsContainer::assignStateProps(unsigned stateNumber,
                                      const StatePropsPtr& src)
{
    check_claim(maxPropVal > 0,
                "In StatePropsContainer::getStateProps(): "
                "Attempt to create state props without initializing state "
                "prop max value.");

    if(!src.get()) return src;

    if(sets.size() <= stateNumber)
        sets.resize(stateNumber+1, emptyBitVector);

    StatePropsPtr ptr(&sets[stateNumber]);
    (*ptr.get()) = (*src.get());

    return ptr;
}


/***OTFVI***
inline StatePropsContainer::OTFVI&
StatePropsContainer::getOTFVI(unsigned stateNumber)
{
    if(otfvi.size() <= stateNumber) otfvi.resize(stateNumber+1);
    if(sets.size() <= stateNumber)
        sets.resize(stateNumber+1, maxPropVal);

    return otfvi[stateNumber];
}

inline const StatePropsContainer::OTFVI&
StatePropsContainer::getOTFVI(unsigned stateNumber) const
{
    if ( otfvi.size() <= stateNumber )
    {
        return emptyotfvi;
    }
    else
    {
	// check_claim(otfvi.size() > stateNumber,
	//	    "In StatePropsContainer::getOTFVI() const: "
	//	    "Can't create new OTVFI in a const method.");

	return otfvi[stateNumber];
    }
}
*/

inline std::string& StatePropsContainer::getStatePropName(unsigned propNumber)
{
    if(statepropnames.size() <= propNumber)
        statepropnames.resize(propNumber+1);

    return statepropnames[propNumber];
}

inline const std::string&
StatePropsContainer::getStatePropName(unsigned propNumber) const
{
    check_claim(statepropnames.size() > propNumber,
                "In StatePropsContainer::getStatePropName() const: "
                "Internal error: Can't create new state prop name in a const method. Please make a bug report.");

    return statepropnames[propNumber];
}

inline unsigned StatePropsContainer::getMaxStatePropNameNumber() const
{
    return statepropnames.size()-1;
}


inline unsigned StatePropsContainer::getStatePropSetsAmount() const
{
    return sets.size();
}


inline void StatePropsContainer::StatePropsPtr::setProp(unsigned propNumber)
{
#ifdef DEBUG
    check_claim(ptr!=0,
                "In StatePropsContainer::StatePropsPtr::setProp(): "
                "Attempt to modify the empty StateProps.");
#endif

    ptr->setBit(propNumber-1);
}


inline void StatePropsContainer::StatePropsPtr::unsetProp(unsigned propNumber)
{
#ifdef DEBUG
    check_claim(ptr!=0,
                "In StatePropsContainer::StatePropsPtr::unsetProp(): "
                "Attempt to modify the empty StateProps.");
#endif

    ptr->setBit(propNumber-1, false);
}

inline bool
StatePropsContainer::StatePropsPtr::isPropSet(unsigned propNumber) const
{
    if(ptr==0) return false;
    return ptr->operator[](propNumber-1);
}


// Comparison
//=========================================================================
inline bool StatePropsContainer::StatePropsPtr::isEmpty() const
{
    return ptr == 0 || ptr->isEmpty();
}


inline bool
StatePropsContainer::StatePropsPtr::operator==(const StatePropsPtr& rhs) const
{
    if(ptr==0 && rhs.ptr==0) return true;
    if(ptr==0) return rhs.ptr->isEmpty();
    if(rhs.ptr==0) return ptr->isEmpty();
    return (*ptr) == (*rhs.ptr);
}

inline bool
StatePropsContainer::StatePropsPtr::operator!=(const StatePropsPtr& rhs) const
{
    return !((*this)==rhs);
}

inline bool
StatePropsContainer::StatePropsPtr::operator<(const StatePropsPtr& rhs) const
{
    return
        rhs.ptr==0 ? false :
        ptr==0 ? !(rhs.ptr==0 || rhs.ptr->isEmpty()) :
        (*ptr) < (*rhs.ptr);
}

/*
inline StatePropsContainer::StatePropsPtr&
StatePropsContainer::StatePropsPtr::operator=(const StatePropsPtr& rhs)
{
    (*ptr) = (*rhs.ptr);
    return *this;
}
*/


// Union
//==========================================================================
inline void
StatePropsContainer::StatePropsPtr::unionAndAssign(const StatePropsPtr& src)
{
#ifdef DEBUG
    check_claim(ptr!=0,
                "In StatePropsContainer::StatePropsPtr::unionAndAssign(): "
                "Attempt to modify the empty StateProps.");
#endif

    if(src.ptr == 0) return;
    (*ptr) |= (*src.ptr);
}


// Reading
//==========================================================================
inline unsigned StatePropsContainer::nextProp(StatePropsPtr handle) const
{
#ifdef DEBUG
    check_claim(!handle.isEmpty(),
                "In StatePropsContainer::nextProp(): "
                "Called nextProp for an empty StatePropsPtr, which is a bug.");
#endif

    for(; propIndex<maxPropVal; propIndex++)
    {
        if((*handle.get())[propIndex]) return (++propIndex);
    }
    return 0;
}

inline unsigned StatePropsContainer::firstProp(StatePropsPtr handle) const
{
    if(handle.isEmpty()) return 0;

    propIndex = 0;
    return nextProp(handle);
}


inline unsigned StatePropsContainer::calculateStatePropCnt() const
{
/***OTFVI***
    unsigned usedotfvi = 0;
    for(unsigned i=0; i<otfvi.size(); ++i)
        if((usedotfvi|=otfvi[i].getAllBitsAsUnsigned()) == 31) break;

    unsigned cnt = 0;
    while(usedotfvi)
    {
        if(usedotfvi&1) ++cnt;
        usedotfvi >>= 1;
    }

    return cnt + getMaxStatePropNameNumber();
*/
    return getMaxStatePropNameNumber();
}

#undef CIRC_STATEPROPS_HH_
#endif
