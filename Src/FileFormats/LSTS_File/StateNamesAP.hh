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

// FILE_DES: StateNamesAP.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: StateNamesAP.hh 1.10 Wed, 20 Mar 2002 16:45:34 +0200 timoe $
// 
// StateNamesAP gives services for reading and writing state names.
//

// $Log:$

#ifdef CIRC_STATENAMESAP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATENAMESAP_HH_
#define ONCE_STATENAMESAP_HH_
#define CIRC_STATENAMESAP_HH_

#include "SectionAP.hh"


// Predeclaration:
class Header;


class iStateNamesAP : public iSectionAP
{
 public:
    virtual ~iStateNamesAP();
    
    virtual void lsts_StartStateNames( Header& ) = 0;
    
    virtual void lsts_StateName( lsts_index_t state_number,
                                 const std::string& name ) = 0;
    
    virtual void lsts_EndStateNames() = 0;
    
    typedef class iStateNamesSection section_t;
    typedef class StateNamesChecker checker_t;
};


class oStateNamesAP : public oSectionAP
{
 public:
    virtual ~oStateNamesAP();

    virtual bool lsts_doWeWriteStateNames() = 0;
    virtual void lsts_WriteStateNames( iStateNamesAP& writer ) = 0;

    typedef class oStateNamesSection section_t;
    typedef class StateNamesChecker checker_t;
};


#undef CIRC_STATENAMESAP_HH_
#endif
