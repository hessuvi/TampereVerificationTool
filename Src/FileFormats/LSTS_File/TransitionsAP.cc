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

#include "TransitionsAP.hh"


iTransitionsAP::~iTransitionsAP() { }

void
iTransitionsAP::lsts_TransitionWithExtraData
( lsts_index_t start_state, lsts_index_t dest_state,
  lsts_index_t action, const std::string& )
{ lsts_Transition( start_state, dest_state, action ); }

void
iTransitionsAP::lsts_TransitionWithLayoutInfo
( lsts_index_t start_state, lsts_index_t dest_state,
  lsts_index_t action, const std::string& )
{ lsts_Transition( start_state, dest_state, action ); }

void
iTransitionsAP::lsts_TransitionWithExtraDataAndLayoutInfo
( lsts_index_t start_state, lsts_index_t dest_state,
  lsts_index_t action, const std::string& extra_data,
  const std::string& layout_info )
{
    if ( ignore_layout_info )
    {
        lsts_TransitionWithExtraData( start_state, dest_state, action,
                                      extra_data );
        return;
    }

    lsts_TransitionWithLayoutInfo( start_state, dest_state, action,
                                   layout_info );
}

#ifdef RATE
void
iTransitionsAP::lsts_Transition( lsts_index_t, lsts_index_t, lsts_index_t,
                                 lsts_float_t ) { }
#endif


oTransitionsAP::~oTransitionsAP() { }
