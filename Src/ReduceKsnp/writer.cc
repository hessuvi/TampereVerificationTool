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
 * File:        writer.cc
 * Description: Writer for LSTSs.
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#include "bitset.hh"
#include "global.hh"
#include "writer.hh"

#include "LSTS_File/oLSTS_File.hh"
#include "LSTS_File/ActionNamesAP.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "LSTS_File/StatePropsAP.hh"
#include "error_handling.hh"
#ifdef TUTNEW
#include <tutnew>
#endif

/* ------------------------------------------------------------------------
 * Write transitions to LSTS file.
 * ------------------------------------------------------------------------
 */

#define TRANS_IS_GREATER(a,b) \
	((act[a] > act[b]) || \
	((act[a] == act[b]) && (block_nr[dst[a]] > block_nr[dst[b]])))
#define TRANS_IS_SMALLER(a,b) \
	((act[a] < act[b]) || \
	((act[a] == act[b]) && (block_nr[dst[a]] < block_nr[dst[b]])))
#define TRANS_IS_EQUAL(a,b) \
	((act[a] == act[b]) && (block_nr[dst[a]] == block_nr[dst[b]]))

class TransitionsWriter: public oTransitionsAP
{
	public:
		lsts_index_t lsts_numberOfTransitionsToWrite()
		{
			lsts_index_t i;
			lsts_index_t r;
			lsts_index_t q;
			lsts_index_t p;
			lsts_index_t j;
			lsts_index_t k;
			lsts_index_t a;
			lsts_index_t d;

			if (trans == 0) { return 0; }

			final_M = 0;
			for (i = 0; i < final_block_count; i++) {
				r = trans[block[i]];
				if (r != UNDEFINED) {
					final_M++;
					q = p = r + 1;
					while (!BS_IS_SET(first, q)) {
						k = p;
						while ((k > r) && (!TRANS_IS_SMALLER(k - 1, q))) {
							k = k - 1;
						}
						if (k == p) {
							if (p != q) {
								act[p] = act[q];
								dst[p] = dst[q];
							}
							p = p + 1;
							final_M++;
						}
						else if (!TRANS_IS_EQUAL(k, q)) {
							a = act[q]; d = dst[q];
							j = p;
							while (j > k) {
								act[j] = act[j - 1];
								dst[j] = dst[j - 1];
								j = j - 1;
							}
							act[j] = a; dst[j] = d;
							p = p + 1;
							final_M++;
						}
						else {
							act[q] = UNDEFINED;
						}
						q = q + 1;
					}
					if (!BS_IS_SET(first, p)) {
						act[p] = UNDEFINED;
					}
				}
			}
			return final_M;
		}

		void lsts_WriteTransitions(iTransitionsAP& writer)
		{
			lsts_index_t i;
			lsts_index_t r;

			for (i = 0; i < final_block_count; i++) {
				writer.lsts_StartTransitionsFromState(i + 1);
				r = trans[block[i]];
				if (r != UNDEFINED) {
					do {
						if (act[r] == UNDEFINED) { break; }
						writer.lsts_Transition(i + 1, block_nr[dst[r]] + 1, act[r]);
						r++;
					} while (!BS_IS_SET(first, r));
				}
				writer.lsts_EndTransitionsFromState(i + 1);
			}
		}

		/*
		lsts_index_t lsts_numberOfTransitionsToWrite()
		{
			lsts_index_t i;
			lsts_index_t j;
			lsts_index_t r;
			BITSET visited;

			if (trans == 0) { return 0; }

			final_M = 0;
			ALLOC_BS(visited, final_block_count);
			for (i = 0; i < final_block_count; i++) {
				for (j = 0; j <= K; j++) {
					BS_RESET(visited, final_block_count);
					r = trans[block[i]];
					if (r != UNDEFINED) {
						do {
							if (act[r] == j) {
								if (!BS_IS_SET(visited, block_nr[dst[r]])) {
									BS_SET(visited, block_nr[dst[r]]);
									final_M++;
								}
								else {
									act[r] = UNDEFINED;
								}
							}
							r++;
						} while (!BS_IS_SET(first, r));
					}
				}
			}
			FREE_BS(visited, final_block_count);
			return final_M;
		}

		void lsts_WriteTransitions(iTransitionsAP& writer)
		{
			lsts_index_t i;
			lsts_index_t r;

			for (i = 0; i < final_block_count; i++) {
				writer.lsts_StartTransitionsFromState(i + 1);
				r = trans[block[i]];
				if (r != UNDEFINED) {
					do {
						if (act[r] != UNDEFINED) {
							writer.lsts_Transition(i + 1, block_nr[dst[r]] + 1, act[r]);
						}
						r++;
					} while (!BS_IS_SET(first, r));
				}
				writer.lsts_EndTransitionsFromState(i + 1);
			}
		}
		*/

