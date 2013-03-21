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

// FILE_DES: AccSetsAP.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: AccSetsAP.hh 1.4.2.5 Wed, 20 Mar 2002 16:45:34 +0200 timoe $
// 
// AccSetsAP gives services for reading and writing acceptance sets.
//

// $Log:$

#ifdef CIRC_ACCSETSAP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_ACCSETSAP_HH_
#define ONCE_ACCSETSAP_HH_
#define CIRC_ACCSETSAP_HH_

#include "SectionAP.hh"


// Predeclaration:
class Header;


class iAccSetsAP : public iSectionAP
{
 public:
    virtual ~iAccSetsAP();

    virtual void lsts_StartAccSets( Header& ) = 0;
    
    virtual void lsts_StartAccSetsOfState( lsts_index_t state ) = 0;
    virtual void lsts_StartSingleAccSet( lsts_index_t state ) = 0;

    virtual void lsts_AccSetAction( lsts_index_t state,
                                    lsts_index_t action ) = 0;
    virtual void lsts_AccSetActions( lsts_index_t state,
                                     lsts_index_t subrange_start,
                                     lsts_index_t subrange_end );

    virtual void lsts_EndSingleAccSet( lsts_index_t state ) = 0;
    virtual void lsts_EndAccSetsOfState( lsts_index_t state ) = 0;

    virtual void lsts_EndAccSets() = 0;


    typedef class iAccSetsSection section_t;
    typedef class AccSetsChecker checker_t;
};



class oAccSetsAP : public oSectionAP
{
 public:
    virtual ~oAccSetsAP();

    virtual bool lsts_doWeWriteAccSets() = 0;
    virtual void lsts_WriteAccSets( iAccSetsAP& writer ) = 0;

    virtual bool lsts_isIntervalSearchOn() const;

    typedef class oAccSetsSection section_t;
    typedef class AccSetsChecker checker_t;
};


#undef CIRC_ACCSETSAP_HH_
#endif
