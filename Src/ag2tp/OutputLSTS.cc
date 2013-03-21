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

#include "LSTS_File/oLSTS_File.hh"
#include "OutputLSTS.hh"

using namespace std;

OutputLSTS::OutputLSTS(InputLSTS& ilsts):
	lsts(ilsts),
	nrOfOldStates(ilsts.getHeaderData().GiveStateCnt()),
	nrOfNewStates(nrOfOldStates),
	nrOfActions(ilsts.getHeaderData().GiveActionCnt()),
    initialState(ilsts.getStateNumber(ilsts.getInitialState())),
	traceRejectState(0),
	deadlockFlag(1 + nrOfOldStates * nrOfActions),
	livelockFlag(1 + nrOfOldStates * nrOfActions),
	transitions(nrOfActions, nrOfOldStates, true),
	hasTaus(false)
{
	check_claim(lsts.getHeaderData().hasNoTaus(),
		"The input header does not have the \"No_taus\" attribute set."
		"  Are you sure it is a determinized acceptance graph?");

	check_claim(lsts.getHeaderData().isDeterministic(),
		"The input header does not have the \"Deterministic\" attribute set."
		"  Are you sure it is a determinized acceptance graph?");
}

void
OutputLSTS::calculateTP()
{

	for (lsts_index_t s = 1; s <= nrOfOldStates; ++s)
	{
		const InputLSTS::State& state = lsts.getState(s);
		const RO_AccSets accsets = lsts.getRO_AccSets(s);
		BitVector inherited(1);
		vector<lsts_index_t> newStates;

		/**
		 * Mark this state as a deadlock-reject state if its set of
		 * acceptance sets is empty.
		 */
		if (accsets.setsAmount() != 1)
		{
			deadlockFlag.setBit(s);
		}
		else {
			RO_AccSets::AccSetPtr as = accsets.firstAccSet();
			if (accsets.begin(as) != accsets.end(as)) {
				deadlockFlag.setBit(s);
			}
		}

		/**
		 * Mark this state as a livelock-reject state if the divergence
		 * bit is off.
		 */
		if (!state.getDivBit())
		{
			livelockFlag.setBit(s);
		}

		/**
		 * Calculate the mirror image of the set of acceptance sets
		 * and add an extra state for each of the elements.
		 */
		const AccSets mirror = calculateMirror(accsets);
		if (mirror.setsAmount() > 1) {
			inherited = mirror.getAllUsedActionsAsBitVector();

			for (AccSets::AccSetPtr as = mirror.firstAccSet();
				as != mirror.endAccSet(); ++as)
			{
				/**
				 * Create a new state.
				 */
				lsts_index_t newState = ++nrOfNewStates;
				deadlockFlag.setBit(newState, deadlockFlag[s]);
				livelockFlag.setBit(newState, livelockFlag[s]);

				/**
				 * Record the number of the new state so that a
				 * transition from the current state s can be added
				 * to it later.
				 */
				newStates.push_back(newState);

				/**
				 * Record that the tester process now has at least
				 * one tau-transition.
				 */
				hasTaus = true;

				/**
				 * For each of the transitions of the current state
				 * labeled with an action in the in the mirror
				 * subset "as", copy the transition to the new state.
				 */
				transitions.startAddingTransitionsToState(newState);
				for (AccSets::action_const_iterator a = mirror.begin(as);
					a != mirror.end(as); ++a)
				{
					InputLSTS::tr_const_iterator t = lsts.findTransition(state, *a);
					if (t != lsts.tr_end(state))
					{
						lsts_index_t d = lsts.getStateNumber(t->getDestinationState());
						transitions.addTransitionToState(*a, d);
					}
				}
				transitions.doneAddingTransitionsToState();
			}
		}

		/**
		 * Add transitions to the trace-reject state for
		 * those actions not in enabled(s) and the transitions of
		 * the original state that have not been inherited by
		 * some new sub-state.
		 */
		transitions.startAddingTransitionsToState(s);
		lsts_index_t expected_action = 1;
		for (InputLSTS::tr_const_iterator t = lsts.tr_begin(state);
				t != lsts.tr_end(state); ++t)
		{
			lsts_index_t a = t->getTransitionNumber();

			if (!inherited[a]) {
				transitions.addTransitionToState(a,
					lsts.getStateNumber(t->getDestinationState()));
			}
			while (a > expected_action)
			{
				addTraceRejectTransition(expected_action++);
			}
			++expected_action;
		}
		while (expected_action <= nrOfActions)
		{
			addTraceRejectTransition(expected_action++);
		}

		/**
		 * Add a tau-transitions from the current state
		 * to the new states.
		 */
		for (unsigned i = 0; i < newStates.size(); ++i)
		{
			transitions.addTransitionToState(0, newStates[i]);
		}
		transitions.doneAddingTransitionsToState();
	}
}

void
OutputLSTS::addTraceRejectTransition(const lsts_index_t& action)
{
	if (traceRejectState == 0) {
		++nrOfNewStates;
		traceRejectState = nrOfNewStates;
	}
	transitions.addTransitionToState(action, traceRejectState);
}

