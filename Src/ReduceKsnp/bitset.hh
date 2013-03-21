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

#ifndef BITSET_HH
#define BITSET_HH

typedef unsigned long int bitset_unit;
typedef bitset_unit* BITSET;

#ifdef BS_UNIT_BITS
#error "Symbol BS_UNIT_BITS has a meaning that clashes with this program."
#endif
#define BS_UNIT_BITS (sizeof(bitset_unit) * 8)

#ifdef BS_UP_ROUND
#error "Symbol BS_UP_ROUND has a meaning that clashes with this program."
#endif
#define BS_UP_ROUND(x) (((x) + BS_UNIT_BITS - 1) / BS_UNIT_BITS)

#ifdef BS_ALLOC
#error "Symbol BS_ALLOC has a meaning that clashes with this program."
#endif
#define BS_ALLOC(x) new bitset_unit[BS_UP_ROUND(x)]

#ifdef BS_SET
#error "Symbol BS_SET has a meaning that clashes with this program."
#endif
#define BS_SET(b,i) (b)[(i) / BS_UNIT_BITS] |= (1ul << ((i) % BS_UNIT_BITS))

#ifdef BS_CLEAR
#error "Symbol BS_CLEAR has a meaning that clashes with this program."
#endif
#define BS_CLEAR(b,i) (b)[(i) / BS_UNIT_BITS] &= ~(1ul << ((i) % BS_UNIT_BITS))

#ifdef BS_IS_SET
#error "Symbol BS_IS_SET has a meaning that clashes with this program."
#endif
#define BS_IS_SET(b,i) ((b)[(i) / BS_UNIT_BITS] & (1ul << ((i) % BS_UNIT_BITS)))

#ifdef BS_RESET
#error "Symbol BS_RESET has a meaning that clashes with this program."
#endif
#define BS_RESET(b,x) \
	do { \
		unsigned long int BS_n = BS_UP_ROUND(x); \
		for (unsigned long int BS_i = 0; BS_i < BS_n; BS_i++) \
			(b)[BS_i] = 0; \
	} while(false)

#ifdef BS_COPY
#error "Symbol BS_COPY has a meaning that clashes with this program."
#endif
#define BS_COPY(dst,src,x) \
	do { \
		unsigned long int BS_n = BS_UP_ROUND(x); \
		for (unsigned long int BS_i = 0; BS_i < BS_n; BS_i++) \
			(dst)[BS_i] = (src)[BS_i]; \
	} while(false)

#endif

bool BS_EQUAL(const int*, const int*, const int);

/* vim: set tabstop=4 shiftwidth=4: */
