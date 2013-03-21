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

#ifndef STATENAMESSECTION_HH
#define STATENAMESSECTION_HH

#include "ActionNamesSection.hh"
#include "LSTS_File/StateNamesAP.hh"


#include <string>


// Class predeclaration:
class Header;


class iStateNamesSection : public iSection
{
 public:
    iStateNamesSection( iStateNamesAP& ap ) : iap( ap ) { } 
    virtual ~iStateNamesSection() { }
    virtual const char* GiveName() const { return "STATE_NAMES"; }

    virtual void ReadSection( Header& );
 private:
    iStateNamesAP& iap;
};


class oStateNamesSection : public oSection, public iStateNamesAP
{
 public:
    oStateNamesSection( oStateNamesAP& oAp ) : oap( &oAp ) { }
    oStateNamesSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oStateNamesSection() { }
    virtual const char* GiveName() const { return "STATE_NAMES"; }

    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );
    
    virtual void lsts_StartStateNames( Header& );
    // Writes the beginning of the section.

    virtual void lsts_StateName( lsts_index_t state_number,
                                 const std::string& state_name );
    // Writes the name of the state.
    
    virtual void lsts_EndStateNames();
    // Writes the end of the section.

 private:
    oStateNamesAP* oap;
};


#endif
