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

#include "StatePropsSection.hh"
#include "LSTS_File/Header.hh"

#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"


namespace
{
    void readProps( ITokenStream& t_src, iStatePropsAP& ap,
                    void ( iStatePropsAP::*to_meth_ptr )( lsts_index_t ),
                    void ( iStatePropsAP::*to_meth_ptr_inter )
                    ( lsts_index_t, lsts_index_t ) )
    {
        t_src.Get( TT::PUNCTUATION, ":" );
        
        lsts_index_t state = 0;
        lsts_index_t inter_end = 0;
        
        while ( Interval::Read( t_src, state, inter_end ) )
        {
            if ( inter_end )
            {
                // cerr << "hep:" << state << "," << inter_end << endl;
                ( ap.*to_meth_ptr_inter )( state, inter_end );
                continue;
            }
            
            ( ap.*to_meth_ptr )( state );
        }
        
        t_src.Get( TT::PUNCTUATION, ";" );
    }
    
}

void
iStatePropsSection::ReadSection( Header& header )
{
    iap.lsts_StartStateProps( header );
    ITokenStream& t_src = its();
    t_src.SetPunctuation( std::string( ":#;#" ) +
                          Interval::GiveSubrangePunctuator() );
    
    while ( true )
    {
        const TT::Token* token = &t_src.Peek();

        // Special state prop or end of the section?
        if ( token->isIdentifier() )
        {
            if ( token->stringData() == "END" )
            {
                break;
            }

            token = &t_src.Get();

#define CheckSpecialProp( M, prop_type, prop_name ) \
            if ( token->stringData() == SP::SpecialPropIds[ prop_type ] ) \
            { \
                readProps( t_src, iap, &iStatePropsAP::M, &iStatePropsAP::M##s ); \
                iap.CheckOutSpecialProps( prop_type, prop_name ); \
                continue; \
            }

            CheckSpecialProp( lsts_CutState, SP::CUT,
                              SP::cut );

            CheckSpecialProp( lsts_RejectState, SP::REJ,
                              SP::rej );

            CheckSpecialProp( lsts_DeadlockRejectState, SP::DL_REJ,
                              SP::dl_rej );

            CheckSpecialProp( lsts_LivelockRejectState, SP::LL_REJ,
                              SP::ll_rej );

            CheckSpecialProp( lsts_InfinityRejectState, SP::INF_REJ,
                              SP::inf_rej );

            
            Exs::Throw( t_src.errorMsg( *token, "bad identifier" ) );
        }
        
        const std::string prop_name = t_src.Get( TT::STRING ).stringData();
        
        iap.lsts_StartPropStates( prop_name ); 
        readProps( t_src, iap, &iStatePropsAP::lsts_PropState,
                   &iStatePropsAP::lsts_PropStates );
        
        iap.lsts_EndPropStates( prop_name );        
    }
    
    
    iap.lsts_EndStateProps();
}


void
oStatePropsSection::lsts_StartPropStates( const std::string& prop_name )
{
    // WriteSpecialProps();
    type = SP::GiveStatePropType( prop_name );

    //std::cerr << type << ":" << prop_name << std::endl;
    if ( type ) { return; }
    
    ots().PutString( prop_name );
    ots().PutPunctuation( ":" );
}

void
oStatePropsSection::lsts_PropState( lsts_index_t state )
{
    //std::cerr << "<" << type << ":" << state << ">" << std::endl;

    if ( type )
    {
        special_props[ type ].push_back( state );
        return;
    }

    interv.Write( ots(), state );
}

void
oStatePropsSection::lsts_PropStates( lsts_index_t subrange_start,
                                    lsts_index_t subrange_end )
{
    Interval::WriteSubrange( ots(), subrange_start, subrange_end );
}

void
oStatePropsSection::lsts_EndPropStates( const std::string& )
{
    if ( type ) return;
 
    interv.WriteLast( ots() );
    
    ots().spaceOff();
    ots().PutPunctuation( ";" );
    ots().spaceOn();

    ots().Endl();
}

bool
oStatePropsSection::doWeWriteSectionWithInit( Header& hd )
{
    lsts_index_t prop_amount = oap->lsts_numberOfStatePropsToWrite();
    if ( prop_amount ) { hd.SetStatePropCnt( prop_amount ); }
    return prop_amount;
}

void
oStatePropsSection::WriteSpecialProps()
{
    //    bool endlWritten = false;

    for ( s_prop_t::size_type t = 1; t < special_props.size(); ++t )
    {
        if ( special_props[ t ].empty() )
        {
            continue;
        }
        
        /*  if ( !endlWritten )
        {
            ots().Endl();
            endlWritten = true;
            }*/

        ots().PutIdentifier( SP::SpecialPropIds[ t ] );
        ots().PutPunctuation( ":" );
        
        for ( lsts_vec_size_t i = 0; i < special_props[ t ].size(); ++i )
        {
            interv.Write( ots(), special_props[ t ][ i ] );
        }
        interv.WriteLast( ots() );
        
        ots().spaceOff();
        ots().PutPunctuation( ";" );
        ots().spaceOn();
        ots().Endl();

        special_props[ t ].clear();
    }
    
}

void
oStatePropsSection::WriteSection( Header& header )
{
    interv.EnableOutputIntervals( oap->lsts_isIntervalSearchOn() );
    
    lsts_StartStateProps( header );
    
    // Here we give control and the writer to the user:
    oap->lsts_WriteStateProps( *this );

    WriteSpecialProps();

    lsts_EndStateProps();
}

void
oStatePropsSection::lsts_StartStateProps( Header& ) { WriteBeginSection(); }

void
oStatePropsSection::lsts_EndStateProps() { WriteEndSection(); }
