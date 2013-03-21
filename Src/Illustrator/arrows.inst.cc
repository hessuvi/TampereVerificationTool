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



#include "arrows.hh"

#ifndef ILLUS_SURROUND_HH
#include "surround.hh"
#endif

#ifndef MAKEDEPEND
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#endif






class IlLineLengthLess
{
public:
  bool operator () (const IlLTS::Transition *a,
                    const IlLTS::Transition *b);
  bool operator () (const Line *a, const Line *b);
};


bool
IlLineLengthLess::operator () (const Line *a, const Line *b)
{
  double lenA = euclNormP2(*a);
  double lenB = euclNormP2(*b);

  if( lenA == lenB ) return( false );
  if( lenA == 0.0 ) return( false );
  if( lenB == 0.0 ) return( true );
  return( lenA < lenB );
}

bool
IlLineLengthLess::operator () (const IlLTS::Transition *a,
                               const IlLTS::Transition *b)
{
  double lenA = euclNormP2(a->line());
  double lenB = euclNormP2(b->line());

  if( lenA == lenB ) return( false );
  if( lenA == 0.0 ) return( false );
  if( lenB == 0.0 ) return( true );
  return( lenA < lenB );
}


typedef pair<Point,Point> GridSegment;
enum SegmentDirection
{
  DIR_LEFT,
  DIR_UP,
  DIR_RIGHT,
  DIR_DOWN,
  DIR_LAST
};

#include <cassert>

static GridSegment
makeSegment(Point p, int direc)
{
  switch( direc )
  {
  case DIR_LEFT:
    return( make_pair( p+Point(-1.0,0.0), p ) );
  case DIR_UP:
    return( make_pair( p+Point(0.0,-1.0), p) );
  case DIR_RIGHT:
    return( make_pair( p, p+Point(1.0,0.0) ) );
  case DIR_DOWN:
    return( make_pair( p, p+Point(0.0,1.0) ) );
  default:
    assert( false && "Illigal grid segment direction" );
    return( make_pair( p,p));
  }
}

inline bool
horizontalSegment( const GridSegment &seg)
{
  return( seg.first.yCoord() == seg.second.yCoord());
}

inline bool
verticalSegment( const GridSegment &seg)
{
  return( seg.first.xCoord() == seg.second.xCoord());
}


class CrossPoint
{
public:
  CrossPoint
    (
    const GridSegment &seg,
    double crossingDelta
    ): segment(seg),
       naturalCrossPoint(crossingDelta),
       realCrossPoint(0.0)
  {
  };

  bool
  operator <
    (
    const CrossPoint &o
    )
  {
    return (naturalCrossPoint < o.naturalCrossPoint) ;
  };
  
public:
  GridSegment segment;
  double      naturalCrossPoint;
  double      realCrossPoint;
};


template <class SetOrMap, class Key>
inline bool
contains(const SetOrMap &cont, const Key &key)
{
  return( cont.end() != cont.find(key) );
}



bool arrowCrossesNode( Line& trline, set<Point>& grid) {
    const double NODE_TO_LINE_DIST = 0.2;
    LineAlgorithm points(trline.front(),trline.back());
    LineAlgorithm endp(trline.back(),trline.back());
    for( ++points; points != endp ; ++points) {
        if( contains(grid,(*points)) &&
            euclNorm(*points,
                     trline.front(),
                     trline.back()) < NODE_TO_LINE_DIST ) return true;
        }
    return false;
    }




template <typename Number>
Number sign(Number n) {
    if( n < 0 ) return -1;
    if( n > 0 ) return 1;
    return 0;
}

#ifndef M_PI
#define M_PI 3.14159265359
#endif

void anglesForCrossingLine(Line& trline,
                           double& beginAngle,
                           double& endAngle ) {
    const double CROSS_SHIFT_ANGLE = M_PI / 10.0 ;

    Point vec = trline.back()-trline.front();
    double deltaX = vec.xCoord() ;
    double deltaY = vec.yCoord() ;
    double diff = fabs(deltaX)-fabs(deltaY);
    double signOfAngle = sign(deltaX) * sign(deltaY) * sign(diff);

    if( signOfAngle < 0 ) {
        beginAngle -= CROSS_SHIFT_ANGLE;
        endAngle -= CROSS_SHIFT_ANGLE;
        }
    else if( signOfAngle > 0 ) {
        beginAngle += CROSS_SHIFT_ANGLE;
        endAngle += CROSS_SHIFT_ANGLE;
        }
    else {
        beginAngle -= CROSS_SHIFT_ANGLE;
        endAngle += CROSS_SHIFT_ANGLE;
        }
}



