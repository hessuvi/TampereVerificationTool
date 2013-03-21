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

// FILE_DES: outputlsts.cc: Parallel
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: outputlsts.cc 1.15 Mon, 07 Jun 2004 13:55:09 +0300 warp $";
// 
// Rinnankytketyn LSTS-luokan toteutus
//

// $Log:$

#include "outputlsts.hh"

#include <climits>
#include <sstream>
using namespace std;


//===========================================================================
// RadixTreeMemory
//===========================================================================
#ifdef USE_RADIX_TREE

namespace
{
#define INDBITS (sizeof(lsts_index_t)*CHAR_BIT)

    inline unsigned requiredBits(lsts_index_t maxValue)
    {
        if(maxValue==0) return 1;
        unsigned bit = INDBITS;
        while(!(maxValue&(1<<(bit-1)))) bit--;
        return bit;
    }

    inline void requiredBits(const std::vector<lsts_index_t>& maxValues,
                             std::vector<unsigned>& bits,
                             unsigned valOffset)
    {
        bits.resize(maxValues.size());
        for(unsigned i=0; i<maxValues.size(); i++)
            bits[i] = requiredBits(maxValues[i]-valOffset);
    }

    inline unsigned getDataSizeInBytes
    (const std::vector<lsts_index_t>& maxValues, unsigned valOffset)
    {
        unsigned result = 0;
        for(unsigned i=0; i<maxValues.size(); ++i)
            result += requiredBits(maxValues[i]-valOffset);
        return (result+7)/8;
    }
}

OutputLSTS::RadixTreeMemory::RadixTreeMemory
(const std::vector<lsts_index_t>& ilstsStateAmnts, unsigned valueOffset):
    keySize(getDataSizeInBytes(ilstsStateAmnts, valueOffset)),
    radixTree(keySize),
    valOffset(valueOffset)
{
    requiredBits(ilstsStateAmnts, valuesBits, valueOffset);

    //cerr << "*** keySize: " << keySize << "\n";
}

OutputLSTS::RadixTreeMemory::~RadixTreeMemory()
{}


void
OutputLSTS::RadixTreeMemory::putItems(dataContainer_t& dest,
                                      const std::vector<lsts_index_t>& data)
    const
{
    dest.clear();
    dest.resize(keySize, 0);

    unsigned destIndex = 0, destBitInByteIndex = 0;
    for(unsigned i = 0; i < data.size(); ++i)
    {
        unsigned value = data[i]-valOffset;
        unsigned bitIndexInValue = 0, valueSizeInBits = valuesBits[i];

        while(true)
        {
            if(destBitInByteIndex == 0)
                dest[destIndex] = value >> bitIndexInValue;
            else
                dest[destIndex] |=
                    (value >> bitIndexInValue) << destBitInByteIndex;

            if(bitIndexInValue+(8-destBitInByteIndex) >= valueSizeInBits)
            {
                destBitInByteIndex += valueSizeInBits-bitIndexInValue;
                if(destBitInByteIndex > 7)
                {
                    destBitInByteIndex -= 8;
                    ++destIndex;
                }
                break;
            }

            bitIndexInValue += 8-destBitInByteIndex;
            ++destIndex;
            destBitInByteIndex = 0;
        }
    }
}

bool OutputLSTS::RadixTreeMemory::addPackedData(const dataContainer_t& key,
                                                unsigned data,
                                                unsigned& loc_i)
{
    //return radixTree.add(&key[0], data, loc_i);

    if(radixTree.add(&key[0], data, loc_i))
    {
        //cerr<<" (Added: {";
        //for(unsigned i=0;i<key.size();++i)cerr<<(i==0?"":",")<<unsigned(key[i]);
        //cerr << "},)";
        return true;
    }
    return false;
}


void
OutputLSTS::RadixTreeMemory::getItems(unsigned loc_i,
                                      std::vector<lsts_index_t>& dest) const
{
    const unsigned char* packedData = radixTree.readKey(loc_i);
    //cerr<<"Reading:";
    //for(unsigned i=0;i<keySize;++i)cerr<<" "<<unsigned(packedData[i]);
    //cerr<<"\n";

    unsigned srcIndex = 0, srcBitInByteIndex = 0;
    for(unsigned i = 0; i < dest.size(); ++i)
    {
        unsigned value = 0;
        unsigned bitIndexInValue = 0, valueSizeInBits = valuesBits[i];

        while(true)
        {
            unsigned byte = packedData[srcIndex] >> srcBitInByteIndex;

            if(bitIndexInValue+(8-srcBitInByteIndex) >= valueSizeInBits)
            {
                byte &= (1 << (valueSizeInBits-bitIndexInValue)) - 1;
                value |= (byte << bitIndexInValue);

                srcBitInByteIndex += valueSizeInBits-bitIndexInValue;
                if(srcBitInByteIndex > 7)
                {
                    srcBitInByteIndex -= 8;
                    ++srcIndex;
                }
                break;
            }

            value |= (byte << bitIndexInValue);
            bitIndexInValue += 8-srcBitInByteIndex;
            ++srcIndex;
            srcBitInByteIndex = 0;
        }

        dest[i] = value+valOffset;
    }
}


