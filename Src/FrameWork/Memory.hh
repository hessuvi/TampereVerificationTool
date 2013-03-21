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

// FILE_DES: memory.hh: Parallel
// Juha Nieminen

// $Id: Memory.hh 1.21 Mon, 31 May 2004 17:20:54 +0300 warp $
// 
// Rinnankytkijän käyttämä geneerinen muistihallintapalikka
//

// $Log:$

#ifdef CIRC_MEMORY_HH_
#error "Include recursion"
#endif

#ifndef ONCE_MEMORY_HH_
#define ONCE_MEMORY_HH_
#define CIRC_MEMORY_HH_

#include "error_handling.hh"

#include "MemoryStack.hh"

#include <climits>
#include <vector>
#include <TVTvec.hh>

//#define MEMORYVAR_HASHTABLE_DEBUG

// Class declaration:

//===========================================================================
//===========================================================================
// Base class
//===========================================================================
//===========================================================================
class MemoryBase
{
 public:
    typedef unsigned UnitType;
    typedef unsigned Pointer;
    typedef std::vector<unsigned> dataContainer_t;

    static const Pointer NULLPTR = ~0;

    inline void putPackedData(Pointer ptr, const dataContainer_t& data);
    inline unsigned getPackedDataVal(Pointer ptr, unsigned index) const;

    inline int
    comparePackedData(Pointer ptr, const dataContainer_t& data) const;

    // Info
    inline unsigned getAllocatedMemoryAmount() const
    {
        //return RAM.size()*sizeof(UnitType);
        return RAM.allocatedMem();
    }

 protected:
    inline MemoryBase();

// Return the amount of bits required by the given values:
    static inline unsigned requiredBits(UnitType maxValue);

    static inline void requiredBits(const std::vector<UnitType>& maxValues,
                                    std::vector<unsigned>& bits,
                                    unsigned valOffset=0);

// Return the amount of UnitTypes required by the given amount of items:
    static inline unsigned requiredMem(unsigned bits, unsigned amnt);

    static inline unsigned requiredMem(const std::vector<unsigned>& bits);


    //std::vector <UnitType> RAM;
    TVTvec<UnitType> RAM;
    unsigned memPtr;

 private:
    // Disable copying:
    MemoryBase (const MemoryBase&);
    MemoryBase& operator=(const MemoryBase&);
};

//===========================================================================
//===========================================================================
// MemoryEq
//===========================================================================
//===========================================================================
class MemoryEq: public MemoryBase
{
public:
//---
#ifdef DEBUG
#ifndef MEMORYEQ_USE_SMART_POINTER
#define MEMORYEQ_USE_SMART_POINTER
#endif
#endif

#ifdef MEMORYEQ_USE_SMART_POINTER
    class Pointer
    {
     public:
        inline Pointer(MemoryBase::Pointer ptr = MemoryBase::NULLPTR):
            value(ptr), allocatedAmnt(0) {}
        inline unsigned allocatedItemsAmnt() const { return allocatedAmnt; }

        inline bool operator==(const Pointer& rhs) const
        { return value == rhs.value && allocatedAmnt == rhs.allocatedAmnt; }

     private:
        friend class MemoryEq;
        friend class MemoryBase;
        inline Pointer(unsigned v, unsigned a):
            value(v), allocatedAmnt(a) {}
        inline operator unsigned() { return value; }
        inline unsigned operator++() { return ++value; }
        inline unsigned operator+=(int i) { return value+=i; }

        unsigned value, allocatedAmnt;
    };
#endif
//---

    // Constructors:
    inline MemoryEq();

    inline MemoryEq(UnitType itemMaxValue);
    inline void setItemMaxValue(UnitType itemMaxValue);

    // Memory allocation:
    inline Pointer allocateItems(unsigned itemsAmnt);

    // Put given group to the given location:
    inline void putItems(Pointer ptr, const std::vector<UnitType>& values);

    // Read group from the given location:
    inline void getItems(Pointer ptr, std::vector<UnitType>& values) const;

    inline UnitType getItem(Pointer ptr, unsigned index) const;

