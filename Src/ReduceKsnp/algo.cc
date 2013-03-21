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
 * File:        algo.cc
 * Description: Kanellakis/Smolka's Naive algorithm
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#include "algo.hh"
#include "global.hh"
#ifdef TUTNEW
#include <tutnew>
#endif

void
ksnaive_alloc()
{
	ALLOC_MEM(block, lsts_index_t, N);
	ALLOC_MEM(next, lsts_index_t, N + 1);
	ALLOC_MEM(waiting_next, lsts_index_t, N);
	ALLOC_MEM(bucket, lsts_index_t, N + 1);
	ALLOC_MEM(old_block_nr, lsts_index_t, N + 1);
	ALLOC_MEM(minbnr, lsts_index_t, N);
}

void
ksnaive(const lsts_index_t a)
{
	lsts_index_t i; /* the iteration count for lexicographic sorting */
	lsts_index_t e; /* for iteration over elements */
	lsts_index_t f; /* for stroing element numbers */
	lsts_index_t b; /* for storing block numbers */
	lsts_index_t c; /* for storing block numbers */
	lsts_index_t d; /* for storing block numbers */
	lsts_index_t n; /* for storing block numbers */
	lsts_index_t k; /* bucket number for next transition */
	lsts_index_t waiting; /* first of the waiting blocks */
	lsts_index_t bucket_count; /* number of buckets in use */
	lsts_index_t bucket_base; /* bucket content offset */
	lsts_index_t bucket_base_limit; /* limit for bucket_base reset */
	lsts_index_t r; /* used to step through transitions of state e*/
	lsts_index_t j; /* number of smallest elements collected */
	lsts_index_t x; /* block number for next transition */
	lsts_index_t m; /* iterator through minbnr[] */
	bool changed;

	bucket_base_limit = MAX_LSTS_INDEX_T - 1 - N;
	for (d = 0; d <= N; d++) { bucket[d] = 0; }
	bucket_base = 1;
	do {
		changed = false;
		for (e = 1; e <= N; e++) { old_block_nr[e] = block_nr[e]; }
		for (b = 0; b < block_count - 1; b++) { waiting_next[b] = b + 1; }
		waiting_next[block_count - 1] = UNDEFINED;
		b = 0;
		waiting = UNDEFINED;
		i = 1;
		while (b != UNDEFINED) {
			// print_blocks();
			c = waiting_next[b];
			e = block[b];
			if (next[e] != UNDEFINED) {
				bucket_count = 0;
				while (e != UNDEFINED) {
					/*** BEGIN Calculate k: ***/
					r = trans[e];
					if (r != UNDEFINED) {
						j = 0;
						while (true) {
							if (act[r] == a) {
								x = old_block_nr[dst[r]];
								m = 0;
								while ((m < j) && (x > minbnr[m])) { m = m + 1; }
								if (m < j) {
									if (x != minbnr[m]) {
										while (m < j) {
											k = minbnr[m];
											minbnr[m] = x;
											x = k;
											m = m + 1;
										}
										minbnr[j] = x;
										j = j + 1;
									}
								}
								else if (j < i) {
									minbnr[j] = x;
									j = j + 1;
								}
							}
							r = r + 1;
							if (BS_IS_SET(first, r)) { break; }
						}
						if (j < i) {
							k = 0;
						}
						else {
							k = 1 + minbnr[i - 1];
						}
					}
					else {
						k = 0;
					}
					/*** END Calculate k ***/
					if (bucket[k] < bucket_base) {
						n = b;
						bucket_count = bucket_count + 1;
						if (bucket_count > 1) {
							n = block_count;
							block_count = block_count + 1;
							changed = true;
						}
						block[n] = UNDEFINED;
						bucket[k] = n + bucket_base;
					}
					else {
						n = bucket[k] - bucket_base;
					}
					f = next[e];
					next[e] = block[n];
					if ((block[n] != UNDEFINED) && (next[block[n]] == UNDEFINED)
							&& (k != 0)) {
						waiting_next[n] = waiting;
						waiting = n;
					}
					block[n] = e;
					block_nr[e] = n;
					e = f;
				}
				bucket_base += block_count + 1;
				if (bucket_base >= bucket_base_limit) {
					for (d = 0; d <= N; d++) { bucket[d] = 0; }
					bucket_base = 1;
				}
			}
			b = c;
			if (b == UNDEFINED) {
				i = i + 1;
				b = waiting;
				waiting = UNDEFINED;
			}
		}
	} while (changed);
	final_block_count = block_count;
}

void
print_blocks()
{
	lsts_index_t b; /* for iteration over block numbers */
	lsts_index_t e; /* for iteration over elements */

	for (b = 0; b < block_count; b++) {
		e = block[b];
		while (e != UNDEFINED) {
			printf("%d ", e);
			e = next[e];
		}
		printf("; ");
	}
	printf("\n");
}

void
ksnaivep()
{
	lsts_index_t i; /* for iteration over blocks/elements */
	lsts_index_t h; /* for iteration over actions */
	lsts_index_t b; /* for storing block numbers */
	lsts_index_t e; /* for iteration over elements */
	lsts_index_t last_block_count;

	ksnaive_alloc();

	block_count = initial_block_count;
	for (b = 0; b < N; b++) {
		block[b] = UNDEFINED;
	}
	for (e = 1; e <= N; e++) {
		b = block_nr[e];
		next[e] = block[b];
		block[b] = e;
	}

	h = 0;
	i = 0;
	last_block_count = initial_block_count;
	while (true) {
		ksnaive(h);
		h = (h + 1) % (K + 1);
		if (last_block_count != final_block_count) { i = 0; }
		else { i++; }
		if (i == K) { break; }
		last_block_count = final_block_count;
	}
	FREE_MEM(waiting_next, lsts_index_t, N);
	FREE_MEM(bucket, lsts_index_t, N + 1);
	FREE_MEM(old_block_nr, lsts_index_t, N + 1);
	FREE_MEM(minbnr, lsts_index_t, N);
}

/* vim: set tabstop=4 shiftwidth=4: */