unsigned OutputLSTS::RadixTreeMemory::getAllocatedMemoryAmount() const
{
    return
        radixTree.getAllocatedMemoryAmount()+
        valuesBits.size()*sizeof(unsigned);
}


#endif


//===========================================================================
// Rakentaja ja purkaja
//===========================================================================
OutputLSTS::OutputLSTS(lsts_index_t ilstsAmnt,
                       const vector<lsts_index_t>& ilstsStateAmnts,
                       lsts_index_t maxOutputTransitionNumber
#ifdef USE_ALTERNATIVE_STATESET
                       , int bii, int bik, int kib, int bis
                       ):
    memory(ilstsStateAmnts, 1, bii, bik, kib, bis),
#else
                       ):
    memory(ilstsStateAmnts, 1),
#endif
    stateCnt(0),
    ParallelLSTSAmnt(ilstsAmnt),
    actionCnt(maxOutputTransitionNumber),
    transitionCount(0),
    trCont(maxOutputTransitionNumber, 15, true),
    isInterrupted(false),
    extraData(~0U)
{
/*
unsigned maxVals[] = { 4, 32, 1024, 8, 67108864, 8, 4, 1, 16, 256, 16, 16, 32, 4000000000};
vector<unsigned> maxV(maxVals, maxVals+14);
RadixTreeMemory mem(maxV, 1);
RadixTreeMemory::dataContainer_t buffer;
vector<unsigned> check(14);
unsigned ptr1, ptr2, ptr3, ptr4;

unsigned vals1[] = { 1, 2, 3, 4, 50000000, 6, 3, 1, 9, 10, 11, 12, 13, 1 };
unsigned vals2[] = { 2, 26, 25, 4, 23, 7, 1, 1, 13, 18, 12, 11, 15, 100000 };
unsigned vals3[] = { 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 10000000 };
unsigned vals4[] = { 1, 5, 896, 2, 2251, 1, 2, 1, 6, 247, 10, 11, 15, 4000000000 };

vector<unsigned> v1(vals1, vals1+14);
mem.putItems(buffer, v1);
if(!mem.addPackedData(buffer, 1, ptr1)) {std::cerr<<"add1 failed\n";exit(1);}
mem.getItems(ptr1, check);
cerr <<  "Added:"; for(unsigned i=0;i<14;++i) cerr << " " << v1[i];
cerr <<"\nGot  :"; for(unsigned i=0;i<14;++i) cerr << " " << check[i];
cerr << "\n";
if(check != v1) {std::cerr<<"check1 failed\n";exit(1);}

vector<unsigned> v2(vals2, vals2+14);
mem.putItems(buffer, v2);
if(!mem.addPackedData(buffer, 2, ptr2)) {std::cerr<<"add2 failed\n";exit(1);}
mem.getItems(ptr2, check);
if(check != v2) {std::cerr<<"check2 failed\n";exit(1);}

vector<unsigned> v3(vals3, vals3+14);
mem.putItems(buffer, v3);
if(!mem.addPackedData(buffer, 3, ptr3)) {std::cerr<<"add3 failed\n";exit(1);}
mem.getItems(ptr3, check);
if(check != v3) {std::cerr<<"check3 failed\n";exit(1);}

vector<unsigned> v4(vals4, vals4+14);
mem.putItems(buffer, v4);
if(!mem.addPackedData(buffer, 4, ptr4)) {std::cerr<<"add4 failed\n";exit(1);}
mem.getItems(ptr4, check);
if(check != v4) {std::cerr<<"check4 failed\n";exit(1);}

std::cerr<<"Checks ok\n";
exit(0);
*/
}

OutputLSTS::~OutputLSTS()
{
}


