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

#include "LayoutChecker.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/FileFormat.hh"


LayoutChecker::~LayoutChecker() { }


// PRIVATE:
void
LayoutChecker::lsts_StartLayout( Header& hd )
{
    check_isGiven( "State_cnt", hd.isStateCntGiven() );
    state_cnt = hd.GiveStateCnt();
    ap.lsts_StartLayout( hd );
}

void
LayoutChecker::lsts_EndLayout() { ap.lsts_EndLayout(); }

void
LayoutChecker::lsts_StateLayout( lsts_index_t state_n, int x, int y )
{
    check_state( state_n, state_cnt, "state number" );
    ap.lsts_StateLayout( state_n, x, y );
}
