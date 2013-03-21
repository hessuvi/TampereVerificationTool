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

#include <sstream>
#include "Process.hh"

Process::Process(bool debugMode)
  : enumValueTimes(0), hasInitState(false), debug(debugMode), unnamedStateCount(0)
{
  myGates.push_back("tau");
  myGatesUsed.push_back(true);
  myOriginalGatesUsed.push_back(true);
}

Process::~Process()
{
  if (debug) cout << "Deleting process." << endl;
  StateList::iterator i = myStates.begin();

  while (i != myStates.end())
    {
      // delete state proposition conditions
      vector<Expression*> conds = (*i)->getHereOffConditions();
      vector<Expression*>::iterator condIter = conds.begin();
      while (condIter != conds.end())
	{
	  if (*condIter) delete *condIter;
	  conds.erase(condIter);
	  condIter = conds.begin();
	}
      
      conds = (*i)->getHereOnConditions();
      condIter = conds.begin();
      while (condIter != conds.end())
	{
	  if (*condIter) delete *condIter;
	  conds.erase(condIter);
	  condIter = conds.begin();
	}

      // delete the state itself
      delete *i;
      myStates.erase(i);
      i = myStates.begin();
    }

  vector<Expression*>::iterator init = myInitialAssignments.begin();
  while (init != myInitialAssignments.end())
    {
      delete *init;
      myInitialAssignments.erase(init);
      init = myInitialAssignments.begin();
    }

  TransitionList::iterator t = myTransitions.begin();

  while (t != myTransitions.end())
    {
      if ((*t)->condition)
	{
	  delete (*t)->condition; (*t)->condition = NULL;
	}

      vector<Expression*>::iterator a = (*t)->assignments.begin();
      while (a != (*t)->assignments.end())
	{
	  delete *a;
	  (*t)->assignments.erase(a);
	  a = (*t)->assignments.begin();
	}

      vector<VarName*>::iterator als = (*t)->assignLeftSides.begin();
      while (als != (*t)->assignLeftSides.end())
	{
	  if (*als != NULL) delete *als;
	  (*t)->assignLeftSides.erase(als);
	  als = (*t)->assignLeftSides.begin();
	}

      vector<unsigned*>::iterator ars = (*t)->assignRightSides.begin();
      while (ars != (*t)->assignRightSides.end())
	{
	  if (*ars != NULL) delete *ars;
	  (*t)->assignRightSides.erase(ars);
	  ars = (*t)->assignRightSides.begin();
	}

      vector<Expression*>::iterator e = (*t)->exclamations.begin();
      while (e != (*t)->exclamations.end())
	{
	  delete *e;
	  (*t)->exclamations.erase(e);
	  e = (*t)->exclamations.begin();
	}

      delete *t;
      myTransitions.erase(t);
      t = myTransitions.begin();
    }

  if (debug) cout << "Process deleted." << endl;
}

void Process::setProcessName(string newName)
{
  myName = newName;
}

void Process::setInitState(StateName state)
{
  hasInitState = true;
  myInitState = state;

  if (!stateExists(state))
    {
      addState(state, false);
    }
}

unsigned Process::getEnumValue(EnumType e, EnumValue v)
{
  ++enumValueTimes;

  if (e == 0)
    {
      if (v == "FALSE") return 0;
      if (v == "TRUE") return 1;
    }

  if (e > myEnumTypes.size())
    {
      string s = "Internal error: tried to look for enum type that doesn't exist.";
      throw ILError(s);
      return 0;
    }

  for (unsigned i = 0; i < myEnumTypes[e-1].size(); ++i)
    {
      if (myEnumTypes[e-1][i] == v)
	{
	  return i+1;
	}
    }

  return 0;
}

unsigned Process::getEnumType(EnumValue v)
{
  if (v == "FALSE") return 0;
  if (v == "TRUE") return 0;

  for (unsigned i = 0; i < myEnumTypes.size(); ++i)
    {
      for (unsigned j = 0; j < myEnumTypes[i].size(); ++j)
	{
	  if (myEnumTypes[i][j] == v)
	    {
	      return i+1;
	    }
	}
    }

  return 0;
}

