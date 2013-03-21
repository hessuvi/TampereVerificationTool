/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#include "ActionNamesStore.hh"

#include "LSTS_File/Header.hh"
#include "LSTS_File/LSTS_Sections/ActionNamesChecker.hh"


// PUBLIC:

ActionNamesForwarder::ActionNamesForwarder( bool readAlways ) :
    Forwarder( readAlways ), string_map( 0 ), owns_smap( true ) { }
ActionNamesForwarder::ActionNamesForwarder( StringMap<unsigned>& smap ) :
    Forwarder( false ), string_map( &smap ), owns_smap( false ) { }
ActionNamesForwarder::~ActionNamesForwarder()
{ if ( owns_smap ) { delete string_map; } string_map = 0; }

ActionNamesForwarder::ActionNamesForwarder( const ActionNamesForwarder& src ) :
    iActionNamesAP( src ), oActionNamesAP( src ), Forwarder( src ),
    string_map( 0 ) { *this = src; }

ActionNamesForwarder&
ActionNamesForwarder::operator=( const ActionNamesForwarder& src )
{
    if ( &src != this )
    {
        iActionNamesAP::operator=( src );
        oActionNamesAP::operator=( src );
        Forwarder::operator=( src );

        if ( src.string_map )
        {
            if ( !string_map || !owns_smap )
            { string_map = new StringMap<unsigned>; }
            *string_map = *src.string_map;
        }
        else { if ( owns_smap ) { delete string_map; } string_map = 0; }
        actions.clear();

        owns_smap = src.owns_smap;
    }

    return *this;
}

bool
ActionNamesForwarder::addAction( lsts_index_t action_number,
                                 const std::string& action_name )
{
    if ( !string_map )
    { string_map = new StringMap<unsigned>; owns_smap = true; }
    return string_map->add( action_name, action_number );
}

void
ActionNamesForwarder::lsts_ActionNames( StringMap<unsigned>* s_map,
                                        bool receiver_owns )
{ string_map = s_map; owns_smap = receiver_owns; }

lsts_index_t
ActionNamesForwarder::lsts_numberOfActionNamesToWrite()
{
    if ( !string_map ) { return 0; }
    SetIncludeFilename( GiveSectionFilename() ); return string_map->size();
}

void
ActionNamesForwarder::lsts_WriteActionNames( iActionNamesAP& ap )
{ ap.lsts_ActionNames( string_map, false ); }


// PRIVATE:

// Copies contents of the action string map into the vector.
void
ActionNamesForwarder::UpdateVector() const
{
    actions.resize( string_map->size() + 1 );
    for ( StringMap<unsigned>::iterator it( *string_map ); !it.is_at_end();
          ++it ) { actions[it.target_data()] = *it; }
}
