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



#ifndef ILLUS_GRID_HH
#define ILLUS_GRID_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif





class IlGrid
{


public:
  bool isFree(int x, int y) const;
  IlLTS::State &content(int x, int y);
  bool swapStates(IlLTS::State &u, IlLTS::State &v);
  bool moveState(IlLTS::State &s, int x, int y);
  bool undoLast();


  bool placeState(IlLTS::State &s, int x, int y);


  void reInit();




private:
  typedef IlLTS::State *state_ptr;
  typedef vector<IlLTS::State *> grid_row;
  typedef vector<grid_row> grid_array;
  grid_array grid;


public:
  IlGrid(int N);
  IlGrid();


public:
  class BeforeModification;
private:
  BeforeModification *mark;


};





#endif // ILLUS_GRID_HH