unsigned Process::getEnumTypeSize(EnumType e)
{
  if (e == 0) return 2;

  if (e <= myEnumTypes.size())
    {
      return myEnumTypes[e-1].size();
    }
  else
    {
      string s = "Internal error: tried to get an enum type's size, but it doesn't exist.";
      throw ILError(s);
      return 0;
    }
}

void Process::getTransitions(TransitionList& list)
{
  list = myTransitions;
}

void Process::getStates(StateList& list)
{
  list = myStates;
}

void Process::getFinalTransitions(FinalTransitionList& list)
{
  list = myFinalTransitions;
}

void Process::addInitialAssignment(Expression* a)
{
  //  createVariables(a, myStates[getStateNumber(myInitState)]);
  myInitialAssignments.push_back(a);
}

void Process::addInitState(FinalState* state)
{
  myInitStates.push_back(state);
}

void Process::getInitStates(FinalStateList& list)
{
  list = myInitStates;
}

void Process::addInitialSwitchOn(PropName prop)
{
  initialSwitchOns.push_back(prop);
}

void Process::getInitialSwitchOns(PropList& list)
{
  list = initialSwitchOns;
}

void Process::getInitialAssignments(vector<Expression*>& list)
{
  list = myInitialAssignments;
}

void Process::addGate(GateName newGateName)
{
  if (constantExists(newGateName))
    {
      string e = "Error: gate name ";
      e += newGateName;
      e += " already defined as a constant value name.";
      throw ILError(e);
    }

  if (typeExists(newGateName))
    {
      string e = "Error: gate name ";
      e += newGateName;
      e += " already defined as a type name.";
      throw ILError(e);
    }

  if (!gateExists(newGateName))
    {
      myGates.push_back(newGateName);
      myGatesUsed.push_back(false);
    }
}

bool Process::gateExists(GateName gateName)
{
  if (myGates.end() == find(myGates.begin(), myGates.end(), gateName))
    {
      return false;
    }
  else
    {
      return true;
    }
}

void Process::getGates(GateList& list)
{
  list = myGates;
}

void Process::setGates(GateList& list)
{
  myGates = list;

  myGatesUsed.clear();
  for (GateList::iterator i = myGates.begin(); i != myGates.end(); ++i)
    {
      // the first one is tau and it should always be used
      if (i == myGates.begin()) 
	{
	  myGatesUsed.push_back(true);
	}
      else
	{
	  myGatesUsed.push_back(false);
	}
    }
}

void Process::addState(StateName newStateName, bool isExplicit)
{
  if (constantExists(newStateName))
    {
      string e = "Error: state name ";
      e += newStateName;
      e += " already defined as a constant value name.";
      throw ILError(e);
    }

  if (typeExists(newStateName))
    {
      string e = "Error: state name ";
      e += newStateName;
      e += " already defined as a type name.";
      throw ILError(e);
    }

  if (gateExists(newStateName))
    {
      string e = "Error: state name ";
      e += newStateName;
      e += " already defined as a gate name.";
      throw ILError(e);
    }
  
  if (myStateProps.end() != find(myStateProps.begin(), myStateProps.end(), newStateName))
    {
      string e = "Error: state name ";
      e += newStateName;
      e += " already defined as a state property name.";
      throw ILError(e);
    }

  if (myVariableNames.end() != find(myVariableNames.begin(), myVariableNames.end(), newStateName))
    {
      string e = "Error: state name ";
      e += newStateName;
      e += " already defined as a variable name.";
      throw ILError(e);
    }

  if (!stateExists(newStateName))
    {
      State* s = new State;
      s->setName(newStateName);
      myStates.push_back(s);

      // if the state was created explicitly, remove it from the implicit list
      if (isExplicit)
	{
	  vector<StateName>::iterator i = 
	    find(myImplicitStates.begin(), myImplicitStates.end(), newStateName);
	  if (i != myImplicitStates.end())
	    {
	      myImplicitStates.erase(i);
	    }
	}
      else // if the state was created implicitly, add it to the implicit list
	{
	  // $-states are implicit states, they don't have to be added
	  if (newStateName[0] != '$')
	    {
	      vector<StateName>::iterator i =
		find(myImplicitStates.begin(), myImplicitStates.end(), newStateName);
	      if (i == myImplicitStates.end())
		{
		  myImplicitStates.push_back(newStateName);
		}
	    }
	}
    }
  else
    {
      if (isExplicit)
	{
	  vector<StateName>::iterator i = 
	    find(myImplicitStates.begin(), myImplicitStates.end(), newStateName);
	  if (i != myImplicitStates.end())
	    {
	      myImplicitStates.erase(i);
	    }
	  else
	    {
	      string s = "Error: Two states cannot have the same name.";
	      throw ILError(s);
	    }
	}
    }
}

