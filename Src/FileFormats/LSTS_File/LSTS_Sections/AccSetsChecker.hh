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

// FILE_DES: AccSetsChecker.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: AccSetsChecker.hh 1.9 Fri, 08 Feb 2002 14:14:18 +0200 timoe $
//
// AccSetsChecker gives services for checking acc sets.

// $Log:$

#ifdef CIRC_ACCSETSCHECKER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_ACCSETSCHECKER_HH_
#define ONCE_ACCSETSCHECKER_HH_
#define CIRC_ACCSETSCHECKER_HH_

#include "FileFormat/Checker.hh"
#include "LSTS_File/AccSetsAP.hh"

#include <string>


// Predeclarations:
class Header;
class FileFormat;
class TokenStream;


class AccSetsChecker : public iAccSetsAP, public Checker
{
 public:
    
    AccSetsChecker( iAccSetsAP&, FileFormat& );
    virtual ~AccSetsChecker();
    
    iAccSetsAP& GiveiAP() const { return AP; }

 private:

    // Preventing the use of copy constructor and assignment operator:
    AccSetsChecker( const AccSetsChecker& );
    AccSetsChecker& operator=( const AccSetsChecker& );

    virtual void lsts_StartAccSets( Header& );
    
    virtual void lsts_StartAccSetsOfState( lsts_index_t state );
    virtual void lsts_StartSingleAccSet( lsts_index_t state );
    
    virtual void lsts_AccSetAction( lsts_index_t state, lsts_index_t action );
    virtual void lsts_AccSetActions( lsts_index_t state,
                                     lsts_index_t interv_start,
                                     lsts_index_t interv_end );
    
    virtual void lsts_EndSingleAccSet( lsts_index_t state );
    virtual void lsts_EndAccSetsOfState( lsts_index_t state );

    virtual void lsts_EndAccSets();


    void isStateSame( lsts_index_t givenState, lsts_index_t expState,
                      const std::string& methodName );

    lsts_index_t state_cnt;
    lsts_index_t action_cnt;
    lsts_index_t exp_state;

    iAccSetsAP& AP;
};


#undef CIRC_ACCSETSCHECKER_HH_
#endif
