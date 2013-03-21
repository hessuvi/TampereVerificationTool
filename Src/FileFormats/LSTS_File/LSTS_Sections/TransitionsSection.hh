/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#ifndef TRANSITIONSSECTION_HH
#define TRANSITIONSSECTION_HH

#include "FileFormat/Section.hh"
#include "LSTS_File/TransitionsAP.hh"


class iTransitionsSection : public iSection
{
 public:
    iTransitionsSection( iTransitionsAP& ap ) : iap( ap ) { }
    virtual ~iTransitionsSection() { }
    virtual const char* GiveName() const { return "TRANSITIONS"; }
    virtual void ReadSection( Header& );
 private:
    iTransitionsAP& iap;
};


class oTransitionsSection : public oSection, public iTransitionsAP
{
 public:
    oTransitionsSection( oTransitionsAP& oAp ) : oap( &oAp ) { }
    oTransitionsSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oTransitionsSection() { }
    virtual const char* GiveName() const { return "TRANSITIONS"; }

    virtual oTransitionsAP* GiveoAP() { return oap; }
    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );
    
    virtual void lsts_StartTransitions( Header& );
    // Writes the beginning of the section.
    
    virtual void lsts_StartTransitionsFromState( lsts_index_t start_state );
    // Writes a number of start state for transitions to follow.
    
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action );
    // Writes a transition.

    virtual void lsts_TransitionWithExtraData
    ( lsts_index_t start_state, lsts_index_t dest_state,
      lsts_index_t action, const std::string& extra_data );
    // Writes a transition with extra data.
    virtual void lsts_TransitionWithLayoutInfo
    ( lsts_index_t start_state, lsts_index_t dest_state,
      lsts_index_t action, const std::string& layout_info );
    virtual void lsts_TransitionWithExtraDataAndLayoutInfo
    ( lsts_index_t start_state, lsts_index_t dest_state,
      lsts_index_t action, const std::string& extra_data,
      const std::string& layout_info );

    virtual void lsts_EndTransitionsFromState( lsts_index_t start_state );
    // This method closes the sequence of transitions starting from
    //   start state.
    
    virtual void lsts_EndTransitions();
    // Writes the end of the section.

#ifdef RATE
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action,
                                  lsts_float_t decimal_number );
#endif

 private:
    oTransitionsAP* oap;
};


#endif
