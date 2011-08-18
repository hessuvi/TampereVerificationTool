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



#include "localsea.hh"

#ifndef ILLUS_OBJFUNC_HH
#include "objfunc.hh"
#endif

#ifndef MAKEDEPEND
#include <algorithm>
#include <set>
#include <iostream>
#endif





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





void
IlLocalSearchCoordinates( IlGrid          grid_IO,
                          IlLTS::StateIterator start_I,
                          IlLTS::StateIterator end_I,
                          double &costFuncVal_IO)
{
  double currentCost = costFuncVal_IO;
  double lastCost = currentCost;
  
#if 0
Seuraava koodi on kommentoitu pois
  cout << "Valitulos" << endl;
  for( int ii=0; ii<N_I ; ++ii )
    {
      cout << ii + 1 << " "
           << states_IO[ii].Xcoord() << " "
           << states_IO[ii].Ycoord() << endl;
    }
#endif
  do {
    lastCost = currentCost;
      // std::cerr << "ILLUS: " << "Kohdefunktio: " << currentCost << std::endl;
#if 1
      
      
        double swapCost;
	bool jatkuu = true;
        do {
          swapCost = currentCost ;
          for( IlLTS::StateIterator ii = start_I ;
               ii != end_I ;
               ++ii )
            {
              IlLTS::StateIterator jj = ii;
              for( ++jj ;
                   jj != end_I ;
                   ++jj )
                {
                  if( *ii == *jj ) continue;
                  double markCost = IlStatePairCost( *ii, *jj );
                  grid_IO.swapStates( *ii, *jj );
                  markCost -= IlStatePairCost( *ii, *jj );
                  if( markCost > 0.0 )
                    {
                      currentCost -= markCost ;
                    }
                  else
                    {
                      grid_IO.undoLast();
                    }
                }
            }
        } while( currentCost < (swapCost-1.0e-8) );
      

#endif
      // cerr << "Kohdefunktio (vaihto): " << currentCost << endl;
#if 1

      
      
        double moveCost;
        const int nof_neighbour = 8;
        static const int neighbourX[nof_neighbour]={-1,0,1,-1,1,-1,0,1};
        static const int neighbourY[nof_neighbour]={-1,-1,-1,0,0,1,1,1};
      
        vector<IlLTS::StateIterator> applyStack;
        long stateCnt = 0;
        for( IlLTS::StateIterator ii = start_I ; ii != end_I ; ++ii)
          {
            applyStack.push_back(ii);
            ++stateCnt;
          }
        do {
          moveCost = currentCost ;
          make_heap( applyStack.begin(),
                     applyStack.end(),
                     IlGreaterSingleStateCost());
          long outFromLoop = stateCnt ;
          // cerr << "Uusi kierros " << currentCost << " " << outFromLoop <<  endl;
      
          for( vector<IlLTS::StateIterator>::iterator ii = applyStack.begin() ;
               outFromLoop && ii != applyStack.end() ;
               ++ii, --outFromLoop )
            {
              IlLTS::State &currentState = *(*ii);
              int bestX = 0;
              int bestY = 0;
              double bestDeltaCost = -0.1;
              double originalCost = IlSingleStateCost(currentState) ;
              for( int jj = 0; jj < nof_neighbour ; ++jj )
                {
                  int tx = currentState.Xcoord() + neighbourX[jj] ;
                  int ty = currentState.Ycoord() + neighbourY[jj] ;
                  if( grid_IO.isFree(tx,ty) )
                    {
                      double markCost = originalCost ;
                      grid_IO.moveState(currentState,tx,ty);
                      markCost -= IlSingleStateCost(currentState) ;
                      if( markCost > bestDeltaCost )
                        {
                          bestDeltaCost = markCost ;
                          bestX = tx;
                          bestY = ty;
                          grid_IO.undoLast();
                        }
                      else
                        {
                          grid_IO.undoLast();
                        }
                    }
                }
              if( bestDeltaCost > 0.0 )
                {
                  grid_IO.moveState(currentState,bestX,bestY);
                  currentCost -= bestDeltaCost ;
                  // outFromLoop /= 2;
                  // -- outFromLoop;
                }
            }
        } while( currentCost < (moveCost-1e-8) );
      

#endif
#if 0
      if( currentCost >= lastCost )
        {
          IlShrinkLayout(grid_IO,start_I,end_I,currentCost);
          cerr << "Kutistettu kohdefunktio " << currentCost << endl;
        }
#endif
  } while ( currentCost < (lastCost-1e-10) );

  // Comment out to end of function, if arrow
  // placement function needs grid to be up to date
#if 0
  
  
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
  

#endif
  costFuncVal_IO = currentCost ;
} 









const int NEIGHBOUR_RADIUS = 1;
const int NEIGHBOUR_DIAMETER = NEIGHBOUR_RADIUS + NEIGHBOUR_RADIUS + 1;
const int NEIGHBOUR_SIZE = NEIGHBOUR_DIAMETER * NEIGHBOUR_DIAMETER;

