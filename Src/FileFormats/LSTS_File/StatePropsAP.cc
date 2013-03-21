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

// FILE_DES: StatePropsAP.cc: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: StatePropsAP.cc 1.16 Wed, 18 Aug 2004 19:30:57 +0300 timoe $";
//
// The implementation of StatePropsAP class.

// $Log:$


#include "StatePropsAP.hh"
#include "error_handling.hh"

using std::string;


StateProps::~StateProps() { }


StateProps::lsts_StateProposition::lsts_StateProposition() :
    name( "<no name was given>" ),
    cut( false ), rej( false ), dl_rej( false ), ll_rej( false ), 
    inf_rej( false )
{ }
        

iStatePropsAP::iStatePropsAP() :
    iSectionAP(),
    specials_delivered_among_common( true ),
    special_props( SP::SIZE ),
    special_props_inter( SP::SIZE )
{ }

iStatePropsAP::~iStatePropsAP() { }

void
iStatePropsAP::lsts_PropStates( lsts_index_t i_start,
                               lsts_index_t i_end )
{
    while ( i_start <= i_end )
    {
        lsts_PropState( i_start );
        ++i_start;
    }
}

// Methods for handling on the fly verification information:



#define AddSpecialState( m_name, id ) \
void \
iStatePropsAP::m_name( lsts_index_t state ) \
{ special_props[ id ].push_back( state ); } \
void \
iStatePropsAP::m_name##s( lsts_index_t i_start, lsts_index_t i_end ) \
{ \
    while ( i_start <= i_end ) \
    { \
        m_name( i_start ); \
        ++i_start; \
    } \
}

/*special_props_inter[ id ].push_back( i_start ); \
  special_props_inter[ id ].push_back( i_end ); }
*/

AddSpecialState( lsts_CutState, SP::CUT )
AddSpecialState( lsts_RejectState, SP::REJ )
AddSpecialState( lsts_DeadlockRejectState, SP::DL_REJ )
AddSpecialState( lsts_LivelockRejectState, SP::LL_REJ )
AddSpecialState( lsts_InfinityRejectState, SP::INF_REJ )



void
iStatePropsAP::CheckOutSpecialProps( SP::StatePropType type,
                                     const string& name )
{
    if ( special_props[ type ].size() || special_props_inter[ type ].size() )
    {
        lsts_StartPropStates( name );

        for ( states_t::size_type i = 0;
              i < special_props[ type ].size();
              ++i )
        {
            lsts_PropState( special_props[ type ][ i ] );
        }

        /*for ( states_t::size_type i = 0;
              i < special_props_inter[ type ].size();
              ++i )
        {
            lsts_PropStates( special_props_inter[ type ][ i ],
                             special_props_inter[ type ][ ++i ] );
                             }*/

        lsts_EndPropStates( name );

        special_props[ type ].clear();
        special_props_inter[ type ].clear();
    }

}

void iStatePropsAP::AddSpecialStatePropState( SP::StatePropType id,
                                              lsts_index_t state )
{ special_props[ id ].push_back( state ); }



oStatePropsAP::~oStatePropsAP() { }

bool
oStatePropsAP::lsts_isIntervalSearchOn() const { return true; }