    // Compare two groups:
    inline bool compare(Pointer ptr1, Pointer ptr2, unsigned itemsAmnt) const;


// Low-level methods:
    inline void putItems(dataContainer_t& dest,
                         const std::vector<UnitType>& values) const;
    inline void getPackedData(Pointer ptr, dataContainer_t& dest,
                              unsigned itemsAmnt) const;
    inline unsigned getPackedDataMaxIndex(unsigned itemsAmnt) const;

#ifdef DEBUG
    inline void putPackedData(Pointer ptr, const dataContainer_t& data);
    inline unsigned getPackedData(Pointer ptr, unsigned index) const;
    inline int
    comparePackedData(Pointer ptr, const dataContainer_t& data) const;
#endif

//===========================================================================
 private:
//===========================================================================
    static const unsigned UBITS=(sizeof(unsigned)*CHAR_BIT);

    unsigned itemBits;

#ifdef DEBUG
    bool itemMaxValIsSet;
    unsigned itemMaxVal;
#endif

    // Disable copying:
    MemoryEq (const MemoryEq&);
    MemoryEq& operator=(const MemoryEq&);
};








//===========================================================================
//===========================================================================
// MemoryVar
//===========================================================================
//===========================================================================
class MemoryVar: public MemoryBase
{
public:
    // Constructors:
    inline MemoryVar(unsigned minVal=0);

    inline MemoryVar(const std::vector<UnitType>& itemGroupMaxValues,
                     unsigned minVal=0);
    inline void setItemMaxValue(const std::vector<UnitType>& itemGroupMaxValues);

    // Allocate memory:
    inline Pointer allocateVarItemsGroup();

    // Put given group to the given location:
    inline void putItems(Pointer ptr, const std::vector<UnitType>& values);

    // Read group from the given location:
    inline void getItems(Pointer ptr, std::vector<UnitType>& values) const;

    inline UnitType getItem(Pointer ptr, unsigned index) const;

    // Compare two groups:
    inline bool compare(Pointer ptr1, Pointer ptr2) const;


// Low-level methods:
    inline void putItems(dataContainer_t& dest,
                         const std::vector<UnitType>& values) const;
    inline void getPackedData(Pointer ptr, dataContainer_t& dest) const;
    inline unsigned getPackedDataMaxIndex() const;

#ifdef MEMORYVAR_ALLOW_FINDING
    inline unsigned findPackedData(const dataContainer_t& data);

    inline unsigned getAllocatedMemoryAmount() const
    {
        return MemoryBase::getAllocatedMemoryAmount()+
            //nextPtrs.size()*sizeof(unsigned)+
            nextPtrs.usedMemory()+
            //hashTable.size()*sizeof(unsigned);
            hashTable.usedMemory();
    }
#endif

#if defined(DEBUG) || defined(MEMORYVAR_ALLOW_FINDING)
    inline void putPackedData(Pointer ptr, const dataContainer_t& data);
#endif

#ifdef DEBUG
    inline unsigned getPackedData(Pointer ptr, unsigned index) const;
    inline int
    comparePackedData(Pointer ptr, const dataContainer_t& data) const;
#endif

    inline Pointer getPointer(unsigned which) const
    {
        return which*requiredMemForItemGroup;
    }


//===========================================================================
 private:
//===========================================================================
    static const unsigned UBITS=(sizeof(unsigned)*CHAR_BIT);

    std::vector<unsigned> itemGroupBits;
    unsigned requiredMemForItemGroup;
    const unsigned valOffset;
    mutable unsigned prevIndex, prevLowBit;

#ifdef MEMORYVAR_ALLOW_FINDING
    //Hash table stuff
    static const unsigned HASHTABLE_INIT_SIZE = 32768;
    static const unsigned MAX_HASHTABLE_LENGTH = 16;
    unsigned maxHashtableLength;
    MemoryStack nextPtrs;
    MemoryStack hashTable;
    unsigned hashTableSizeMask;

    inline void increaseHashTable();

    template<typename contType>
    inline unsigned hashKeyFunction(//const dataContainer_t& data,
                                    const contType& data,
                                    unsigned ptr) const
    {
        const unsigned prime = 100000037;
        const unsigned s = 159;
        unsigned value = data[ptr++];
        unsigned res = (value&0xFFFF)%prime;
        res = (res*s)%prime + (value>>16)%prime;
        for(unsigned i=1; i<requiredMemForItemGroup; ++i, ++ptr)
        {
            value = data[ptr];
            res = ((res*s)%prime + (value&0xFFFF))%prime;
            res = ((res*s)%prime + (value>>16))%prime;
        }
        res = ((159*res)%prime + 93)%prime;
        return res&hashTableSizeMask;
    }

    inline unsigned getHashIndex(const dataContainer_t& data) const
    {
        return hashKeyFunction(data, 0);
    }
    inline unsigned getHashIndex(Pointer ptr) const
    {
        return hashKeyFunction(RAM, ptr);
    }
#endif

#ifdef DEBUG
    std::vector<UnitType> itemGroupMaxVals;
#endif

