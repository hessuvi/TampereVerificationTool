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

// FILE_DES: StatePropsChecker.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: StatePropsChecker.hh 1.13 Fri, 20 Sep 2002 20:04:50 +0300 timoe $
//
// StatePropsChecker gives services for checking state props.

// $Log:$

#ifdef CIRC_STATEPROPSCHECKER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATEPROPSCHECKER_HH_
#define ONCE_STATEPROPSCHECKER_HH_
#define CIRC_STATEPROPSCHECKER_HH_

#include "FileFormat/Checker.hh"
#include "LSTS_File/StatePropsAP.hh"
#include "StringTree/StringSet.hh"

#include <string>


// Predeclarations:
class Header;
class FileFormat;
class TokenStream;


class StatePropsChecker : public iStatePropsAP, public Checker
{
 public:
    
    StatePropsChecker( iStatePropsAP&, FileFormat& );
    virtual ~StatePropsChecker();

 private:

    // Preventing the use of copy constructor and assignment operator:
    StatePropsChecker( const StatePropsChecker& );
    StatePropsChecker& operator=( const StatePropsChecker& );


    virtual void lsts_StartStateProps( Header& );

    virtual void lsts_StartPropStates( const std::string& prop_name );

    virtual void lsts_PropState( lsts_index_t state );
    virtual void lsts_PropStates( lsts_index_t subrange_start,
                                  lsts_index_t subrange_end );

    virtual void lsts_EndPropStates( const std::string& prop_name );
    
    virtual void lsts_EndStateProps();


    void checkBeginProp( const std::string& name,
                         const std::string& m_name );
    void checkEndProp( const std::string& name,
                       const std::string& m_name );

    virtual void lsts_CutState( lsts_index_t state );
    virtual void lsts_CutStates( lsts_index_t subrange_start,
                                 lsts_index_t subrange_end );

    virtual void lsts_RejectState( lsts_index_t state );
    virtual void lsts_RejectStates( lsts_index_t subrange_start,
                                    lsts_index_t subrange_end );

    virtual void lsts_DeadlockRejectState( lsts_index_t state );
    virtual void lsts_DeadlockRejectStates( lsts_index_t subrange_start,
                                            lsts_index_t subrange_end );

    virtual void lsts_LivelockRejectState( lsts_index_t state );
    virtual void lsts_LivelockRejectStates( lsts_index_t subrange_start,
                                            lsts_index_t subrange_end );

    virtual void lsts_InfinityRejectState( lsts_index_t state );
    virtual void lsts_InfinityRejectStates( lsts_index_t subrange_start,
                                            lsts_index_t subrange_end );

    virtual void CheckOutSpecialProps( SP::StatePropType,
                                       const std::string& );

    void AddActualNumber();

    lsts_index_t state_cnt;
    lsts_index_t state_prop_cnt;
    lsts_index_t actual_number;

    unsigned cut_states_passed;
    unsigned rej_states_passed;
    unsigned dl_rej_states_passed;
    unsigned ll_rej_states_passed;
    unsigned inf_rej_states_passed;

    std::string curr_prop;

    struct MyBool
    {
        MyBool() : value( false ) { }
        bool value;
    };

    std::vector<MyBool> special_props_passed;

    iStatePropsAP& AP;

    StringSet* string_set;
};


#undef CIRC_STATEPROPSCHECKER_HH_
#endif
