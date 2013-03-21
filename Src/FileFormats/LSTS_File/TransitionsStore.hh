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

// FILE_DES: TransitionsStore.hh: Src/FileFormats/LSTS_File
// Timo Erkkilä

// $Id: TransitionsStore.hh 1.15 Fri, 08 Feb 2002 14:14:18 +0200 timoe $
// 
// This class is a store for Transitions section. It can be used to store
//   read transitions and to later write them out.
//

// $Log:$

#ifdef CIRC_TRANSITIONSSTORE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_TRANSITIONSSTORE_HH_
#define ONCE_TRANSITIONSSTORE_HH_
#define CIRC_TRANSITIONSSTORE_HH_


#include "TransitionsAP.hh"
#include "renumber.hh"


#include <vector>


class TransitionsStore : public iTransitionsAP, public oTransitionsAP
{
 public:
    TransitionsStore();
    
    virtual lsts_index_t lsts_numberOfTransitionsToWrite();
    virtual void lsts_WriteTransitions( iTransitionsAP& ap );
    
    virtual void lsts_StartTransitions( Header& );
    virtual void lsts_StartTransitionsFromState( lsts_index_t start_state );
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action );
    virtual void lsts_EndTransitionsFromState( lsts_index_t start_state );
    virtual void lsts_EndTransitions();

#ifdef RATE
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action,
                                  lsts_float_t decimal_number );
#endif

 private:

    // No copy constructor nor assigment operation in use:
    TransitionsStore( const TransitionsStore& );
    TransitionsStore& operator=( const TransitionsStore& );

    lsts_index_t i;
    std::vector<lsts_index_t> transitions;
};


struct Transition
{
    inline Transition() { }

    inline Transition( lsts_index_t sState, lsts_index_t dState,
                       lsts_index_t ac ) :
        
        s_state( sState ),
        d_state( dState ),
        action( ac )
#ifdef RATE
        , fl_number( -1.1 )
#endif
    { }
     
    inline bool operator!=( const Transition& cmp ) const
    {
        return s_state != cmp.s_state ||
            d_state != cmp.d_state ||
            action != cmp.action;
    }

    inline bool operator<( const Transition& cmp ) const
    {
        return s_state < cmp.s_state ||

            s_state == cmp.s_state &&
            d_state < cmp.d_state ||

            s_state == cmp.s_state &&
            d_state == cmp.d_state &&
            action < cmp.action;
    }
    
    lsts_index_t s_state;
    lsts_index_t d_state;
    lsts_index_t action;
#ifdef RATE
    lsts_float_t fl_number;
#endif
};




class SafeTransitionsStore : public iTransitionsAP, public oTransitionsAP
{
 public:
    SafeTransitionsStore();
    
    virtual lsts_index_t lsts_numberOfTransitionsToWrite();
    virtual void lsts_WriteTransitions( iTransitionsAP& ap );
    
    virtual void lsts_StartTransitions( Header& );
    virtual void lsts_StartTransitionsFromState( lsts_index_t start_state );
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action );
    virtual void lsts_EndTransitionsFromState( lsts_index_t start_state );
    virtual void lsts_EndTransitions();

 #ifdef RATE
    virtual void lsts_Transition( lsts_index_t start_state,
                                  lsts_index_t dest_state,
                                  lsts_index_t action,
                                  lsts_float_t decimal_number );
#endif

    lsts_index_t GiveGreatestState() const;
    lsts_index_t GiveGreatestAction() const;
    lsts_index_t GiveTransitionCnt() const;

    // Refining features:

    lsts_index_t RemoveDuplicateTransitions();
    // Returns the amount of removed transitions.

    lsts_index_t RemoveUnreachablePart( lsts_index_t initial_state );
    // Returns the amount of removed transitions.

    bool RenumberStates( const Renumber<lsts_index_t>&
                         action_names_renumbering );
    // Returns true if the numbering of states really changed.

    Renumber<lsts_index_t>& giveRenumber() { return renumber; }

 private:

    // No copy constructor nor assigment operation in use:
    SafeTransitionsStore( const SafeTransitionsStore& );
    SafeTransitionsStore& operator=( const SafeTransitionsStore& );
    
    void checkIfGreatestState( lsts_index_t state );

    lsts_index_t initial_state;
    
    lsts_index_t greatest_state;
    lsts_index_t  greatest_action;
    std::vector<Transition> transitions;

    Renumber<lsts_index_t> renumber;
};


#undef CIRC_TRANSITIONSSTORE_HH_
#endif
