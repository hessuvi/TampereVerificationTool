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

// FILE_DES: StatePropRulesStore.cc: Src/FileFormats/LSTS_File
// Timo Erkkilä

static const char * const ModuleVersion=
"Module version: $Id: StatePropRulesStore.cc 1.2 Tue, 21 Jan 2003 01:45:54 +0200 timoe $";

// $Log:$


#include "StatePropRulesStore.hh"
#include "error_handling.hh"
#include "LSTS_File/Header.hh"

#include <string>
using std::string;


StatePropRulesStore::StatePropRulesStore() : Forwarder( true ) { }

bool
StatePropRulesStore::lsts_doWeWriteStatePropRules() { return rules.size(); }

void
StatePropRulesStore::lsts_WriteStatePropRules( iStatePropRulesAP& ap )
{
    for ( std::vector<string*>::size_type i = 0; i < rules.size(); ++i )
    { ap.lsts_StatePropRule( rules[ i ] ); }
}

void
StatePropRulesStore::lsts_StartStatePropRules( Header& ) { rules.clear(); }

void
StatePropRulesStore::lsts_StatePropRule( const string& rule )
{ rules.push_back( rule ); }

void
StatePropRulesStore::lsts_EndStatePropRules() { }
