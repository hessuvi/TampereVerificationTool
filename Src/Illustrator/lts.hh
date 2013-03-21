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



#ifndef ILLUS_LTS_HH
#define ILLUS_LTS_HH

#ifndef TOOLS_POINT_HH
#include "point.hh"
#endif

#ifndef TOOLS_LINE_HH
#include "line.hh"
#endif

#ifndef TOOLS_GRAPH_HH
#include "graph.hh"
#endif

#include "StateProps.hh"

#ifndef MAKEDEPEND
#include <map>
#include <string>
#include <LSTS_File/TransitionsAP.hh>
#include <LSTS_File/ActionNamesAP.hh>
#endif

class InStream;
class Header;



typedef lsts_index_t state_t;
typedef lsts_index_t action_t;


class IlStateBase;
class IlTransitionBase;
class IlLTS;




class IlStateBase
{
public:
  enum StateFlag
  {
    INITIAL_STATE       = 1,
    REJECTION_FLAG      = 2,
    DEADLOCK_FLAG       = 4,
    LIVELOCK_FLAG       = 8,
    HASTRANSITIONS_FLAG = 16,
    CUTSTATE_FLAG       = 32,
    STICKYPROP_FLAG     = 64
  };
  void addFlag(StateFlag f) { d_flags |= f; };
  int  flags() const { return d_flags ; };
  void clearFlags() {d_flags = 0 ;};

private:
  int  d_flags;

public:
  static const state_t UNDEFINED_NAME = 0;
  state_t name() const { return d_name; };
  void    name(state_t n) { d_name = n; };

  bool operator == (const IlStateBase & o) const 
     { return (d_name == o.d_name); };
  bool operator < (const IlStateBase & o) const 
     { return (d_name < o.d_name); };

private:
  state_t d_name;

public:
  IlStateBase():d_flags(0),d_name(UNDEFINED_NAME) {};
  ~IlStateBase() {};


public:
  Point &coords() {return d_coords;};
  const Point &coords() const {return d_coords;};
private:
  Point d_coords;


public:
  int Xcoord() {return (int)(coords().xCoord()); };
  int Ycoord() {return (int)(coords().yCoord()); };


};


class IlTransitionBase
{
public:
  IlTransitionBase():d_action(~1) {};
  ~IlTransitionBase() {};

  action_t action() const {return d_action; };
  void action(action_t a) {d_action = a ;};
private:
  action_t d_action;


public:
  Line &line() {return d_line;};
  const Line &line() const {return d_line;};
private:
  Line d_line;


};




class IlLTS :public iTransitionsAP,
             public iActionNamesAP,
             private Graph<IlStateBase,IlTransitionBase>
{
  typedef Graph<IlStateBase,IlTransitionBase> Ancestor;
public:
  typedef Ancestor::Node State;
  typedef Ancestor::Edge Transition;

  typedef Ancestor::NodeIterator StateIterator;
  typedef Ancestor::EdgeIterator TransitionIterator;

  StateIterator beginStates();
  StateIterator endStates();
  long          nofStates() const;
  
  unsigned getStateNumber(StateIterator stIter)
  { return stIter-beginStates()+1; }

  TransitionIterator beginTransitions();
  TransitionIterator endTransitions();
  long               nofTransitions() const;

  bool initializeFromFile( InStream&, const string& comment );

  IlLTS();
  ~IlLTS();

  const StatePropsContainer& getStatePropsContainer() { return spcontainer; }

private:
  typedef map<action_t,string> Alphabet;
  Alphabet d_alphabet;

  StatePropsContainer spcontainer;

public:
  const string &nameOfAction(action_t action);
  typedef Alphabet::iterator ActionIterator;
  ActionIterator beginAlphabet();
  ActionIterator endAlphabet();


private:
  IlLTS(const IlLTS &); // Not implemented
  IlLTS &operator = (const IlLTS &); // Not implemented


private:
void initializeDataStructures(const Header&, const string& comment);


void lsts_StartActionNames( Header& ); // <<--Timo muutti parametrin 
void lsts_ActionName(lsts_index_t action, const string& name);
void lsts_EndActionNames( );

void lsts_StartTransitions( Header& );
void lsts_StartTransitionsFromState( lsts_index_t start_state );
void lsts_Transition( lsts_index_t start_state,
                      lsts_index_t dest_state,
                      lsts_index_t action );
void lsts_EndTransitionsFromState( lsts_index_t start_state );
void lsts_EndTransitions();



};



inline
bool
operator == (const IlLTS::Transition &t1,
             const IlLTS::Transition &t2)
{
  return( &t1 == &t2 );
}



#endif // ILLUS_LTS_HH
