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

// FILE_DES: ParComponentsSection_AP.hh: ParrulesFile
// Nieminen Juha

// $Id: ParComponentsAP.hh 1.1 Thu, 19 Dec 2002 02:14:34 +0200 timoe $
// 

// $Log: ParComponentsSection_AP.hh,v $
// Revision 1.2  1999/09/08 10:53:16  warp
// *** empty log message ***
//
// Revision 1.1  1999/08/12 13:22:18  warp
// Initial revision
//

#ifdef CIRC_PARCOMPONENTSAP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_PARCOMPONENTSAP_HH_
#define ONCE_PARCOMPONENTSAP_HH_
#define CIRC_PARCOMPONENTSAP_HH_

#include "../LSTS_File/SectionAP.hh"

#include <string>


// Luokan esittely:

class iParComponentsAP : public iSectionAP
{
 public:
    virtual ~iParComponentsAP();

    virtual void lsts_StartParComponents() = 0;
    virtual void lsts_Component( unsigned filenameCnt,
                                 const std::string& filename,
                                 const std::string& prefix ) = 0;
    virtual void lsts_EndParComponents() = 0;

    typedef class iParComponentsSection section_t;
    typedef class ParComponentsChecker checker_t;
};


class oParComponentsAP : public oSectionAP
{
 public:
    virtual ~oParComponentsAP();

    virtual bool lsts_doWeWriteParComponents() = 0;
    virtual void lsts_WriteParComponents( iParComponentsAP& writer ) = 0;

    typedef class oParComponentsSection section_t;
    typedef class ParComponentsChecker checker_t;
};


#undef CIRC_PARCOMPONENTSAP_HH_
#endif