void Process::checkImplicitStates()
{
  if (myImplicitStates.size() > 0)
    {
      string errMsg = "ERROR: The following states were implicitly defined:\n";
      for (vector<StateName>::iterator i = myImplicitStates.begin(); 
	   i != myImplicitStates.end();
	   ++i)
	{
	  errMsg += *i;
	  errMsg += " ";
	} 
      throw ILError(errMsg);
    }
}

string Process::getNewUnnamedState()
{
  std::ostringstream temp;

  temp << "$unnamed" << unnamedStateCount++;

  return temp.str();
}

void Process::addFinalState(FinalState* state)
{
  myFinalStates.push_back(state);
}

void Process::removeFinalStates()
{
  myFinalStates.clear();
}

unsigned Process::getFinalStateCount()
{
  return myFinalStates.size();
}

unsigned Process::getReachableFinalStateCount()
{
  unsigned count = 0;
  for (FinalStateList::iterator i = myFinalStates.begin(); i != myFinalStates.end(); ++i)
    {
      if ( (*i)->isReachable())
	{
	  ++count;
	}
    }

  return count;
}

bool Process::stateExists(StateName stateName)
{
  for (StateList::iterator i = myStates.begin(); i != myStates.end(); ++i)
    {
      if ((*i)->getName() == stateName)
	{
	  return true;
	}
    }

  return false;
}

void Process::addStateProp(PropName propName, PropType type)
{
  if (constantExists(propName))
    {
      string e = "Error: state property name ";
      e += propName;
      e += " already defined as a constant value name.";
      throw ILError(e);
    }

  if (typeExists(propName))
    {
      string e = "Error: state property name ";
      e += propName;
      e += " already defined as a type name.";
      throw ILError(e);
    }

  if (myGates.end() == find(myGates.begin(), myGates.end(), propName))
    {
      if (myStateProps.end() == find(myStateProps.begin(), myStateProps.end(), propName))
	{
	  myStateProps.push_back(propName);
	  myPropTypes.push_back(type);
	  myPropsUsed.push_back(false);
	}
      else
	{
	  string s = "Error: two state properties cannot have the same name: ";
	  s += propName;
	  throw ILError(s);
	}
    }
  else
    {
      string e = "Error: Property name ";
      e += propName;
      e += " already defined as a gate name.";
      throw ILError(e);
    }
}

void Process::addPropChange(StateName state, string type, PropName prop, Expression* e)
{
  unsigned stateNumber = getStateNumber(state);

  unsigned tmp = getPropNumber(prop);
  if (myPropsUsed.size() > tmp)
    {
      myPropsUsed[tmp] = true;
    }
  else
    {
      string e = "Error: Tried to use an undefined state proposition ";
      e += prop;
      throw ILError(e);
    }

  if (type == "SWITCH_OFF")
    {
      myStates[stateNumber]->addSwitchOff(prop);
    }
  else if (type == "SWITCH_ON")
    {
      myStates[stateNumber]->addSwitchOn(prop);
    }
  else if (type == "HERE_OFF")
    {
      myStates[stateNumber]->addHereOff(prop, e);
    }
  else if (type == "HERE_ON")
    {
      myStates[stateNumber]->addHereOn(prop, e);
    }
  else
    {
      string s = "Internal error: unrecognized prop change";
      throw ILError(s);
    }
}

