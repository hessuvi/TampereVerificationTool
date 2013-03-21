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

#include "AccSetsSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "LSTS_File/Header.hh"

#include "otfvi.hh"


void
iAccSetsSection::ReadSection( Header& header )
{
    ITokenStream& t_src = its();

    std::string puncts = Interval::GiveSubrangePunctuator();
    puncts += "#{#;#}";

    t_src.SetPunctuation( puncts );

    iap.lsts_StartAccSets( header );

    const TT::Token* token;
    lsts_index_t state = 0;
    lsts_index_t action = 0;
  
    // We read acceptance sets until first not a number token
    //  (excepted: END id.).
    while ( t_src.GetNumber( state ) )
    {
        iap.lsts_StartAccSetsOfState( state );

        // We read bracketed acceptance sets until semicolon:
        while ( ( token = &t_src.Get( TT::PUNCTUATION ) )
                ->stringData() != ";" )
        {
            
            // Excepting the bracket open punctuation mark:
            check_claim( token->stringData() == "{",
                         t_src.errorMsg( *token,
                           std::string("leading bracket (`{') expected "
                                  "on the start of an acceptance set" ) ) );
      
            iap.lsts_StartSingleAccSet( state );

            // We read actions until first not a number token:
            
            lsts_index_t inter_end = 0;
            while ( Interval::Read( t_src, action, inter_end ) )
            {
                if ( inter_end )
                {
                    iap.lsts_AccSetActions( state, action, inter_end );
                    continue;
                }
                
                iap.lsts_AccSetAction( state, action );
            }

            t_src.Get( "closing bracket (`}') expected "
                       "in the end of an acceptance set",
                       TT::PUNCTUATION, "}" );
      
            iap.lsts_EndSingleAccSet( state );
            
        } // End of while.

    
        iap.lsts_EndAccSetsOfState( state );
        
    } // End of while.

    iap.lsts_EndAccSets();
}


bool
oAccSetsSection::doWeWriteSectionWithInit( Header& )
{ return oap->lsts_doWeWriteAccSets(); }

void
oAccSetsSection::WriteSection( Header& header )
{
    interv.EnableOutputIntervals( oap->lsts_isIntervalSearchOn() );

    lsts_StartAccSets( header );
    oap->lsts_WriteAccSets( *this );
    lsts_EndAccSets();
}

void
oAccSetsSection::lsts_StartAccSets( Header& ) { WriteBeginSection(); }
    
void
oAccSetsSection::lsts_StartAccSetsOfState( lsts_index_t state )
{ ots().PutNumber( state ); }

void
oAccSetsSection::lsts_StartSingleAccSet( lsts_index_t )
{ ots().PutPunctuation( "{" ); }

void
oAccSetsSection::lsts_AccSetAction( lsts_index_t, lsts_index_t action )
{ interv.Write( ots(), action ); }

void
oAccSetsSection::lsts_AccSetActions( lsts_index_t,
                                     lsts_index_t subrange_start,
                                     lsts_index_t subrange_end )
{ Interval::WriteSubrange( ots(), subrange_start, subrange_end ); }

void
oAccSetsSection::lsts_EndSingleAccSet( lsts_index_t )
{
    interv.WriteLast( ots() );
    ots().PutPunctuation( "}" );
}

void
oAccSetsSection::lsts_EndAccSetsOfState( lsts_index_t )
{
    ots().spaceOff();
    ots().PutPunctuation( ";" );
    ots().spaceOn();
    ots().Endl();
}

void
oAccSetsSection::lsts_EndAccSets() { WriteEndSection(); }