inline static bool
localNeighbourAdjust(int place, int &x, int &y, int cx=0, int cy=0)
{
  int xDelta = (place / NEIGHBOUR_DIAMETER) - NEIGHBOUR_RADIUS;
  int yDelta = (place % NEIGHBOUR_DIAMETER) - NEIGHBOUR_RADIUS;
  x = cx + xDelta;
  y = cy + yDelta;
  return ( xDelta != 0 || yDelta != 0 );
}


void
IlShrinkLayout(IlGrid &grid_IO,
               IlLTS::StateIterator start,
               IlLTS::StateIterator end,
               double &curObjFuncVal)
{
  
  
  vector<int> oldSolution;
  double oldObjective = curObjFuncVal;
  set<state_t> notMoved;
  {
    long idx;
    IlLTS::StateIterator ss;
    set<state_t>::iterator pos = notMoved.begin();
    for( idx=0, ss = start ; ss != end ; ++ss )
      {
        oldSolution.push_back(ss->Xcoord());
        oldSolution.push_back(ss->Ycoord());
        pos = notMoved.insert(pos,ss->name());
      }
  }
  

  
  
  {
  typedef list<IlLTS::State *> StateList;
  typedef IlLTS::State::AdjacencyIterator AdjacencyIterator;
  typedef IlLTS::State State;
  StateList handlingQueue;
  handlingQueue.push_back( &(*start) );
  notMoved.erase(start->name());
  
  for( ; ! handlingQueue.empty() ; handlingQueue.pop_front() )
    {
      State &current = *(handlingQueue.front());
      for( AdjacencyIterator ii = current.beginLeaving() ;
           ii != current.endLeaving();
           ++ii )
        {
          State &other = (*ii).head();
          
          
          if( notMoved.end() != notMoved.find(other.name() ) )
            {
              handlingQueue.push_back( &other );
              notMoved.erase(other.name());
          
              double baseCost = IlSingleStateCost(other);
                      double bestDeltaCost = 0.0;
                      int    bestPlace = -1;
              int nx, ny;
              for( int neix = 0 ; neix < NEIGHBOUR_SIZE ; ++neix )
                {
                  if( localNeighbourAdjust(neix,nx,ny,
                                           current.Xcoord(),
                                           current.Ycoord())
                      && grid_IO.isFree(nx,ny) )
                    {
                      grid_IO.moveState(other,nx,ny);
                      double markCost = baseCost - IlSingleStateCost(other);
                      if( bestPlace < 0 || bestDeltaCost < markCost )
                        {
                          bestPlace = neix;
                          bestDeltaCost = markCost;
                        }
                      grid_IO.undoLast();
                    }
                }
              if( bestPlace >= 0 )
                {
                  localNeighbourAdjust(bestPlace,nx,ny,
                                       current.Xcoord(),
                                       current.Ycoord());
                  grid_IO.moveState(other,nx,ny);
                  curObjFuncVal -= bestDeltaCost;
                }
            }
          
  
        }
      for( AdjacencyIterator ii = current.beginEntering() ;
           ii != current.endEntering();
           ++ii )
        {
          State &other = (*ii).tail();
          
          
          if( notMoved.end() != notMoved.find(other.name() ) )
            {
              handlingQueue.push_back( &other );
              notMoved.erase(other.name());
          
              double baseCost = IlSingleStateCost(other);
                      double bestDeltaCost = 0.0;
                      int    bestPlace = -1;
              int nx, ny;
              for( int neix = 0 ; neix < NEIGHBOUR_SIZE ; ++neix )
                {
                  if( localNeighbourAdjust(neix,nx,ny,
                                           current.Xcoord(),
                                           current.Ycoord())
                      && grid_IO.isFree(nx,ny) )
                    {
                      grid_IO.moveState(other,nx,ny);
                      double markCost = baseCost - IlSingleStateCost(other);
                      if( bestPlace < 0 || bestDeltaCost < markCost )
                        {
                          bestPlace = neix;
                          bestDeltaCost = markCost;
                        }
                      grid_IO.undoLast();
                    }
                }
              if( bestPlace >= 0 )
                {
                  localNeighbourAdjust(bestPlace,nx,ny,
                                       current.Xcoord(),
                                       current.Ycoord());
                  grid_IO.moveState(other,nx,ny);
                  curObjFuncVal -= bestDeltaCost;
                }
            }
          
  
        }
    }  
  }

  
  
  #if 1
  if( oldObjective < curObjFuncVal )
    {
      grid_IO.reInit();
      long idx;
      IlLTS::StateIterator ss;
      for( idx=0, ss = start ; ss != end ; ++ss, idx+=2 )
        {
          grid_IO.placeState(*ss,oldSolution[idx],oldSolution[idx+1]);
        }
      curObjFuncVal = oldObjective;
    }  
  #endif
  

}