void Process::addTransition(StateName from,
			    StateName to,
			    GateName gateName,
			    Expression* cond,
			    vector<Expression*>& assignments,
			    VarList& deletes,
			    vector<Expression*>& excls,
			    vector<unsigned>& exclpos,
			    VarList& quests,
			    vector<unsigned>& questpos,
			    PropList& noSwitches)
{
  if (gateName == "") { gateName = "tau"; }

  State* fromState = myStates[getStateNumber(from)];
  State* toState = myStates[getStateNumber(to)];

  unsigned gatenum = getGateNumber(gateName);
  if (gatenum < myGatesUsed.size())
    {
      myOriginalGatesUsed[gatenum] = true;
    }
  else
    {
      string e = "Internal error: getGateNumber returned too big a number";
      throw ILError(e);
    }

  // store the conditions, they will be checked later
  // add the transition
  Transition* tr = new Transition;

  tr->fromState = fromState;
  tr->toState = toState;
  
  tr->gates.push_back(gateName);

  tr->condition = cond;
  tr->assignments = assignments;
  tr->exclamations = excls;
  tr->exclamationPositions = exclpos;
  tr->questions = quests;
  tr->questionPositions = questpos;
  tr->deletes = deletes;
  tr->noSwitches = noSwitches;
  myTransitions.push_back(tr);
  fromState->addTransition(tr);
}

/*
void Process::getNumericalValue(string data, 
				unsigned& value, 
				unsigned& type, 
				FinalState* from,
				FinalState* to)
{
  unsigned icount = 0;
  for (vector<EnumDefinition>::iterator i = myEnumTypes.begin(); i != myEnumTypes.end(); ++i)
    {
      unsigned jcount = 0;
      for (EnumDefinition::iterator j = (*i).begin(); j != (*i).end(); ++j)
	{
	  if (*j == data)
	    {
	      value = jcount; type = icount;
	      return;
	    }
	  ++jcount;
	}
      ++icount;
    }

  // not an enum name, should be a variable
  if (Utils::hasApostrophe(data))
    {
      string temp = Utils::removeApostrophe(data);
      myVariableUsed[getVariableNumber(data)] = true;
      getNumericalValue(to->getVariableValue(temp), value, type, from, to);
    }
  else
    {
      myVariableUsed[getVariableNumber(data)] = true;
      getNumericalValue(from->getVariableValue(data), value, type, from, to);
    }
}
*/

EnumType Process::createEnumType(EnumDefinition& typeInfo)
{
  // if it's just a boolean type, don't create it, just return 0
  if (typeInfo.size() == 2 && typeInfo[0] == "FALSE" && typeInfo[1] == "TRUE")
    {
      return 0;
    }

  // TODO: when type checks are more advanced, remove this check
  // check that the new constant values are not already in use
  for (EnumDefinition::iterator i = typeInfo.begin(); i != typeInfo.end(); ++i)
    {
      for (vector<EnumDefinition>::iterator j = myEnumTypes.begin();
	   j != myEnumTypes.end();
	   ++j)
	{
	  for (EnumDefinition::iterator k = (*j).begin(); k != (*j).end(); ++k)

	    {
	      if ( *i == *k)
		{
		  string s = "Sorry, but in this version a constant enum value cannot"
		    "appear in two different types (";
		  s += *i;
		  s += ")";
		  throw ILError(s);
		}
	    }
	}
    }

  myEnumTypes.push_back(typeInfo);
  return myEnumTypes.size(); // the first one is 1
}

EnumValue Process::getEnumValueName(EnumType type, unsigned number)
{
  if (type > 0 && number > 0 && type <= myEnumTypes.size() && 
      number <= myEnumTypes[type-1].size())
    {
      return myEnumTypes[type-1][number-1];
    }
  else if (type == 0)
    {
      if (number == 0) return "FALSE";
      if (number == 1) return "TRUE";
      throw ILError("Internal error: getEnumValueName (boolean) got an odd parameter");
    }
  else
    {
      
      throw ILError("Internal error: getEnumValueName found an unexistent type");
    }
}

