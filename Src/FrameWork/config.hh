/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// FILE_DES: config.hh: Src/FrameWork/config.hh
// Timo Erkkilä
// 

// $Id: config.hh 1.5.1.7 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// This header consists of the definitions of environment dependent constants
// needed by all TVT programs.
// 

// $Log:$
//

#ifndef CONFIG_HH
#define CONFIG_HH

#include <climits>

#define TVT_VERSION  3000000L

// ISO latin 1 character set is in use:
#define ISO_LATIN_1_CHAR_SET
// If you are using some other character set, please remove the
// defination above.


// This is a defition of a new type lsts_index_t that is used with
// variables that store the number of states, transitions, etc:
#if UINT_MAX < 65536
typedef unsigned long lsts_index_t;
typedef unsigned long state_prop_t;
#else
typedef unsigned int lsts_index_t;
typedef unsigned int state_prop_t;
#endif

typedef double lsts_float_t;

// Constant action number reserved for unnamed action.
const lsts_index_t TAU_ACTION = 0;

// The maximum number of characters (the newline character ending a
// line is excluded) per line.
const unsigned MAX_CHARS_IN_LINE = 72;

// Idiot-proof NULL-pointer.
// Can't be named 'NULL' because a subsequent C header inclusion can
// override it. Named 'NIL' here.
/*static const
class
{
 public:
    template<typename T>
    operator T*() const { return 0; }

    template<typename C, typename T>
    operator T C::*() const { return 0; }

 private:
    void operator&() const;
} NIL;
*/


#endif
