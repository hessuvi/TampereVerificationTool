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
 * File:        clp.cc
 * Description: Command line processor.
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#include "clp.hh"
#include "global.hh"
#ifdef TUTNEW
#include <tutnew>
#endif

/* ------------------------------------------------------------------------
 * Constructor.
 * ------------------------------------------------------------------------
 */

CLP::CLP(const std::string& programDescription):
	TvtCLP(programDescription)
{
	print_info = false;
	setOptionHandler("-info", &CLP::infoHandler, true,
		"Prints extra information about memory usage and "
		"calculation time.");
}

/* ------------------------------------------------------------------------
 * Handle the initial split "-i" option.
 * ------------------------------------------------------------------------
 */

bool
CLP::infoHandler(const std::string&)
{
	print_info = true;
	return true;
}

/* vim: set tabstop=4 shiftwidth=4: */
