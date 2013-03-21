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
 * File:        bitset.hh
 * Description: Typedef and macro definitions for manipulating bitsets.
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#include "bitset.hh"
#ifdef TUTNEW
#include <tutnew>
#endif

bool
BS_EQUAL(const int* b1, const int* b2, const int x)
{
	unsigned long int BS_n = BS_UP_ROUND(x);

	for (unsigned long int BS_i = 0; BS_i < BS_n; BS_i++) {
		if (b1[BS_i] != b2[BS_i]) { return false; }
	}
	return true;
}


/* vim: set tabstop=4 shiftwidth=4: */
