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



#ifndef TOOLS_LINEALG_HH
#define TOOLS_LINEALG_HH

#ifndef TOOLS_POINT_HH
#include "point.hh"
#endif

class LineAlgorithm
{
public:
  
  
                 LineAlgorithm(int x0, int y0, int x1, int y1);
                 LineAlgorithm(Point p1, Point p2);
  Point          operator * () const;
  LineAlgorithm &operator ++ ();
  LineAlgorithm  operator ++ (int);
  bool           operator == (const LineAlgorithm &o);
  bool           operator != (const LineAlgorithm &o)
                   {
                     return( !(*this == o) );
                   }
  bool           operator == (const Point &p);
  bool           exact() const { return( rem == 0 ); };
  bool           horizontalOrVertical() const { return( delta_min == 0 ); };
  

private:
  
  
  int cur_x, cur_y;
  int rem;
  int delta_max, delta_min;
  int step_max, step_min;
  int *cp_max, *cp_min;
  
  void         init(int x0, int y0, int x1, int y1);
  void         step();
};
#endif