    // Disable copying:
    MemoryVar (const MemoryVar&);
    MemoryVar& operator=(const MemoryVar&);
};








//===========================================================================
// Implementations
//===========================================================================
#define INDBITS (sizeof(UnitType)*CHAR_BIT)

// Required bits
// -------------
inline unsigned MemoryBase::requiredBits(UnitType maxValue)
{
    if(maxValue==0) return 1;
    unsigned bit = INDBITS;
    while(!(maxValue&(1<<(bit-1)))) bit--;
    return bit;
}

inline void MemoryBase::requiredBits(const std::vector<UnitType>& maxValues,
                                     std::vector<unsigned>& bits,
                                     unsigned valOffset)
{
    bits.resize(maxValues.size());
    for(unsigned i=0; i<maxValues.size(); i++)
        bits[i] = requiredBits(maxValues[i]-valOffset);
}

// Required mem
// ------------
inline unsigned MemoryBase::requiredMem(unsigned bits, unsigned amnt)
{
    return ((bits*amnt-1)/INDBITS)+1;
}

inline unsigned MemoryBase::requiredMem(const std::vector<unsigned>& bits)
{
    unsigned total=0;
    for(unsigned i=0; i<bits.size(); i++)
        total += bits[i];
    return ((total-1)/INDBITS)+1;
}

//===========================================================================
// Constructors
//===========================================================================
inline MemoryBase::MemoryBase(): memPtr(0) {}

inline MemoryEq::MemoryEq():
    itemBits(1)
#ifdef DEBUG
    , itemMaxValIsSet(false), itemMaxVal(0)
#endif
{}

inline MemoryVar::MemoryVar(unsigned minVal):
    requiredMemForItemGroup(1), valOffset(minVal),
    prevIndex(~0U-1), prevLowBit(0)
#ifdef MEMORYVAR_ALLOW_FINDING
    ,maxHashtableLength(MAX_HASHTABLE_LENGTH)
    //,hashTable(HASHTABLE_INIT_SIZE, NULLPTR)
    ,hashTable(HASHTABLE_INIT_SIZE, 65535)
    ,hashTableSizeMask(HASHTABLE_INIT_SIZE-1)
#endif
{}

inline MemoryEq::MemoryEq(UnitType itemMaxValue):
    itemBits(requiredBits(itemMaxValue))
#ifdef DEBUG
    , itemMaxValIsSet(true), itemMaxVal(itemMaxValue)
#endif
{}

inline void MemoryEq::setItemMaxValue(UnitType itemMaxValue)
{
    itemBits = requiredBits(itemMaxValue);

#ifdef DEBUG
    warn_ifnot(itemMaxValIsSet == false,
               "In MemoryEq::setItemMaxValue(): "
               "Setting item max. value more than once might be a bug.");

    itemMaxValIsSet = true;
    itemMaxVal = itemMaxValue;
#endif
}

inline void
MemoryVar::setItemMaxValue(const std::vector<UnitType>& itemGroupMaxValues)
{
#ifdef DEBUG
    warn_ifnot(itemGroupBits.size() == 0,
               "In MemoryVar::setItemMaxValue(): "
               "Setting max. values for items more than once might be a bug.");
    itemGroupMaxVals = itemGroupMaxValues;
#endif

    requiredBits(itemGroupMaxValues, itemGroupBits, valOffset);
    requiredMemForItemGroup = requiredMem(itemGroupBits);
}

inline MemoryVar::MemoryVar(const std::vector<UnitType>& itemGroupMaxValues,
                            unsigned minVal):
    valOffset(minVal),
    prevIndex(~0U-1), prevLowBit(0)
#ifdef MEMORYVAR_ALLOW_FINDING
    ,maxHashtableLength(MAX_HASHTABLE_LENGTH)
    //,hashTable(HASHTABLE_INIT_SIZE, NULLPTR)
    ,hashTable(HASHTABLE_INIT_SIZE, 65535)
    ,hashTableSizeMask(HASHTABLE_INIT_SIZE-1)
#endif
{
    setItemMaxValue(itemGroupMaxValues);
}

// Allocate
//===========================================================================
inline MemoryEq::Pointer MemoryEq::allocateItems(unsigned itemsAmnt)
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::allocateItems(): "
                "Trying to allocate space wihtout setting item max. value "
                "first.");
