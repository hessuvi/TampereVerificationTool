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

#include "config.hh"

#include "global.hh"
#include "bitset.hh"
#ifdef TUTNEW
#include <tutnew>
#endif

/* ------------------------------------------------------------------------
 * Global variables.
 * ------------------------------------------------------------------------
 */

lsts_index_t N; /* nr of states in the LSTS */
lsts_index_t K; /* nr of actions in the LSTS */
lsts_index_t M; /* nr of transitions in the LSTS */
lsts_index_t final_M; /* final nr of transitions in the LSTS */

bool print_info; /* should memory and time info be printed? */

BITSET init_state; /* keeps track of which states are initial */
lsts_index_t* dst; /* destination state [0..M-1] */
lsts_index_t* act; /* actions [0..M-1] */
lsts_index_t* trans; /* first transition index [1..N] */
BITSET first; /* flags for first transitions [0..M] */
ActionNamesStore znames;
Header input_header;

lsts_index_t initial_block_count; /* nr of blocks in initial partition */
lsts_index_t final_block_count; /* nr of blocks in final partition */

lsts_index_t block_count; /* nr of current blocks in partition */
lsts_index_t* block; /* maps blocks to first state [0..N-1] */
lsts_index_t* next; /* maps states to next state in block [1..N] */
lsts_index_t* block_nr; /* maps states to block number [1..N] */

lsts_index_t* waiting_next; /* maps block to next waiting block [0..N-1] */
lsts_index_t waiting; /* first waiting block */
lsts_index_t* bucket; /* maps bucket to block number [0..N] */

lsts_index_t* old_block_nr; /* maps states to block number [1..N] */
lsts_index_t* minbnr; /* stores smallest block numbers [0..N-1] */

unsigned int static_memory; /* static memory used by program (bytes) */
unsigned int current_memory; /* dynamic memory currently used by program (bytes) */
unsigned int dynamic_memory; /* maximmum dynamic memory used by program (bytes) */

/* ------------------------------------------------------------------------
 * Compute the amount of static memory used by the program.
 * ------------------------------------------------------------------------
 */

void
compute_static()
{
	static_memory
		= sizeof(lsts_index_t) /* N */
		+ sizeof(lsts_index_t) /* K */
		+ sizeof(lsts_index_t) /* M */
		+ sizeof(lsts_index_t) /* final_M */
		+ sizeof(bool) /* print_info */
		+ sizeof(BITSET) /* init_state */
		+ sizeof(lsts_index_t*) /* dst */
		+ sizeof(lsts_index_t*) /* act */
		+ sizeof(lsts_index_t*) /* trans */
		+ sizeof(BITSET) /* first */
		+ sizeof(ActionNamesStore) /* znames */
		+ sizeof(Header) /* input_header */
		+ sizeof(lsts_index_t) /* initial_block_count */
		+ sizeof(lsts_index_t) /* final_block_count */
		+ sizeof(lsts_index_t) /* block_count */
		+ sizeof(lsts_index_t*) /* block_nr */
		+ sizeof(lsts_index_t*) /* block */
		+ sizeof(lsts_index_t*) /* next */
		+ sizeof(lsts_index_t*) /* waiting_next */
		+ sizeof(lsts_index_t) /* waiting */
		+ sizeof(lsts_index_t*) /* bucket */
		+ sizeof(lsts_index_t*) /* old_block_nr */
		+ sizeof(lsts_index_t*) /* minbnr */
		;
}

/* ------------------------------------------------------------------------
 * Initialize some variables.
 * ------------------------------------------------------------------------
 */

void
init_vars()
{
	current_memory = 0;
	dynamic_memory = 0;
	compute_static();
}

/* ------------------------------------------------------------------------
 * Record allocation of memory.
 * ------------------------------------------------------------------------
 */

void
alloc_mem(unsigned int bytes)
{
	current_memory += bytes;
	if (current_memory > dynamic_memory) {
		dynamic_memory = current_memory;
	}
}

/* ------------------------------------------------------------------------
 * Record deallocation of memory.
 * ------------------------------------------------------------------------
 */

void
free_mem(unsigned int bytes)
{
	current_memory -= bytes;
}

/* vim: set tabstop=4 shiftwidth=4: */
