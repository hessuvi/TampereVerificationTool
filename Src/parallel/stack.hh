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

// FILE_DES: stack.hh: Parallel
// Juha Nieminen

// $Id: stack.hh 1.5 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// Rinnankytkijän käyttämä suht geneerinen pinoluokka.
//

// $Log:$

#ifdef CIRC_STACK_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STACK_HH_
#define ONCE_STACK_HH_
#define CIRC_STACK_HH_

#ifndef MAKEDEPEND
#include <vector>
#endif

// Luokan esittely:

template <typename T>
class Stack
{
public:
    inline void Push(T item)
    {
        stack.push_back(item);
        stackPtr++;
    }

    inline bool Pop()//T& item)
    {
        if(stackPtr==0) return false;
        stackPtr--;
        //item = stack[stackPtr];
        stack.pop_back();
        return true;
    }
    inline bool Peek(T& item)
    {
        if(stackPtr==0) return false;
        item = stack[stackPtr-1];
        return true;
    }

    inline unsigned size() const { return stack.size(); }

    inline Stack():stackPtr(0) { stack.reserve(1024); }

private:
    std::vector<T> stack;
    unsigned stackPtr;
};


#undef CIRC_STACK_HH_
#endif
