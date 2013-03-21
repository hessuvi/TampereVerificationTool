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

Contributor(s): Juha Nieminen.
*/

// FILE_DES: writer.hh: Parallel
// Nieminen Juha

// $Id: writer.hh 1.8 Tue, 23 Mar 2004 17:01:05 +0200 warp $
// 

// $Log:$

#ifdef CIRC_WRITER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_WRITER_HH_
#define ONCE_WRITER_HH_
#define CIRC_WRITER_HH_

#include "outputlsts.hh"
#include "parrules.hh"
#include "InOutStream.hh"

#ifndef MAKEDEPEND
#endif

// Luokan esittely:

class Writer //: public oTransitionsAP
{
public:
    Writer(OutStream&, OutputLSTS&, ParRules&, unsigned guardProcess);
};


#undef CIRC_WRITER_HH_
#endif

