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



#ifndef ILLUS_SURROUND_HH
#define ILLUS_SURROUND_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif





class IlTransitionRingItem
{
public:
  IlTransitionRingItem(double angle,
                       IlLTS::Transition &trans,
                       IlLTS::State      &other)
    :d_transition(trans),
     d_otherEnd(other),
     d_naturalAngle(angle),
     d_realAngle(angle)
  {};

    // T‰m‰ on `feikki'-sijoitusoperaattori k‰‰nt‰j‰‰ varten!
    IlTransitionRingItem& operator=( const IlTransitionRingItem& rhs )
    {
        if ( &rhs != this )
        {
            d_naturalAngle = rhs.d_naturalAngle;
            d_realAngle = rhs.d_realAngle;
        }

        return *this;
    }

  bool operator < (const IlTransitionRingItem &o)
    { return( d_naturalAngle < o.d_naturalAngle ); };
  bool operator > (const IlTransitionRingItem &o)
    { return( d_naturalAngle > o.d_naturalAngle ); };
  
  double getAngle() const       { return( d_realAngle ); };
  double naturalAngle() const   { return( d_naturalAngle ); };
  void   setAngle(double angle) { d_realAngle = angle; };

  IlLTS::Transition &transition() {return d_transition; };
  IlLTS::State      &otherEnd()   {return d_otherEnd;   };
private:
  IlLTS::Transition &d_transition;
  IlLTS::State      &d_otherEnd;
  double             d_naturalAngle;
  double             d_realAngle;
};

class IlTransitionRing: public list<IlTransitionRingItem>
{
  typedef list<IlTransitionRingItem> List;
public:
  typedef list<IlTransitionRingItem>::iterator Iterator;
  typedef IlTransitionRingItem Item;
  typedef Item *ItemPtr;

  void insertAfter( double angle,
                    IlLTS::Transition &trans,
                    IlLTS::State &other)
  {
    Item item(angle,trans,other);
    Iterator ii;
    for( ii = begin();   (ii != end()) && !( *ii > item) ;  ++ii)
      {;}
    insert( ii , item );
  };
  void insertBefore( double angle,
                     IlLTS::Transition &trans,
                     IlLTS::State &other )
  {
    Item item(angle,trans,other);
    Iterator ii;
    for( ii = begin();   (ii != end()) && ( *ii < item ) ;  ++ii)
      {;}
    insert( ii , item );
  };

  bool addLoop(IlLTS::Transition &trans, double minAngle);

  bool optimize(double minAngle);
};





#endif // ILLUS_SURROUND_HH
