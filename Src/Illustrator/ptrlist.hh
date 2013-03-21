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

Contributor(s): Heikki Virtanen.
*/


#ifndef TOOLS_PTRLIST_HH
#define TOOLS_PTRLIST_HH

#ifndef TOOLS_ITERADAP_HH
#include "iteradap.hh"
#endif

template <class OBJ>
class PointerList: public list<OBJ *>
{
  typedef list<OBJ *> Ancestor;

public:
  typedef IteratorAdaptor<OBJ, Ancestor::iterator> iterator;

  iterator begin() {return Ancestor::begin(); };
  iterator end()   {return Ancestor::end(); };}

  iterator insert(iterator pos, OBJ &obj)
    { return Ancestor::insert(pos, &obj) ; };
};

#endif