// Uuden tilan luonti
//===========================================================================
OutputLSTS::StatePointer OutputLSTS::CreateNewState()
{
    stateColors.resize(stateColors.size()+2);
    dlrejBits.push_back(false);
    llrejBits.push_back(false);

#ifndef USE_RADIX_TREE
    unsigned tmp_stateNumbers = memory.allocateVarItemsGroup();
    memory.putPackedData(tmp_stateNumbers, packedDataBuffer);
#endif

    return stateCnt++;
}

// Alkutilan luonti
//===========================================================================
OutputLSTS::StatePointer
OutputLSTS::CreateInitialState(const vector<lsts_index_t>& stateNumbers)
{
    memory.putItems(packedDataBuffer, stateNumbers);
#ifdef USE_ALTERNATIVE_STATESET
    memory.findPackedData(packedDataBuffer);
#endif
#ifdef USE_RADIX_TREE
    CreateNewState();
    memory.addPackedData(packedDataBuffer, stateCnt, currentState);
#else
    currentState = CreateNewState();
#endif
    return currentState;
}

// Palauttaa oletustilan numerot
//===========================================================================
void
OutputLSTS::GetCurrentStateNumbers(vector<lsts_index_t>& stateNumbers) const
{
    memory.getItems(memory.getPointer(currentState), stateNumbers);
    //cerr<<"GetCurrentStateNumbers ("<<getStateNumber(currentState)<<"):";
    //for(unsigned i=0; i<stateNumbers.size(); ++i)
    //    cerr << " " << stateNumbers[i];
    //cerr << "\n";
}

inline bool
OutputLSTS::StateAlreadyExists(const MemoryVar::dataContainer_t& pdata,
                               StatePointer& newState)
{
#ifndef USE_RADIX_TREE
    unsigned res = memory.findPackedData(pdata);
    if(res != MemoryVar::NULLPTR)
    {
        newState = res; return true;
    }
#endif
    return false;
}

// Lisää uuden transition oletustilaan ja mahdollisesti uuden tilan
//===========================================================================
unsigned
OutputLSTS::AddTransition(lsts_index_t transitionNumber,
                          const vector<lsts_index_t>& destStateNumbers,
                          StatePointer& newState)
{
    unsigned returnValue=0;

    ++transitionCount;

    //cerr<<"AddTransition ("<<transitionNumber<<"):";
    //for(unsigned i=0; i<destStateNumbers.size(); ++i)
    //    cerr << " " << destStateNumbers[i];

    memory.putItems(packedDataBuffer, destStateNumbers);

    // Katsotaan, onko tila jo olemassa
#ifdef USE_RADIX_TREE
    if(!memory.addPackedData(packedDataBuffer, stateCnt+1, newState))
        returnValue = IsWhite(newState) ? 1 : (IsGray(newState)?2 : 3);
    else
        CreateNewState();

    //if(returnValue)
    //    cerr << ": State exists (" << getStateNumber(newState) << ")\n";
    //else
    //    cerr << ": New state (" << stateCnt << ")\n";
#else
    if(StateAlreadyExists(packedDataBuffer, newState))
        returnValue = IsWhite(newState) ? 1 : (IsGray(newState)?2 : 3);
    else
        newState = CreateNewState();
#endif

    if(extraData == ~0U)
        trCont.addTransitionToState(transitionNumber,
                                    getStateNumber(newState));
    else
        trCont.addTransitionToState(transitionNumber,
                                    getStateNumber(newState), extraData);

    return returnValue;
}

unsigned OutputLSTS::DoneAddTransition()
{
    trCont.doneAddingTransitionsToState();
    return trCont.numberOfTransitions(getStateNumber(currentState));
}


bool OutputLSTS::lsts_doWeWriteStateNames() { return true; }

void OutputLSTS::lsts_WriteStateNames(iStateNamesAP& pipe)
{
#ifdef USE_RADIX_TREE
#else
    vector<lsts_index_t> stateNumbers(ParallelLSTSAmnt);
    ostringstream stateName;

    for(unsigned state = 0; state < stateCnt; ++state)
    {
        SetCurrentState(state);
        GetCurrentStateNumbers(stateNumbers);
        stateName.str("");
        for(unsigned i = 0; i < stateNumbers.size(); ++i)
        {
            stateName << stateNumbers[i];
            if(i < stateNumbers.size()-1) stateName << " ";
        }
        pipe.lsts_StateName(state+1, stateName.str());
    }
#endif
}



#ifdef USE_ALTERNATIVE_STATESET
const state_set::byte state_set::linear_overflow = 0xF;
const state_set::byte state_set::link_overflow = 0xE;
const state_set::elem_nr_type state_set::not_there = -1;
#endif
