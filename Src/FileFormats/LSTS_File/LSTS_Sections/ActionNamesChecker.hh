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

#ifndef ACTIONNAMESCHECKER_HH
#define ACTIONNAMESCHECKER_HH

#include "FileFormat/Checker.hh"
#include "LSTS_File/ActionNamesAP.hh"

#include "StringTree/StringMap.hh"

#include <vector>


class ActionNamesChecker : public iActionNamesAP, public Checker
{
 public:
    ActionNamesChecker( iActionNamesAP&, class FileFormat& );
    virtual ~ActionNamesChecker();

    virtual void lsts_StartActionNames( class Header& );
    virtual void lsts_ActionName( lsts_index_t number,
                                  const std::string& name );
    virtual void lsts_ActionNames( StringMap<unsigned>* s_map,
                                   bool receiver_owns );
    virtual void lsts_EndActionNames();

 private:
    // Preventing use of copy constructor and assignment operator:
    ActionNamesChecker( const ActionNamesChecker& );
    ActionNamesChecker& operator=( const ActionNamesChecker& );

    void CheckAction( lsts_index_t number, const std::string& name );

    StringMap<unsigned>* string_map;
    std::vector<bool> checked_actions;
    lsts_index_t action_cnt;
    iActionNamesAP& AP;
};


#endif
