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


#ifdef CIRC_BITVECTOR_HH_
#error "Include recursion"
#endif

#ifndef ONCE_BITVECTOR_HH_
#define ONCE_BITVECTOR_HH_
#define CIRC_BITVECTOR_HH_

#include "error_handling.hh"

#ifndef MAKEDEPEND
#include <climits>
//#include <iostream>
#endif


class BitVector
{
public:

    inline BitVector(unsigned bitsAmnt);
    inline BitVector(unsigned bitsAmnt, unsigned* bitsBuf);
    inline BitVector (const BitVector&);
    inline BitVector (const BitVector&, unsigned* bitsBuf);
    inline BitVector& operator=(const BitVector&);

    inline ~BitVector();

    // If the bitvector is created using preallocated memory, this
    // method tells how much of that memory (in unsigned ints) is needed
    // to store the given amount of bits.
    static inline unsigned bufferSize(unsigned bitsAmnt);

    // Comparison
    inline bool operator==(const BitVector&) const;
    inline bool operator!=(const BitVector&) const;
    inline bool operator<(const BitVector&) const;

    // Return true if all bits are 0
    inline bool isEmpty() const;

    // Returns bit value at 'index'
    inline bool operator[](unsigned index) const;

    // Sets bit value at 'index' to 'bit'
    inline void setBit(unsigned index, bool bit=true);

    // Calculates bitwise AND operation and store result to this bitvector
    inline BitVector& operator&=(const BitVector&);

    // Calculates bitwise OR operation and store result to this bitvector
    inline BitVector& operator|=(const BitVector&);

    // Returns true if this is a subset of the given bitvector
    inline bool isSubsetOf(const BitVector&) const;


    // Deep copy (copies data bypassing copy-on-write mechanism). Usually
    // not needed and not recommended.
    //inline void deepCopy(const BitVector&);



//==========================================================================
private:
    static const unsigned UBITS = sizeof(unsigned)*CHAR_BIT;
    static const unsigned LOWMASK = sizeof(unsigned)*CHAR_BIT-1;

    // Size of the allocated table. Upper bit set if it's dynamically allocated
    unsigned size;
    // Data:
    unsigned* bits;

// Methods for the copy-on-write technique:
    inline unsigned getSize() const { return size&(~(1<<LOWMASK)); }
    inline unsigned isAllocated() const { return size&(1<<LOWMASK); }
    inline void setAllocatedBit() { size|=(1<<LOWMASK); }
    inline void clearAllocatedBit() { size=getSize(); }

    inline unsigned getRefCnt() { return *bits; }
    inline void setRefCnt(unsigned cnt) { (*bits) = cnt; }
    inline void incRefCnt() { ++(*bits); }
    inline void decRefCnt()
    { if((--(*bits))==0 && isAllocated()) { delete[] bits; /*std::cerr << "BV: Data deleted.\n";*/} }

    // Has to be called at the beginning of every non-const method:
    inline void copyOnWrite()
    {
        if((*bits) > 1) // If reference count is > 1 we have to copy
        {
            //std::cerr << "BV: Data copied. Allocated " << size*4 << " bytes.\n";
            --(*bits); // Decrement current reference count
            unsigned* old = bits;
            size = getSize(); // Unset the 'allocated' bit if it's set
            bits = new unsigned[size]; // create new buffer
            (*bits) = 1; // Set its reference count to 1
            for(unsigned i=1; i<size; i++) bits[i] = old[i]; // Copy
            setAllocatedBit();
        }
    }
};


// Amount of unsigneds needed to hold given amount of bits:
inline unsigned BitVector::bufferSize(unsigned bitsAmnt)
{
    return
        ((bitsAmnt%UBITS == 0 && bitsAmnt != 0) ?
         bitsAmnt/UBITS : bitsAmnt/UBITS+1) + 1; // +1 is for ref count
}

// Constructors and destructor
// ---------------------------
inline BitVector::BitVector(unsigned bitsAmnt):
    size(bufferSize(bitsAmnt)), bits(0)
{
    //std::cerr << "BV: Allocating " << size*4 << " bytes.\n";
    bits = new unsigned[size];
    for(unsigned i=1; i<size; i++) bits[i]=0;
    setAllocatedBit();
    setRefCnt(1);
}

inline BitVector::BitVector(unsigned bitsAmnt, unsigned* bitsBuf):
    size(bufferSize(bitsAmnt)), bits(bitsBuf)
{
    //std::cerr << "BV: Placement constructor.\n";
    incRefCnt();
}

inline BitVector::BitVector(const BitVector& copy):
    size(copy.size), bits(copy.bits)
{
    //static unsigned amnt=0;
    //if((++amnt)%1000 == 0) std::cerr << "BV: Copy constructor called " << amnt << " times.\n";
    incRefCnt();
}