#endif

    unsigned ptr = memPtr;
    unsigned memAmnt = requiredMem(itemBits, itemsAmnt);
    memPtr += memAmnt;
    RAM.resize(memPtr);

#ifdef MEMORYEQ_USE_SMART_POINTER
    return Pointer(ptr, itemsAmnt);
#else
    return ptr;
#endif
}

inline MemoryVar::Pointer MemoryVar::allocateVarItemsGroup()
{
#ifdef DEBUG
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::allocateVarItemsGroup(): "
                "Trying to allocate space without setting max. values first.");
#endif

    unsigned ptr = memPtr;
    memPtr += requiredMemForItemGroup;
    RAM.resize(memPtr);

    return ptr;
}

// Put
//===========================================================================
// Eq
// --
inline void MemoryEq::putItems(Pointer ptr, const std::vector<UnitType>& values)
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::putItems(): "
                "Trying to put items although item max. value has not "
                "been set.");

    check_claim(ptr < RAM.size(),
                "In MemoryEq::putItems(): "
                "Given pointer is invalid.");

    check_claim(values.size() == ptr.allocatedItemsAmnt(),
                "In MemoryEq::putItems(): "
                "Parameter 'values' has different amount of items than "
                "allocated for parameter 'ptr'.");

    for(unsigned i=0; i<values.size(); i++)
    {
        check_claim(values[i] <= itemMaxVal,
                    valueToMessage("In MemoryEq::putItems(): "
                                   "Given value (", values[i],
                                   valueToMessage(") is larger than the "
                                                  "item max. value (",
                                                  itemMaxVal, ").")));
    }
#endif

    for(unsigned i=0, lowbit=0; i<values.size(); i++)
    {
        RAM[ptr] |= values[i]<<lowbit;
        if(lowbit+itemBits > INDBITS)
            RAM[ptr+1] |= values[i]>>(INDBITS-lowbit);

        lowbit += itemBits;
        if(lowbit >= INDBITS) { ++ptr; lowbit-=INDBITS; }
    }
}

inline void MemoryEq::putItems(dataContainer_t& dest,
                               const std::vector<UnitType>& values) const
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::putItems(): "
                "Trying to put items although item max. value has not "
                "been set.");

    for(unsigned i=0; i<values.size(); i++)
    {
        check_claim(values[i] <= itemMaxVal,
                    valueToMessage("In MemoryEq::putItems(): "
                                   "Given value (", values[i],
                                   valueToMessage(") is larger than the "
                                                  "item max. value (",
                                                  itemMaxVal, ").")));
    }
#endif

    dest.clear();
    dest.resize(requiredMem(itemBits, values.size()), 0);
    for(unsigned i=0, lowbit=0, ptr=0; i<values.size(); i++)
    {
        dest[ptr] |= values[i]<<lowbit;
        if(lowbit+itemBits > INDBITS)
            dest[ptr+1] |= values[i]>>(INDBITS-lowbit);

        lowbit += itemBits;
        if(lowbit >= INDBITS) { ++ptr; lowbit-=INDBITS; }
    }
}

// Var
// ---
inline void MemoryVar::putItems(Pointer ptr, const std::vector<UnitType>& values)
{
#ifdef DEBUG
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::putItems(): "
                "Trying to put items although item max. value has not "
                "been set.");

    check_claim(ptr < RAM.size(),
                "In MemoryVar::putItems(): "
                "Given pointer is invalid.");

    check_claim(values.size() == itemGroupMaxVals.size(),
                "In MemoryVar::putItems(): "
                "Parameter 'values' has different amount of items than "
                "allocated for parameter 'ptr'.");

    for(unsigned i=0; i<values.size(); i++)
    {
        check_claim(values[i] <= itemGroupMaxVals[i],
                    valueToMessage("In MemoryVar::putItems(): "
                                   "Given value (", values[i],
                                   valueToMessage(") is larger than the "
                                                  "item max. value (",
                                                  itemGroupMaxVals[i], ").")));
    }
#endif

#ifdef MEMORYVAR_ALLOW_FINDING
    check_claim(false, "In MemoryVar::putItems(): "
                "Sorry, you must use putPackedData() when"
                "MEMORYVAR_ALLOW_FINDING is enabled.");
#endif

    for(unsigned i=0, lowbit=0; i<values.size(); i++)
    {
        RAM[ptr] |= (values[i]-valOffset)<<lowbit;
        if(lowbit+itemGroupBits[i] > INDBITS)
            RAM[ptr+1] |= (values[i]-valOffset)>>(INDBITS-lowbit);

        lowbit += itemGroupBits[i];
        if(lowbit >= INDBITS) { ++ptr; lowbit-=INDBITS; }
    }
}

