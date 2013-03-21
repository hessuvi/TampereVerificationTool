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



#include "line.hh"

void
Line::move(const Point &delta)
{
  for( iterator ii=begin(); ii != end() ; ++ii)
    {
      (*ii).move(delta);
    }
}

void
Line::scale(double xFactor, double yFactor)
{
  for( iterator ii=begin(); ii != end() ; ++ii)
    {
      (*ii).scale(xFactor, yFactor);
    }
}

double
maxNorm(const Line &line)
{
  return( maxNorm(line.front(),line.back()));
}

double
euclNorm(const Line &line)
{
  return( euclNorm(line.front(),line.back()));
}

double
euclNormP2(const Line &line)
{
  return( euclNormP2(line.front(),line.back()));
}

double
euclNorm(const Point &p, const Line &line)
{
  return( euclNorm(p, line.front(),line.back()));
}

static double
detTwo(const Point &p1, const Point &p2)
{
  return( p1.xCoord()*p2.yCoord() - p1.yCoord()*p2.xCoord());
}

static double
detThree(const Point &p1, const Point &p2, const Point &p3)
{
  return( detTwo(p2,p3) - detTwo(p1,p3) + detTwo(p1,p2));
}


bool
linesCross(const Point &a1, const Point &a2,
           const Point &b1, const Point &b2)
{
  // Close hits to end points are not counted
  double A1 = detThree(a1, a2, b1);
  double A2 = detThree(a1, a2, b2);
  if( (A1<0 && A2 > 0) || (A1 > 0 && A2 < 0 ))
    {
      double B1 = detThree(b1, b2, a1 );
      double B2 = detThree(b1, b2, a2 );
      return(  (B1<0 && B2 > 0) || (B1 > 0 && B2 < 0 ) );
    }
  else
    {
      return( false );
    }
}

bool
linesCross(const Line &a, const Line &b)
{
  // Close hits to end points are not counted
  double A1 = detThree(a.front(), a.back(), b.front());
  double A2 = detThree(a.front(), a.back(), b.back());
  if( (A1<0 && A2 > 0) || (A1 > 0 && A2 < 0 ))
    {
      double B1 = detThree(b.front(), b.back(), a.front() );
      double B2 = detThree(b.front(), b.back(), a.back() );
      return(  (B1<0 && B2 > 0) || (B1 > 0 && B2 < 0 ) );
    }
  else
    {
      return( false );
    }
}


Point
crossingPoint
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  )
{
  Point uv = q0 - p0 ;
  Point deltaP = p1 - p0;
  Point deltaQ = q1 - q0;

  double a = deltaP.xCoord();
  double c = deltaP.yCoord();

  double b = deltaQ.xCoord();
  double d = deltaQ.yCoord();

  double u = uv.xCoord();
  double v = uv.yCoord();

  double determinant = a*d - b*c;

  double lambda = (d*u - b*v) / determinant ;

  deltaP.scale(lambda);
  return (p0 + deltaP) ;
}


double
crossingFractionInP
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  )
{
  Point uv = q0 - p0 ;
  Point deltaP = p1 - p0;
  Point deltaQ = q1 - q0;

  double a = deltaP.xCoord();
  double c = deltaP.yCoord();

  double b = deltaQ.xCoord();
  double d = deltaQ.yCoord();

  double u = uv.xCoord();
  double v = uv.yCoord();

  double determinant = a*d - b*c;

  return (d*u - b*v) / determinant ;
}


static const Point &
min(const Point &a, const Point &b)
{
  if( a < b )
    {
      return a;
    }
  else
    {
      return b;
    }
}

bool
operator < (const Line &a, const Line &b)
{
  Point minA = min(a.front(),a.back());
  Point minB = min(b.front(),b.back());
  return (minA < minB);
}
