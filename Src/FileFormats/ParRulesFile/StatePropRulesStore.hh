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

// FILE_DES: StatePropRulesStore.hh: Src/FileFormats/LSTS_File
// Timo Erkkilä

// $Id: StatePropRulesStore.hh 1.2 Tue, 21 Jan 2003 01:45:54 +0200 timoe $
// 
// This class is a store for State_prop_rules section. It can be used
// to store read action names and to later write them out.
//

// $Log:$

#ifndef STATEPROPRULESSTORE_HH
#define STATEPROPRULESSTORE_HH

#include "StatePropRulesAP.hh"
#include "FileFormat/Forwarder.hh"

#include <string>
#include <vector>


class StatePropRulesStore : public iStatePropRulesAP,
                            public oStatePropRulesAP,
                            public Forwarder
{
 public:
    StatePropRulesStore();
    // ~StatePropRulesStore();
    // StatePropRulesStore( const StatePropRulesStore& );
    // StatePropRulesStore& operator=( const StatePropRulesStore& );

    virtual bool lsts_doWeWriteStatePropRules();
    virtual void lsts_WriteStatePropRules( iStatePropRulesAP& ap );
    
    virtual void lsts_StartStatePropRules( class Header& );
    virtual void lsts_StatePropRule( const std::string& rule );
    virtual void lsts_EndStatePropRules();

 private:
    std::vector<std::string> rules;
};


#endif
