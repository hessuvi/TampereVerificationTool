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

Contributor(s): Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: $Source: Src/lsts2ag/dfsiter.hh $
// FILE_DES: $Id: dfsiter.hh 1.3 Fri, 27 Feb 2004 16:36:58 +0200 warp $

// 
// 
// 

#ifdef CIRC_DFSITER_HH
#error Recursive include
#endif
#ifndef ONCE_DFSITER_HH
#define ONCE_DFSITER_HH
#define CIRC_DFSITER_HH

#ifndef MAKEDEPEND
//#include <>
#endif
#include "guard.hh"
#include "automaton.hh"

class DFS_Iterator
{
    REFERENCE_CLASS( DFS_Iterator )
 public:
    DFS_Iterator( Automaton&, lsts_index_t startState );
    ~DFS_Iterator();

    lsts_index_t operator* ();
    DFS_Iterator& operator ++();

    bool isValid() const;
    bool isLoop() const;
    bool isVisited() const;

 private:
    lsts_index_t          stackTop() const;
    bool                  isVisited( lsts_index_t state ) const;
    void                  recursiveCall(lsts_index_t state);
    void                  returnFromRecursion();
    void                  step();
    void                  pop_stack();

    Automaton&            a_lsts;
    BitVector             isInStack;
    BitVector             isOld;
    vector<lsts_index_t>  travelStack;
    vector<InvisIterator> currentTransition;
};

#undef CIRC_DFSITER_HH
#endif  // ONCE_DFSITER_HH
// Local variables:
// mode: c++
// mode: font-lock
// compile-command: "gmake"
// End:
