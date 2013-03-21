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



#include "random.hh"
#include <stdlib.h>

double
Random::normal()
{
  Random &obj = instance();
  return( obj.value() / ( 1.0 + obj.maxValue() ) );
}

double
Random::generate(double maximum, double minimum)
{
  return( minimum + normal() * (maximum - minimum) );
}

int
Random::generate(int maximum, int minimum)
{
  int delta = maximum - minimum ;
  Random &obj = instance();

  int limit = (obj.maxValue() / delta ) * delta ;
  int v;
  for( v = obj.value() ; v > limit ; v = obj.value() )
    ;
  return( minimum + v % delta );
}

void
Random::reseed( unsigned int newSeed )
{
  instance();
  srand( newSeed );
}

Random::~Random()
{
}

Random::Random()
{
  srand( RANDOM_SEED );
}

Random &
Random::instance()
{
  static Random onlyObject;
  return( onlyObject );
}

int
Random::value()
{
  return( rand() );
}

int
Random::maxValue()
{
  return( RAND_MAX );
}
