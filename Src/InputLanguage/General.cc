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
#include "General.hh"
/////////////////////////////////////////////////////////
/////////////////// UTILS ///////////////////////////////
/////////////////////////////////////////////////////////

const string Utils::operators[] = { "UNKNOWN", "INC", "DEC", "=", "!=", ">",
				    "<", "/\\", "\\/", "-", "AND", "OR", "NOT",
				    "INC_NO_WRAP", "DEC_NO_WRAP", ">=", "<=" };

const string Utils::exprTypes[] = { "UNKNOWN", "BOOLEAN", "PROPERTY", "ENUM" };

bool Utils::hasApostrophe(const string& s)
{
  if (s[s.size()-1] == '\'') 
    {
      return true;
    }
  else
    {
      return false;
    }

/*
  if (s.find_first_of("'") == std::string::npos)
    {
      return false;
    }
  else
    {
      return true;
    }
*/
}

string Utils::removeApostrophe(const string& s)
{
  return s.substr(0, s.size()-1);
}

OperatorType Utils::getOperatorType(string operName)
{
  if (operName == "INC") { return INC; }
  if (operName == "DEC") { return DEC; }
  if (operName == "=") { return EQUALS; }
  if (operName == "!=") { return NOTEQUAL; }
  if (operName == ">") { return GREATERTHAN; }
  if (operName == "<") { return LESSTHAN; }
  if (operName == "/\\") { return AND; }
  if (operName == "\\/") { return OR; }
  if (operName == "-") { return NOT; }
  if (operName == "AND") { return AND; }
  if (operName == "OR") { return OR; }
  if (operName == "NOT") { return NOT; }
  if (operName == "INC_NO_WRAP") { return INC_NO_WRAP; }
  if (operName == "DEC_NO_WRAP") { return DEC_NO_WRAP; }
  if (operName == ">=") { return GREATER_EQUAL; }
  if (operName == "<=") { return LESS_EQUAL; }
  return UNKNOWN_OPERATOR;
}


// this version produces stuff like gate<val1,val2,val3>
/*
GateName Utils::addVarToGate(GateName gate, string var, unsigned position)
{
  //cout << "Adding " << var << " to gate " << gate << " at position " << position << endl;
  bool added = false;
  GateName result = "";
  bool foundBegin = false;
  unsigned currentPlace = 0;

  // go through the original gate one character at a time
  for (unsigned i = 0; i < gate.length(); ++i)
    {
      // when we find the <, we know that the gate's name has ended
      // and the parameters begin
      if (gate[i] == '<')
	{
	  foundBegin = true;
	  result += '<';
	}

      // if this is the correct position for the variable we are adding,
      // do it
      if (foundBegin && !added && currentPlace == position)
	{
	  result += var;
	  added = true;
	}

      // add this to the result, unless it's a >
      if (gate[i] != '>' && gate[i] != '<') result += gate[i];

      // each , we find means that the place is for the next variable 
      if (foundBegin && gate[i] == ',')
	{
	  ++currentPlace;
	}

      // if we found the end of the variable list
      if (foundBegin && gate[i] == '>')
	{
	  // we still have to add the variable in the list before
	  // the ending >.
	  if (!added)
	    {
	      // add enough ,:s so the variable is in it's own place
	      while (currentPlace < position)
		{
		  result += ',';
		  ++currentPlace;
		}
	      
	      result += var;
	      added = true;
	    }

	  // now we can add the end sign to the result
	  result += '>';
	}
    }

  // this should happen only if there were no <>:s at all, which is
  // the first time that a variable is added to a gatename
  if (!added)
    {
      // first add the beginning <
      currentPlace = 0;
      result += '<';

      // then add enough ,:s
      while (currentPlace < position)
	{
	  result += ',';
	  ++currentPlace;
	}
      
      // and finally the variable and the end sign.
      result += var;
      result += '>';
    }

  //cout << "Result: " << result << endl;
  return result;
}
*/

// This version produces stuff like gate<val1><val2><val3>
GateName Utils::addVarToGate(GateName gate, string var, unsigned position)
{
  bool added = false;
  GateName result = "";
  unsigned found = 0;
  for (unsigned i = 0; i < gate.length(); ++i)
    {
      result += gate[i];

      if (gate[i] == '<')
	{
	  found++;
	}

      if (!added && found == position+1)
	{
	  result += var;
	  added = true;
	}
    }

  // not enough <>'s found
  if (!added)
    {
      for (; found < position; ++found)
	{
	  result += "<>";
	}
      
      result += '<';
      result += var;
      result += '>';
    }

  return result;
}

