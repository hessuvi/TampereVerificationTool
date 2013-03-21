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

Contributor(s): Jacobus Geldenhuys.
*/

/* ------------------------------------------------------------------------
 * File:        clp.hh
 * Description: Command line processor.
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#ifndef KSNP_CLP_HH
#define KSNP_CLP_HH

#include "TvtCLP.hh"

class CLP: public TvtCLP
{
	public:
		CLP(const std::string&);

	private:
		bool infoHandler(const std::string&);
};

#endif

/* vim: set tabstop=4 shiftwidth=4: */