inline void MemoryVar::putItems(dataContainer_t& dest,
                                const std::vector<UnitType>& values) const
{
#ifdef DEBUG
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::putItems(): "
                "Trying to put items although items max. values have not "
                "been set.");

    for(unsigned i=0; i<values.size(); i++)
    {
        check_claim(values[i] <= itemGroupMaxVals[i],
                    valueToMessage("In MemoryVar::putItems(): "
                                   "Given value (", values[i],
                                   valueToMessage(") is larger than the "
                                                  "item max. value (",
                                                  itemGroupMaxVals[i], ").")));
    }
#endif

    dest.clear();
    dest.resize(requiredMemForItemGroup, 0);
    for(unsigned i=0, lowbit=0, ptr=0; i<values.size(); i++)
    {
        dest[ptr] |= (values[i]-valOffset)<<lowbit;
        if(lowbit+itemGroupBits[i] > INDBITS)
            dest[ptr+1] |= (values[i]-valOffset)>>(INDBITS-lowbit);

        lowbit += itemGroupBits[i];
        if(lowbit >= INDBITS) { ++ptr; lowbit-=INDBITS; }
    }
}


// Get
//===========================================================================
inline void MemoryEq::getItems(Pointer ptr, std::vector<UnitType>& values) const
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::getItems(): "
                "Trying to get items although item max. value has not "
                "been set.");

    check_claim(ptr < RAM.size(),
                "In MemoryEq::getItems(): "
                "Given pointer is invalid.");

    check_claim(values.size() == ptr.allocatedItemsAmnt(),
                "In MemoryEq::getItems(): "
                "Parameter 'values' has different size than "
                "allocated for parameter 'ptr'.");
#endif

    //UnitType mask = (1<<itemBits)-1;
    UnitType mask = (~0U)>>(UBITS-itemBits);
    for(unsigned i=0, lowbit=0; i<values.size(); i++)
    {
        if(lowbit+itemBits > INDBITS)
            values[i] =
                ((RAM[ptr]>>lowbit)|(RAM[ptr+1]<<(INDBITS-lowbit)))&mask;
        else
            values[i] = (RAM[ptr]>>lowbit)&mask;

        lowbit += itemBits;
        if(lowbit >= INDBITS) { ++ptr; lowbit-=INDBITS; }
    }
}

inline MemoryEq::UnitType MemoryEq::getItem(Pointer ptr, unsigned index) const
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::getItem(): "
                "Trying to get items although item max. value has not "
                "been set.");

    check_claim(ptr < RAM.size(),
                "In MemoryEq::getItem(): "
                "Given pointer is invalid.");

    check_claim(index < ptr.allocatedItemsAmnt(),
                "In MemoryEq::getItem(): "
                "Parameter 'index' is larger than items "
                "allocated for parameter 'ptr'.");
#endif

    //UnitType mask = (1<<itemBits)-1;
    UnitType mask = (~0U)>>(UBITS-itemBits);
    unsigned lowbit = index*itemBits;
    ptr += lowbit/INDBITS;
    lowbit %= INDBITS;
    if(lowbit+itemBits > INDBITS)
        return ((RAM[ptr]>>lowbit)|(RAM[ptr+1]<<(INDBITS-lowbit)))&mask;
    else
        return (RAM[ptr]>>lowbit)&mask;
}

inline void
MemoryVar::getItems(Pointer ptr, std::vector<UnitType>& values) const
{
#ifdef DEBUG
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::getItems(): "
                "Trying to get items although items max. values have not "
                "been set.");

    check_claim(ptr < RAM.size(),
                "In MemoryVar::getItems(): "
                "Given pointer is invalid.");

    check_claim(values.size() == itemGroupMaxVals.size(),
                "In MemoryVar::getItems(): "
                "Parameter 'values' has different size than "
                "allocated for parameter 'ptr'.");
#endif

    for(unsigned i=0, lowbit=0; i<values.size(); i++)
    {
        //UnitType mask = (1<<itemGroupBits[i])-1;
        UnitType mask = (~0U)>>(UBITS-itemGroupBits[i]);

        if(lowbit+itemGroupBits[i] > INDBITS)
            values[i] = valOffset +
                (((RAM[ptr]>>lowbit)|(RAM[ptr+1]<<(INDBITS-lowbit)))&mask);
        else
            values[i] = valOffset + ((RAM[ptr]>>lowbit)&mask);

        lowbit += itemGroupBits[i];
        if(lowbit >= INDBITS) { ++ptr; lowbit-=INDBITS; }
    }
}

