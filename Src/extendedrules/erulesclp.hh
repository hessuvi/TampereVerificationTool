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

#ifdef CIRC_ERULESCLP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_ERULESCLP_HH_
#define ONCE_ERULESCLP_HH_
#define CIRC_ERULESCLP_HH_


#include "TvtCLP.hh"

class ERulesCLP: public TvtCLP
{
 public:
    ERulesCLP(const std::string& desc):
        TvtCLP(desc), noerror_(true), revConv_(false)
    {
        setOptionHandler("-check", &ERulesCLP::noerrorHandler, true,
                         "Issue an error message when an "
                         "invalid rule is found (ie. the name of the "
                         "action does not exist in the correspondent "
                         "LSTS file).");
        setOptionHandler("x", &ERulesCLP::xHandler, true,
                         "Convert a regular rules file to an extended one.");
    }

    inline bool noError() { return noerror_; }
    inline bool reverseConv() { return revConv_; }

 private:
    bool noerror_, revConv_;

    bool noerrorHandler(const std::string&)
    {
        noerror_ = false;
        return true;
    }

    bool xHandler(const std::string&)
    {
        revConv_ = true;
        return true;
    }
};




#undef CIRC_ERULESCLP_HH_
#endif