void Process::addVariable(EnumType type, VarName name)
{
  if (constantExists(name))
    {
      string e = "Error: variable name ";
      e += name;
      e += " already defined as a constant value name.";
      throw ILError(e);
    }

  if (typeExists(name))
    {
      string e = "Error: variable name ";
      e += name;
      e += " already defined as a type name.";
      throw ILError(e);
    }

  if (myGates.end() != find(myGates.begin(), myGates.end(), name))
    {
      string e = "Error: Variable name ";
      e += name;
      e += " already defined as a gate name.";
      throw ILError(e);
    }
      
  if (myStateProps.end() != find(myStateProps.begin(), myStateProps.end(), name))
    {
      string e = "Error: Variable name ";
      e += name;
      e += " already defined as a state property name";
      throw ILError(e);
    }
  
  if (myVariableNames.end() == find(myVariableNames.begin(), myVariableNames.end(), name))
    {
      myVariableNames.push_back(name);
      myVariableTypes.push_back(type);
      myVariableUsed.push_back(false);
    }
  else
    {
      string s = "Error: two variables cannot have the same name: ";
      s += name;
      throw ILError(s);
    }
}

unsigned Process::getStateNumber(StateName stateName)
{
  unsigned count = 0;
  for (StateList::iterator i = myStates.begin(); i != myStates.end(); ++i)
    {
      if (stateName == (*i)->getName())
	{
	  return count;
	}
      ++count;
    }

  string s = "Internal error: Tried to get a state number, but such state doesn't exist";
  throw ILError(s);

  return 0;
}

unsigned Process::getGateNumber(GateName gateName)
{
  unsigned count = 0;
  for (GateList::iterator i = myGates.begin(); i != myGates.end(); ++i)
    {
      if (gateName == *i)
	{
	  return count;
	}
      ++count;
    }

  string s = "Error: The specified gate '";
  s += gateName;
  s += "' doesn't exist.";
  throw ILError(s);

  return 0;
}

void Process::gateUsed(GateName gate)
{
  myGatesUsed[getGateNumber(gate)] = true;
}

unsigned Process::getUsedGateNumber(GateName gate)
{
  unsigned count = 0;
  vector<bool>::iterator usedGates = myGatesUsed.begin();

  for (GateList::iterator i = myGates.begin(); i != myGates.end(); ++i)
    {
      if (gate == *i)
	{
	  return count;
	}

      if (*usedGates)
	{
	  ++count;
	}
      ++usedGates;
    }

  string s = "Error: The specified gate '";
  s += gate;
  s += "' doesn't exist.";
  throw ILError(s);

  return 0;  
}

void Process::addGateParams(GateName gateName, vector<EnumType>& paramTypes)
{
  myLegalGateInfoGates.push_back(gateName);
  myLegalGateInfoTypes.push_back(paramTypes);
}

void Process::getLegalParams(GateName gateName, vector<vector<EnumType> >& list)
{
  vector<vector<EnumType> >::iterator type = myLegalGateInfoTypes.begin();

  for (GateList::iterator gate = myLegalGateInfoGates.begin();
       gate != myLegalGateInfoGates.end();
       ++gate)
    {
      if (*gate == gateName)
	{
	  list.push_back(*type);
	}

      ++type;
    }
}

