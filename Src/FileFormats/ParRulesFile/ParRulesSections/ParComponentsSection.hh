/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright � 2001 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen, Timo Erkkil�, Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: ParComponentsSection.hh: ParrulesFile
// Nieminen Juha

// $Id: ParComponentsSection.hh 1.3 Wed, 15 Sep 2004 21:48:51 +0300 timoe $
// 
//

// $Log: ParComponentsSection.hh,v $
// Revision 1.3  1999/09/14 14:06:56  warp
// *** empty log message ***
//
// Revision 1.2  1999/09/08 10:53:16  warp
// *** empty log message ***
//
// Revision 1.1  1999/08/12 13:22:18  warp
// Initial revision
//

#ifdef CIRC_PARCOMPONENTSSECTION_HH_
#error "Include recursion"
#endif

#ifndef ONCE_PARCOMPONENTSSECTION_HH_
#define ONCE_PARCOMPONENTSSECTION_HH_
#define CIRC_PARCOMPONENTSSECTION_HH_


#include "FileFormat/Section.hh"
#include "../ParComponentsAP.hh"

#include <string>


// Class predeclaration:
class Header;


class iParComponentsSection : public iSection
{
 public:
    iParComponentsSection( iParComponentsAP& ap ) : iap( ap ) { } 
    virtual ~iParComponentsSection() { }
    virtual const char* GiveName() const { return "PARCOMPONENTS"; }
    virtual void ReadSection( Header& );
 private:
    iParComponentsAP& iap;
};



class oParComponentsSection : public oSection, public iParComponentsAP
{
 public:
    oParComponentsSection( oParComponentsAP& oAp ) : oap( &oAp ) { }
    oParComponentsSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oParComponentsSection() { }
    virtual const char* GiveName() const { return "PARCOMPONENTS"; }
  
    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );

    virtual void lsts_StartParComponents();
    virtual void lsts_Component( unsigned filenameCnt,
                                 const std::string& filename,
                                 const std::string& prefix );
    virtual void lsts_EndParComponents();

 private:
    oParComponentsAP* oap;
};


#undef CIRC_PARCOMPONENTSSECTION_HH_
#endif

