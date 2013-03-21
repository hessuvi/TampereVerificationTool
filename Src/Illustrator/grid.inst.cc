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



#include "grid.hh"




class IlGrid::BeforeModification
{
public:
  virtual void restore(IlGrid &) = 0;
};

class SwapMod: public  IlGrid::BeforeModification
{
public:
  SwapMod(IlLTS::State &u, IlLTS::State &v): s1(&u),s2(&v) {};
private:
  IlLTS::State *s1;
  IlLTS::State *s2;
public:
  void restore(IlGrid &g) { g.swapStates(*s1,*s2); };
};

class MoveMod: public  IlGrid::BeforeModification
{
public:
  MoveMod(IlLTS::State &u, int org_x, int org_y): s(&u), x(org_x), y(org_y) {};
private:
  IlLTS::State *s;
  int x;
  int y;
public:
  void restore(IlGrid &g) { g.moveState(*s,x,y); }; 
};




bool
IlGrid::isFree(int x, int y) const
{
  if( x < 0 ) return false;
  if( y < 0 ) return false;
  if( (unsigned)x >= grid.size() ) return false ;
  if( (unsigned)y >= grid.size() ) return false ;

  return 0 == grid[x][y];
}

#include <cassert>

IlLTS::State &
IlGrid::content(int x, int y)
{
  assert(    (x>=0)
          && (y>=0)
          && ((unsigned)x < grid.size())
          && ((unsigned)y < grid.size())
          && (grid[x][y] != 0)
          && "There is not state in given position" );
  return( *grid[x][y] );
}


bool
IlGrid::swapStates(IlLTS::State &u, IlLTS::State &v)
{
  if( 0 != mark ) delete mark;
  mark = new SwapMod(u,v);
  Point tmp= u.coords();
  u.coords()=v.coords();
  v.coords()=tmp;

  grid[u.Xcoord()][u.Ycoord()] = &u;
  grid[v.Xcoord()][v.Ycoord()] = &v;
  return( true );
}

bool
IlGrid::moveState(IlLTS::State &s, int x, int y)
{
  if( x < 0 ) return false;
  if( y < 0 ) return false;
  if( (unsigned)x >= grid.size() ) return false ;
  if( (unsigned)y >= grid.size() ) return false ;

  if( 0 != grid[x][y] )
    {
      return( false );
    }

  if( 0 != mark )
    {
      delete mark;
    }

  mark = new MoveMod(s,s.Xcoord(),s.Ycoord());
  grid[s.Xcoord()][s.Ycoord()] = 0;

  s.coords() = Point(x,y);

  grid[s.Xcoord()][s.Ycoord()] = &s;
  return( true );
}

bool
IlGrid::undoLast()
{
  if( 0 == mark )
    {
      return( false );
    }
  BeforeModification *oldmark =  mark;
  mark = 0;
  oldmark ->restore(*this);
  delete oldmark;
  return( true );
}

bool
IlGrid::placeState(IlLTS::State &s, int x, int y)
{
  if( 0 != grid[x][y] )
    {
      return( false );
    }
  s.coords()=Point(x,y);

  grid[s.Xcoord()][s.Ycoord()] = &s;
  return( true );
}

IlGrid::IlGrid(int N)
: grid(N,grid_row(N,(IlLTS::State *)0)),mark(0)
{
}

void
IlGrid::reInit()
{
  for( long idx=0; static_cast<size_t>(idx) < grid.size() ; ++idx)
    {
      for( long jdx=0; static_cast<size_t>(jdx) < grid.size() ; ++jdx )
        {
          grid[idx][jdx] = 0;
        }
    }
}

IlGrid::IlGrid()
{
}





