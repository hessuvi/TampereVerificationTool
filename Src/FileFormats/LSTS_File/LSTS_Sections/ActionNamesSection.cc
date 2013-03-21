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

#include "ActionNamesSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "LSTS_File/Header.hh"


void
iActionNamesSection::ReadSection( Header& header )
{ 
    iap.lsts_StartActionNames( header );

    its().SetPunctuation( "=" );
    lsts_index_t action_number;
    
    while ( its().GetNumber( action_number ) )
    {
        its().Get( TT::PUNCTUATION, "=" );
        const std::string& action_name = its().Get( TT::STRING ).stringData();
        
        // Sending action name information to an application:   
        iap.lsts_ActionName( action_number, action_name );
    }

    iap.lsts_EndActionNames();
}

/*

  bool operator>>( action& i )
  {
     if ( its >> action_number )
     {   
        its.get<punct>( "=" );
        action_name = its.get<str>();
        return true;
     }
     return false;
  }

     its >>  >> "=" >> action_name;
     action.number = action_number;
     action.name = action_name;
     return *this;
}


if ( iactions >> ch_action )
{ }


  int c = is.sws_get(); if ( !isdigit( c ) ) { syntax_error(); }
   

  while ( its >> action_n )
  {

     
     


 */

bool
oActionNamesSection::doWeWriteSectionWithInit( Header& header )
{
    lsts_index_t action_cnt = oap->lsts_numberOfActionNamesToWrite();
    header.SetActionCnt( action_cnt );
    return action_cnt;
}

void
oActionNamesSection::WriteSection( Header& header )
{
    lsts_StartActionNames( header );
    // Here we give control and the writer to the user:
    oap->lsts_WriteActionNames( *this );
    lsts_EndActionNames();
}

void
oActionNamesSection::lsts_ActionName( lsts_index_t action_number,
                                      const std::string& name )
{
    ots().PutNumber( action_number ); 
    ots().PutPunctuation( "=" ); 
    ots().PutString( name ); 
    ots().Endl();
}

void
oActionNamesSection::lsts_StartActionNames( Header& ) { WriteBeginSection(); }

void
oActionNamesSection::lsts_EndActionNames() { WriteEndSection(); }