const AccSets
OutputLSTS::calculateMirror(const RO_AccSets& accsets)
{
	AccSets mirror(nrOfActions);
	lsts_index_t nrOfAccSets = accsets.setsAmount();

	if (nrOfAccSets == 1)
	{
		RO_AccSets::AccSetPtr as = accsets.firstAccSet();
		for (RO_AccSets::action_const_iterator a = accsets.begin(as);
			a != accsets.end(as); ++a)
		{
			mirror.createNewAccSet();
			mirror.addActionToNewAccSet(*a);
			mirror.addNewAccSet();
		}
	}
	else
	{
		vector<lsts_index_t> value;
		vector<lsts_index_t> start;
		vector<lsts_index_t> action;

		for (RO_AccSets::AccSetPtr as = accsets.firstAccSet();
			as != accsets.endAccSet(); ++as)
		{
			value.push_back(action.size());
			start.push_back(action.size());
			/*-- DEBUG BEGIN --*/
			/*
			cerr << '{';
			*/
			/*-- DEBUG END --*/
			for (RO_AccSets::action_const_iterator a = accsets.begin(as);
				a != accsets.end(as); ++a)
			{
				action.push_back(*a);
				/*-- DEBUG BEGIN --*/
				/*
				cerr << ' ' << *a;
				*/
				/*-- DEBUG END --*/
			}
			/*-- DEBUG BEGIN --*/
			/*
			cerr << " } (" << action.size() << ") ";
			*/
			/*-- DEBUG END --*/
		}
		/*-- DEBUG BEGIN --*/
		/*
		cerr << endl;
		*/
		/*-- DEBUG END --*/
		start.push_back(action.size());
		while (true)
		{
			/*-- DEBUG BEGIN --*/
			/*
			cerr << "Index: ";
			for (lsts_index_t j = 0; j < action.size(); ++j)
			{
				cerr.width(4); cerr << j;
			}
			cerr << endl << "Actns: ";
			for (lsts_index_t j = 0; j < action.size(); ++j)
			{
				cerr.width(4); cerr << action[j];
			}
			cerr << endl << "Cntr:  ";
			lsts_index_t k = 0;
			for (lsts_index_t j = 0; j < action.size(); ++j)
			{
				if (start[k] == j)
				{
					cerr.width(2); cerr << "| ";
				}
				else
				{
					cerr.width(2); cerr << "  ";
				}
				if (value[k] == j)
				{
					cerr.width(2); cerr << "^ ";
					++k;
				}
				else
				{
					cerr.width(2); cerr << "  ";
				}
			}
			cerr << endl;
			*/
			/*-- DEBUG END --*/
			mirror.createNewAccSet();
			for (lsts_index_t j = 0; j < nrOfAccSets; ++j)
			{
				mirror.addActionToNewAccSet(action[value[j]]);
			}
			mirror.addNewAccSet();
			lsts_index_t i = 0;
			do
			{
				++value[i];
				if (value[i] < start[i + 1]) { break; }
				++i;
			} while (i < nrOfAccSets);
			if (i == nrOfAccSets) { break; }
		}
	}
	return mirror;
}

void
OutputLSTS::calculateStateProps(StatePropsContainer& oprops)
{
	const StatePropsContainer& iprops = lsts.getStateProps();
	const lsts_index_t nrOfStateProps = iprops.getMaxStatePropNameNumber();

	/**
	 * State prop names.
	 */
	for (unsigned i = 1; i <= nrOfStateProps; ++i)
	{
		oprops.getStatePropName(i) = iprops.getStatePropName(i);
	}
	oprops.getStatePropName(nrOfStateProps + 1) = "/rej";
	oprops.getStatePropName(nrOfStateProps + 2) = "/dl_rej";
	oprops.getStatePropName(nrOfStateProps + 3) = "/ll_rej";

	/**
	 * State propositions.
	 */
	for (lsts_index_t s = 1; s <= nrOfNewStates; ++s)
	{
		StatePropsPtr sp = oprops.getStateProps(s);
		sp.unionAndAssign(iprops.getStateProps(s));
		if (s == traceRejectState)
		{
			sp.setProp(nrOfStateProps + 1);
			sp.setProp(nrOfStateProps + 2);
			sp.setProp(nrOfStateProps + 3);
		}
		if (deadlockFlag[s])
		{
			sp.setProp(nrOfStateProps + 2);
		}
		if (livelockFlag[s])
		{
			sp.setProp(nrOfStateProps + 3);
		}
		oprops.assignStateProps(s, sp);
	}
}

void OutputLSTS::writeLSTSFile(OutStream& os)
{
    oLSTS_File ofile;

    ofile.AddTransitionsWriter(transitions);
    ofile.AddActionNamesWriter(lsts.getActionNamesStore());

    StatePropsContainer props(lsts.getStateProps().getMaxVal() + 3);
    calculateStateProps(props);
    ofile.AddStatePropsWriter(props);

    Header headerData = lsts.getHeaderData();
    headerData.SetStateCnt(nrOfNewStates);
    headerData.SetStatePropCnt(headerData.GiveStatePropCnt() + 3);
    headerData.SetNoTaus(!hasTaus);
    headerData.SetDeterministic(!hasTaus);
    ofile.GiveHeader() = headerData;

    ofile.WriteFile(os);
}

/* vim: set tabstop=4 shiftwidth=4: */