bool Process::checkGateParams(GateName gateName, vector<EnumType>& paramTypes)
{
  vector<GateName>::iterator gate = myLegalGateInfoGates.begin();
  vector<vector<EnumType> >::iterator type = myLegalGateInfoTypes.begin();
  bool gateFound = false;

  while (gate != myLegalGateInfoGates.end())
    {
      if (*gate == gateName)
	{
	  gateFound = true;
	  // check whether the corresponding type is the same as paramTypes
	  vector<EnumType>::iterator i = (*type).begin();
	  vector<EnumType>::iterator j = paramTypes.begin();

	  bool isSame = true;
	  while (i != (*type).end() && j != paramTypes.end())
	    {
	      if (*i != *j)
		{
		  isSame = false;
		}
	      ++i; ++j;
	    }
	  if (i != (*type).end() || j != paramTypes.end())
	    {
	      isSame = false;
	    }

	  if (isSame) return true;
	}

      ++gate;
      ++type;
    }

  // the correct type was not found in the legal list
  // we still have the possibility that the gate was not mentioned
  // at all, when the version with no parameters is allowed
  if (!gateFound && paramTypes.size() == 0)
    {
      return true;
    }
  else
    {
      return false;
    }
}

unsigned Process::getStateCount()
{
  return myStates.size();
}

unsigned Process::getGateCount()
{
  return myGates.size();
}

unsigned Process::getUsedGateCount()
{
  unsigned count = 0;
  for (vector<bool>::iterator i = myGatesUsed.begin(); 
       i != myGatesUsed.end();
       ++i)
    {
      if (*i)
	{
	  ++count;
	}
    }

  return count;
}

GateName Process::getGateName(unsigned number)
{
  GateName temp = myGates[number];
  return temp;
}

GateName Process::getUsedGateName(unsigned usedGate)
{
  unsigned count = 0;
  vector<bool>::iterator used = myGatesUsed.begin();
  for (vector<GateName>::iterator gate = myGates.begin(); gate != myGates.end(); ++gate)
    {
      if (*used)
	{
	  if (count == usedGate)
	    {
	      return *gate;
	    }
	  else
	    {
	      ++count;
	    }
	}

      ++used;
    }

  string e = "Internal error: getUsedGateName - gate not found.";
  throw ILError(e);
}

State* Process::getInitState()
{
  if (hasInitState)
    {
      return myStates[getStateNumber(myInitState)];
    }
  else
    {
      string e = "Error: Init state must be specified";
      throw ILError(e);
    }
}

EnumDefinition Process::getEnumDefinition(EnumType type)
{
  if (type == 0)
    {
      EnumDefinition e;
      e.push_back("FALSE");
      e.push_back("TRUE");
      return e;
    }
  else if (type <= myEnumTypes.size())
    {
      return myEnumTypes[type-1];
    }
  else
    {
      std::cerr << "Warning: definition not found in Process::getEnumDefinition" << endl;
      EnumDefinition e;
      return e;
    }
}

void Process::addTypeDef(string typeName, TypeDef& enumType)
{
  if (typeExists(typeName))
  {
    string e = "Error: The type ";
    e += typeName;
    e += " has already been defined.";
    throw ILError(e);
  }
  else
  {
    if (enumType.size() == 2 && enumType[0] == "FALSE" && enumType[1] == "TRUE")
      {
	myTypeDefNames.push_back(typeName);
	myTypeDefTypes.push_back(0);
      }
    else
      {
	EnumType e = createEnumType(enumType);
	myTypeDefNames.push_back(typeName);
	myTypeDefTypes.push_back(e);
      }
  }
}