inline MemoryEq::UnitType MemoryVar::getItem(Pointer ptr, unsigned index) const
{
#ifdef DEBUG
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::getItem(): "
                "Trying to get items although items max. values have not "
                "been set.");

    check_claim(ptr < RAM.size(),
                "In MemoryVar::getItem(): "
                "Given pointer is invalid.");

    check_claim(index < itemGroupMaxVals.size(),
                "In MemoryVar::getItem(): "
                "Parameter 'index' is larger than size "
                "allocated for parameter 'ptr'.");
#endif

    //UnitType mask = (1<<itemGroupBits[index])-1;
    UnitType mask = (~0U)>>(UBITS-itemGroupBits[index]);
    unsigned lowbit = 0;
    if(index == prevIndex)
    {
        lowbit = prevLowBit;
    }
    else
    {
        if(index == prevIndex+1)
        {
            lowbit = prevLowBit + itemGroupBits[prevIndex];
        }
        else
        {
            for(unsigned i=0; i<index; ++i) lowbit += itemGroupBits[i];
        }
        prevIndex = index;
        prevLowBit = lowbit;
    }

    ptr += lowbit/INDBITS;
    lowbit %= INDBITS;

    if(lowbit+itemGroupBits[index] > INDBITS)
        return valOffset +
            (((RAM[ptr]>>lowbit)|(RAM[ptr+1]<<(INDBITS-lowbit)))&mask);
    else
        return valOffset + ((RAM[ptr]>>lowbit)&mask);
}

// Compare
//===========================================================================
inline bool
MemoryEq::compare(Pointer ptr1, Pointer ptr2, unsigned itemsAmnt) const
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::compare(): "
                "Trying to compare items although item max. value has not "
                "been set.");
    check_claim(ptr1 < RAM.size(),
                "In MemoryEq::compare(): "
                "Given 'ptr1' is invalid.");
    check_claim(ptr2 < RAM.size(),
                "In MemoryEq::compare(): "
                "Given 'ptr2' is invalid.");
    check_claim(ptr1.allocatedItemsAmnt() == ptr2.allocatedItemsAmnt(),
                "In MemoryEq::compare(): "
                "'ptr1' and 'ptr2' point to groups with different amount of "
                "items.");
    check_claim(ptr1.allocatedItemsAmnt() == itemsAmnt,
                "In MemoryEq::compare(): "
                "Space allocated to given pointers is not the same as given "
                "'itemsAmnt'.");
#endif

    unsigned memAmnt = requiredMem(itemBits, itemsAmnt);
    for(unsigned i=0; i<memAmnt; ++i, ++ptr1, ++ptr2)
    {
        if(RAM[ptr1] != RAM[ptr2]) return false;
    }
    return true;
}

inline bool MemoryVar::compare(Pointer ptr1, Pointer ptr2) const
{
#ifdef DEBUG
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::compare(): "
                "Trying to compare items although items max. values have not "
                "been set.");
    check_claim(ptr1 < RAM.size(),
                "In MemoryVar::compare(): "
                "Given 'ptr1' is invalid.");
    check_claim(ptr2 < RAM.size(),
                "In MemoryVar::compare(): "
                "Given 'ptr2' is invalid.");
#endif

    for(unsigned i=0; i<requiredMemForItemGroup; i++)
    {
        if(RAM[ptr1++] != RAM[ptr2++]) return false;
    }
    return true;
}


// Low-level methods
//===========================================================================
// putPackedData
// -------------
inline void MemoryBase::putPackedData(Pointer ptr, const dataContainer_t& data)
{
#ifdef DEBUG
    check_claim(ptr < RAM.size(),
                "In MemoryBase::putPackedData(): "
                "Given pointer is invalid.");
#endif

    for(unsigned i=0; i<data.size(); i++)
        RAM[ptr+i] = data[i];
}

// debug versions
// --------------
#ifdef DEBUG
inline void MemoryEq::putPackedData(Pointer ptr, const dataContainer_t& data)
{
    check_claim(itemMaxValIsSet,
                "In MemoryEq::putPackedData(): "
                "Trying to put items although item max. value has not "
                "been set.");
    check_claim(requiredMem(itemBits, ptr.allocatedItemsAmnt()) == data.size(),
                "In MemoryEq::putPackedData(): "
                "Size of parameter 'data' is not the same as space allocated "
                "to parameter 'ptr'.");

    MemoryBase::putPackedData(ptr, data);
}

