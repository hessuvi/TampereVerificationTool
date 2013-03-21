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

// FILE_DES: error_handling.hh: Auxiliary
// Heikki Virtanen

// $Id: error_handling.hh 1.15 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// Tools for reporting error situations and building message text
//
// check_claim(claim, msg): If claim is not true, print msg and abort program
//
// valueToMessage( start, value, end):
//       result string is start + value as string + end
// 
// AbortWithMessage(msg): Print msg and abort program

// $Log: error_handling.hh,v $
// Revision 1.3  1999/08/19 13:46:51  hvi
// Lisätty makro, joka tarkistaa väitteen ja varoittaa, ellei se ole voimassa.
//
// Revision 1.2  1999/08/17 08:36:29  hvi
// Lisätty funktio, jolla voi tulostaa viestin kaatamatta ohjelmaa.
//
// Revision 1.1  1999/07/08 12:50:38  hvi
// Initial revision
//

#ifdef CIRC_ERROR_HANDLING_HH_
#error "Include recursion"
#endif

#ifndef ONCE_ERROR_HANDLING_HH_
#define ONCE_ERROR_HANDLING_HH_
#define CIRC_ERROR_HANDLING_HH_

#include <string>


void PrintMessageLine(const std::string& msg);
void AbortWithMessage(const std::string& msg);

inline std::string unsignedToString(unsigned val)
{
    if(!val) return "0";

    std::string res_str;

    while(val)
    {
        res_str += '0' + val%10;
        val /= 10;
    }

    for(unsigned i = 0; i < res_str.size()/2; ++i)
    {
        unsigned j = res_str.size() - i - 1;
        char c = res_str[j];
        res_str[j] = res_str[i];
        res_str[i] = c;
    }

    return res_str;
}

inline std::string valueToMessage(const std::string& start,
                                  unsigned val,
                                  const std::string& end)
{
    return start + unsignedToString(val) + end;
}

// Errors are checked and, if necessary, program is aborted and
// messages printed using preprocessor macro check_claim

#define checkClaim(claim, msg) check_claim(claim, msg)
#define check_claim(claim, msg) \
{ if( !(claim) ) { \
  AbortWithMessage(msg); \
} }

#define warnIfNot(claim, msg) warn_ifnot(claim, msg)
#define warn_ifnot(claim, msg) \
{ if( !(claim) ) { \
  PrintMessageLine(std::string("---WARNING: ") + msg); \
} }

#define writeMessage(msg) write_message(msg)
inline void write_message(const std::string& msg)
{
    PrintMessageLine(msg);
}

// Unconditional termination of program is made visible
//const bool TERMINATE_ALWAYS = false;


#undef CIRC_ERROR_HANDLING_HH_
#endif

