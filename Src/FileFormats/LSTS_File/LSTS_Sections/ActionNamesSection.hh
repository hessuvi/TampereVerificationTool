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

// FILE_DES: ActionNamesSection.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: ActionNamesSection.hh 1.19 Mon, 25 Oct 2004 01:24:25 +0300 timoe $
//
// ActionNamesSection gives services for reading or writing action names.

// $Log:$

#ifndef ACTIONNAMESSECTION_HH
#define ACTIONNAMESSECTION_HH

#include "FileFormat/Section.hh"
#include "LSTS_File/ActionNamesAP.hh"

#include <string>


class iActionNamesSection : public iSection
{
 public:
    iActionNamesSection( iActionNamesAP& ap ) : iap( ap ) { } 
    virtual ~iActionNamesSection() { }
    virtual const char* GiveName() const { return "ACTION_NAMES"; }

    virtual void ReadSection( Header& );
 private:
    iActionNamesAP& iap;
};


class oActionNamesSection : public oSection, public iActionNamesAP
{
 public:
    oActionNamesSection( oActionNamesAP& oAp ) : oap( &oAp ) { }
    oActionNamesSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oActionNamesSection() { }
    virtual const char* GiveName() const { return "ACTION_NAMES"; }
  
    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );
    
    virtual void lsts_StartActionNames( Header& );
    // Writes the beginning of the section.

    virtual void lsts_ActionName( lsts_index_t action_number,
                                  const std::string& action_name );
    // Writes the name of the action.
    
    virtual void lsts_EndActionNames();
    // Writes the end of the section.
    
 private:
    oActionNamesAP* oap;
};


#endif
