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

Contributor(s): 
*/

#ifndef DEBUG_HH

#define DEBUG_HH

//#define DEBUG
#ifdef DEBUG

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#define DPRINT( X )  cerr << X << endl
#define DPRIN( X )  cerr << X
#define DPRINCHAR( X )  if ( X >= 32 ) { DPRIN( X ); } \
                        else { DPRIN( static_cast<unsigned>( X ) ); }
#define DRUNPRINT( X )  
#define DRUNPRIN( X ) 
#define DRUNPRINCHAR( X ) 
#endif

#ifndef DEBUG
#define DPRINT( X )
#define DPRIN( X )
#define DPRINCHAR( X )
#define DRUNPRINT( X )  
#define DRUNPRIN( X ) 
#define DRUNPRINCHAR( X )
#endif

#endif
