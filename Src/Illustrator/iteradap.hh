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


#ifndef TOOLS_ITERADAP_HH
#define TOOLS_ITERADAP_HH

//Usage example:
// typedef IteratorAdaptor<UserObj, list<UserObj *>::iterator > iterator;

template <class OBJ, class ITER>
class IteratorAdaptor: public ITER
{
  typedef ITER Ancestor;
public:
  IteratorAdaptor(const Ancestor &t ): Ancestor(t) {};
  OBJ &operator * () const { return( *( Ancestor::operator * () ) ); };
  OBJ *operator -> () const { return( Ancestor::operator * () ); };
};
#endif
