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



#ifndef TOOLS_POINT_HH
#define TOOLS_POINT_HH

#ifndef MAKEDEPEND
#include <utility>
using namespace std;
#endif



class Point : public pair<double,double>
{
  typedef pair<double,double> Ancestor;
public:
  Point():Ancestor() {};
  Point(const Ancestor &p):Ancestor(p) {};
  Point(double x, double y):Ancestor(x,y) {};

  double xCoord() const {return( first );};
  void   xCoord(double x) { first = x; };

  double yCoord() const {return( second );} ;
  void   yCoord(double y) {second = y ;} ;



public:
  void move(double distance, const Point &direction);
  void move(const Point &delta)
    {
      first += delta.first;
      second += delta.second;
    };
  void scale( double factor)
    {
      first *= factor;
      second *= factor;
    };
  void scale( double xFactor, double yFactor)
    {
      first *= xFactor;
      second *= yFactor;
    };


};





Point
operator + (const Point &u, const Point &v);

Point
operator - (const Point &u, const Point &v);

Point
constProduct(double c, const Point &p);

double
innerProduct(const Point &u, const Point &v);

// Distance between two points measured in infinite norm
double
maxNorm(const Point &u, const Point &v);

// Squared distance between two points measured in Euclidean norm
double
euclNormP2(const Point &u, const Point &v);

// Distance between two point measured in Euclidean norm
double
euclNorm(const Point &u, const Point &v);

// Distance between point and line measured in Euclidean norm
double
euclNorm(const Point &p,
         const Point &lineBeg,
         const Point &lineEnd);

double
angleOfVector(const Point &p);



#endif
