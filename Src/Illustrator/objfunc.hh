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



#ifndef ILLUS_OBJFUNC_HH
#define ILLUS_OBJFUNC_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif




inline
double
IlSingleTransitionCost(const IlLTS::Transition &trans)
{
  return( maxNorm(trans.head().coords(),trans.tail().coords())
          + 0.00001*euclNormP2(trans.head().coords(),trans.tail().coords())
          );
}

template<class ITER>
inline double
IlObjectiveFunction(ITER start,
                    ITER end)
{
  double cost = 0.0;
  for( ; start != end ; ++start )
    {
      cost += IlSingleTransitionCost(*start);
    }
  return( cost );
}



#endif // ILLUS_OBJFUNC_HH