#ifndef MEMORYVAR_ALLOW_FINDING
inline void MemoryVar::putPackedData(Pointer ptr, const dataContainer_t& data)
{
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::putPackedData(): "
                "Trying to put items although items max. values have not "
                "been set.");
    check_claim(requiredMemForItemGroup == data.size(),
                "In MemoryVar::putPackedData(): "
                "Size of parameter 'data' is not the same as space allocated "
                "to parameter 'ptr'.");

    MemoryBase::putPackedData(ptr, data);
}
#endif
#endif

// getPackedData
// -------------
inline void MemoryEq::getPackedData(Pointer ptr, dataContainer_t& dest,
                                    unsigned itemsAmnt) const
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::getPackedData(): "
                "Trying to get items although item max. value has not "
                "been set.");
    check_claim(ptr < RAM.size(),
                "In MemoryEq::getPackedData(): "
                "Given pointer is invalid.");
    check_claim(itemsAmnt == ptr.allocatedItemsAmnt(),
                "In MemoryEq::getPackedData(): "
                "Parameter 'itemsAmnt' is not the same as space allocated "
                "to parameter 'ptr'.");
#endif

    unsigned memAmnt = requiredMem(itemBits, itemsAmnt);
    dest.resize(memAmnt);
    for(unsigned i=0; i<memAmnt; i++)
        dest[i] = RAM[ptr+i];
}

inline void MemoryVar::getPackedData(Pointer ptr, dataContainer_t& dest) const
{
    dest.resize(requiredMemForItemGroup);
    for(unsigned i=0; i<requiredMemForItemGroup; i++)
        dest[i] = RAM[ptr+i];
}

// getPackedDataMaxIndex
// ---------------------
inline unsigned MemoryEq::getPackedDataMaxIndex(unsigned itemsAmnt) const
{
#ifdef DEBUG
    check_claim(itemMaxValIsSet,
                "In MemoryEq::getPackedDataMaxIndex(): "
                "Trying to get index although item max. value has not "
                "been set.");
#endif

    return requiredMem(itemBits, itemsAmnt)-1;
}

inline unsigned MemoryVar::getPackedDataMaxIndex() const
{
    return requiredMemForItemGroup-1;
}

// getPackedData (with index)
// --------------------------
inline unsigned MemoryBase::getPackedDataVal(Pointer ptr, unsigned index) const
{
#ifdef DEBUG
    check_claim(ptr < RAM.size(),
                "In MemoryBase::getPackedData(): "
                "Given pointer is invalid.");
#endif

    return RAM[ptr+index];
}

// comparePackedData
// -----------------
inline int MemoryBase::comparePackedData(Pointer ptr,
                                         const dataContainer_t& data) const
{
#ifdef DEBUG
    check_claim(ptr < RAM.size(),
                "In MemoryBase::comparePackedData(): "
                "Given pointer is invalid.");
#endif

    for(unsigned i=0; i<data.size(); i++)
    {
        if(RAM[ptr+i] > data[i]) return 1;
        else if(RAM[ptr+i] < data[i]) return -1;
    }
    return 0;
}

// debug versions
// --------------
#ifdef DEBUG
inline unsigned MemoryEq::getPackedData(Pointer ptr, unsigned index) const
{
    check_claim(itemMaxValIsSet,
                "In MemoryEq::getPackedData(): "
                "Trying to get items although item max. value has not "
                "been set.");
    check_claim(index <= getPackedDataMaxIndex(ptr.allocatedItemsAmnt()),
                "In MemoryEq::getPackedData(): "
                "Parameter 'index' is out or range.");

    return MemoryBase::getPackedDataVal(ptr, index);
}

inline unsigned MemoryVar::getPackedData(Pointer ptr, unsigned index) const
{
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::getPackedData(): "
                "Trying to get items although items max. values have not "
                "been set.");
    check_claim(index <= getPackedDataMaxIndex(),
                "In MemoryVar::getPackedData(): "
                "Parameter 'index' is out or range.");

    return MemoryBase::getPackedDataVal(ptr, index);
}

inline int MemoryEq::comparePackedData(Pointer ptr,
                                       const dataContainer_t& data) const
{
    check_claim(itemMaxValIsSet,
                "In MemoryEq::comparePackedData(): "
                "Trying to compare items although item max. value has not "
                "been set.");
    check_claim(requiredMem(itemBits, ptr.allocatedItemsAmnt()) == data.size(),
                "In MemoryEq::comparePackedData(): "
                "Size of parameter 'data' is not the same as space allocated "
                "to parameter 'ptr'.");

    return MemoryBase::comparePackedData(ptr, data);
}