void anglesForTransition( Line& trline, set<Point>& grid,
                         double& beginAngle,
                         double& endAngle ) {

    endAngle = angleOfVector(trline.front()-trline.back());
    beginAngle   = angleOfVector(trline.back()-trline.front());

    if( arrowCrossesNode(trline,grid) ) {
        anglesForCrossingLine(trline, beginAngle, endAngle);
        }
}





bool
IlTransitionArrows
  (
  IlLTS::StateIterator nodesStart,
  IlLTS::StateIterator nodesEnd,
  IlLTS::TransitionIterator start_I,
  IlLTS::TransitionIterator end_I
  )

{
  
  
    
    
    for( IlLTS::TransitionIterator ii = start_I;
         ii != end_I;
         ++ii)
      {
        (*ii).line().push_front((*ii).tail().coords());
        (*ii).line().push_back((*ii).head().coords());
      } 
    
  
    
    
    typedef vector<IlLTS::Transition *> TransitionPtrVector;
    TransitionPtrVector transitions;
    for( IlLTS::TransitionIterator ii = start_I;
         ii != end_I;
         ++ii )
      {
        transitions.push_back(&(*ii));
      }
    sort(transitions.begin(),
         transitions.end(),
         IlLineLengthLess());
    
  
    
    set<Point> grid;
    for( IlLTS::StateIterator idx = nodesStart;
         idx != nodesEnd;
         ++idx ) {
        grid.insert(idx->coords());
        }
    
  
  
  
  typedef map<Line *, vector< CrossPoint > > BendContainer;
  BendContainer additionalBendsOf;
  
  
  typedef map<GridSegment, vector< CrossPoint *> > BendsByGridSegment;
  BendsByGridSegment crossesOf;
  
  
  typedef map<Point, set<Line *> > CrossedMap;
  typedef map<GridSegment, set<Line *> > TunnelMap;
  CrossedMap crossed;
  TunnelMap  tunnel;
  for( IlLTS::StateIterator ii = nodesStart ;
       ii != nodesEnd ;
       ++ ii )
    {
      crossed[ii->coords()];
    }
  TransitionPtrVector::iterator longStart = transitions.begin();
  for( ; longStart != transitions.end() && maxNorm((*longStart)->line()) < 1.1;
       ++ longStart )
    ;
  // TransitionPtrVector::iterator longBegin=longStart;
  for( ; longStart != transitions.end() && maxNorm((*longStart)->line()) > 1.0;
       ++ longStart )
    {
      
      
      Line &trline = (*longStart)->line();
      LineAlgorithm points(trline.front(), trline.back());
      LineAlgorithm endp(trline.back(),trline.back());
      for( ++points; points != endp ; ++points)
        {
          if( points.exact() )
            {
              
              
              if( contains(crossed,*points) )
                {
                  double deltaX = trline.back().xCoord() - trline.front().xCoord();
                  double deltaY = trline.back().yCoord() - trline.front().yCoord();
                  if( deltaX <= -0.1 )
                    {
                      if( deltaY <= -0.1 )
                        {
                          {
                            GridSegment segment = makeSegment(*points,DIR_UP);
                            
                            {
                              double delta = crossingFractionInP( segment.first,
                                                                  segment.second,
                                                                  trline.front(),
                                                                  trline.back());
                              additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
                              crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
                            }
                            
              
                          }
                          {
                            GridSegment segment = makeSegment(*points,DIR_RIGHT);
                            
                            {
                              double delta = crossingFractionInP( segment.first,
                                                                  segment.second,
                                                                  trline.front(),
                                                                  trline.back());
                              additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
                              crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
                            }
                            
              
                          }
                        }
                      else if( deltaY >= 0.1 )
                        {
                          {
                            GridSegment segment = makeSegment(*points,DIR_UP);
                            
                            {
                              double delta = crossingFractionInP( segment.first,
                                                                  segment.second,
                                                                  trline.front(),
                                                                  trline.back());
                              additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
                              crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
                            }
                            
              
                          }
                          {
                            GridSegment segment = makeSegment(*points,DIR_LEFT);
                            
                            {
                              double delta = crossingFractionInP( segment.first,
                                                                  segment.second,
                                                                  trline.front(),
                                                                  trline.back());
                              additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
                              crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
                            }
                            
              
                          }
                        }
                      else
                        {
                          {
                            GridSegment  segment = makeSegment(*points,DIR_UP);
                            
                            {
                              double delta = crossingFractionInP( segment.first,
                                                                  segment.second,
                                                                  trline.front(),
                                                                  trline.back());
                              additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
                              crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
                            }
                            
              
                          }
                        }
                    }
                  else if( deltaX >= 0.1 )
                    {
                      if( deltaY <= -0.1 )
                        {
                          {
                            GridSegment segment = makeSegment(*points,DIR_DOWN);
                            
                            {
                              double delta = crossingFractionInP( segment.first,
                                                                  segment.second,
                                                                  trline.front(),
                                                                  trline.back());
                              additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
                              crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
                            }
                            
              
                          }
                          {
                            GridSegment segment = makeSegment(*points,DIR_RIGHT);
                            
                            {
                              double delta = crossingFractionInP( segment.first,
                                                                  segment.second,
                                                                  trline.front(),
                                                                  trline.back());
                              additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
                              crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
                            }
                            
              
                          }
                        }
                      else if( deltaY >= 0.1 )
                        {
                          tunnel[makeSegment(*points,DIR_DOWN)].insert(&trline);
                          tunnel[makeSegment(*points,DIR_LEFT)].insert(&trline);
                        }
                      else
                        {
                          tunnel[makeSegment(*points,DIR_DOWN)].insert(&trline);
                        }
                    }
                  else
                    {
                      if( deltaY <= -0.1 )
                        {
                          tunnel[makeSegment(*points,DIR_RIGHT)].insert(&trline);
                        }
                      else if( deltaY >= 0.1 )
                        {
                          tunnel[makeSegment(*points,DIR_LEFT)].insert(&trline);
                        }
                      else
                        {
                          assert(false && "Loops are not allowed here");
                        }
                    }
                }
              
      
            }
          else
            {
              
              
              for( int direc=DIR_LEFT; direc < DIR_LAST; ++direc )
                {
                  GridSegment seg=makeSegment(*points,direc);
                  if( linesCross(seg.first, seg.second,trline.front(),trline.back()) )
                    {
                      tunnel[seg].insert(&trline);
                    }
                }
              
      
            }
        }
      
  
    }
  // TransitionPtrVector::iterator longEnd=longStart;
  

  

  
  
  typedef map<state_t, IlTransitionRing> SurrMap;
  SurrMap stateSurroundings;
  
  
  TransitionPtrVector::iterator curTransition;
  for( curTransition = transitions.begin();
       curTransition != transitions.end() ;
       ++ curTransition)
    {
      IlLTS::Transition &tr = * * curTransition;
      IlLTS::State &head=tr.head();
      IlLTS::State &tail=tr.tail();
      if( head == tail )
        {
          //% Loops are at end of orderedTransitions -vector
          break;
        }
      IlTransitionRing &headRing = stateSurroundings[head.name()];
      IlTransitionRing &tailRing = stateSurroundings[tail.name()];
  
      double headAngle;
      double tailAngle;
      anglesForTransition(tr.line(), grid, tailAngle, headAngle);
      headRing.insertAfter(headAngle,tr,tail);
      tailRing.insertBefore(tailAngle,tr,head);
    }
  
  
  for( SurrMap::iterator ii=stateSurroundings.begin();
       ii != stateSurroundings.end();
       ++ii)
    {
      IlTransitionRing &ring= (*ii).second;
      //% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      if( ring.optimize(M_PI/8.0) )
        {
          // cerr << "Optimoitu solmulle " << (*ii).first << endl;
        }
      else
        {
          // cerr << "Ei käypä solmulle " << (*ii).first << endl;
        }
    }
  
  
  //% curTransition points still to first loop
  for(   ;
       curTransition != transitions.end() ;
       ++ curTransition)
    {
      IlLTS::Transition &tr = * * curTransition;
      IlLTS::State &head=tr.head();
      IlLTS::State &tail=tr.tail();
      if( !(head == tail) )
        {
          //% !!!!!!!!!!!!!!!!!!!!
          //% Uh! sombody is fooling me
          return( false );
        }
      //% To get first and last two point from line representing
      //% transition. 
      IlTransitionRing &headRing = stateSurroundings[head.name()];
  
      //% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      headRing.addLoop(tr, M_PI / 12.0);
    }
  
  
  for( map<state_t, IlTransitionRing>::iterator ii=stateSurroundings.begin();
       ii != stateSurroundings.end();
       ++ii)
    {
      IlTransitionRing &ring= (*ii).second;
      state_t           stateName = (*ii).first;
      // cerr << "solmun " << stateName << " viivat" << endl;
      //% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      for( IlTransitionRing::Iterator jj = ring.begin();
           jj != ring.end();
           ++jj)
        {
          double angle=(*jj).getAngle();
          const double FIRST_BEND_RADIUS = 0.3;
          Point toAdd(constProduct(FIRST_BEND_RADIUS,
                                   Point(cos(angle),sin(angle))));
          IlLTS::Transition &tr = (*jj).transition();
          if( tr.head().name() == stateName )
            {
              Point p = tr.head().coords() + toAdd ;
              Line::iterator pos=tr.line().end();
              pos--;
              tr.line().insert(pos,p);
            }
          else
            {
              Point p = tr.tail().coords() + toAdd ;
              Line::iterator pos=tr.line().begin();
              pos++;
              tr.line().insert(pos,p);
            }
        }
    }
  

  return( true );
}




