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



#include "linealg.hh"

#ifndef MAKEDEPEND
#include <cmath>
#include <algorithm>
#include <cassert>
using namespace std;
#endif

Point
LineAlgorithm::operator * () const
{
  return( Point(cur_x,cur_y));
}

LineAlgorithm &
LineAlgorithm::operator ++ ()
{
  step();
  return( *this );
}

LineAlgorithm
LineAlgorithm::operator ++ (int)
{
  LineAlgorithm cur = *this;
  step();
  return( cur );
}

bool
LineAlgorithm::operator == (const LineAlgorithm &o)
{
  return( cur_x == o.cur_x && cur_y == o.cur_y );
}
bool
LineAlgorithm::operator == (const Point &p)
{
  return( p == Point(cur_x,cur_y) );
}


LineAlgorithm::LineAlgorithm(int x0, int y0, int x1, int y1)
{
  init(x0,y0,x1,y1);
}

static inline int oma_rint(double x)
{ return x<0 ? -(int((-x)+.5)) : int(x+.5); }

LineAlgorithm::LineAlgorithm(Point p1, Point p2)
{
  init(static_cast<int>(oma_rint(p1.xCoord())),
       static_cast<int>(oma_rint(p1.yCoord())),
       static_cast<int>(oma_rint(p2.xCoord())),
       static_cast<int>(oma_rint(p2.yCoord())));
  assert( euclNormP2(p1,*(*this)) < 0.01 
          && "LineAlgorithm: Given starting point is not close to integer" );
}

void
LineAlgorithm::init(int x0, int y0, int x1, int y1)
{
  cur_x = x0;
  cur_y = y0;
  rem = 0;
  delta_max = abs(x1 - x0);
  delta_min = abs(y1 - y0);
  step_max = (x1 < x0) ? -1 : 1;
  step_min = (y1 < y0) ? -1 : 1;
  cp_max = &cur_x;
  cp_min = &cur_y;
  if( delta_max < delta_min )
    {
      swap( delta_max, delta_min );
      swap( step_max, step_min );
      swap( cp_max, cp_min );
    }
}

void
LineAlgorithm::step()
{
  *cp_max += step_max;
  rem += delta_min * 2;
  if( rem > delta_max )
    {
      rem -= delta_max *2 ;
      *cp_min += step_min;
    }
}
