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

// FILE_DES: StatePropsStore.hh: Src/FileFormats/LSTS_File
// Timo Erkkilä

// $Id: StatePropsStore.hh 1.13 Fri, 08 Feb 2002 14:14:18 +0200 timoe $
// 
// This class is a store for State_props section. It can be used to store
//   read state props and to later write them out.
//

// $Log:$

#ifdef CIRC_STATEPROPSSTORE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_STATEPROPSSTORE_HH_
#define ONCE_STATEPROPSSTORE_HH_
#define CIRC_STATEPROPSSTORE_HH_


#include <vector>
#include <string>

#include "StatePropsAP.hh"
#include "renumber.hh"


struct StateProp
{
    StateProp() { }
    StateProp( const std::string& n ) : name( n ) { }
     
    /* Old stuff...
    inline bool operator!=( const StateProp& cmp ) const
    {
        return state != cmp.state || prop != cmp.prop;
    }
    
    inline bool operator<( const StateProp& cmp ) const
    {
        return state < cmp.state || state == cmp.state && prop < cmp.prop;
    }
    */

    std::string name;
    std::vector<lsts_index_t> states;
};


class StatePropsStore : public iStatePropsAP, public oStatePropsAP
{
 public:
    StatePropsStore();

    virtual lsts_index_t lsts_numberOfStatePropsToWrite();
    virtual void lsts_WriteStateProps( iStatePropsAP& ap );
    
    virtual void lsts_StartStateProps( Header& );

    virtual void lsts_StartPropStates( const std::string& prop_name );
    virtual void lsts_PropState( lsts_index_t state );
    virtual void lsts_EndPropStates( const std::string& prop_name );

    virtual void lsts_EndStateProps();

    // Refining features:
    lsts_index_t RemoveDuplicatePropStates();
    lsts_index_t RenumberPropStates( Renumber<lsts_index_t>& states_renum );

    /*
    // Methods for handling on the fly verification information:
    virtual bool lsts_separateOTFVI();    

    virtual void lsts_CutState( lsts_index_t state );
    virtual void lsts_RejectState( lsts_index_t state );
    virtual void lsts_DeadlockRejectState( lsts_index_t state );
    virtual void lsts_LivelockRejectState( lsts_index_t state );
    virtual void lsts_InfinityRejectState( lsts_index_t state );
    */

 private:

    // No copy constructor nor assigment operation in use:
    StatePropsStore (const StatePropsStore&);
    StatePropsStore& operator=(const StatePropsStore&);

    typedef std::vector<StateProp> props_t;
    props_t state_props;

    //state_prop_t old_max_prop;
    // typedef map<lsts_index_t, int> OtfviMap;
    // OtfviMap otfvi;
};


#undef CIRC_STATEPROPSSTORE_HH_
#endif
