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

// FILE_DES: DivBitsStore.hh: Src/FileFormats/LSTS_File
// Timo Erkkilä

// $Id: DivBitsStore.hh 1.9 Tue, 08 Oct 2002 02:18:05 +0300 timoe $
// 
// This class is a store for Div_bits section. It can be used to store
//   state numbers with divergence bit on and to later write them out.
//

// $Log:$

#ifdef CIRC_DIVBITSSTORE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_DIVBITSSTORE_HH_
#define ONCE_DIVBITSSTORE_HH_
#define CIRC_DIVBITSSTORE_HH_


#include "DivBitsAP.hh"
#include "renumber.hh"

#include <vector>


class DivBitsStore : public iDivBitsAP, public oDivBitsAP
{
 public:
    DivBitsStore();

    virtual bool lsts_doWeWriteDivBits();
    virtual void lsts_WriteDivBits( iDivBitsAP& ap );
    
    virtual void lsts_StartDivBits( Header& );
    virtual void lsts_DivBit( lsts_index_t state );
    virtual void lsts_EndDivBits();

    bool divBitOn( lsts_index_t state ) const;
    
    lsts_index_t RemoveDuplicateDivBits();
    lsts_index_t RenumberDivBits( Renumber<lsts_index_t>& );

 private:

    // No copy constructor nor assigment operation in use:
    DivBitsStore ( const DivBitsStore& );
    DivBitsStore& operator=( const DivBitsStore& );

    std::vector<lsts_index_t> states;
};


#undef CIRC_DIVBITSSTORE_HH_
#endif

