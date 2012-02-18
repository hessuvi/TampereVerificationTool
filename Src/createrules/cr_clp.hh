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

#ifdef CIRC_CR_CLP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_CR_CLP_HH_
#define ONCE_CR_CLP_HH_
#define CIRC_CR_CLP_HH_

#include "TvtCLP.hh"
#include "consts.hh"

#ifndef NO_REGEXP
extern bool USE_REGULAR_EXPRESSIONS;
#endif

class CreateRulesCLP: public TvtCLP
{
public:
  CreateRulesCLP();

  virtual void help( const std::string& program );

  bool rename();
  bool visible();

  InStream& getRenameFile();



//======================================================================
 private:
    bool renameHandler(const std::string& param);
    bool visibleHandler(const std::string& param);

#ifndef NO_REGEXP
    bool useereHandler(const std::string&);
#endif

  bool renameUsed;
  bool visibleFlavor;
  InStream renameFile;

  static const char* const description;
};


#undef CIRC_CR_CLP_HH_
#endif
