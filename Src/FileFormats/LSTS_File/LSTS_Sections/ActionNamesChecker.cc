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

#include "ActionNamesChecker.hh"
#include "LSTS_File/ActionNamesStore.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/FileFormat.hh"


// PUBLIC:
ActionNamesChecker::ActionNamesChecker( iActionNamesAP& ap, FileFormat& ff ) :
    Checker( ff ), string_map( 0 ), action_cnt( 0 ), AP( ap ) { }
ActionNamesChecker::~ActionNamesChecker() { delete string_map; string_map = 0; }

void
ActionNamesChecker::lsts_StartActionNames( Header& hd )
{
    if ( !string_map ) { string_map = new StringMap<unsigned>; }

    /*
        fw = dynamic_cast<ActionNamesForwarder*>( &AP );
        if ( fw )
        {
            fw->SetChecked();
            if ( !fw->receiveStringMap( *string_map ) ) { fw = 0; }
        }
    */

    check_isGiven( "Action_cnt", hd.isActionCntGiven() );

    action_cnt = hd.GiveActionCnt();
    checked_actions.resize( action_cnt + 1, false );
    AP.lsts_StartActionNames( hd );
}

void
ActionNamesChecker::lsts_ActionName( lsts_index_t number,
                                     const std::string& name )
{
    // The empty string as a name?
    check_claim( name.size(), ts().errorMsg( "action number ", number,
                                    " is named with the empty string" ) ); 
    CheckAction( number, name );
    // Duplicate action name?
    check_claim( string_map->add( name, number ),
                 ts().errorMsg( name, "duplicate action name" ) );
}

void
ActionNamesChecker::lsts_ActionNames( StringMap<unsigned>* s_map,
                                      bool receiver_owns )
{
    action_cnt = s_map->size();
    for ( StringMap<unsigned>::iterator it( *s_map ); !it.is_at_end();
          ++it ) { CheckAction( it.target_data(), *it ); }    
    AP.lsts_ActionNames( s_map, receiver_owns );
}

void
ActionNamesChecker::lsts_EndActionNames()
{
    check_amount( string_map->size(), action_cnt,
                  "action names", "Action_cnt" );
    AP.lsts_ActionNames( string_map, true ); string_map = 0;
    AP.lsts_EndActionNames();
}


// PRIVATE:
void
ActionNamesChecker::CheckAction( lsts_index_t number,
                                 const std::string& name )
{
    // Tau?
    check_claim( number, ts().errorMsg
                 ( name, "action number zero (tau) cannot be named" ) );
    // Bad number?
    check_visible_action( number, action_cnt, "visible action" );

    // Duplicate action number?
    check_claim( !checked_actions[number],
                 ts().errorMsg( "action ", number, " is named twice" ) );
    checked_actions[number] = true;
}
