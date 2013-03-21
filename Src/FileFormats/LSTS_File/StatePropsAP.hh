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

// FILE_DES: StatePropsAP.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: StatePropsAP.hh 1.21 Wed, 20 Mar 2002 16:45:34 +0200 timoe $
// 
// StatePropsAP gives services for reading and writing action names.
//

// $Log:$

#ifdef CIRC_STATEPROPSAP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATEPROPSAP_HH_
#define ONCE_STATEPROPSAP_HH_
#define CIRC_STATEPROPSAP_HH_

#include <string>
#include <vector>

#include "SectionAP.hh"
#include "LSTS_Sections/specialStateProps.hh"


class StateProps
{
 public:
    virtual ~StateProps();
    
    struct lsts_StateProposition
    {
        lsts_StateProposition();
        
        std::string name;
        
        // On-the-fly verification flags:
        bool cut;
        bool rej;
        bool dl_rej;
        bool ll_rej;
        bool inf_rej;
    };

};



class iStatePropsAP : public iSectionAP, public StateProps
{
 public:
    iStatePropsAP();
    virtual ~iStatePropsAP();
    
    virtual void lsts_StartStateProps( class Header& ) = 0;

    virtual void lsts_StartPropStates( const std::string& prop_name ) = 0;

    // virtual void lsts_StartPropStates( const lsts_StateProposition& ) = 0;

    virtual void lsts_PropState( lsts_index_t state ) = 0;
    virtual void lsts_PropStates( lsts_index_t subrange_start,
                                  lsts_index_t subrange_end );
  
    virtual void lsts_EndPropStates( const std::string& prop_name ) = 0;

    virtual void lsts_EndStateProps() = 0;


    // Methods for handling on the fly verification information:
 
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


    typedef class iStatePropsSection section_t;
    typedef class StatePropsChecker checker_t;

    
    virtual void CheckOutSpecialProps( SP::StatePropType,
                                       const std::string& );

 private:

    void AddSpecialStatePropState( SP::StatePropType id,
                                   lsts_index_t state );
 
    bool specials_delivered_among_common;
    
    typedef std::vector< std::vector<lsts_index_t> > s_prop_t;
    typedef std::vector<lsts_index_t> states_t;
    
    s_prop_t special_props;
    s_prop_t special_props_inter;

    friend class oStatePropsSection;
    friend class StatePropsChecker;
};



class oStatePropsAP : public oSectionAP, public StateProps
{
 public:
    virtual ~oStatePropsAP();
    
    virtual lsts_index_t lsts_numberOfStatePropsToWrite() = 0;
    virtual void lsts_WriteStateProps( iStatePropsAP& writer ) = 0;

    virtual bool lsts_isIntervalSearchOn() const;

    typedef class oStatePropsSection section_t;
    typedef class StatePropsChecker checker_t;
};


#undef CIRC_STATEPROPSAP_HH_
#endif
