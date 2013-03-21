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



#include "point.hh"

#ifndef MAKEDEPEND
#include <cmath>
using namespace std;
#endif



// Distance between two points measured in infinite norm
double
maxNorm(const Point &u, const Point &v)
{
  double dx=fabs(u.xCoord()-v.xCoord());
  double dy=fabs(u.yCoord()-v.yCoord());

  return( (dy > dx ) ? dy : dx );
}

// Distance between two points measured in Euclidean norm power 2
double
euclNormP2(const Point &u, const Point &v)
{
  double dx=u.xCoord()-v.xCoord();
  double dy=u.yCoord()-v.yCoord();

  return( dx*dx + dy*dy );
}

// Distance between two point measured in Euclidean norm
double
euclNorm(const Point &u, const Point &v)
{
  return( sqrt( euclNormP2(u,v) ) );
}

double
innerProduct(const Point &u, const Point &v)
{
  return( u.xCoord()*v.xCoord() + u.yCoord()*v.yCoord() );
}

Point
constProduct(double c, const Point &p)
{
  return( Point(c*p.xCoord(), c*p.yCoord()) );
}

Point
operator + (const Point &u, const Point &v)
{
  return( Point(u.xCoord()+v.xCoord(), u.yCoord()+v.yCoord()) );
}

Point
operator - (const Point &u, const Point &v)
{
  return( Point(u.xCoord()-v.xCoord(), u.yCoord()-v.yCoord()) );
}

// Distance between point and line measured in Euclidean norm
double
euclNorm(const Point &p,
         const Point &lineBeg,
         const Point &lineEnd)
{
  Point a = p-lineBeg;
  Point b = lineEnd-lineBeg;

  double coff = innerProduct(a,b) / innerProduct(b,b) ;

  return( euclNorm( a , constProduct(coff,b) ) );
}

double
angleOfVector(const Point &p)
{
  return( atan2(p.yCoord(),p.xCoord()) );
}

void Point::move(double distance, const Point &direction)
{
  Point vector = direction - *this;
  vector.scale(distance/euclNorm(direction,*this));
  move( vector );
}




