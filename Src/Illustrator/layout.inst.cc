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



#include "layout.hh"

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_INITIAL_HH
#include "initial.hh"
#endif

#ifndef ILLUS_LEVEL_HH
#include "level.hh"
#endif

#ifndef ILLUS_NONSYM_HH
#include "nonsym.hh"
#endif

#ifndef ILLUS_LOCALSEA_HH
#include "localsea.hh"
#endif

#ifndef ILLUS_SA_HH
#include "sa.hh"
#endif

#ifndef ILLUS_ARROWS_HH
#include "arrows.hh"
#endif

#ifndef ILLUS_OBJFUNC_HH
#include "objfunc.hh"
#endif

#ifndef ILLUS_GENETIC_HH
#include "genetic.hh"
#endif





static void
moveStatesToOrigin(IlLTS::StateIterator start_I,
                   IlLTS::StateIterator end_I)
{


  double minXCoord = (*start_I).coords().xCoord();
  double minYCoord = (*start_I).coords().yCoord();
  for( IlLTS::StateIterator ii = start_I;
       ii != end_I;
       ++ii )
    {
      if( (*ii).coords().xCoord() < minXCoord )
        {
          minXCoord = (*ii).coords().xCoord();
        }
      if( (*ii).coords().yCoord() < minYCoord )
        {
          minYCoord = (*ii).coords().yCoord();
        }
    }
  Point delta(1-minXCoord,1-minYCoord);
  for( IlLTS::StateIterator ii = start_I;
       ii != end_I;
       ++ii )
    {
      (*ii).coords().move(delta);
    }


}



bool
IlLayoutAlgorithm(IlLTS &lts)
{
  
  
    IlGrid stateGrid(lts.nofStates());
    double costFuncVal;
  


  
  
  const int VERY_SMALL_LTS = 3;
  const int MEDIUM_SIZE_LTS = 300;
  if( lts.nofStates() <= VERY_SMALL_LTS )
    {
      IlInitialSolution(lts.nofStates(),
                        stateGrid,
                        lts.beginStates(),
                        lts.endStates());
    }
  else if ( lts.nofStates() <= MEDIUM_SIZE_LTS )
    {
      IlNonSymmetricInitialSolution(lts.nofStates(),
                                    stateGrid,
                                    lts.beginStates(),
                                    lts.endStates());
    }
  else
    {
      IlBigInitialSolution(lts.nofStates(),
                           stateGrid,
                           lts.beginStates(),
                           lts.endStates());
    }
  costFuncVal = IlObjectiveFunction(lts.beginTransitions(),
                                    lts.endTransitions());
  

  
  
  #if 0
  IlLocalSearchCoordinates( stateGrid,
                            lts.beginStates(),
                            lts.endStates(),
                            costFuncVal);
  #endif
  double lowerBoundForCost = lts.nofTransitions();
  
  
  for( IlLTS::TransitionIterator ii = lts.beginTransitions() ;
       ii != lts.endTransitions() ;
       ++ ii )
    {
      if( ii->head() == ii->tail() )
        {
          lowerBoundForCost -= 1.0;
        }
    }
  
  
  if( lts.nofStates() >= 3 ) {
      IlSimulatedAnnealing(stateGrid,
                           lts.beginStates(),
                           lts.endStates(),
                           costFuncVal,
                           lowerBoundForCost);
      IlLocalSearchCoordinates( stateGrid,
                                lts.beginStates(),
                                lts.endStates(),
                                costFuncVal);
  }
  #if 0
    IlGeneticAlgorithForCoordinates(lts.beginStates(),
                                    lts.endStates(),
                                    lts.beginTransitions(),
                                    lts.endTransitions());
  #endif
  

  
  
    moveStatesToOrigin(lts.beginStates(),
                       lts.endStates());
   //!!!!!!!!!!!!!! update grid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  

  
  
  IlTransitionArrows(lts.beginStates(),
                     lts.endStates(),
                     lts.beginTransitions(),
                     lts.endTransitions());
  

  // 
  return true;
}


