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

#include "Section.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "Exceptions.hh"


// PUBLIC:
Section::~Section() { }

iSection::~iSection() { }

// The prime reading method:
void
iSection::Read( ITokenStream& in_ts, Header& header )
{ from = &in_ts; ReadSection( header ); }


oSection::~oSection() { }

// Initializes the section. If return value is false, section
//   will not be written.
bool
oSection::doWeWriteWithInit( OTokenStream& out_ts, Header& header )
{
    tout.to = &out_ts;
    return doWeWriteSectionWithInit( header );
}

// The prime writing method:
void
oSection::Write( Header& header, OTokenStream* oTs )
{
    startSectionCalls = 0;
    endSectionCalls = 0;

    if ( oTs ) { tout.to = oTs; }
    else { tout << endl << endl; }
    
    WriteSection( header );

    check_claim( startSectionCalls <= 1, std::string( GiveName() ) +
                 ": lsts_Start..Section() called in wrong place" );
    
    check_claim( endSectionCalls <= 1, std::string( GiveName() ) +
                 ": lsts_End..Section() called in wrong place" );
}


// PROTECTED:
oSection::oSection() : startSectionCalls( 0 ), endSectionCalls( 0 ) { }

void
oSection::WriteBeginSection()
{
    tout << space_off << id << "Begin" << space_on
         << id << GiveName() << endl << endl;
    startSectionCalls++;
}

void
oSection::WriteEndSection()
{
    tout << endl
         << space_off << id << "End" << space_on << id << GiveName();
    endSectionCalls++;
}
