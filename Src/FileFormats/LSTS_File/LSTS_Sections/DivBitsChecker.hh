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

// FILE_DES: DivBitsChecker.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: DivBitsChecker.hh 1.7 Fri, 08 Feb 2002 14:14:18 +0200 timoe $
//
// DivBitsChecker gives services for checking div bits.

// $Log:$

#ifdef CIRC_DIVBITSCHECKER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_DIVBITSCHECKER_HH_
#define ONCE_DIVBITSCHECKER_HH_
#define CIRC_DIVBITSCHECKER_HH_

#include "FileFormat/Checker.hh"
#include "LSTS_File/DivBitsAP.hh"


// Predeclarations:
class Header;
class FileFormat;
class TokenStream;


class DivBitsChecker : public iDivBitsAP, public Checker
{
 public:
    
    DivBitsChecker( iDivBitsAP&, FileFormat& );
    virtual ~DivBitsChecker();

 private:

    // Preventing the use of copy constructor and assignment operator:
    DivBitsChecker( const DivBitsChecker& );
    DivBitsChecker& operator=( const DivBitsChecker& );

    virtual void lsts_StartDivBits( Header& );
    virtual void lsts_DivBit( lsts_index_t state );
    virtual void lsts_DivBits( lsts_index_t interv_start,
                               lsts_index_t interv_end );
    virtual void lsts_EndDivBits();

    lsts_index_t state_cnt;

    iDivBitsAP& AP;
};


#undef CIRC_DIVBITSCHECKER_HH_
#endif
