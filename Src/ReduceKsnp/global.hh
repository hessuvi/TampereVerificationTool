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
 * File:        global.hh
 * Description: Global variables
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#ifndef KSNP_GLOBAL_HH
#define KSNP_GLOBAL_HH

#include "config.hh"

#include "bitset.hh"

#include "LSTS_File/ActionNamesStore.hh"
#include "LSTS_File/Header.hh"

/* ------------------------------------------------------------------------
 * Convenient constant.
 * ------------------------------------------------------------------------
 */

#ifdef UNDEFINED
#error "Symbol UNDEFINED has a meaning that clashes with this program."
#endif
#define UNDEFINED (lsts_index_t(-1))

#ifdef MAX_LSTS_INDEX_T
#error "Symbol MAX_LSTS_INDEX_T has a meaning that clashes with this program."
#endif
#define MAX_LSTS_INDEX_T (lsts_index_t(-1))

/* ------------------------------------------------------------------------
 * Global variables.
 * ------------------------------------------------------------------------
 */

extern lsts_index_t N; /* nr of states in the LSTS */
extern lsts_index_t K; /* nr of actions in the LSTS */
extern lsts_index_t M; /* nr of transitions in the LSTS */
extern lsts_index_t final_M; /* final nr of transitions in the LSTS */

extern bool print_info; /* should memory and time info be printed? */

extern BITSET init_state; /* keeps track of which states are initial */
extern lsts_index_t* dst; /* destination state [0..M-1] */
extern lsts_index_t* act; /* actions [0..M-1] */
extern lsts_index_t* trans; /* first transition index [1..N] */
extern BITSET first; /* flags for first transitions [0..M] */
extern ActionNamesStore znames;
extern Header input_header;

extern lsts_index_t initial_block_count; /* nr of blocks in initial partition */
extern lsts_index_t final_block_count; /* nr of blocks in final partition */

extern lsts_index_t block_count; /* nr of current blocks in partition */
extern lsts_index_t* block; /* maps blocks to first state [0..N-1] */
extern lsts_index_t* next; /* maps states to next state in block [1..N] */
extern lsts_index_t* block_nr; /* maps states to block number [1..N] */

extern lsts_index_t* waiting_next; /* maps block to next waiting block [0..N-1] */
extern lsts_index_t waiting; /* first waiting block */
extern lsts_index_t* bucket; /* maps bucket to block number [0..N] */

extern lsts_index_t* old_block_nr; /* maps states to block number [1..N] */
extern lsts_index_t* minbnr; /* stores smallest block numbers [0..N-1] */

extern unsigned int static_memory; /* static memory used by program (bytes) */
extern unsigned int current_memory; /* dynamic memory currently used by program (bytes) */
extern unsigned int dynamic_memory; /* maximmum dynamic memory used by program (bytes) */

/* ------------------------------------------------------------------------
 * Routines for global variables.
 * ------------------------------------------------------------------------
 */

void init_vars();
void alloc_mem(unsigned int);
void free_mem(unsigned int);

/* ------------------------------------------------------------------------
 * Macros for memory allocation.
 * ------------------------------------------------------------------------
 */

#ifdef ALLOC_MEM
#error "Symbol ALLOC_MEM has a meaning that clashes with this program."
#endif
#define ALLOC_MEM(v,t,n) v = new t[n]; alloc_mem((n) * sizeof(t))

#ifdef FREE_MEM
#error "Symbol FREE_MEM has a meaning that clashes with this program."
#endif
#define FREE_MEM(v,t,n) delete[] v; free_mem((n) * sizeof(t))

#ifdef ALLOC_BS
#error "Symbol ALLOC_BS has a meaning that clashes with this program."
#endif
#define ALLOC_BS(v,n) v = BS_ALLOC(n); BS_RESET(v, n); alloc_mem(BS_UP_ROUND(n) * sizeof(bitset_unit))

#ifdef FREE_BS
#error "Symbol FREE_BS has a meaning that clashes with this program."
#endif
#define FREE_BS(v,n) delete[] v; free_mem(BS_UP_ROUND(n) * sizeof(bitset_unit))

#endif

/* vim: set tabstop=4 shiftwidth=4: */
