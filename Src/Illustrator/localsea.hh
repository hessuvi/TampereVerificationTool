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



#ifndef ILLUS_LOCALSEA_HH
#define ILLUS_LOCALSEA_HH

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif



void
IlLocalSearchCoordinates( IlGrid          grid_IO,
                          IlLTS::StateIterator start_I,
                          IlLTS::StateIterator end_I,
                          double &costFuncVal_IO);




void
IlShrinkLayout(IlGrid &grid_IO,
               IlLTS::StateIterator start,
               IlLTS::StateIterator end,
               double &curObjFuncVal);



#endif