inline BitVector::BitVector(const BitVector& copy, unsigned* bitsBuf):
    size(copy.size), bits(bitsBuf)
{
    //std::cerr << "BV: Placement copy constructor.\n";
    clearAllocatedBit();
    setRefCnt(1);
    for(unsigned i=1; i<getSize(); i++) bits[i] = copy.bits[i];
}

inline BitVector::~BitVector()
{
    //std::cerr << "BV: Destructor.\n";
    decRefCnt();
}

inline BitVector& BitVector::operator=(const BitVector& rhs)
{
    //std::cerr << "BV: Assignment operator.\n";
    // This is an important test if an instance is tried to be assigned to
    // itself:
    if(bits == rhs.bits) return *this;

    decRefCnt();
    size = rhs.size;
    bits = rhs.bits;
    incRefCnt();

    return *this;
}



// Operators
// ---------
inline bool BitVector::operator==(const BitVector& rhs) const
{
#ifdef DEBUG
    // Not necessarily an error (because it is checked in the comparison),
    // thus only a warning:
    warn_ifnot(getSize() == rhs.getSize(),
               "In BitVector::operator==(): "
               "Sizes do not match.");
#endif

    if(bits == rhs.bits) return true;

    if(getSize() != rhs.getSize()) return false;

    for(unsigned i=1; i<getSize(); i++)
        if(bits[i] != rhs.bits[i]) return false;
    return true;
}

inline bool BitVector::operator!=(const BitVector& rhs) const
{
    return !((*this)==rhs);
}

inline bool BitVector::operator<(const BitVector& rhs) const
{
#ifdef DEBUG
    warn_ifnot(getSize() == rhs.getSize(),
               "In BitVector::operator<(): "
               "Sizes do not match.");
#endif

    if(bits == rhs.bits) return false;

    if(getSize() != rhs.getSize()) return getSize() < rhs.getSize();

    for(unsigned i=1; i<getSize(); i++)
    {
        if(bits[i] < rhs.bits[i]) return true;
        if(rhs.bits[i] < bits[i]) return false;
    }
    return false;
}

inline bool BitVector::isEmpty() const
{
    for(unsigned i=1; i<getSize(); i++)
        if(bits[i] != 0) return false;
    return true;
}

inline bool BitVector::operator[](unsigned index) const
{
#ifdef DEBUG
    check_claim(1+index/UBITS < getSize(),
                "In BitVector::operator[]: "
                "Index value out of range.");
#endif

    return (bits[1+index/UBITS] >> (index&LOWMASK)) & 1;
}

inline void BitVector::setBit(unsigned index, bool bit)
{
#ifdef DEBUG
    check_claim(1+index/UBITS < getSize(),
                "In BitVector::setBit(): "
                "Index value out of range.");
#endif

    copyOnWrite();

    bit ?
        bits[1+index/UBITS] |= 1<<(index&LOWMASK) :
        bits[1+index/UBITS] &= ~(1<<(index&LOWMASK));
}

/*
inline void BitVector::deepCopy(const BitVector& rhs)
{
#ifdef DEBUG
    check_claim(getSize() == rhs.getSize(),
                "In BitVector::deepCopy(): "
                "Attempt to assign a bitvector of different size.");
#endif

    copyOnWrite();

    for(unsigned i=1; i<getSize(); i++)
        bits[i] = rhs.bits[i];
}
*/

inline BitVector& BitVector::operator&=(const BitVector& rhs)
{
#ifdef DEBUG
    check_claim(getSize() == rhs.getSize(),
                "In BitVector::operator&=(): "
                "Sizes do not match.");
#endif

    copyOnWrite();

    for(unsigned i=1; i<getSize(); i++)
        bits[i] &= rhs.bits[i];
    return *this;
}

inline BitVector& BitVector::operator|=(const BitVector& rhs)
{
#ifdef DEBUG
    check_claim(getSize() == rhs.getSize(),
                "In BitVector::operator|=(): "
                "Sizes do not match.");
#endif

    copyOnWrite();

    for(unsigned i=1; i<getSize(); i++)
        bits[i] |= rhs.bits[i];
    return *this;
}

inline bool BitVector::isSubsetOf(const BitVector& v) const
{
#ifdef DEBUG
    check_claim(getSize() == v.getSize(),
                "In BitVector::isSubsetOf(): "
                "Sizes do not match.");
#endif

    if(bits == v.bits) return true;

    for(unsigned i=1; i<getSize(); i++)
        if((bits[i] & v.bits[i]) != bits[i])
            return false;
    return true;
}



#undef CIRC_BITVECTOR_HH_
#endif

