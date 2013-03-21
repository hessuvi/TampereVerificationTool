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

#include "StateNamesSection.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "LSTS_File/Header.hh"


void
iStateNamesSection::ReadSection( Header& header )
{ 
    iap.lsts_StartStateNames( header );

    its().SetPunctuation( "=" );
    lsts_index_t state_number;
    
    while ( its().GetNumber( state_number ) )
    {
        its().Get( TT::PUNCTUATOR, "=" );
        const std::string& state_name = its().Get( TT::STRING ).stringData();
        // Sending the state name information to the application program:   
        iap.lsts_StateName( state_number, state_name );
    }

    iap.lsts_EndStateNames();
}


bool
oStateNamesSection::doWeWriteSectionWithInit( Header& )
{ return oap->lsts_doWeWriteStateNames(); }

void
oStateNamesSection::WriteSection( Header& header )
{
    lsts_StartStateNames( header );
    // Here we give control and the writer to the user:
    oap->lsts_WriteStateNames( *this );
    lsts_EndStateNames();
}

void
oStateNamesSection::lsts_StateName( lsts_index_t state_number,
                                    const std::string& name )
{
    ots().PutNumber( state_number ); 
    ots().PutPunctuation( "=" ); 
    ots().PutString( name ); 
    ots().Endl();
}

void
oStateNamesSection::lsts_StartStateNames( Header& ) { WriteBeginSection(); }

void
oStateNamesSection::lsts_EndStateNames() { WriteEndSection(); }
