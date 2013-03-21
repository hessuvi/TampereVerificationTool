/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// FILE_DES: StateNamesChecker.cc: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: StateNamesChecker.cc 1.12 Sat, 18 Jan 2003 03:19:33 +0200 timoe $";

// $Log:$

#include "StateNamesChecker.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/FileFormat.hh"

#include <string>
using std::string;


// PUBLIC:
#define DELETE_STRING_SET  delete string_set; string_set = 0

#define CREATE_STRING_SET  if( string_set ){ DELETE_STRING_SET; }\
                               string_set = new StringSet

StateNamesChecker::StateNamesChecker( iStateNamesAP& ap,
                                      FileFormat& ff ) :
    
    iStateNamesAP(), Checker( ff ),
    state_cnt( 0 ),

    AP( ap ),
    string_set( 0 )
{ }

StateNamesChecker::~StateNamesChecker() { DELETE_STRING_SET; }


// PRIVATE:

void
StateNamesChecker::lsts_StartStateNames( Header& hd )
{
    CREATE_STRING_SET;

    check_isGiven( "State_cnt", hd.isStateCntGiven() );
    
    state_cnt = hd.GiveStateCnt();
    state_names_not_given.resize( state_cnt + 1, true );
    
    AP.lsts_StartStateNames( hd );
}

void
StateNamesChecker::lsts_EndStateNames()
{
    DELETE_STRING_SET;
    AP.lsts_EndStateNames();
}

void
StateNamesChecker::lsts_StateName( lsts_index_t state, const string& name )
{
    // An empty name string?
    check_claim( name.size(),
                 ts().errorMsg( "state number ", state,
                                " is named with empty string" ) );
    // Bad value?
    check_state( state, state_cnt, "state number" );

    // Duplicate action numbers?
    check_claim( state_names_not_given[ state ],
                 ts().errorMsg( "state ", state, " is named twice" ) );
    state_names_not_given[ state ] = false;

    check_claim( string_set->add( name ),
                 ts().errorMsg( name, "duplicate state name" ) );

    AP.lsts_StateName( state, name );
}
