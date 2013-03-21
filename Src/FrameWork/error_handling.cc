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

Contributor(s): Juha Nieminen, Timo Erkkilä, Heikki Virtanen.
*/

#include "LogWrite.hh"

#include <cstdlib>

using std::string;

void PrintMessageLine(const std::string& msg)
{
    //LogWrite::getOStream() << LogWrite::getInfoString() << msg << std::endl;
    LogWrite::writeNeatly( msg );
}

void AbortWithMessage(const std::string& msg)
{
  PrintMessageLine( std::string("***ERROR: ") + msg);
  std::exit(1);
}
