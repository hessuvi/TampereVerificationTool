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

#include "ActionNamesAP.hh"

#include <iostream>

// iActionNamesAP:
iActionNamesAP::~iActionNamesAP() { }

void
iActionNamesAP::lsts_StartActionNames( class Header& ) { }
void
iActionNamesAP::lsts_ActionName( lsts_index_t, const std::string& ) { }
void
iActionNamesAP::lsts_ActionNames( StringMap<unsigned>* s_map,
                                  bool receiver_deletes )
{
    for ( StringMap<unsigned>::iterator it( *s_map ); !it.is_at_end();
          ++it ) { lsts_ActionName( it.target_data(), *it ); }
    if ( receiver_deletes ) { delete s_map; } s_map = 0;
}
void
iActionNamesAP::lsts_EndActionNames() { }


// oActionNamesAP:
oActionNamesAP::~oActionNamesAP() { }