		/*
		lsts_index_t lsts_numberOfTransitionsToWrite()
		{
			lsts_index_t i;
			lsts_index_t r;

			if (trans == 0) { return 0; }

			final_M = 0;
			for (i = 0; i < final_block_count; i++) {
				r = trans[block[i]];
				if (r != UNDEFINED) {
					do {
						final_M++;
						r++;
					} while (!BS_IS_SET(first, r));
				}
			}
			return final_M;
		}

		void lsts_WriteTransitions(iTransitionsAP& writer)
		{
			lsts_index_t i;
			lsts_index_t r;

			for (i = 0; i < final_block_count; i++) {
				writer.lsts_StartTransitionsFromState(i + 1);
				r = trans[block[i]];
				if (r != UNDEFINED) {
					do {
						writer.lsts_Transition(i + 1, block_nr[dst[r]] + 1, act[r]);
						r++;
					} while (!BS_IS_SET(first, r));
				}
				writer.lsts_EndTransitionsFromState(i + 1);
			}
		}
		*/

		/*
		lsts_index_t lsts_numberOfTransitionsToWrite()
		{
			lsts_index_t i;
			lsts_index_t e;
			lsts_index_t j;
			lsts_index_t r;
			BITSET visited;

			if (trans == 0) { return 0; }

			final_M = 0;
			ALLOC_BS(visited, final_block_count);
			for (i = 0; i < final_block_count; i++) {
				for (j = 0; j <= K; j++) {
					BS_RESET(visited, final_block_count);
					e = block[i];
					while (e != UNDEFINED) {
						r = trans[e];
						if (r != UNDEFINED) {
							do {
								if (act[r] == j) {
									if (!BS_IS_SET(visited, block_nr[dst[r]])) {
										BS_SET(visited, block_nr[dst[r]]);
										final_M++;
									}
									else {
										act[r] = UNDEFINED;
									}
								}
								r++;
							} while (!BS_IS_SET(first, r));
						}
						e = next[e];
					}
				}
			}
			FREE_BS(visited, final_block_count);
			return final_M;
		}

		void lsts_WriteTransitions(iTransitionsAP& writer)
		{
			lsts_index_t i;
			lsts_index_t e;
			lsts_index_t r;

			for (i = 0; i < final_block_count; i++) {
				writer.lsts_StartTransitionsFromState(i + 1);
				e = block[i];
				while (e != UNDEFINED) {
					r = trans[e];
					if (r != UNDEFINED) {
						do {
							if (act[r] != UNDEFINED) {
								writer.lsts_Transition(i + 1, block_nr[dst[r]] + 1, act[r]);
							}
							r++;
						} while (!BS_IS_SET(first, r));
					}
					e = next[e];
				}
				writer.lsts_EndTransitionsFromState(i + 1);
			}
		}
		*/
};

/* ------------------------------------------------------------------------
 * Write LSTS from file.
 * ------------------------------------------------------------------------
 */

void
write_LSTS(OutStream& output)
{
	oLSTS_File olsts;
	lsts_index_t i;
	lsts_index_t e;

	/* Write header information.
	 */
	if (trans == 0) {
		input_header.SetStateCnt(N);
	}
	else {
		input_header.SetStateCnt(final_block_count);
	}
	/* action count set by ActionNamesStore class */
	/* transition count set by TransitionsWriter class */
	/* input_header.AddPromise("Deterministic"); */

	/* Write the initial states.
	 */
	if (trans == 0) {
		for (e = 1; e <= N; e++) {
			if (BS_IS_SET(init_state, e)) {
				input_header.AddInitialState(e);
			}
		}
	}
	else {
		int initial_state_count = 0;

		for (i = 0; i < final_block_count; i++) {
			e = block[i];
			while (e != UNDEFINED) {
				if (BS_IS_SET(init_state, e)) {
					if (initial_state_count == 0) {
						input_header.SetInitialState(i + 1);
					}
					else {
						input_header.AddInitialState(i + 1);
					}
					initial_state_count++;
					break;
				}
				e = next[e];
			}
		}
	}

	olsts.GiveHeader() = input_header;

	/* Write the file.
	 */
	TransitionsWriter trans;

	olsts.AddActionNamesWriter(znames);
	olsts.AddTransitionsWriter(trans);
	//olsts.AddStatePropsReader(props);
	olsts.WriteFile(output);
	output.CloseFile();
}

/* vim: set tabstop=4 shiftwidth=4: */
