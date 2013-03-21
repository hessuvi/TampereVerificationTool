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
 * File:        reader.cc
 * Description: Reader for LSTSs.
 * Author:      Jaco Geldenhuys <jaco@cs.tut.fi>
 * ------------------------------------------------------------------------
 */

#include "bitset.hh"
#include "global.hh"
#include "reader.hh"

#include "LSTS_File/iLSTS_File.hh"
#include "LSTS_File/ActionNamesAP.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "LSTS_File/StatePropsAP.hh"
#include "error_handling.hh"
#ifdef TUTNEW
#include <tutnew>
#endif

/* ------------------------------------------------------------------------
 * Read transitions from LSTS file.
 * ------------------------------------------------------------------------
 */

class TransitionsReader: public iTransitionsAP
{
	public:
		void lsts_StartTransitions(Header&)
		{
			lsts_index_t i;

			ALLOC_BS(first, M + 1);
			ALLOC_MEM(dst, lsts_index_t, M);
			ALLOC_MEM(act, lsts_index_t, M);
			ALLOC_MEM(trans, lsts_index_t, N + 1);
			for (i = 1; i <= N; i++) { trans[i] = UNDEFINED; }
			t = 0;
		}

		void lsts_StartTransitionsFromState(lsts_index_t s)
		{
			trans[s] = t;
			BS_SET(first, t);
			trans_count = 0;
		}

		void lsts_Transition(lsts_index_t, lsts_index_t d, lsts_index_t a)
		{
			dst[t] = d;
			act[t] = a;
			t++;
			trans_count++;
		}

		void lsts_EndTransitionsFromState(lsts_index_t s)
		{
			if (trans_count == 0) { trans[s] = UNDEFINED; }
		}

		void lsts_EndTransitions()
		{
			BS_SET(first, t);
		}

	private:
		lsts_index_t t; /* number of transitions encountered so far */
		lsts_index_t trans_count; /* nr of transitions read for each state */
};

/* ------------------------------------------------------------------------
 * Read state propositions from LSTS file.
 * ------------------------------------------------------------------------
 */

class PropsReader: public iStatePropsAP
{
	public:
		void lsts_StartStateProps(Header&)
		{
			ALLOC_MEM(new_block, lsts_index_t, N);
			ALLOC_MEM(block_elems, lsts_index_t, N);

			lsts_index_t i;
			for (i = 0; i < N; i++) {
				new_block[i] = UNDEFINED;
				block_elems[i] = 0;
			}
			for (i = 1; i <= N; i++) { block_elems[block_nr[i]]++; }
		}

		void lsts_StartPropStates(const std::string&) { }

		void lsts_PropState(lsts_index_t s)
		{
			if (new_block[block_nr[s]] == UNDEFINED) {
				int j;
				if (block_elems[block_nr[s]] == 1) {
					j = block_nr[s];
				}
				else {
					for (j = 0; block_elems[j] != 0; j++) { }
				}
				new_block[block_nr[s]] = j;
			}
			lsts_index_t k = block_nr[s];
			block_elems[k]--;
			block_nr[s] = k = new_block[k];
			block_elems[k]++;
		}

		void lsts_PropState(lsts_index_t s, lsts_index_t e)
		{
			lsts_index_t i;
			for (i = s; i <= e; i++) { lsts_PropState(i); }
		}

		void lsts_EndPropStates(const std::string&)
		{
			lsts_index_t i;
			for (i = 0; i < N; i++) { new_block[i] = UNDEFINED; }
		}

		void lsts_EndStateProps()
		{
			lsts_index_t i;
			initial_block_count = 0;
			for (i = 1; i <= N; i++) {
				if (new_block[block_nr[i]] == UNDEFINED) {
					new_block[block_nr[i]] = initial_block_count++;
				}
				block_nr[i] = new_block[block_nr[i]];
			}
			FREE_MEM(new_block, lsts_index_t, N);
			FREE_MEM(block_elems, lsts_index_t, N);
		}

	private:
		lsts_index_t* new_block;
		lsts_index_t* block_elems;
};

/* ------------------------------------------------------------------------
 * Read LSTS from file.
 * ------------------------------------------------------------------------
 */

void
read_LSTS(InStream& input)
{
	iLSTS_File ilsts(input);
	lsts_index_t i;

	/* Get object counts from the LSTS file.
	 */
	input_header = ilsts.GiveHeader();
	N = input_header.GiveStateCnt();
	K = input_header.GiveActionCnt();
	M = input_header.GiveTransitionCnt();

	/* Allocate the initial_state bit, and set the flags for the initial
	 * states.
	 */
	ALLOC_BS(init_state, N + 1);
	i = input_header.numberOfInitialStates();
	while (i > 0) {
		i--;
		BS_SET(init_state, input_header.GiveInitialStates()[i]);
	}

	/* Allocate space for the initial partition and give it a default
	 * value.  This is also where the answer should be stored.
	 */
	ALLOC_MEM(block_nr, lsts_index_t, N + 1);
	initial_block_count = 1;
	for (i = 1; i <= N; i++) {
		block_nr[i] = 0;
		/* If split is based on initial blocks:
			if (BS_IS_SET(init_state, i)) {
				block_nr[i] = 0;
			}
			else {
				block_nr[i] = 1;
				initial_block_count = 2;
			}
		*/
	}

	/* Read the file.
	 */
	TransitionsReader trans;
	PropsReader props;

	ilsts.AddActionNamesReader(znames);
	ilsts.AddTransitionsReader(trans);
	ilsts.AddStatePropsReader(props);
	ilsts.SetNoReaderAction(iLSTS_File::IGNORE);
	ilsts.ReadFile();
	input.CloseFile();
}

/* vim: set tabstop=4 shiftwidth=4: */
