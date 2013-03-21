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

Contributor(s): Juha Nieminen, Timo Erkkilä, Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: AccSetsAP.cc: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: AccSetsAP.cc 1.3.2.2 Wed, 20 Mar 2002 16:45:34 +0200 timoe $";

// $Log:$

#include "AccSetsAP.hh"


iAccSetsAP::~iAccSetsAP() { }

void
iAccSetsAP::lsts_AccSetActions( lsts_index_t state,
                              lsts_index_t subrange_start,
                              lsts_index_t subrange_end )
{
    while ( subrange_start <= subrange_end )
    {
        lsts_AccSetAction( state, subrange_start );
        ++subrange_start;
    }
}



oAccSetsAP::~oAccSetsAP() { }

bool
oAccSetsAP::lsts_isIntervalSearchOn() const { return true; }
