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

// FILE_DES: StateNamesChecker.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: StateNamesChecker.hh 1.9 Fri, 20 Sep 2002 20:04:50 +0300 timoe $
//
// StateNamesChecker gives services for checking state names.

// $Log:$

#ifdef CIRC_STATENAMESCHECKER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATENAMESCHECKER_HH_
#define ONCE_STATENAMESCHECKER_HH_
#define CIRC_STATENAMESCHECKER_HH_

#include "FileFormat/Checker.hh"
#include "LSTS_File/StateNamesAP.hh"
#include "StringTree/StringSet.hh"

#include <vector>

// Predeclarations:
class Header;
class FileFormat;
class TokenStream;
class StringMatcher;


class StateNamesChecker : public iStateNamesAP, public Checker
{
 public:
    StateNamesChecker( iStateNamesAP&, FileFormat& );
    virtual ~StateNamesChecker();
    
 private:
    // Preventing the use of copy constructor and assignment operator:
    StateNamesChecker( const StateNamesChecker& );
    StateNamesChecker& operator=( const StateNamesChecker& );

    virtual void lsts_StartStateNames( Header& );
    
    virtual void lsts_StateName( lsts_index_t state_number,
                                 const std::string& name );
    
    virtual void lsts_EndStateNames();
    
    
    lsts_index_t state_cnt;
    std::vector<bool> state_names_not_given;
    
    iStateNamesAP& AP;
    
    StringSet* string_set;
};


#undef CIRC_STATENAMESCHECKER_HH_
#endif
