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

#include "TransitionsSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "LSTS_File/Header.hh"
#include "Exceptions.hh"


void
iTransitionsSection::ReadSection( Header& header )
{
    iap.lsts_StartTransitions( header );
  
    ITokenStream& it = its();
    it.SetPunctuation( ",#:#;#{#}" );

    lsts_index_t start_state; lsts_index_t end_state; lsts_index_t action;

    // We read transitions until first not a number token:
    while ( it.GetNumber( start_state ) )
    {
        iap.lsts_StartTransitionsFromState( start_state );

        it.skip( TT::PUNCTUATOR, ":" );

        // We read transitions starting from this very start state
        //   until semicolon:
        while ( it.GetNumber( end_state ) )
        {
            const TT::Token& tok = it.Get(); 

            if ( tok.isPunctuator( "," ) )
            {
                check_claim( it.GetNumber( action ),
                             "missing action number" );
            }
            else if ( tok.isNumber() ) { action = tok.numberData(); }
            else { Exs::Throw( it.errorMsg( "transition syntax error" ) ); }

            // Has the transition extra data?
            if ( it.Peek().isString() )
            {
                std::string extra_data( it.Get().stringData() );
                
                // Layout info included as well?
                if ( it.skip( TT::PUNCTUATOR, "{" ) )
                {
                    iap.lsts_TransitionWithExtraDataAndLayoutInfo
                        ( start_state, end_state, action, extra_data,
                          it.Get( TT::STRING ).stringData() );
                    it.Get( TT::PUNCTUATOR, "}" );
                    continue;
                }
                
                iap.lsts_TransitionWithExtraData
                    ( start_state, end_state, action, extra_data );
                continue;
            }
            
            // Layout info?
            if ( it.skip( TT::PUNCTUATOR, "{" ) )
            {
                iap.lsts_TransitionWithLayoutInfo
                    ( start_state, end_state, action, it.Get().stringData() );
                it.Get( TT::PUNCTUATOR, "}" );
                continue;
            }

#ifdef RATE
            if ( it.Peek().isFloat() )
            {
                lsts_float_t fl = it.Get().floatData();
                iap.lsts_Transition( start_state, end_state, action, fl );
                continue;
            }
#endif

            iap.lsts_Transition( start_state, end_state, action );
        }
    
        // Reading ";":
        it.Get();
    
        iap.lsts_EndTransitionsFromState( start_state );
          
    } // End of while.
  

    iap.lsts_EndTransitions();
}

// Writes a number of start state for transitions to follow:
void
oTransitionsSection::lsts_StartTransitionsFromState( lsts_index_t start_state )
{
    ots().PutNumber( start_state );
    ots().spaceOff();
    ots().PutPunctuation( ":" );
    ots().spaceOn();
}

// Writes a single transition:
void
oTransitionsSection::lsts_Transition( lsts_index_t /*start_state*/,
                                     lsts_index_t dest_state,
                                     lsts_index_t action )
{
    ots().PutNumber( dest_state );
    ots().spaceOff();
    ots().PutPunctuation( "," );
    ots().PutNumber( action );
    ots().spaceOn();
}

// Writes a transition with extra data.
void
oTransitionsSection::lsts_TransitionWithExtraData
( lsts_index_t /*start_state*/, lsts_index_t dest_state,
  lsts_index_t action, const std::string& extra_data )
{
    tout << space_off
         << dest_state << ',' << action << extra_data
         << space_on;
}

void
oTransitionsSection::lsts_TransitionWithLayoutInfo
( lsts_index_t /*start_state*/, lsts_index_t dest_state,
  lsts_index_t action, const std::string& layout_info )
{
    tout << space_off
         << dest_state << ',' << action << '{' << layout_info << '}'
         << space_on;
}

void
oTransitionsSection::lsts_TransitionWithExtraDataAndLayoutInfo
( lsts_index_t /*start_state*/, lsts_index_t dest_state,
  lsts_index_t action, const std::string& extra_data,
  const std::string& layout_info )
{
    tout << space_off
         << dest_state << ',' << action << extra_data
         << '{' << layout_info << '}'
         << space_on;
}

#ifdef RATE
void
oTransitionsSection::lsts_Transition( lsts_index_t s, lsts_index_t ds,
                                      lsts_index_t a, lsts_float_t fl )
{
    lsts_Transition( s, ds, a );
    ots().PutFloat( fl );
}
#endif

// This method closes the sequence of transitions starting from start state:
void
oTransitionsSection::lsts_EndTransitionsFromState( lsts_index_t )
{ tout << space_off << ';' << space_on << endl; }

bool
oTransitionsSection::doWeWriteSectionWithInit( Header& header )
{
    lsts_index_t tr_cnt = oap->lsts_numberOfTransitionsToWrite();
    header.SetTransitionCnt( tr_cnt );
    return tr_cnt;
}

void
oTransitionsSection::WriteSection( Header& header )
{
    lsts_StartTransitions( header );
    // Here we give control and the writer to the user:
    oap->lsts_WriteTransitions( *this );
    lsts_EndTransitions();
}

void
oTransitionsSection::lsts_StartTransitions( Header& ) { WriteBeginSection(); }

void
oTransitionsSection::lsts_EndTransitions() { WriteEndSection(); }