inline int MemoryVar::comparePackedData(Pointer ptr,
                                        const dataContainer_t& data) const
{
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::comparePackedData(): "
                "Trying to compare items although items max. values have not "
                "been set.");
    check_claim(requiredMemForItemGroup == data.size(),
                "In MemoryVar::comparePackedData(): "
                "Size of parameter 'data' is not the same as space allocated "
                "to parameter 'ptr'.");

    return MemoryBase::comparePackedData(ptr, data);
}
#endif


// =======================================================================
// Hash table implementation
// =======================================================================
#ifdef MEMORYVAR_ALLOW_FINDING
inline void MemoryVar::putPackedData(Pointer ptr, const dataContainer_t& data)
{
#ifdef DEBUG
    check_claim(itemGroupBits.size() != 0,
                "In MemoryVar::putPackedData(): "
                "Trying to put items although items max. values have not "
                "been set.");
    check_claim(requiredMemForItemGroup == data.size(),
                "In MemoryVar::putPackedData(): "
                "Size of parameter 'data' is not the same as space allocated "
                "to parameter 'ptr'.");
#endif

    MemoryBase::putPackedData(ptr, data);


#if defined(DEBUG) || defined(MEMORYVAR_HASHTABLE_DEBUG)
    //check_claim(ptr/requiredMemForItemGroup == nextPtrs.size(),
    check_claim(ptr/requiredMemForItemGroup == nextPtrs.valuesAmount(),
                "In MemoryVar::putPackedData(): "
                "Oops! The hash table implementation doesn't allow changing "
                "old items, only adding new ones. Sorry...");
#endif

    unsigned hashInd = getHashIndex(data);
    //nextPtrs.push_back(hashTable[hashInd]);
    nextPtrs.push(hashTable.getValue(hashInd));
    //hashTable[hashInd] = nextPtrs.size()-1;
    hashTable.setValue(hashInd, nextPtrs.valuesAmount());
}

inline void MemoryVar::increaseHashTable()
{
    //unsigned size = hashTable.size()*2;
    unsigned size = hashTable.valuesAmount()*2;
    maxHashtableLength += 2;
    //hashTable.clear();
    //hashTable.resize(size, NULLPTR);
    hashTable.reset(size, 65535);
    hashTableSizeMask = size-1;

    //for(unsigned ind=0; ind<nextPtrs.size(); ++ind)
    for(unsigned ind=0; ind<nextPtrs.valuesAmount(); ++ind)
    {
        unsigned hashInd = getHashIndex(ind*requiredMemForItemGroup);
        //nextPtrs[ind] = hashTable[hashInd];
        nextPtrs.setValue(ind, hashTable.getValue(hashInd));
        //hashTable[hashInd] = ind;
        hashTable.setValue(hashInd, ind+1);
    }

#ifdef MEMORYVAR_HASHTABLE_DEBUG
    //cerr << "Incremented hash table size to " << hashTable.size()
    cerr << "Incremented hash table size to " << hashTable.valuesAmount()
         << " items." << endl;
    unsigned maxLength = 0;
    //for(unsigned i=0; i<hashTable.size(); ++i)
    for(unsigned i=0; i<hashTable.valuesAmount(); ++i)
    {
        unsigned length = 0;
        //for(unsigned ind=hashTable[i]; ind!=NULLPTR; ind=nextPtrs[ind])
        for(unsigned ind=hashTable.getValue(i); ind!=0;
            ind=nextPtrs.getValue(ind-1))
            ++length;
        if(length > maxLength) maxLength = length;
    }
    cerr << "Maximum length is now " << maxLength << " items." << endl;
#endif
}

inline unsigned MemoryVar::findPackedData(const dataContainer_t& data)
{
    unsigned length = 0;
    for(unsigned ind = hashTable.getValue(getHashIndex(data));
        ind != 0; ind = nextPtrs.getValue(ind-1))
    {
        if(comparePackedData((ind-1)*requiredMemForItemGroup, data) == 0)
        {
            if(length > maxHashtableLength) increaseHashTable();
            return ind-1;
        }
        ++length;
    }

    if(length > maxHashtableLength) increaseHashTable();

    return NULLPTR;
}

#endif

#undef CIRC_MEMORY_HH_
#endif