bool Process::typeExists(string typeName)
{
  if (find(myTypeDefNames.begin(), myTypeDefNames.end(), typeName) != myTypeDefNames.end())
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool Process::constantExists(string constName)
{
  for (vector<EnumDefinition>::iterator i = myEnumTypes.begin(); i != myEnumTypes.end(); ++i)
    {
      if (find( (*i).begin(), (*i).end(), constName) != (*i).end())
	{
	  return true;
	}
    }

  return false;
}

EnumType Process::getTypeDefNumber(string typeName)
{
  vector<string>::iterator i = myTypeDefNames.begin();
  vector<EnumType>::iterator j = myTypeDefTypes.begin();

  while (i != myTypeDefNames.end())
    {
      if (*i == typeName)
	{
	  return *j;
	}

      ++i;
      ++j;
    }

  string errStr = "ERROR: Tried to use an undefined type: ";
  errStr += typeName;
  errStr += ".";
  throw ILError(errStr);
  return 0;
}


bool Process::variableExists(VarName name)
{
  if (myVariableNames.end() == find(myVariableNames.begin(), myVariableNames.end(), name))
    {
      return false;
    }

  return true;
}

unsigned Process::getVariableType(VarName name)
{
  vector<EnumType>::iterator result = myVariableTypes.begin();
  vector<bool>::iterator used = myVariableUsed.begin();
  for (VarList::iterator i = myVariableNames.begin(); i != myVariableNames.end(); ++i)
    {
      if (*i == name)
	{
	  *used = true;
	  return (*result);
	}
      ++result;
      ++used;
    }

  string e = "Error: Variable ";
  e += name;
  e += " undefined.";
  throw ILError(e);

  return 0;
}

unsigned Process::getVariableNumber(VarName name)
{
  for (unsigned i = 0; i < myVariableNames.size(); ++i)
    {
      if (myVariableNames[i] == name)
	{
	  return i;
	}
    }

  string e = "Error: Variable ";
  e += name;
  e += " undefined.";
  throw ILError(e);

  return 0;
}

unsigned Process::getPropNumber(PropName propName)
{
  // if it's a property name with / or %, ignore the first char
  if (propName.size() != 0 && ( propName[0] == '/' || propName[0] == '%') )
    {
      propName = propName.substr(1, propName.size()-1);
    }

  unsigned count = 0;
  for (PropList::iterator i = myStateProps.begin(); i != myStateProps.end(); ++i)
    {
      if (*i == propName)
	{
	  return count;
	}

      ++count;
    }

  string e = "Error: State property ";
  e += propName;
  e += " not defined.";
  throw ILError(e);

  return 0;
}

void Process::getFinalStates(FinalStateList& list)
{
  list = myFinalStates;
}

unsigned Process::getFinalStateNumber(FinalState* state, bool countOnlyReachables)
{
  unsigned count = 0;
  for (FinalStateList::iterator i = myFinalStates.begin(); i != myFinalStates.end(); ++i)
    {
      if (state == *i)
	{
	  return ++count;
	}

      if (!countOnlyReachables || (*i)->isReachable())
	{
	  ++count;
	}
    }

  std::cerr << "Warning: state not found in Process::getFinalStateNumber" << endl;
  return 0;
}

void Process::getStateProps(PropList& list, bool withTypes)
{
  if (!withTypes)
    {
      list = myStateProps;
    }
  else
    {
      list.clear();
      
      vector<PropType>::iterator t = myPropTypes.begin();
      for (PropList::iterator i = myStateProps.begin(); i != myStateProps.end(); ++i)
	{
	  string temp = "";
	  if (*t == STICKYPROP)
	    {
	      temp += "/";
	    }
	  else if (*t == CUTPROP)
	    {
	      temp += "%";
	    }

	  temp += *i;

	  list.push_back(temp);

	  ++t;
	}
    }
}

void Process::checkUnused()
{
  vector<bool>::iterator i;
  VarList::iterator v = myVariableNames.begin();
  for (i = myVariableUsed.begin(); i != myVariableUsed.end(); ++i)
    {
      if (*i == false)
	{
	  std::cerr << "Warning: unused variable " << *v << endl;
	}
      ++v;
    }

  GateList::iterator g = myGates.begin();
  for (i = myOriginalGatesUsed.begin(); i != myOriginalGatesUsed.end(); ++i)
    {
      if (*i == false)
	{
	  // don't warn about gates with parameters, they don't get
	  // mark for being used but their existence proves that the
	  // original gate was used.
	  if (string::npos == (*g).find_first_of("<"))
	  {
	    std::cerr << "Warning: unused gate " << *g << endl;
	  }
	}
      ++g;
    }

  PropList::iterator p = myStateProps.begin();
  for (i = myPropsUsed.begin(); i != myPropsUsed.end(); ++i)
    {
      if (*i == false)
	{
	  std::cerr << "Warning: unused state property " << *p << endl;
	}
      ++p;
    }
}


