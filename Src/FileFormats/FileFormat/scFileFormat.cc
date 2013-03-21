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

// FILE_DES: Scfileformat.cc: FileFormats/FileFormat
// Timo Erkkilä

// $Id: scFileFormat.cc 1.1 Fri, 08 Feb 2002 14:14:18 +0200 timoe $

// Scfileformat is a general class for reading and writing that uses a
//   section principle and can read from or write to a file using any
//   kind of Section based objects.

// $Log: Scfileformat.cc,v $
// Revision 1.1  1999/08/09 13:06:15  timoe
// Initial revision
//

#include "scFileFormat.hh"
#include "Section.hh"
#include "Checker.hh"


scFileFormat::scFileFormat( const std::string& begin_file,
                            const std::string& end_file,
                            bool checks_on ) :

    FileFormat( begin_file, end_file ),
    checks( checks_on )
{ }

scFileFormat::~scFileFormat() { Clear(); }

void
scFileFormat::Clear()
{
    for ( unsigned i = 0; i < sections.size(); ++i )
    { delete sections[ i ]; }
    sections.clear();
    for ( unsigned i = 0; i < checkers.size(); ++i )
    { delete checkers[ i ]; }
    checkers.clear();
    FileFormat::Clear();
}
