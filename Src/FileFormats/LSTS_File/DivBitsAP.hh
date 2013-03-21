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

// FILE_DES: DivBitsAP.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: DivBitsAP.hh 1.12 Wed, 20 Mar 2002 16:45:34 +0200 timoe $
// 
// DivBitsAP gives services for reading and writing divergence bits.
//

// $Log:$

#ifdef CIRC_DIVBITSAP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_DIVBITSAP_HH_
#define ONCE_DIVBITSAP_HH_
#define CIRC_DIVBITSAP_HH_

#include "config.hh"
#include "SectionAP.hh"


// Predeclaration:
class Header;


class iDivBitsAP : public iSectionAP
{
 public:
    virtual ~iDivBitsAP();

    virtual void lsts_StartDivBits( Header& ) = 0;
    
    virtual void lsts_DivBit( lsts_index_t state_number ) = 0;
    virtual void lsts_DivBits( lsts_index_t subrange_start,
                               lsts_index_t subrange_end );
    
    virtual void lsts_EndDivBits() = 0;

    typedef class iDivBitsSection section_t;
    typedef class DivBitsChecker checker_t;
};



class oDivBitsAP : public oSectionAP
{
 public:
    virtual ~oDivBitsAP();

    virtual bool lsts_doWeWriteDivBits() = 0;
    
    virtual void lsts_WriteDivBits( iDivBitsAP& writer ) = 0;

    virtual bool lsts_isIntervalSearchOn() const;
        
    typedef class oDivBitsSection section_t;
    typedef class DivBitsChecker checker_t;
};


#undef CIRC_DIVBITSAP_HH_
#endif
