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



#include "level.hh"



void
IlBigInitialSolution(int             N_I,
                     IlGrid          &grid_O,
                     IlLTS::StateIterator start_I,
                     IlLTS::StateIterator /*end_I*/)
{
  typedef map<state_t,int> NodeSet;
  NodeSet done;
  typedef list<IlLTS::State *> Queue;
  typedef IteratorAdaptor<IlLTS::State, Queue::iterator> QueueIterator;
  Queue order;
  vector<int> levelSize;


  done.insert(make_pair(start_I->name(),0));
  order.push_back(&(*start_I));
  levelSize.push_back(1);

  for( QueueIterator current = order.begin();
       current != order.end() ;
       ++current )
    {
      int level = (done.find(current->name()))->second + 1;
      if( static_cast<size_t>(level) >= levelSize.size() )
        {
          levelSize.push_back(0);
        }

      typedef IlLTS::State::AdjacencyIterator AdjacencyIterator;

      for( AdjacencyIterator ii = current->beginLeaving() ;
           ii != current->endLeaving();
           ++ii )
        {
          IlLTS::State &other = (*ii).head();
          if( done.end() == done.find(other.name()) )
            {
              done.insert(make_pair(other.name(),level));
              levelSize[level] += 1;
              order.push_back(&other);
            }
      }
      for( AdjacencyIterator ii = current->beginEntering() ;
           ii != current->endEntering();
           ++ii )
        {
          IlLTS::State &other = (*ii).tail();
          if( done.end() == done.find(other.name()) )
            {
              done.insert(make_pair(other.name(),level));
              levelSize[level] += 1;
              order.push_back(&other);
            }
        }
    }

  int center = N_I / 2 ;
  QueueIterator current = order.begin();
  grid_O.placeState(*current, center, center);
  ++current;
  int radius_add = 0;

  while( current != order.end() )
    {
      int level = (done.find(current->name()))->second;
      int radius = level + radius_add;
      for( ; radius * 8 < levelSize[level] ; radius = level + radius_add )
        {
          ++ radius_add;
        }
      int diam = radius * 2;
      int offset = (radius * 8 ) / (levelSize[level]+1);
      if ( 0 == offset ) offset = 1;
      for( int ii = 0; ii < levelSize[level] ; ++ii , ++current )
        {
          int side = (ii *offset ) / diam;
          int item = (ii *offset ) % diam;
          int x=0;
          int y=0;

          if( 0 == side )
            {
              x = item;
              y=0;
            }
          else if ( 1 == side )
            {
              x = diam;
              y = item;
            }
          else if ( 2 == side )
            {
              x = diam - item;
              y = diam;
            }
          else
            {
              x = 0;
              y = diam - item;
            }
          x += (center - radius);
          y += (center - radius);
          grid_O.placeState(*current,x,y);
        }
    }
}


