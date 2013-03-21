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

// Regular Expression
// ------------------------------------------------------------------------

#ifdef CIRC_REGEXP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_REGEXP_HH_
#define ONCE_REGEXP_HH_
#define CIRC_REGEXP_HH_

#include "consts.hh"
#include "cr_clp.hh"

#ifndef MAKEDEPEND
#include <string>
#endif



extern "C"
{
    class RegExp
    {
     public:
        RegExp(const std::string& regular_expression);
        ~RegExp();

        bool match(const std::string& str) const;
        std::string replace(const std::string& matchingStr,
                            const std::string& replacement) const;


//------------------------------------------------------------------------
#ifndef NO_REGEXP
        RegExp(const RegExp& cpy);
        RegExp& operator=(const RegExp&);

     private:
        struct CompRE;
        CompRE* comp;

        void incRefCnt();
        void decRefCnt();
        void printError(int errcode) const;

#else
     private:
        std::string regexp;
#endif
    };
}


#undef CIRC_REGEXP_HH_
#endif
