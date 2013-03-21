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



#include "lts.hh"

#ifndef MAKEDEPEND
#include <fstream>
#include <LSTS_File/iLSTS_File.hh>
#endif

#include "lts_draw_write.hh"
#include "error_handling.hh"




IlLTS::StateIterator
IlLTS::beginStates()
{
  return( beginNodes() );
}

IlLTS::StateIterator
IlLTS::endStates()
{
  return( endNodes() );
}

long
IlLTS::nofStates() const
{
  return( nofNodes() );
}

IlLTS::TransitionIterator
IlLTS::beginTransitions()
{
  return( beginEdges() );
}

IlLTS::TransitionIterator
IlLTS::endTransitions()
{
  return( endEdges() );
}

long
IlLTS::nofTransitions() const
{
  return( nofEdges() );
}

IlLTS::ActionIterator
IlLTS::beginAlphabet()
{
  return( d_alphabet.begin());
}

IlLTS::ActionIterator
IlLTS::endAlphabet()
{
  return( d_alphabet.end());
}

const string &
IlLTS::nameOfAction(action_t action)
{
  ActionIterator place = d_alphabet.find(action);
  if( d_alphabet.end() == place )
    {
      // Action was not found, return tau
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      return( (*d_alphabet.find(0)).second );
    }
  return( (*place).second );
}


IlLTS::IlLTS()
  :  Ancestor()
{
  d_alphabet.insert(make_pair((action_t)0,string("tau")));
  // d_alphabet[0]="tau";
}

IlLTS::~IlLTS()
{
}


bool
IlLTS::initializeFromFile(InStream& lsts_file, const string& comment)
{
  iLSTS_File lsts( lsts_file );
  lsts.AddActionNamesReader( *this );
  lsts.AddTransitionsReader( *this );
  //spcontainer.separateOTFVI();
  lsts.AddStatePropsReader(spcontainer);
  lsts.SetNoReaderAction( iLSTS_File::WARN );

  initializeDataStructures( lsts.GiveHeader(), comment );

  lsts.ReadFile();
  return true;
}

void IlLTS::initializeDataStructures(const Header& header,
                                     const string& comment) {
    LTS_Draw_Write  piirros( cout );

    nodeIndexing(1, header.GiveStateCnt());
    for( lsts_index_t node_name = 1 ;
         node_name < header.GiveStateCnt() ;
         ++node_name ) {
        getNode(node_name).name(node_name);
    }
    edgeIndexing(1, header.GiveTransitionCnt() );
    getNode( header.GiveInitialState()).addFlag(State::INITIAL_STATE);
    if(comment.size())
        piirros.TulostaHistoria(comment);
    piirros.TulostaHistoria
        ( valueToMessage("No of states: " , header.GiveStateCnt(), ""));
    piirros.TulostaHistoria
        ( valueToMessage("  No of arcs: " , header.GiveTransitionCnt(), ""));
}
void IlLTS::lsts_StartActionNames( Header& ) {  }
void IlLTS::lsts_ActionName(lsts_index_t action, const string& name) {
    d_alphabet.insert( make_pair(action,name) );
    }
void IlLTS::lsts_EndActionNames( ) { }

void IlLTS::lsts_StartTransitions( Header& hd)
{
    if(spcontainer.isInitialized())
    {
        vector<bool> cutProps(spcontainer.getMaxStatePropNameNumber()+1);
        vector<bool> stickyProps(spcontainer.getMaxStatePropNameNumber()+1);
        for(unsigned i=1; i<cutProps.size(); ++i)
        {
            cutProps[i] = (spcontainer.getStatePropName(i)[0] == '%');
            stickyProps[i] = (spcontainer.getStatePropName(i)[0] == '/');
        }

        for(unsigned state = 1; state <= hd.GiveStateCnt(); ++state)
        {
            StatePropsContainer::StatePropsPtr spptr =
                spcontainer.getStateProps(state);
            for(unsigned sp = spcontainer.firstProp(spptr); sp;
                sp = spcontainer.nextProp(spptr))
            {
                if(cutProps[sp])
                {
                    getNode(state).addFlag(State::CUTSTATE_FLAG);
                }
                if(stickyProps[sp])
                {
                    getNode(state).addFlag(State::STICKYPROP_FLAG);
                }
            }
        }
    }
}

void IlLTS::lsts_StartTransitionsFromState( lsts_index_t) {}

void IlLTS::lsts_Transition( lsts_index_t start_state,
                      lsts_index_t dest_state,
                      lsts_index_t action ) {
    Transition trans;
    trans.action(action);
    addConnection(getNode(start_state),getNode(dest_state),trans);
    getNode(start_state).addFlag(State::HASTRANSITIONS_FLAG);
}

void IlLTS::lsts_EndTransitionsFromState( lsts_index_t ) {}
void IlLTS::lsts_EndTransitions() {}
