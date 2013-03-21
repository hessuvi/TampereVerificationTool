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

// FILE_DES: MemoryStack.hh
// Juha Nieminen

// $Id: MemoryStack.hh 1.15 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// $Log:$

#ifdef CIRC_MEMORYSTACK_HH_
#error "Include recursion"
#endif

#ifndef ONCE_MEMORYSTACK_HH_
#define ONCE_MEMORYSTACK_HH_
#define CIRC_MEMORYSTACK_HH_

#ifdef DEBUG
#include "error_handling.hh"
#endif

#include <climits>
#include "TVTvec.hh"

// Class declaration:

class MemoryStack
{
 public:
    inline MemoryStack(unsigned maximumValue = 1);
    inline MemoryStack(unsigned initialSize, unsigned maximumValue);
    inline void setMaximumValue(unsigned maximumValue);

    // Adds a value to the end of the container. Returns its index.
    inline unsigned push(unsigned value);

    // Returns the last value in the container and removes it.
    inline unsigned pop();

    // Puts the given value at the given index.
    inline void setValue(unsigned n, unsigned value);

    // Returns the value at the given index.
    inline unsigned getValue(unsigned n) const;

    // Returns the size of the container (ie. how many items it contains).
    inline unsigned valuesAmount() const;

    // Returns the amount of allocated memory in bytes.
    inline unsigned usedMemory() const;


    // Resizes the container to the given size.
    inline void reset(unsigned newSize, unsigned maximumValue = 1);


//========================================================================
 private:
//========================================================================
    static const unsigned UBITS=(sizeof(unsigned)*CHAR_BIT);
    static const unsigned RAM_INCREASE_SIZE=10;

    struct Index
    {
        static const unsigned UBITS=(sizeof(unsigned)*CHAR_BIT);

        inline Index(): wIndex(0), bitIndex(0) {}
        inline Index(unsigned wind, unsigned bind):
            wIndex(wind), bitIndex(bind) {}
        inline bool operator<(const Index& rhs) const
        {
            return
                wIndex>rhs.wIndex ? false :
                wIndex<rhs.wIndex || bitIndex<rhs.bitIndex;
        }
        inline void addBitIndex(const unsigned bits)
        {
            bitIndex += bits;
            if(bitIndex >= UBITS) { ++wIndex; bitIndex %= UBITS; }
        }
        inline void subBitIndex(const unsigned bits)
        {
            if(bitIndex < bits)
            { --wIndex; bitIndex = bitIndex+UBITS-bits; }
            else
                bitIndex -= bits;
        }

        unsigned wIndex, bitIndex;
    };

    TVTvec<unsigned> RAM;
    unsigned currentBits; // Amount of bits currently taken by the items
    Index nextIndex; // Index to the next unused item
    unsigned currentMaxVal; // Closest 2^n-1, which is >= largest item
    unsigned valuesAmnt; // The size of the container

    inline unsigned requiredBits(unsigned value) const;
    inline unsigned maxVal(const unsigned bits) const;
    inline unsigned getMask(const unsigned bind,
                            const unsigned newBitInd) const;
    inline void setValue(const unsigned value, const Index ind,
                         const unsigned bits);
    inline void pushValue(const unsigned value);
    inline unsigned getValue(const TVTvec<unsigned>& vals,
                             const Index ind, const unsigned bits) const;
    inline void increaseCurrentBits(const unsigned maximumValue);
    inline Index getIndex(const unsigned n) const;
};

// Private method implementations
//========================================================================

// Returns the amount of bits required by the given value:
inline unsigned MemoryStack::requiredBits(unsigned value) const
{
    unsigned bits;
    for(bits = 0; value; ++bits, value >>= 1);
    return bits;
}

// Returns the correct currentMaxVal for the given amount of bits:
inline unsigned MemoryStack::maxVal(const unsigned bits) const
{
    return (bits>=UBITS)? ~0U : (1<<bits)-1;
}

// Returns the mask for a value completely inside an unsigned int:
inline unsigned MemoryStack::getMask(const unsigned bind,
                                     const unsigned newBitInd) const
{
    return ((~0U)>>bind) & ((~0U)<<(UBITS-newBitInd));
}

// Returns the Index pointing to the nth value in the container:
inline MemoryStack::Index MemoryStack::getIndex(const unsigned n) const
{
    //return Index((n*currentBits)/UBITS, (n*currentBits)%UBITS);

    // The above implementation can cause an overflow if n and currentbits
    // are too large. The implementation below works for all cases (and
    // isn't essentially slower):

    unsigned div = n/UBITS, rem = n%UBITS;
    unsigned rembits = rem*currentBits;
    return Index(div*currentBits+rembits/UBITS, rembits%UBITS);
}

// Puts the given value to the given index:
inline void MemoryStack::setValue(const unsigned value, const Index ind,
                                  const unsigned bits)
{
    unsigned newBitInd = ind.bitIndex+bits;
    if(newBitInd <= UBITS)
    {
        unsigned mask = ~getMask(ind.bitIndex, newBitInd);
        RAM[ind.wIndex] =
            (RAM[ind.wIndex]&mask) | (value << (UBITS-newBitInd));
    }
    else
    {
        unsigned extraBits = newBitInd%UBITS;
        RAM[ind.wIndex] =
            (RAM[ind.wIndex]&((~0U)<<(UBITS-ind.bitIndex))) |
            (value >> extraBits);
        RAM[ind.wIndex+1] =
            (RAM[ind.wIndex+1]&((~0U)>>extraBits)) |
            (value << (UBITS-extraBits));
    }
}

