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

// FILE_DES: StateNamesStore.hh: Src/FileFormats/LSTS_File
// Timo Erkkilä

// $Id: StateNamesStore.hh 1.10 Fri, 20 Sep 2002 20:04:50 +0300 timoe $
// 
// This class is a store for State_names section. It can be used to store
//   read state names and to later write them out.
//

// $Log:$

#ifdef CIRC_STATENAMESSTORE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATENAMESSTORE_HH_
#define ONCE_STATENAMESSTORE_HH_
#define CIRC_STATENAMESSTORE_HH_

#include "StateNamesAP.hh"
#include "renumber.hh"

#include <string>
#include <vector>


class StateNamesStore : public iStateNamesAP, public oStateNamesAP
{
 public:
    StateNamesStore();

    virtual bool lsts_doWeWriteStateNames();
    virtual void lsts_WriteStateNames( iStateNamesAP& ap );
    
    virtual void lsts_StartStateNames( Header& );
    virtual void lsts_StateName( lsts_index_t state_number,
                                 const std::string& state_name );
    virtual void lsts_EndStateNames();

    // Refining features:
    lsts_index_t RenumberStateNames( Renumber<lsts_index_t>& renum );

 private:
    // No copy constructor nor assigment operation in use:
    StateNamesStore (const StateNamesStore&);
    StateNamesStore& operator=(const StateNamesStore&);

    lsts_index_t state_cnt;

    vector<lsts_index_t> state_numbers;
    vector<std::string> state_names;
};


#undef CIRC_STATENAMESSTORE_HH_
#endif
