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

#ifndef ACTIONNAMESFORWARDER_HH
#define ACTIONNAMESFORWARDER_HH

#include "LSTS_File/ActionNamesAP.hh"
#include "FileFormat/Forwarder.hh"
#include "StringTree/StringMap.hh"

#include <vector>
#include <string>


class ActionNamesForwarder : public iActionNamesAP, public oActionNamesAP,
                             public Forwarder
{
 public:
    ActionNamesForwarder( bool readSectionAlways = true );
    // Get a reference to StringMap, used for writing:
    ActionNamesForwarder( StringMap<unsigned>& action_map );
    virtual ~ActionNamesForwarder();

    ActionNamesForwarder( const ActionNamesForwarder& );
    ActionNamesForwarder& operator=( const ActionNamesForwarder& );

    bool addAction( lsts_index_t action_number,
                    const std::string& action_name );
    unsigned size() const
    { if ( !string_map ) { return 0; } return string_map->size(); }
    void clear() { if ( owns_smap ) { delete string_map; } string_map = 0;
                   actions.clear(); }

    const std::string& GiveActionName( lsts_index_t action_number ) const
    { if ( action_number >= actions.size() ) { UpdateVector(); }
      return actions[action_number]; }
    
    bool findActionNumber( const std::string& action_name,
                           unsigned& action_number ) const
    { if ( !string_map ) { return false; }
      return string_map->find( action_name, action_number ); }

 private:
    virtual void lsts_ActionNames( StringMap<unsigned>* s_map,
                                   bool reveiver_owns );
    virtual lsts_index_t lsts_numberOfActionNamesToWrite();
    virtual void lsts_WriteActionNames( iActionNamesAP& );

    void UpdateVector() const;

    mutable StringMap<unsigned>* string_map;
    mutable std::vector<std::string> actions;
    bool owns_smap;
};


typedef ActionNamesForwarder ActionNamesStore;


#endif
