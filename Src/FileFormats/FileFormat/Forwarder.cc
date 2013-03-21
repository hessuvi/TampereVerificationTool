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

#include "Forwarder.hh"


Forwarder::Forwarder( bool readAlways ) :
    read_always( readAlways ), checked( false ) { }
Forwarder::~Forwarder() { }

void
Forwarder::ShowHeader( const class Header& ) { }

void
Forwarder::SetSectionFilename( const std::string& secFilename )
{ section_filename = secFilename; }
    
const std::string&
Forwarder::GiveSectionFilename() const
{ return section_filename; }