/*
// this version reads stuff like gate<a,b,c>
EnumValue Utils::getVarFromGate(GateName gate, unsigned position)
{
  //cout << "Getting var from gate " << gate << " at position " << position << endl;
  bool foundFirst = false;
  unsigned current = 0;
  EnumValue result = "";
  for(unsigned i = 0; i < gate.length(); ++i)
    {
      if (!foundFirst && gate[i] == '<')
	{
	  foundFirst = true;
	  ++i;
	}

      if (foundFirst && position == current)
	{
	  do
	    {
	      result += gate[i++];
	    }
	  while (gate[i] != ',' && gate[i] != '>' && i < gate.length()-1);

	  //cout << "Result: " << result << endl;
	  return result;
	}
     
      if (foundFirst && gate[i] == ',')
	{
	  ++current;
	}
    }

  string errStr = "Internal error: No such position in getVarFromGate.";
  throw ILError(errStr);
  return "";
}
*/

// this version reads stuff like gate<a><b><c>
EnumValue Utils::getVarFromGate(GateName gate, unsigned position)
{
  unsigned found = 0;
  EnumValue result = "";
  for (unsigned i = 0; i < gate.length(); ++i)
    {
      if (gate[i] == '<')
	{
	  if (found++ == position)
	    {
	      while (gate[++i] != '>')
		{
		  result += gate[i];
		}
	      return result;
	    }
	}
    }

  return "";
}


/////////////////////////////////////////////////////////
/////////////////// STATE ///////////////////////////////
/////////////////////////////////////////////////////////
State::State()
{
  myName = "";
}

StateName State::getName()
{
  return myName;
}

void State::addSwitchOff(PropName prop)
{
  for (PropList::iterator i = mySwitchOffs.begin(); i != mySwitchOffs.end(); ++i)
    {
      if (*i == prop) return;
    }
  mySwitchOffs.push_back(prop);
}
void State::addSwitchOn(PropName prop)
{
  for (PropList::iterator i = mySwitchOns.begin(); i != mySwitchOns.end(); ++i)
    {
      if (*i == prop) return;
    }
  mySwitchOns.push_back(prop);
}
void State::addHereOff(PropName prop, Expression* e)
{
  for (PropList::iterator i = myHereOffs.begin(); i != myHereOffs.end(); ++i)
    {
      if (*i == prop) return; // TODO: should we throw an error here?
    }
  myHereOffs.push_back(prop);
  myHereOffConditions.push_back(e);
}
void State::addHereOn(PropName prop, Expression* e)
{
  for (PropList::iterator i = myHereOns.begin(); i != myHereOns.end(); ++i)
    {
      if (*i == prop) return; // TODO: should we throw an error here
    }
  myHereOns.push_back(prop);
  myHereOnConditions.push_back(e);
}

void State::getSwitchOffs(PropList& list) { list = mySwitchOffs; }
void State::getSwitchOns(PropList& list) { list = mySwitchOns; }
void State::getHereOffs(PropList& list) { list = myHereOffs; }
void State::getHereOns(PropList& list) { list = myHereOns; }

vector<Expression*> State::getHereOffConditions() { return myHereOffConditions; }
vector<Expression*> State::getHereOnConditions() { return myHereOnConditions; }

void State::getOnProps(PropList& list)
{
  list = myOnProps;
}

void State::setOnProps(PropList& list)
{
  myOnProps = list;
}

void State::addFinalState(FinalState* newState)
{
  if (myFinalStates.end() == find(myFinalStates.begin(), myFinalStates.end(), newState))
    {
      myFinalStates.push_back(newState);
    }
}

void State::removeFinalStates()
{
  FinalStateList::iterator fs = myFinalStates.begin();

  while (fs != myFinalStates.end())
    {
      delete *fs;
      myFinalStates.erase(fs);
      fs = myFinalStates.begin();
    }
}

bool State::hasVariable(const VarName& variableName)
{
  for (VarList::iterator i = myEnabledVariables.begin(); i != myEnabledVariables.end(); ++i)
    {
      if (*i == variableName)
	{
	  return true;
	}
    }

  return false;
}