// Adds the given value to the end of the container:
inline void MemoryStack::pushValue(const unsigned value)
{
    Index next = nextIndex;
    next.addBitIndex(currentBits);
    if(next.wIndex >= RAM.size())
        RAM.resize(RAM.size()+RAM_INCREASE_SIZE);
    setValue(value, nextIndex, currentBits);
    nextIndex = next;
}

// Returns the value at the given index from the given container (vals):
inline unsigned MemoryStack::getValue(const TVTvec<unsigned>& vals,
                                      const Index ind,
                                      const unsigned bits) const
{
    unsigned newBitInd = ind.bitIndex+bits;
    if(newBitInd <= UBITS)
    {
        unsigned mask = getMask(ind.bitIndex, newBitInd);
        return (vals[ind.wIndex]&mask) >> (UBITS-newBitInd);
    }
    else
    {
        unsigned extraBits = newBitInd%UBITS;
        return
            ((vals[ind.wIndex]&((~0U)>>ind.bitIndex)) << extraBits) |
            (vals[ind.wIndex+1] >> (UBITS-extraBits));
    }
}

// Increases the amount of bits taken by each item (recalculates the
// whole container):
inline void MemoryStack::increaseCurrentBits(const unsigned maximumValue)
{
    unsigned oldBits = currentBits;
    Index oldIndex = nextIndex;
    currentBits = requiredBits(maximumValue);
    currentMaxVal = maxVal(currentBits);

/*
// Old implementation:
    nextIndex.wIndex = nextIndex.bitIndex = 0;

    TVTvec<unsigned> oldRAM;
    //unsigned div = valuesAmnt/UBITS, rem = valuesAmnt%UBITS;
    //unsigned rembits = rem*currentBits;
    //oldRAM.reserve(div*currentBits+rembits/UBITS + 1);
    oldRAM.swap(RAM);

    for(Index ind(0, 0); ind<oldIndex; ind.addBitIndex(oldBits))
    {
        pushValue(getValue(oldRAM, ind, oldBits));
    }
*/
// New implementation:
    nextIndex = getIndex(valuesAmnt);
    Index ind = getIndex(valuesAmnt-1);
    if(RAM.size() < nextIndex.wIndex+1) RAM.resize(nextIndex.wIndex+1);
    if(valuesAmnt > 0)
        while(true)
        {
            oldIndex.subBitIndex(oldBits);
            setValue(getValue(RAM, oldIndex, oldBits), ind, currentBits);
            if(!(ind.wIndex || ind.bitIndex)) break;
            ind.subBitIndex(currentBits);
        }
}


// Public method implementations
//========================================================================

//========================================================================
// Constructor
//========================================================================
inline MemoryStack::MemoryStack(unsigned maximumValue):
    currentBits(requiredBits(maximumValue)),
    currentMaxVal(maxVal(requiredBits(maximumValue))),
    valuesAmnt(0)
{
    if(maximumValue == 0) currentBits = currentMaxVal = 1;
}

inline MemoryStack::MemoryStack(unsigned initialSize, unsigned maximumValue):
    currentBits(requiredBits(maximumValue)),
    currentMaxVal(maxVal(requiredBits(maximumValue))),
    valuesAmnt(0)
{
    if(maximumValue == 0) currentBits = currentMaxVal = 1;
    setValue(initialSize-1, 0);
}

inline void MemoryStack::setMaximumValue(unsigned maximumValue)
{
    if(maximumValue > currentMaxVal) increaseCurrentBits(maximumValue);
}

//========================================================================
// push
//========================================================================
inline unsigned MemoryStack::push(unsigned value)
{
    if(value > currentMaxVal) increaseCurrentBits(value);
    pushValue(value);
    return valuesAmnt++;
}

//========================================================================
// pop
//========================================================================
inline unsigned MemoryStack::pop()
{
    nextIndex.subBitIndex(currentBits);
    --valuesAmnt;
    return getValue(RAM, nextIndex, currentBits);
}

//========================================================================
// setValue
//========================================================================
inline void MemoryStack::setValue(unsigned n, unsigned value)
{
    if(value > currentMaxVal) increaseCurrentBits(value);
    Index ind = getIndex(n);
    if(!(ind<nextIndex))
    {
        if(ind.wIndex+1 >= RAM.size())
            RAM.resize(ind.wIndex+RAM_INCREASE_SIZE);
        valuesAmnt = n+1;
        nextIndex = ind;
        nextIndex.addBitIndex(currentBits);
    }
    setValue(value, ind, currentBits);
}

//========================================================================
// getValue
//========================================================================
inline unsigned MemoryStack::getValue(unsigned n) const
{
    return n>=valuesAmnt ? 0 : getValue(RAM, getIndex(n), currentBits);
}

//========================================================================
// valuesAmount
//========================================================================
inline unsigned MemoryStack::valuesAmount() const
{
    return valuesAmnt;
}

//========================================================================
// usedMemory
//========================================================================
inline unsigned MemoryStack::usedMemory() const
{
    return RAM.size()*sizeof(unsigned);
}


//========================================================================
// reset
//========================================================================
inline void MemoryStack::reset(unsigned newSize, unsigned maximumValue)
{
    RAM.clear();
    valuesAmnt = 0;
    currentBits = requiredBits(maximumValue);
    currentMaxVal = maxVal(currentBits);
    if(maximumValue == 0) currentBits = currentMaxVal = 1;
    setValue(newSize-1, 0);
}

#undef CIRC_MEMORYSTACK_HH_
#endif
