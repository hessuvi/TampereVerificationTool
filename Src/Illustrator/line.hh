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



#ifndef TOOLS_LINE_HH
#define TOOLS_LINE_HH

#ifndef TOOLS_POINT_HH
#include "point.hh"
#endif

#ifndef MAKEDEPEND
#include <list>
using namespace std;
#endif

class Line: public list<Point>
{


public:
void trimBegin(double dist)
  {
    iterator two = begin();
    iterator one = two++;
    (*one).move(dist,*two);
  };
void trimEnd(double dist)
  {
    iterator prev = end();
    prev --;
    iterator last = prev--;
    (*last).move(dist,*prev);
  };
void move(const Point &delta);
void scale(double factor)
  {
    scale(factor,factor);
  };
void scale(double xFactor, double yFactor);


};



double
maxNorm(const Line &line);

double
euclNorm(const Line &line);

double
euclNormP2(const Line &line);

double
euclNorm(const Point &p, const Line &line);

bool
linesCross(const Line &a, const Line &b);

bool
linesCross(const Point &a1, const Point &a2,
           const Point &b1, const Point &b2);

Point
crossingPoint
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  );

double
crossingFractionInP
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  );

bool
operator < (const Line &a, const Line &b);



#endif
