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

Contributor(s): Heikki Virtanen.
*/

// FILE_DES: guard.hh: Src/guard.hh
// 

// $Id: guard.hh 1.3 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// 
// 

#ifdef CIRC_GUARD_HH
#error Recursive include
#endif
#ifndef ONCE_GUARD_HH
#define ONCE_GUARD_HH
#define CIRC_GUARD_HH

#ifndef MAKEDEPEND
//#include <>
#endif
//#include ""

// Prevent copy construction and assignment for a class
// If used in other context, move to separe header file.
#define REFERENCE_CLASS(className) \
private: \
               className (const className& other); \
    className& operator= (const className& other); 


#undef CIRC_GUARD_HH
#endif  // ONCE_GUARD_HH
// Local variables:
// mode: c++
// mode: font-lock
// compile-command: "gmake"
// End:
