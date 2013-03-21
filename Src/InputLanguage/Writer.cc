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

Contributor(s): Juha Nieminen, Timo Erkkilä, Terhi Kilamo, Heikki Virtanen.
*/

#include "Writer.hh"

Writer::Writer(Process* process, bool debugMode, bool unusedGates)
  : myProcess(process), stateProps(NULL), debug(debugMode), unused(unusedGates)
{
  actionStore = new ActionNamesStore;
  transitions = new TransitionsContainer(true);
}

Writer::~Writer()
{
  if (debug) cout << "Deleting writer." << endl;
  if (actionStore)
    {
      delete actionStore; actionStore = NULL;
    }
  if (transitions)
    {
      delete transitions; transitions = NULL;
    }
  if (stateProps) 
    {
      delete stateProps; stateProps = NULL;
    }

  FinalStateList fslist;
  FinalTransitionList ftlist;

  myProcess->getFinalStates(fslist);
  FinalStateList::iterator fs = fslist.begin();
  while (fs != fslist.end())
    {
      (*fs)->getTransitions(ftlist);
      FinalTransitionList::iterator ft = ftlist.begin();
      while (ft != ftlist.end())
	{
	  delete *ft;
	  ftlist.erase(ft);
	  ft = ftlist.begin();
	}

      delete *fs;
      fslist.erase(fs);
      fs = fslist.begin();
    }

  if (debug) cout << "Writer deleted." << endl;
}

void Writer::calculateGatenames()
{
  GateList gates;
  myProcess->getGates(gates);

  GateList finalGates;

  // TAU is always the first gate
  finalGates.push_back("tau");

  // for each gate
  for (GateList::iterator gate = gates.begin(); gate != gates.end(); ++gate)
    {
      if (*gate != "tau")
	{
	  // see which parameter combinations are legal for this gate
	  vector<vector<EnumType> > legalParams;
	  myProcess->getLegalParams(*gate, legalParams);

	  // if no combinations were given, add the gate with no parameters to the finals
	  if (legalParams.size() == 0)
	    {
	      if (debug) cout << "Adding an implicit final gate: " << *gate << endl;
	      finalGates.push_back(*gate);
	    }
      
	  // for each legal combination
	  for (vector<vector<EnumType> >::iterator param = legalParams.begin(); 
	       param != legalParams.end(); 
	       ++param)
	    {
	      // in the first phase, there's only the unparameterized version of the gate
	      GateList tempGates, tempGates2;
	      tempGates.push_back(*gate);
	      
	      unsigned position = 0;
	      // for each variable of this combination
	      for (vector<EnumType>::iterator type = (*param).begin(); 
		   type != (*param).end(); 
		   ++type)
		{
		  // get the values of this enum type
		  EnumDefinition def = myProcess->getEnumDefinition(*type);
		  
		  // for each value of this variable
		  for (EnumDefinition::iterator value = def.begin(); value != def.end(); ++value)
		    {
		      // for each gate in the temp list
		      for (GateList::iterator tempGate = tempGates.begin(); 
			   tempGate != tempGates.end();
			   ++tempGate)
			{
			  // add this value to the gate and put it into the other temp list
			  tempGates2.push_back(Utils::addVarToGate(*tempGate, *value, position));
			}
		    }
		  
		  // now move the modified temp list to the original for the next variable to be
		  // replaced by it's values
		  tempGates.clear();
		  tempGates = tempGates2;
		  tempGates2.clear();
		  
		  ++position;
		}
	      
	      // the combination of parameters is complete. Now the temp gates are final
	      // and we are ready for the next combination
	      for (GateList::iterator i = tempGates.begin(); i != tempGates.end(); ++i)
		{
		  if (debug) cout << "Adding an explicit final gate: " << *i << endl;
		  finalGates.push_back(*i);
		}
	      tempGates.clear(); // just in case
	    }
	}
    }

  // now all the final gates are calculated. Replace the old gates with these.
  myProcess->setGates(finalGates);
}

void Writer::resetBeenBits()
{
  beenBits.clear();
  beenBitsF.clear();
}

void Writer::setBeenBit(FinalState* state)
{
  if (!beenHere(state))
    {
      beenBitsF.push_back(state);
    }
}
void Writer::setBeenBit(State* state)
{
  if (!beenHere(state))
    {
      beenBits.push_back(state);
    }
}

bool Writer::beenHere(FinalState* state)
{
  if (beenBitsF.end() == find(beenBitsF.begin(), beenBitsF.end(), state))
    {
      return false;
    }
  else
    {
      return true;
    }
}
bool Writer::beenHere(State* state)
{
  if (beenBits.end() == find(beenBits.begin(), beenBits.end(), state))
    {
      return false;
    }
  else
    {
      return true;
    }
}

void Writer::calculateVariables()
{
  resetBeenBits();
  VarList vars;
  StateList q;

  // see which variables are created in the initialization
  vector<Expression*> inits;
  myProcess->getInitialAssignments(inits);
  for (vector<Expression*>::iterator i = inits.begin(); i != inits.end(); ++i)
    {
      createVariablesRecursion(vars, *i);
    }

  State* initState = myProcess->getInitState();
  initState->setVariables(vars);
  setBeenBit(initState);
  q.push_back(initState);
  
  do
    {
      State* current = q[q.size()-1];
      q.pop_back();

      TransitionList trList;
      current->getTransitions(trList);
      for (TransitionList::iterator tr = trList.begin(); tr != trList.end(); ++tr)
	{
	  vars.clear();
	  current->getVariables(vars);

	  getCreatedVariables(vars, *tr);
	  
	  if (!beenHere( (*tr)->toState ) )
	    {
	      setBeenBit( (*tr)->toState );
	      (*tr)->toState->setVariables(vars);
	      q.push_back((*tr)->toState);
	    }
	  else
	    {
	      // check that the two sets of variables are the same
	      VarList targetVars;
	      (*tr)->toState->getVariables(targetVars);
	      if (vars.size() == targetVars.size())
		{
		  while (vars.size() > 0)
		    {
		      if (targetVars.end() != 
			  find(targetVars.begin(), targetVars.end(), *(vars.begin()) )
			 )
			{
			  vars.erase(vars.begin());
			}
		      else
			{
			  string errStr = "ERROR: I don't know whether variable ";
			  errStr += *(vars.begin());
			  errStr += " exists in state ";
			  errStr += (*tr)->toState->getName();
			  throw ILError(errStr);
			}
		    }
		}
	      else
		{
		  // TODO: a better message, which says which vars are different
		  string errStr = "ERROR: Different paths produce different variables in state ";
		  errStr += (*tr)->toState->getName();
		  throw ILError(errStr);
		}
	    }
	}
    }
  while (q.size() != 0);
}

void Writer::createVariablesRecursion(VarList& list, Expression* expr)
{
  // bottom of the recursion: if it's a variable, it should be created
  if (expr->isLeaf())
    {
      string temp = expr->getData();
      if (Utils::hasApostrophe(temp))
	{
	  temp = Utils::removeApostrophe(temp);
	}

      // if the variable doesn't exist, then this must be a constant value
      // otherwise, add it to the list
      if (myProcess->variableExists(temp))
	{
	  if (list.end() == find(list.begin(), list.end(), temp))
	    {
	      list.push_back(temp);
	    }
	}
    }
  else if (!expr->isLeaf())
    {
      // recursion
      for (unsigned i = 0; i < expr->getOperands().size(); ++i)
	{
	  createVariablesRecursion(list, expr->getOperands()[i]);
	}
    }
}

void Writer::getCreatedVariables(VarList& list, Transition* tr)
{
  if (tr->condition != NULL) createVariablesRecursion(list, tr->condition);

  for (unsigned k=0;k<tr->assignments.size();++k)
    {
      createVariablesRecursion(list, tr->assignments[k]);
    }

  for (unsigned i=0;i<tr->exclamations.size();++i)
    { 
      createVariablesRecursion(list, tr->exclamations[i]); 
    }

  for (unsigned j=0;j<tr->questions.size();++j)
    { 
      if (list.end() == find(list.begin(), list.end(), tr->questions[j]))
	{
	  list.push_back(tr->questions[j]);
	}
    }

  for(unsigned l=0;l<tr->deletes.size();++l)
    {
      VarList::iterator deletee = find(list.begin(), list.end(), tr->deletes[l]);

      if (list.end() != deletee)
	{
	  list.erase(deletee);
	}
      else
	{
	  string errStr = "ERROR: Tried to delete ";
	  errStr += *deletee;
	  errStr += ", but it doesn't exist.";
	  throw ILError(errStr);
	}
    }

}

void Writer::splitStates()
{
  StateList st;
  myProcess->getStates(st);

  // do the whole thing for all the states
  for (StateList::iterator state = st.begin(); state != st.end(); ++state)
    {
      // the new states will be stored here
      FinalStateList tempStates;

      // create the first final state, which has no variables enabled
      FinalState* f = new FinalState(*state);
      (*state)->addFinalState(f);

      // see which variables are enabled in this state
      VarList var;
      (*state)->getVariables(var);

      // and split the state for each of them
      for (VarList::iterator variable = var.begin(); variable != var.end(); ++variable)
	{
	  // get the current final states, which will all need to be split
	  FinalStateList finSt;
	  (*state)->getFinalStates(finSt);
	  for (FinalStateList::iterator finals = finSt.begin(); 
	       finals != finSt.end(); 
	       ++finals)
	    {
	      // see what values this variable has
	      EnumDefinition def = 
		myProcess->getEnumDefinition(myProcess->getVariableType(*variable));

	      // create a new final state for each value
	      for (EnumDefinition::iterator value = def.begin(); value != def.end(); ++value)
		{
		  FinalState* newState = new FinalState(*state);

		  // remember which variables were already enabled and their values
		  (*finals)->giveSameEnabledVariablesToAnother(newState);

		  // enable this variable and give it the correct value
		  newState->
		    enableVariable(*variable, 
				   myProcess->getEnumValue(myProcess->getVariableType(*variable),
							   *value));

		  newState->setInitialStateness( (*finals)->isInitialState() );

		  // store the new final state
		  tempStates.push_back(newState);
		}
	    }
	     
	  // if we created any new final states, move them from the temporary store to the
	  // state's list of final states
	  if (tempStates.size() > 0)
	    {
	      // the states that existed before the split are now outdated
	      (*state)->removeFinalStates();
	      
	      for (FinalStateList::iterator finals = tempStates.begin(); 
		   finals != tempStates.end(); 
		   ++finals)
		{
		  (*state)->addFinalState(*finals);
		}
	      
	      tempStates.clear();
	    }
	}
    }

  // now see that the process knows the final states, too
  StateList stList;
  myProcess->getStates(stList);
  for (StateList::iterator st = stList.begin(); st != stList.end(); ++st)
    {
      FinalStateList finalList;
      (*st)->getFinalStates(finalList);
      for (FinalStateList::iterator final = finalList.begin(); 
	   final != finalList.end(); 
	   ++final)
	{
	  if (debug) cout << "Adding a final state: " << (*final)->getName() << endl;
	  myProcess->addFinalState(*final);
	}
    }
}

bool Writer::checkConditions(Transition* tr, 
			     FinalState* from, 
			     FinalState* to,
			     unsigned gateNumber)
{
  // this will hold those variables that already appeared in the conditions,
  // so that we don't check that they say the same, as we do with those vars
  // which are not mentioned anywhere but are on in both states.
  VarList alreadyProcessed;

  ExpressionValue e;

  vector<unsigned*>::iterator ars = tr->assignRightSides.begin();
  vector<VarName*>::iterator als = tr->assignLeftSides.begin();

  for (vector<Expression*>::iterator assign = tr->assignments.begin();
       assign != tr->assignments.end(); ++assign)
    {
      if (*ars == NULL)
	{
	  if (debug) cout << "Evaluating generic assignment " << (*assign)->toString() << endl;

	  e = (*assign)->evaluate(from, to, myProcess, tr, gateNumber);
	  (*assign)->markHandledVars(tr);

	  if (e.logical == false)
	    {
	      if (debug) cout << "Assignment not ok: " << (*assign)->toString() << endl;
	      return false;
	    }
	}
      else
	{
	  if (debug) cout << "Checking real assignment " << (*assign)->toString() << endl;
	  if (to->getVariableValue(*(*als) ) != *(*ars) )
	    {
	      if (debug) cout << "Real assignment not ok: " << (*assign)->toString() << endl;
	      return false;
	    }
	  tr->handled.push_back(**als);
	}

      ++ars; ++als;
    }

  // add the variables that are deleted in this transition to the list of already
  // processed variables
  for (VarList::iterator del = tr->deletes.begin(); del != tr->deletes.end(); ++del)
    {
      tr->handled.push_back(*del);
    }
  
  // now see that the values of the other variables stay the same,
  // unless they have already been handled
  VarList fromVars, toVars;
  from->getVariables(fromVars);
  to->getVariables(toVars);

  for (VarList::iterator currVar = fromVars.begin(); currVar != fromVars.end(); ++currVar)
    {
      if (tr->handled.end() == find(tr->handled.begin(), 
					 tr->handled.end(),
					 *currVar))
	{
	  if (to->getVariableValue(*currVar) != from->getVariableValue(*currVar))
	    {
	      if (debug) cout << "It seems that the variable " << *currVar << " has different "
			      << "values in the states." << endl;

	      if (debug) cout << "Transition = " << &tr << endl;

	      return false;
	    }
	}
    }

  return true;
}

bool Writer::checkPrecondition(Transition* tr, FinalState* from)
{
  if (tr->condition)
    {
      ExpressionValue e = tr->condition->evaluate(from, NULL, myProcess);
      tr->condition->markHandledVars(tr);

      if (e.logical == false)
	{
	  if (debug) cout << "Condition not ok: " << tr->condition->toString() << endl;
	  return false;
	}
    }

  return true;
}

// checks those assignments that are really just an assignment, 
// that is, x' = <expression>, where x is any variable and 
// <expression> has no ':d variables. Those can be evaluated
// to x' = <constant> without knowing the toState
void Writer::evaluateRealAssignments(Transition* tr, FinalState* from)
{
  // first clear the previous results, since they're not correct anymore
  // because the from-state has changed
  vector<VarName*>::iterator alsit = tr->assignLeftSides.begin();
  while (alsit != tr->assignLeftSides.end())
    {
      if (*alsit != NULL) delete *alsit;
      tr->assignLeftSides.erase(alsit);
      alsit = tr->assignLeftSides.begin();
    }
  
  vector<unsigned*>::iterator arsit = tr->assignRightSides.begin();
  while (arsit != tr->assignRightSides.end())
    {
      if (*arsit != NULL) delete *arsit;
      tr->assignRightSides.erase(arsit);
      arsit = tr->assignRightSides.begin();
    }

  for (vector<Expression*>::iterator a = tr->assignments.begin(); a != tr->assignments.end(); ++a)
    {
      VarName* ls = NULL;
      unsigned* rs = NULL;
      
      (*a)->isRealAssignment(from, myProcess, ls, rs);
      tr->assignLeftSides.push_back(ls);
      tr->assignRightSides.push_back(rs);

      if (debug && ls != NULL && rs != NULL)
	{
	  cout << "Evaluating a real assignment: " << (*a)->toString() << endl;
	  cout << "The right side evaluated to " << *rs << endl;
	}
    }
}

bool Writer::createTransition(FinalState* fromfstate, 
			      FinalState* tofstate, 
			      Transition* trans,
			      GateName* gate)
{
  // create the transition
  FinalTransition* newTrans = new FinalTransition;
  newTrans->fromState = fromfstate;
  newTrans->toState = tofstate;
  newTrans->gate = *gate;

  // add the parameter values to the names of the gates
  vector<unsigned>::iterator exclpos = 
    trans->exclamationPositions.begin();
  vector<Expression*> excs = trans->exclamations;
  for (vector<Expression*>::iterator i = excs.begin(); 
       i != excs.end(); 
       ++i)
    {
      ExpressionValue e = (*i)->
	evaluate(fromfstate, tofstate, myProcess);

      if (e.type == LOGICAL)
	{
	  e.type = ENUM; e.enumType = 0; e.enumValue = e.logical;
	}

      EnumValue val = myProcess->
	getEnumValueName(e.enumType, e.enumValue);

      newTrans->gate = 
	Utils::addVarToGate(newTrans->gate, 
			    val, 
			    *exclpos);
      exclpos++;
    }

  if (checkQuestions(trans, newTrans->gate, tofstate))
    {
      if ( true ) // TODO: check for cut state
	{
	  fromfstate->addTransition(newTrans);
	  if (debug) cout << "Transition added." << endl;
	  return true;
	}
      else
	{
	  string e = "Error: No transitions are allowed "
	    "from the cut state ";
	  e += fromfstate->getMother()->getName();
	  throw ILError(e);
	}
    }
  else
    {
      // it wasn't added, delete it
      delete newTrans;
      return false;
    }
}

void Writer::addTransitions()
{
  if (debug) cout << "In addtransitions." << endl;
  StateList stList;
  myProcess->getStates(stList);

  for (StateList::iterator state = stList.begin(); state != stList.end(); ++state)
    {
      TransitionList trList;
      (*state)->getTransitions(trList);

      for (TransitionList::iterator trans = trList.begin(); trans != trList.end(); ++trans)
	{
	  // if there are any ?:s which go to a state where the variable will be deleted,
	  // handle them by creating extra gates
	  unfoldQuestions(*trans);
	}
    }

  resetBeenBits();

  FinalStateList fsList, fsList2;
  myProcess->getFinalStates(fsList2);

  myProcess->getInitStates(fsList);
  FinalStateList::iterator fromfstate = fsList.begin();
  while (fromfstate != fsList.end())
    {
      if (!beenHere(*fromfstate))
	{
	  setBeenBit(*fromfstate);
	  
	  if (debug) cout << "FromState: " << (*fromfstate)->getName() << endl;

	  TransitionList trList;
	  (*fromfstate)->getMother()->getTransitions(trList);
	  if (debug) cout << "This state has " << trList.size() << " transitions." << endl;
	  for (TransitionList::iterator trans = trList.begin(); trans != trList.end(); ++trans)
	    {
	      if (debug) cout << "Checking preconditions." << endl;
	      if (checkPrecondition(*trans, *fromfstate))
		{
		  if (debug) cout << "Preconds true. Evaluating real assignments." << endl;
		  evaluateRealAssignments(*trans, *fromfstate);

		  (*trans)->toState->getFinalStates(fsList2);
		  if (debug) cout << "The target state has " << fsList2.size() 
				  << " final states." << endl;

		  for (FinalStateList::iterator tofstate = fsList2.begin();
		       tofstate != fsList2.end();
		       ++tofstate)
		    {
		      if (debug) cout << "ToState: " << (*tofstate)->getName() << endl;

		      // we might have multiple gates if we had any ?'s
		      GateList gates = (*trans)->gates;
		      unsigned gateNumber = 0;
		      if (debug) cout << "The transition has " << gates.size() 
				      << " gates." << endl;

		      for (GateList::iterator j = gates.begin(); j != gates.end(); ++j)
			{
			  if (debug) cout << "Checking postconditions." << endl;
			  if ( checkConditions(*trans, *fromfstate, *tofstate, gateNumber) )
			    {
			      // create the transition
			      // if it gets created, add the target state to the list of 
			      // states to handle
			      if (debug) cout << "Creating the transition." << endl;

			      if (createTransition(*fromfstate, *tofstate, *trans, &(*j)))
				{
				  if (debug) cout << "Transition created, adding the target state"
					       " to the stack" << endl;

				  // this screws up the iterator fromfstate, but we know
				  // that the iterator should always be fsList.begin() anyway
				  // so just set it as fsList.begin() again
				  fsList.push_back(*tofstate);
				  fromfstate = fsList.begin();
				}
			    }

			  if (debug) cout << "Next gate." << endl;
			  ++gateNumber;
			}
		    }
		}
	    }
	}
      if (debug) cout << "From state " << (*fromfstate)->getName() 
		      << " handled, erasing." << endl;
      fsList.erase(fromfstate);
      fromfstate = fsList.begin();
    }
}

void Writer::unfoldQuestions(Transition* tr)
{
  vector<GateName> tempGates;

  vector<unsigned>::iterator qpos = tr->questionPositions.begin();
  for (VarList::iterator i = tr->questions.begin(); i != tr->questions.end(); ++i)
    {
      tr->handled.push_back(*i);

      EnumDefinition e = myProcess->getEnumDefinition(myProcess->getVariableType(*i));
      for (EnumDefinition::iterator var = e.begin(); var != e.end(); ++var)
	{
	  for (GateList::iterator j = tr->gates.begin(); j != tr->gates.end(); ++j)
	    {
	      tempGates.push_back(Utils::addVarToGate(*j, *var, *qpos));
	    }
	}

      tr->gates = tempGates;
      tempGates.clear();

      ++qpos;
    }
}

bool Writer::checkQuestions(Transition* tr, GateName gate, FinalState* to)
{
  vector<unsigned>::iterator qpos = tr->questionPositions.begin();
  for (VarList::iterator i = tr->questions.begin(); i != tr->questions.end(); ++i)
    {
      if (to->hasVariable(*i))
	{
	  if (to->getVariableValue(*i) != 
	      myProcess->getEnumValue(myProcess->getVariableType(*i),  
				      Utils::getVarFromGate(gate, *qpos)))
	    {
	      return false;
	    }
	}

      ++qpos;
    }

  return true;
}

void Writer::updateOnPropList(PropList& list, Transition* tr)
{
  State* to = tr->toState;

  PropList temp;
  to->getSwitchOns(temp);
  for (PropList::iterator i = temp.begin(); i != temp.end(); ++i)
    {
      // if it's on the no_switch list, don't switch
      if (tr->noSwitches.end() == find(tr->noSwitches.begin(), tr->noSwitches.end(), *i))
	{
	  // is it already on
	  if ( list.end() == find(list.begin(), list.end(), *i))
	    {
	      list.push_back(*i);
	    }
	  else
	    {
	      string errStr = "ERROR: Tried to switch on ";
	      errStr += *i;
	      errStr += " in state ";
	      errStr += to->getName();
	      errStr += ", but it was already on.";
	      throw ILError(errStr);
	    }
	}
    }

  // similar to the first part of the method
  to->getSwitchOffs(temp);
  for (PropList::iterator j = temp.begin(); j != temp.end(); ++j)
    {
      if (tr->noSwitches.end() == find(tr->noSwitches.begin(), tr->noSwitches.end(), *j))
	{
	  PropList::iterator result = find(list.begin(), list.end(), *j);
	  if (result != list.end())
	    {
	      list.erase(result);
	    }
	  else
	    {
	      string errStr = "ERROR: Tried to switch off ";
	      errStr += *j;
	      errStr += " in state ";
	      errStr += to->getName();
	      errStr += ", but it wasn't on.";
	      throw ILError(errStr);
	    }
	}
    }
}

void Writer::calculateStateProps()
{
  StateList q;
  State* current = myProcess->getInitState();
  PropList props;

  if (debug) cout << "Initial state = " << current->getName() << endl;

  resetBeenBits();
  setBeenBit(current);

  myProcess->getInitialSwitchOns(props);

  if (debug)
    {
      cout << "Initial switch ons: ";
      for (PropList::iterator p = props.begin(); p != props.end(); ++p)
	{
	  cout << *p;
	}
      cout << endl;
    }

  // init state might have some switches
  Transition* tempTr = new Transition;
  tempTr->toState = current;
  updateOnPropList(props, tempTr);
  delete tempTr;

  if (debug)
    {
      cout << "After applying the initial state's modifiers: ";
      for (PropList::iterator p = props.begin(); p != props.end(); ++p)
	{
	  cout << *p;
	}
      cout << endl;
    }

  current->setOnProps(props);
  q.push_back(current);

  do
    {
      current = q[q.size()-1];
      q.pop_back();

      if (debug) cout << "Current state = " << current->getName() << endl;

      TransitionList trList;
      current->getTransitions(trList);

      for (TransitionList::iterator tr = trList.begin(); tr != trList.end(); ++tr)
	{
	  if (debug) cout << "Transition to " << (*tr)->toState->getName() << endl;

	  props.clear();
	  current->getOnProps(props);

	  if (debug)
	    {
	      cout << "Before transition: ";
	      for (PropList::iterator p = props.begin(); p != props.end(); ++p)
		{
		  cout << *p;
		}
	      cout << endl;
	    }

	  updateOnPropList(props, *tr);

	  if (debug)
	    {
	      cout << "After transition: ";
	      for (PropList::iterator p = props.begin(); p != props.end(); ++p)
		{
		  cout << *p;
		}
	      cout << endl;
	    }

	  if (!beenHere( (*tr)->toState))
	    {
	      if (debug) cout << "Not been here." << endl;
	      setBeenBit( (*tr)->toState );
	      (*tr)->toState->setOnProps(props);
	      q.push_back( (*tr)->toState );
	    }
	  else
	    {
	      if (debug) cout << "Already been here. Checking similarity." << endl;
	      // check that the proplists are similar

	      PropList targetProps;
	      (*tr)->toState->getOnProps(targetProps);

	      if (debug)
		{
		  cout << "Already calculated: ";
		  for (PropList::iterator p = targetProps.begin(); p != targetProps.end(); ++p)
		    {
		      cout << *p;
		    }
		  cout << endl;
		}

	      if (props.size() == targetProps.size())
		{
		  while (props.size() != 0)
		    {
		      if (targetProps.end() != 
			  find(targetProps.begin(), targetProps.end(), *(props.begin()) )
			 )
		      {
			props.erase(props.begin());
		      }
		      else
			{
			  string errStr = "ERROR: I don't know whether ";
			  errStr += *(props.begin());
			  errStr += " should be on or off in the state ";
			  errStr += (*tr)->toState->getName();
			  throw ILError(errStr);
			}
		    }
		}
	      else
		{
		  string errStr = "ERROR: Different paths give different enabled propositions"
		                  " in state ";
		  errStr += (*tr)->toState->getName();
		  throw ILError(errStr);
		}
	    }
	}
    }
  while (q.size() != 0);

  // now switches have been handled. Next stop: heres
  StateList sList;
  myProcess->getStates(sList);

  for (StateList::iterator state = sList.begin(); state != sList.end(); ++state)
    {
      FinalStateList fsList;
      (*state)->getFinalStates(fsList);

      PropList hereOns, hereOffs, alreadyOn;
      vector<Expression*> offConds, onConds;

      (*state)->getHereOns(hereOns);
      (*state)->getHereOffs(hereOffs);
      (*state)->getOnProps(alreadyOn);
      offConds = (*state)->getHereOffConditions();
      onConds = (*state)->getHereOnConditions();

      for (FinalStateList::iterator fstate = fsList.begin(); fstate!= fsList.end(); ++fstate)
	{
	  PropList::iterator i;

	  for (i = alreadyOn.begin(); i != alreadyOn.end(); ++i)
	    {
	      (*fstate)->setProp(*i, true);
	    }

	  ExpressionValue val;
	  vector<Expression*>::iterator j = onConds.begin();
	  for (i = hereOns.begin(); i != hereOns.end(); ++i)
	    {
	      if (*j)
		{
		  val = (*j)->evaluate(*fstate, NULL, myProcess);
		} 

	      if (!(*j) || (val.type == LOGICAL && val.logical == true) )
		{
		  (*fstate)->setProp(*i, true);
		}

	      ++j;
	    }

	  j = offConds.begin();
	  for (i = hereOffs.begin(); i != hereOffs.end(); ++i)
	    {
	      if (*j)
		{
		  val = (*j)->evaluate(*fstate, NULL, myProcess);
		}

	      if (!(*j) || (val.type == LOGICAL && val.logical == true) )
		{
		  (*fstate)->setProp(*i, false);
		}
	    }
	}
    }
}

void Writer::findInitStates()
{
  State* state = myProcess->getInitState();

  FinalStateList list;
  vector<Expression*> assigns;
  myProcess->getInitialAssignments(assigns);
  state->getFinalStates(list);
  for (FinalStateList::iterator fs = list.begin(); fs != list.end(); ++fs)
    {
      bool good = true;
      for (vector<Expression*>::iterator ass = assigns.begin(); ass != assigns.end(); ++ass)
	{
	  ExpressionValue e = (*ass)->evaluate(NULL, *fs, myProcess);

	  if (!e.logical)
	    {
	      good = false;
	    }
	}

      if (good)
	{
	  myProcess->addInitState(*fs);
	}
    }
}
void Writer::findReachableStates()
{
  resetBeenBits();
  FinalStateStack s;

  FinalStateList initstates;
  myProcess->getInitStates(initstates);

  for (FinalStateList::iterator is = initstates.begin(); is != initstates.end(); ++is)
    {
      if (debug) cout << "Added initial state to the stack: " << (*is)->getName() << endl;
      s.push_back(*is);
    }

  FinalState* curr;
  while (s.size() > 0)
    {
      curr = s[s.size()-1];
      s.pop_back();
      if (debug) cout << "Handling " << curr->getName() << endl;

      if (!beenHere(curr))
	{
	  setBeenBit(curr);
	  curr->setReachable();
        
	  FinalTransitionList list;
	  curr->getTransitions(list);
	  for (FinalTransitionList::iterator i = list.begin(); i != list.end(); ++i)
	    {
	      if (debug) cout << "Transition to " << (*i)->toState->getName() 
		   << ", added to the stack" << endl;
	      s.push_back( (*i)->toState);
	    }
	}
    }
}

void Writer::markUsedGates()
{
  FinalStateList states;
  myProcess->getFinalStates(states);
  for (FinalStateList::iterator i = states.begin(); i != states.end(); ++i)
    {
      if ( (*i)->isReachable() )
	{
	  FinalTransitionList ft;
	  (*i)->getTransitions(ft);
	  for (FinalTransitionList::iterator j = ft.begin(); j != ft.end(); ++j)
	    {
	      myProcess->gateUsed( (*j)->gate );
	    }
	}
    }
}

void Writer::convertToLSTS()
{
  if (debug) cout << "******* Converting to LSTS *******" << endl;

  if (debug) cout << "Calculating gate names" << endl;
  calculateGatenames();

  if (debug) cout << "Calculating variables" << endl;
  calculateVariables();
  
  if (debug) cout << "Splitting states according to the variables." << endl;
  splitStates();

  if (debug) cout << "Calculating state props" << endl;
  calculateStateProps();
  
  if (debug) cout << "Finding init states" << endl;
  findInitStates();

  if (debug) cout << "Adding transitions to the final states." << endl;
  addTransitions();

  if (debug) cout << "Reachability analysis" << endl;
  findReachableStates();

  //  if (debug) cout << "Removing unused gates" << endl;
  if (!unused) markUsedGates();
  
  // write the transitions to the transitioncontainer
  if (debug) cout << "Writing transitions" << endl;

  FinalStateList states;
  myProcess->getFinalStates(states);

  unsigned stateCount = 0;
  for (FinalStateList::iterator i = states.begin(); i != states.end(); ++i)
    {
      if ( (*i)->isReachable())
	{
	  transitions->startAddingTransitionsToState(++stateCount);
	  if (debug) cout << "Adding transitions from state number " << stateCount << endl;
      
	  FinalTransitionList finalTrans;
	  (*i)->getTransitions(finalTrans);

	  for (FinalTransitionList::iterator j = finalTrans.begin(); 
	       j != finalTrans.end(); 
	       ++j)
	    {
	      if (unused)
		{
		  if (debug) cout << "Adding transition from " << (*i)->getName() << " to "
				  << (*j)->toState->getName() << ", gate number " 
				  << myProcess->getGateNumber((*j)->gate) << endl;

		  if (debug) cout << "Gate's name was " << (*j)->gate << endl;
		  if (debug) cout << "Target state's number is " 
				  << myProcess->getFinalStateNumber( (*j)->toState, true) << endl;

		  transitions->
		    addTransitionToState(myProcess->getGateNumber((*j)->gate),
					 myProcess->getFinalStateNumber( (*j)->toState, true) );
		}
	      else
		{
		  if (debug) cout << "Adding transition from " << (*i)->getName() << " to "
				  << (*j)->toState->getName() << ", gate number " 
				  << myProcess->getUsedGateNumber((*j)->gate) << endl;

		  if (debug) cout << "Gate's name was " << (*j)->gate << endl;

		  if (debug) cout << "Target state's number is " 
				  << myProcess->getFinalStateNumber( (*j)->toState, true) << endl;

		  transitions->
		    addTransitionToState(myProcess->getUsedGateNumber((*j)->gate),
					 myProcess->getFinalStateNumber( (*j)->toState, true) );
		}

	    }
	  
	  transitions->doneAddingTransitionsToState();
	}
    }

  if (debug) cout << "Writing state props" << endl;

  PropList statePropNames;
  myProcess->getStateProps(statePropNames, true);

  if (!debug) stateProps = new StatePropsContainer(statePropNames.size());
  if (debug)  
    {
      cout << "Constructor gets: " << statePropNames.size() +
	myProcess->getFinalStateCount() << endl;
      stateProps = new StatePropsContainer(statePropNames.size() + 
						   myProcess->getFinalStateCount());
    }

  // set the state property names
  for (PropList::iterator i = statePropNames.begin(); i != statePropNames.end(); ++i)
    {
      string& s = stateProps->getStatePropName(myProcess->getPropNumber(*i)+1);
      s = *i;
      if (debug) cout << "Set the name of state prop number " 
		      << myProcess->getPropNumber(*i)+1
		      << " to " << *i << endl;
    }

  if (debug) cout << "Writing states..." << endl;
  unsigned count = 0;
  FinalStateList statelist;
  myProcess->getFinalStates(statelist);
  for (FinalStateList::iterator state = statelist.begin();
       state != statelist.end(); ++state)
    {
      if ( (*state)->isReachable() )
	{
	  StatePropsContainer::StatePropsPtr p = stateProps->getStateProps(++count);

	  if (debug) cout << "Writing state number " << count << ": " 
			  << (*state)->getName() << endl;

	  if (debug)
	    {
	      // in debug mode, create property names for each state name
	      // and set them on in the corresponding states
	      string& s = stateProps->getStatePropName(statePropNames.size() + count);
	      s = (*state)->getName();

	      p.setProp(statePropNames.size() + count);
	    }

	  PropList pList = (*state)->getOnProps();
	  for (PropList::iterator prop = pList.begin(); prop != pList.end(); ++prop)
	    {
	      if (debug) cout << pList.size() << " props." << endl;

	      if (debug) cout << "Writing prop: " << *prop << "(number "
			      << myProcess->getPropNumber(*prop)+1 << ")" << endl;
	      p.setProp(myProcess->getPropNumber(*prop) + 1 );
	    }
	}
    }

  olsts.AddActionNamesWriter( *actionStore );
  olsts.AddTransitionsWriter( *transitions );
  olsts.AddStatePropsWriter( *stateProps );

  if (debug) cout << "Writers created" << endl;

  hd = &olsts.GiveHeader();
  hd->SetStateCnt(myProcess->getReachableFinalStateCount());

  // if we don't have any states, we don't have an init state
  if (myProcess->getReachableFinalStateCount() > 0)
    {
      FinalStateList list;
      myProcess->getInitStates(list);

      if (list.size() > 1)
	{
	  std::cerr << "Warning: more than one init state, using the first one." << endl;
	}

      if (list.size() == 0)
	{
	  string s = "Error: no init states.";
	  throw ILError(s);
	}

      hd->SetInitialState(myProcess->getFinalStateNumber(list[0], true));
      //      for (FinalStateList::iterator is = list.begin(); is != list.end(); ++is)
      //	{
      //	  hd->SetInitialState(myProcess->getFinalStateNumber(*is, true));
      //	}
    }

  if (unused)
    {
      hd->SetActionCnt(myProcess->getGateCount()-1);
    }
  else
    {
      hd->SetActionCnt(myProcess->getUsedGateCount()-1);
    }

  // write the action names to the action store
  actionStore->lsts_StartActionNames(*hd);

  if (unused)
    {
      //  for (unsigned i = 1; i < myProcess->getUsedGateCount(); ++i)
      for (unsigned i = 1; i < myProcess->getGateCount(); ++i)
	{
	  if (debug) cout << "gate " << i << ":" << myProcess->getGateName(i) << endl;
	  GateName temp = myProcess->getGateName(i);
	  actionStore->lsts_ActionName(i, temp);
	}
    }
  else
    {
      for (unsigned i = 1; i < myProcess->getUsedGateCount(); ++i)
	{
	  if (debug) cout << "gate " << i << ":" << myProcess->getUsedGateName(i) << endl;
	  GateName temp = myProcess->getUsedGateName(i);
	  actionStore->lsts_ActionName(i, temp);
	}
    }
  actionStore->lsts_EndActionNames();

  if (debug) cout << "Conversion done." << endl;

  myProcess->checkUnused();
}

//void Writer::writeOutputFile(string outputFilename)
void Writer::writeOutputFile(OutStream& output)
{
  if (debug) cout << "Writing the actual file..." << endl;

  // Write the LSTS-file using lsts-i/o-library
  // OutStream outfile(outputFilename, true);
  olsts.WriteFile(output);  
  
  if (debug) cout << "Output file succesfully written." << endl;

  if (debug) cout << Expression::evaluations << " expressions evaluated." << endl;
  if (debug) cout << myProcess->enumValueTimes << " enum values searched." << endl;
}
