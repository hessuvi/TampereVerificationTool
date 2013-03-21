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



#include "sa.hh"

#ifndef ILLUS_OBJFUNC_HH
#include "objfunc.hh"
#endif

#ifndef TOOLS_RANDOM_HH
#include "random.hh"
#endif

#ifndef MAKEDEPEND
#include <cmath>
#include <iostream>
#endif





#define DO_NOT_INCLUDE 1


static double
IlStatePairCost(IlLTS::State &u, IlLTS::State &v)
{
  double cost = 0.0;
  cost += IlObjectiveFunction(u.beginLeaving(),u.endLeaving());
  cost += IlObjectiveFunction(u.beginEntering(),u.endEntering());
  cost += IlObjectiveFunction(v.beginLeaving(),v.endLeaving());
  cost += IlObjectiveFunction(v.beginEntering(),v.endEntering());
  // Transitions, which connect states u and v are counted twise.
  // This do not matter, iff costfuntion for single transition is
  // symmetric and swap operator do not change cost of those transitions.
  return cost;
}


static double
IlSingleStateCost(IlLTS::State &s)
{
  double cost = 0.0;
  cost += IlObjectiveFunction(s.beginLeaving(),s.endLeaving());
  cost += IlObjectiveFunction(s.beginEntering(),s.endEntering());
  return cost;
}

#ifndef DO_NOT_INCLUDE
class IlGreaterSingleStateCost
{
public:
  bool operator () (IlLTS::StateIterator &a, IlLTS::StateIterator &b);
};

bool
IlGreaterSingleStateCost::operator () (IlLTS::StateIterator &a,
                                       IlLTS::StateIterator &b)
{
  Point pA(0.0,0.0);
  Point pB(0.0,0.0);
  for(IlLTS::State::AdjacencyIterator jj = (*a).beginEntering() ;
      jj != (*a).endEntering() ;
      ++jj )
    {
      pA.move( (*jj).tail().coords() - (*a).coords() );
    }

  for(IlLTS::State::AdjacencyIterator jj = (*a).beginLeaving() ;
      jj != (*a).endLeaving() ;
      ++jj )
    {
      pA.move( (*jj).head().coords() - (*a).coords() );
    }

  for(IlLTS::State::AdjacencyIterator jj = (*b).beginEntering() ;
      jj != (*b).endEntering() ;
      ++jj )
    {
      pB.move( (*jj).tail().coords() - (*b).coords() );
    }

  for(IlLTS::State::AdjacencyIterator jj = (*b).beginLeaving() ;
      jj != (*b).endLeaving() ;
      ++jj )
    {
      pB.move( (*jj).head().coords() - (*b).coords() );
    }

  return( innerProduct(pA,pA) > innerProduct(pB,pB) );
}
#endif


#undef DO_NOT_INCLUDE



void
IlSimulatedAnnealing(IlGrid grid_IO,
                     IlLTS::StateIterator start_I,
                     IlLTS::StateIterator end_I,
                     double &costFuncVal_IO,
                     double absolutMinumunEstimate )
{
  int N = end_I - start_I;
  double currentCost = costFuncVal_IO;
  double bestFound = currentCost ;
  int loopCnt;
  int solutionsGenerated = 0;
  double temperature = currentCost;
  bool   solutionGoodEnought = false;
  //cerr << "Starting SA" << endl;
  //cerr << currentCost << "  " << absolutMinumunEstimate << endl;
  
  
    loopCnt = N;
  

  
  
  {
    bestFound = min( bestFound , currentCost );
    double costDistance = bestFound - absolutMinumunEstimate ;
    const double COOLING_FACTOR = 0.999;
    if( 0.3 * costDistance < COOLING_FACTOR * temperature )
      {
        temperature = 0.3 * costDistance;
        //cerr << "Temp = " << temperature << endl;
      }
    else
      {
        temperature *= COOLING_FACTOR;
      }
  }
  

  
  
  {
    const int ROUND_CNT_COFF = 500;
    solutionGoodEnought = (ROUND_CNT_COFF * N * N < solutionsGenerated
                           || currentCost < 1.11 * absolutMinumunEstimate );
  }
  

  while( ! solutionGoodEnought )
    {
      for( int ii=0; ii < loopCnt ; ++ii)
        {
          
          
          double markCost = 0.0;
          
          
          const double SWAP_RATIO_IN_SA = 0.5;
          if( Random::normal() < SWAP_RATIO_IN_SA )
            {
              
              
                int inx = Random::generate(N);
                int jnx = Random::generate(N-1);
                if( jnx >= inx ) ++jnx;
                markCost = IlStatePairCost(*(start_I+inx), *(start_I+jnx));
                grid_IO.swapStates( *(start_I+inx), *(start_I+jnx) );
                markCost -= IlStatePairCost(*(start_I+inx), *(start_I+jnx));
              
          
            }
          else
            {
              
              
                static const int NOF_NEIGHBOUR = 8;
                static const int neighbourX[NOF_NEIGHBOUR]={-1,0,1,-1,1,-1,0,1};
                static const int neighbourY[NOF_NEIGHBOUR]={-1,-1,-1,0,0,1,1,1};
                int sidx = Random::generate(N);
                int neighb = Random::generate(NOF_NEIGHBOUR);
                int tx = (start_I+sidx)->Xcoord() + neighbourX[neighb];
                int ty = (start_I+sidx)->Ycoord() + neighbourY[neighb];
                while( !grid_IO.isFree(tx,ty) )
                  {
                    sidx = Random::generate(N);
                    neighb = Random::generate(NOF_NEIGHBOUR);
                    tx = (start_I+sidx)->Xcoord() + neighbourX[neighb];
                    ty = (start_I+sidx)->Ycoord() + neighbourY[neighb];
                  }
                markCost = IlSingleStateCost(*(start_I+sidx));
                grid_IO.moveState(*(start_I+sidx),tx,ty);
                markCost -= IlSingleStateCost(*(start_I+sidx));
              
          
            }
          

          
          
          if( markCost < 0.0 && exp(markCost / temperature) < Random::normal() )
            {
              grid_IO.undoLast();
            }
          else
            {
               currentCost -= markCost ;
            }
          

        }
      
      
        solutionsGenerated += loopCnt;
      

      
      
      {
        bestFound = min( bestFound , currentCost );
        double costDistance = bestFound - absolutMinumunEstimate ;
        const double COOLING_FACTOR = 0.999;
        if( 0.3 * costDistance < COOLING_FACTOR * temperature )
          {
            temperature = 0.3 * costDistance;
            //cerr << "Temp = " << temperature << endl;
          }
        else
          {
            temperature *= COOLING_FACTOR;
          }
      }
      

      
      
      {
        const int ROUND_CNT_COFF = 500;
        solutionGoodEnought = (ROUND_CNT_COFF * N * N < solutionsGenerated
                               || currentCost < 1.11 * absolutMinumunEstimate );
      }
      

    }
  /*
  cerr << "End of SA" << endl;
  cerr << solutionsGenerated << "  " << currentCost << " "
       << costFuncVal_IO << endl;
  */
  costFuncVal_IO = currentCost ;
}


