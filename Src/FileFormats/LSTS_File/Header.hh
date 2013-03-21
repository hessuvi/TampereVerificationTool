/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// FILE_DES: Header.hh: FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: Header.hh 1.11 Thu, 11 Mar 2004 18:07:16 +0200 timoe $
// 
// This class contains the data members that are used to store the information
//   gathered from the header and the history sections of an LSTS file.
//

#ifndef HEADER_HH
#define HEADER_HH

#include <string>
#include <vector>

#include "config.hh"
#include "FileFormat/Lexical/OTokenStream.hh"


class Header
{
 public:
    Header();
    ~Header();
    // Header( const Header& );
    // Header& operator=( const Header& );
    // Default copy constructor and assignment operator are in use.
    
    bool isInterrupted() const { return error_message.size() || otf_error; }
    bool isOnTheFlyError() const { return otf_error; }

    void SetError( const std::string& error_msg )
    { error_message = error_msg; }
    const std::string& GiveError() const { return error_message; }

    enum OTF_ERROR
    {
        NONE = 0,
        CUT,
        REJ,
        DL_REJ,
        LL_REJ,
        INF_REJ
    };

    void SetOnTheFlyError( OTF_ERROR type, lsts_index_t in_state )
    { otf_error = type; otf_error_state = in_state; }
    void SetGuard( unsigned process_number ) { guard = process_number; }

    OTF_ERROR GiveOnTheFlyError( lsts_index_t& in_state ) const
    { in_state = otf_error_state; return otf_error; }
    unsigned GiveGuard() const { return guard; }

    lsts_index_t GiveStateCnt() const { return state_cnt; }
    void SetStateCnt( lsts_index_t );

    lsts_index_t GiveActionCnt() const { return action_cnt; }
    void SetActionCnt( lsts_index_t );

    lsts_index_t GiveElusiveActionCnt() const
    { return elusive_action_cnt; }
    void SetElusiveActionCnt( lsts_index_t elacs )
    { elusive_action_cnt = elacs; }

    lsts_index_t GiveTransitionCnt() const { return transition_cnt; }
    void SetTransitionCnt( lsts_index_t );

    lsts_index_t numberOfInitialStates() const
    { return initial_states.size(); }
    const std::vector<lsts_index_t>& GiveInitialStates() const
    { return initial_states; }
    void AddInitialState( lsts_index_t s ) { initial_states.push_back( s ); }
    void ClearInitialStates() { initial_states.clear(); }

    bool hasStatePropCnt() const;
    lsts_index_t GiveStatePropCnt() const { return state_prop_cnt; }
    void SetStatePropCnt( lsts_index_t );

    bool isInitiallyUnstableGiven() const;
    bool isInitiallyUnstable() const;
    void SetInitiallyUnstable( bool );
    void RemoveInitiallyUnstable() { initially_unstable_given = false; }

    bool hasSemantics( const std::string& semantics ) const;
    const std::vector<std::string>& GiveSemantics() const;
    void AddSemantics( const std::string& new_semantics );

    bool hasPromise( const std::string& promise ) const;
    const std::vector<std::string>& GivePromises() const;
    void AddPromise( const std::string& new_promise );
    bool RemovePromise( const std::string& promise );




    // FOR THE INTERNAL USE OF THE LSTS I/O LIB:

    bool isStateCntGiven() const;
    bool isActionCntGiven() const;
    bool isTransitionCntGiven() const;
    
    void WriteInitialStates( OTokenStream& ots ) const
    {
        if ( initial_states.empty() ) { return; }

        ots.PutWord( "Initial_states =" );
        for ( unsigned i = 0; i < initial_states.size(); ++i )
        {
            ots.PutNumber( initial_states[ i ] );
        }
        ots.spaceOff();
        ots.PutPunctuation( ";" );
        ots.spaceOn();
        ots.Endl();
    }

    lsts_index_t GiveInitialState() const { return initial_states.front(); }
    void SetInitialState( lsts_index_t s )
    {
        if ( initial_states.empty() ) { AddInitialState( s ); }
        else { initial_states[ 0 ] = s; }
    }

    void SetNoLongTauLoops( bool );
    void SetShallowTaus( bool );
    void SetNoTaus( bool );
    void SetDeterministic( bool );
    void SetNormal( bool );

    bool hasNoLongTauLoops() const;
    bool hasShallowTaus() const;
    bool hasNoTaus() const;
    bool isDeterministic() const;
    bool isNormal() const;

    bool isNoLongTauLoopsGiven() const;
    bool isShallowTausGiven() const;
    bool isNoTausGiven() const;
    bool isDeterministicGiven() const;
    bool isNormalGiven() const;

    bool hasSemantics() const { return semantics.size(); }
    
 private:
    // ### These following booleans tell whether an attribute value has been
    //   given to this class instance or not.
  
    bool state_cnt_given;
    bool transition_cnt_given;
    bool action_cnt_given;
    bool state_prop_cnt_given;

    bool initially_unstable_given;
    bool interrupted_given;
  

    // ### Information that is gathered about the header section: ###

    lsts_index_t state_cnt;
    lsts_index_t transition_cnt;
    lsts_index_t action_cnt;
    lsts_index_t elusive_action_cnt;
    lsts_index_t state_prop_cnt;
  
    bool initially_unstable;

    std::vector<lsts_index_t> initial_states; 

    std::vector<std::string> semantics;
    std::vector<std::string> promises;

    std::string error_message;
    OTF_ERROR otf_error;
    lsts_index_t otf_error_state;
    unsigned guard;
};


#endif