bool State::isDeletedHere(VarName variable)
{
  if (myDeletes.end() == find(myDeletes.begin(), myDeletes.end(), variable))
    {
      return false;
    }

  return true;
}

void State::setName(StateName newName)
{
  myName = newName;
}

void State::getFinalStates(FinalStateList& list)
{
  list = myFinalStates;
}

void State::addTransition(Transition* tr)
{
  if (myTransitions.end() == find(myTransitions.begin(), myTransitions.end(), tr))
  {
    myTransitions.push_back(tr);
  }
}

void State::getTransitions(TransitionList& list)
{
  list = myTransitions;
}

void State::addVariable(VarName variableName)
{
  if (!hasVariable(variableName))
    {
      myEnabledVariables.push_back(variableName);
    }
}

void State::getVariables(VarList& list)
{
  list = myEnabledVariables;
}

void State::setVariables(VarList& list)
{
  myEnabledVariables.clear();
  myEnabledVariables = list;
}

void State::setProp(PropName prop, bool on)
{
  for (FinalStateList::iterator fs = myFinalStates.begin(); fs != myFinalStates.end(); ++fs)
    {
      (*fs)->setProp(prop, on);
    }
}


/////////////////////////////////////////////////////////
/////////////////// FINALSTATE //////////////////////////
/////////////////////////////////////////////////////////

PropList FinalState::getOnProps()
{
  return myOnProps;
}

void FinalState::setProp(PropName propName, bool on)
{
  if (!getProp(propName))
    {
      if (on)
	{
	  myOnProps.push_back(propName);
	  return;
	}
    }
  else
    {
      if (!on)
	{
	  for (PropList::iterator i = myOnProps.begin(); i != myOnProps.end(); ++i)
	    {
	      if (*i == propName)
		{
		  myOnProps.erase(i);
		  return;
		}
	    }
	}
    }
}

bool FinalState::getProp(PropName propName)
{
  for (PropList::iterator i = myOnProps.begin(); i != myOnProps.end(); ++i)
    {
      if (*i == propName)
	{
	  return true;
	}
    }

  return false;
}

FinalStateName FinalState::getName()
{
  string temp = myMother->getName();
  
  for (vector<unsigned>::iterator i = myVarValues.begin(); i != myVarValues.end(); ++i)
    {
      std::ostringstream temp2;
      temp2 << "<" << *i << ">";
      temp += temp2.str();
    }

  return temp;
}

FinalState::FinalState(State* mother)
  : myReachability(false), myMother(mother)
{
}

void FinalState::enableVariable(VarName name, unsigned value)
{
  myVars.push_back(name);
  myVarValues.push_back(value);
}

bool FinalState::hasVariable(const VarName& name)
{
  if (myVars.end() == find(myVars.begin(), myVars.end(), name))
    {
      return false;
    }
  else
    {
      return true;
    }
}

State* FinalState::getMother()
{
  return myMother;
}

bool FinalState::isInitialState()
{
  return myInitialstateness;
}

void FinalState::setInitialStateness(bool on)
{
  myInitialstateness = on;
}

void FinalState::giveSameEnabledVariablesToAnother(FinalState* whomToGive)
{
  vector<unsigned>::iterator j = myVarValues.begin();
  for (VarList::iterator i = myVars.begin(); i != myVars.end(); ++i)
    {
      whomToGive->enableVariable(*i, *j);
      ++j;
    }
}

void FinalState::addTransition(FinalTransition* tr)
{
  myTransitions.push_back(tr);
}

void FinalState::getTransitions(FinalTransitionList& list)
{
  list = myTransitions;
}

void FinalState::getVariables(VarList& list)
{
  list = myVars;
}

unsigned FinalState::getVariableValue(const VarName& variable)
{
  vector<unsigned>::iterator result = myVarValues.begin();
  for (VarList::iterator i = myVars.begin(); i != myVars.end(); ++i)
    {
      if (*i == variable)
	{
	  return *result;
	}

      ++result;
    }

  std::cerr << "Warning: Undefined variable " << variable 
	    << " used in state " << getName() << endl;
  return 0;
}

void FinalState::setReachable()
{
  myReachability = true;
}

bool FinalState::isReachable()
{
  return myReachability;
}
