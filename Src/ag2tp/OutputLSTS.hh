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

#ifdef CIRC_OUTPUTLSTS_HH_
#error "Include recursion"
#endif

#ifndef ONCE_OUTPUTLSTS_HH_
#define ONCE_OUTPUTLSTS_HH_
#define CIRC_OUTPUTLSTS_HH_

#include "BitVector.hh"
#include "InputLSTSContainer.hh"
#include "TransitionsContainer.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <vector>
#endif

typedef InputLSTSContainer InputLSTS;
typedef StatePropsContainer::StatePropsPtr StatePropsPtr;

class OutputLSTS
{
	public:
		OutputLSTS(InputLSTS&);
		void calculateTP();
		void writeLSTSFile(OutStream&);

	private:
		InputLSTS& lsts;
		lsts_index_t nrOfOldStates;
		lsts_index_t nrOfNewStates;
		lsts_index_t nrOfActions;
		lsts_index_t initialState;
		lsts_index_t traceRejectState;
		BitVector deadlockFlag;
		BitVector livelockFlag;
		TransitionsContainer transitions;
		bool hasTaus;

		void addTraceRejectTransition(const lsts_index_t& action);
		const AccSets calculateMirror(const RO_AccSets& accsets);
		void calculateStateProps(StatePropsContainer& oprops);
		OutputLSTS(const OutputLSTS&);
		OutputLSTS& operator=(const OutputLSTS&);
};

#undef CIRC_OUTPUTLSTS_HH_
#endif

/* vim: set tabstop=4 shiftwidth=4: */
