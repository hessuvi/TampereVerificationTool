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


#ifndef TOOLS_RANDOM_HH
#define TOOLS_RANDOM_HH

class Random
{
  static const unsigned int RANDOM_SEED = 0xab53ae75;
public:
  static double normal();
  static double generate(double maximum, double minimum=0.0);
  static int    generate(int    maximum, int    minimum=0);
  static void   reseed( unsigned int newSeed = RANDOM_SEED );
  ~Random();
private:
  Random();
  int  value();
  int  maxValue();
  static Random &instance();
};
#endif
