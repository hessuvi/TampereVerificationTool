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

// TransitionsAP gives services for reading and writing action names.

#ifndef TRANSITIONSAP_HH
#define TRANSITIONSAP_HH

#include "SectionAP.hh"
#include <string>


class iTransitionsAP : public iSectionAP
{
 public:
    iTransitionsAP() : ignore_layout_info( true ) { }
    virtual ~iTransitionsAP();
    
    virtual void lsts_StartTransitions( class Header& ) = 0;
    
    virtual void lsts_StartTransitionsFromState( lsts_index_t
                                                 start_state ) = 0;
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action ) = 0;

    virtual void lsts_TransitionWithExtraData
    ( lsts_index_t start_state, lsts_index_t dest_state,
      lsts_index_t action, const std::string& extra_data );

    virtual void lsts_TransitionWithLayoutInfo
    ( lsts_index_t start_state, lsts_index_t dest_state,
      lsts_index_t action, const std::string& layout_info );
    
    virtual void lsts_TransitionWithExtraDataAndLayoutInfo
    ( lsts_index_t start_state, lsts_index_t dest_state,
      lsts_index_t action, const std::string& extra_data,
      const std::string& layout_info );

    void IgnoreExtraData() { ignore_layout_info = false; }
    void IgnoreLayoutInfo() { ignore_layout_info = true; }

#ifdef RATE
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action,
                                  lsts_float_t decimal_number );
#endif

    virtual void lsts_EndTransitionsFromState( lsts_index_t
                                               start_state ) = 0;
    virtual void lsts_EndTransitions() = 0;

    typedef class iTransitionsSection section_t;
    typedef class TransitionsChecker checker_t;

 private:
    bool ignore_layout_info;
};



class oTransitionsAP : public oSectionAP
{
 public:
    virtual ~oTransitionsAP();
    
    virtual lsts_index_t lsts_numberOfTransitionsToWrite() = 0;
    // The user must tell the amount of transitions she/he is going
    //   to write (zero means they aren't written at all).

    virtual void lsts_WriteTransitions( iTransitionsAP& pipe ) = 0;
    // Gives control to the user. For writing data, the user can call
    //   the following methods in pipe:

    typedef class oTransitionsSection section_t;
    typedef class TransitionsChecker checker_t;
};


#endif
