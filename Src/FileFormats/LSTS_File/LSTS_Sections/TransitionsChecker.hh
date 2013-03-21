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

// TransitionsChecker gives services for checking transitions.

#ifndef TRANSITIONSCHECKER_HH
#define TRANSITIONSCHECKER_HH

#include "FileFormat/Checker.hh"
#include "LSTS_File/TransitionsAP.hh"

#include <vector>

// Predeclarations:
class Header;
class FileFormat;
class TokenStream;


class TransitionsChecker : public Checker, public iTransitionsAP
{
 public:
    TransitionsChecker( iTransitionsAP&, FileFormat& );
    virtual ~TransitionsChecker();

    virtual void lsts_StartTransitions( Header& );
    virtual void lsts_StartTransitionsFromState( lsts_index_t start_state );
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action );
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

    virtual void lsts_EndTransitionsFromState( lsts_index_t start_state );
    virtual void lsts_EndTransitions();

#ifdef RATE
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action,
                                  lsts_float_t decimal_number );
#endif

 private:
    // Preventing the use of copy constructor and assignment operator:
    TransitionsChecker( const TransitionsChecker& );
    TransitionsChecker& operator=( const TransitionsChecker& );

    void CheckTransition(  lsts_index_t start_state,
                           lsts_index_t dest_state,
                           lsts_index_t action );

    lsts_index_t action_cnt;
    lsts_index_t state_cnt;
    lsts_index_t transition_cnt;

    lsts_index_t amount;
    std::vector<bool> start_states_checked;

    iTransitionsAP& iap;
};


#endif
