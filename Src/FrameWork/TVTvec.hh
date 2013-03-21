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

#ifdef CIRC_TVTVEC_HH_
#error "Include recursion"
#endif

#ifndef ONCE_TVTVEC_HH_
#define ONCE_TVTVEC_HH_
#define CIRC_TVTVEC_HH_

#include <vector>

#include <cstdlib>
#include <iostream>

template<typename Type, int BLOCKSIZE = 8192>
class TVTvec
{
    std::vector<Type*> ptrs_;
    unsigned size_;

    inline void reset(Type* ptr)
    {
        for(unsigned i=0; i<BLOCKSIZE; ++i) ptr[i] = Type();
    }

 public:
    inline Type& operator[](unsigned index)
    {
        return ptrs_[index/BLOCKSIZE][index%BLOCKSIZE];
    }

    inline const Type& operator[](unsigned index) const
    {
        return ptrs_[index/BLOCKSIZE][index%BLOCKSIZE];
    }

    inline void push_back(const Type& value)
    {
        if(size_%BLOCKSIZE == 0)
        {
            ptrs_.push_back(new Type[BLOCKSIZE]);
            reset(ptrs_.back());
        }
        operator[](size_++) = value;
    }

    inline void resize(unsigned newSize)
    {
        if(newSize > size_)
        {
            unsigned newBlockFirstIndex = newSize - newSize%BLOCKSIZE;
            unsigned counter = size_;
            while(counter <= newBlockFirstIndex)
            {
                ptrs_.push_back(new Type[BLOCKSIZE]);
                reset(ptrs_.back());
                counter += BLOCKSIZE;
            }
        }
        else
        {
            std::cerr<<"Oops! Implement TVTvec::resize() smaller\n";
            std::abort();
        }

        size_ = newSize;
    }

    inline unsigned size() const { return size_; }

    inline void swap(TVTvec& rhs)
    {
        ptrs_.swap(rhs.ptrs_);
        unsigned tmp = size_; size_ = rhs.size_; rhs.size_ = tmp;
    }

    inline void clear()
    {
        for(unsigned i=0; i<ptrs_.size(); ++i)
            delete[] ptrs_[i];
        ptrs_.clear();
        size_ = 0;
    }


    inline TVTvec(): size_(0)
    {
        ptrs_.reserve(512);
    }

    inline ~TVTvec()
    {
        clear();
    }

    unsigned allocatedMem() const
    {
        return ptrs_.size()*sizeof(Type*) +
            ptrs_.size()*sizeof(Type)*BLOCKSIZE;
    }

 private:
    TVTvec(const TVTvec&);
    TVTvec& operator=(const TVTvec&);
};

#undef CIRC_TVTVEC_HH_
#endif
