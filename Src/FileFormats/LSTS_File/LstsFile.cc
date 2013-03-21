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

#include "LstsFile.hh"
#include "FileFormat/Checker.hh"
#include "FileFormat/Lexical/charconv.hh"

using std::string;


LSTS_File::LSTS_File( bool checks_on ) :
    scFileFormat( "LSTS", "LSTS", checks_on )
{ SetHeader( header ); LSTS_Format(); }

LSTS_File::~LSTS_File() { }

void
LSTS_File::LSTS_Format()
{
    FileFormat::Clear();

    //            Name   ,  Priority, Optional? (, Skip If Not Supported?) 
    AddSection( "HISTORY",        -1, false );
    AddSection( "HEADER",         -1, false );

    AddSection( "ACTION_NAMES",    1, true );
    AddSection( "STATE_NAMES",     2, true );
    
    AddSection( "STATE_PROPS",     2, true );
    
    AddSection( "DIV_BITS",        2, true );
    AddSection( "ACC_SETS",        2, true );

    AddSection( "TRANSITIONS",     3, true );

    AddSection( "LAYOUT",          4, true, true );
}

void
LSTS_File::DoHeaderChecks( TokenStream* tokenStream ) const
{
    if ( !areChecksOn() ) { return; }

    check_claim( header.isStateCntGiven(),
                 tokenStream->errorMsg( "State_cnt is missing" ) );

    if ( header.GiveElusiveActionCnt() )
    {
        check_claim( header.isActionCntGiven(),
                     tokenStream->errorMsg( "Elusive_action_cnt without "
                                            "Action_cnt" ) );
        Checker::check_action( header.GiveElusiveActionCnt(),
                               header.GiveActionCnt(),
                               "Elusive_action_cnt", tokenStream );
    }

    /*
    check_claim( header.isTransitionCntGiven(),
                 tokenStream->errorMsg( "Transition_cnt is missing" ) );
    */

    check_claim( header.numberOfInitialStates(),
                 tokenStream->errorMsg( "Initial_states is missing" ) );
     
    const std::vector<lsts_index_t>& initial_states =
        header.GiveInitialStates();
    for ( unsigned i = 0; i < initial_states.size(); ++i )
    {
        Checker::check_state( initial_states[ i ], header.GiveStateCnt(),
                              "bad initial state: it", tokenStream );
    }

}

void
LSTS_File::DoEndFileChecks( const std::vector<string>& passed_secs,
                            TokenStream* ts )
{
    if ( !areChecksOn() ) { return; }

    ts->SetSectionName( "" );

    check_claim( !header.GiveActionCnt() ||
                 doesInclude( passed_secs, "ACTION_NAMES" ),
                 ts->errorMsg( "non-zero Action_cnt while "
                               "no Action_names section" ) );
    
    check_claim( !header.GiveTransitionCnt() ||
                 doesInclude( passed_secs, "TRANSITIONS" ),
                 ts->errorMsg( "non-zero Transition_cnt while "
                               "no Transitions section" ) ); 

    check_claim( !header.GiveStatePropCnt() ||
                 doesInclude( passed_secs, "STATE_PROPS" ),
                 ts->errorMsg( "non-zero State_prop_cnt while "
                               "no State_props section" ) ); 
}
