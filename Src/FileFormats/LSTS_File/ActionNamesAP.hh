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

#ifndef ACTION_NAMES_AP_HH
#define ACTION_NAMES_AP_HH

#include "SectionAP.hh"

#include "StringTree/StringMap.hh"

#include <string>


class iActionNamesAP : public iSectionAP
{
 public:
    virtual ~iActionNamesAP();

    virtual void lsts_StartActionNames( class Header& );
    virtual void lsts_ActionName( lsts_index_t action_number,
                                  const std::string& action_name );
    // An optional way to pass action names:
    virtual void lsts_ActionNames( StringMap<unsigned>* s_map,
                                   bool receiver_deletes_s_map );
    virtual void lsts_EndActionNames();

    typedef class iActionNamesSection section_t;
    typedef class ActionNamesChecker checker_t;
};


class oActionNamesAP : public oSectionAP
{
 public:
    virtual ~oActionNamesAP();

    virtual lsts_index_t lsts_numberOfActionNamesToWrite() = 0;
    // Action_cnt greater than zero must be returned here in order
    //   to the section to be written.
    // Return value zero means that the section will not be written (doesn't
    //   mark action_cnt to zero, however).

    virtual void lsts_WriteActionNames( iActionNamesAP& writer ) = 0;
    // Gives control to the user.
  
    typedef class oActionNamesSection section_t;
    typedef class ActionNamesChecker checker_t;
};


#endif
