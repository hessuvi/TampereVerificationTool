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

// TVT-tools 
// Input language compiler: Process.hh
// by Timo Kivelä
// 
// This class stores a process of the kind that an input language
// file represents. This class also stores the temporary data
// structures generated when converting this kind of a process into
// an LSTS file. 

#ifndef PROCESS_STORE_HH
#define PROCESS_STORE_HH

#include "General.hh"
#include "Expression.hh"

using std::string;
using std::vector;

class Process
{
public:
  // ************ GENERAL STUFF ***********
  Process(bool debugMode = false);
  ~Process();

  void setProcessName(string newName);

/*
  // Gets the type and value of a string that could be
  // either a constant or a variable. The from and to are
  // needed to evaluate the value of a variable
  void getNumericalValue(string data, 
			 unsigned& value, 
			 unsigned& type, 
			 FinalState* from,
			 FinalState* to);
*/

  // displays a warning message if some of the variables, gates or props have
  // not been used
  void checkUnused();

  // ************ STATES ***********

  // isExplicit is true when the state is added as it's being introduced.
  // it's false when the state is added because there's a transition to it.
  void addState(StateName newStateName, bool isExplicit = true);
  bool stateExists(StateName stateName);
  unsigned getStateCount();
  unsigned getStateNumber(StateName stateName);
  void getStates(StateList& list);
  void checkImplicitStates();

  string getNewUnnamedState();

  // ********** FINAL STATES *********
  void addFinalState(FinalState* state);
  void removeFinalStates();
  unsigned getFinalStateCount();
  unsigned getReachableFinalStateCount();
  unsigned getFinalStateNumber(FinalState* state, bool countOnlyReachables = false);
  void getFinalStates(FinalStateList& list);

  // ******** INITIAL STATES *******

  // adds an initial state specified in the input language
  void setInitState(StateName state);
  State* getInitState();

  void addInitialAssignment(Expression* a);
  void getInitialAssignments(vector<Expression*>& list);

  // adds an initial final state
  void addInitState(FinalState* state);
  void getInitStates(FinalStateList& list);

  void addInitialSwitchOn(PropName prop);
  void getInitialSwitchOns(PropList& list);

  // ******** STATE PROPOSITIONS ***
  void addStateProp(PropName propName, PropType type);
  unsigned getPropNumber(PropName propName);
  void getStateProps(PropList& list, bool withTypes = false);
  
  // I don't know what this does!
  void addPropChange(StateName state, GateName gate, PropName prop, Expression* e = NULL);

  // ********* GATES *********
  void addGate(GateName newGateName);
  bool gateExists(GateName gateName);
  unsigned getGateCount();
  unsigned getGateNumber(GateName gateName);
  GateName getGateName(unsigned number);

  void getGates(GateList& list);
  void setGates(GateList& list);

  // adds a legal parameter combination for the gate
  void addGateParams(GateName gateName, vector<EnumType>& paramTypes);

  // gets all the legal parameter combinations of the gate
  void getLegalParams(GateName gateName, vector<vector<EnumType> >& list);

  // checks whether the parameter combination is legal for this gate
  bool checkGateParams(GateName gateName, vector<EnumType>& paramTypes);

  // these are used when handling only the gates that were used somewhere
  unsigned getUsedGateCount();
  unsigned getUsedGateNumber(GateName gate);
  GateName getUsedGateName(unsigned number);
  void gateUsed(GateName gate);

  // ******* TRANSITIONS ********
  void addTransition(StateName from, 
		     StateName to, 
		     GateName gateName, 
		     Expression* cond,
		     vector<Expression*>& assignments,
		     VarList& deletes,
		     vector<Expression*>& excls,
		     vector<unsigned>& exclpos,
		     VarList& quests,
		     vector<unsigned>& questpos,
		     PropList& noSwitches);
  void getTransitions(TransitionList& list);
  void getFinalTransitions(FinalTransitionList& list);

  // *********** VARIABLES ************

  // creates a new type definition and a new enum type
  void addTypeDef(string typeName, TypeDef& enumType);
  EnumType getTypeDefNumber(string typeName);

  bool typeExists(string typeName);
  bool constantExists(string constName);

  // creates a new enum type, but no variable of that type.
  EnumType createEnumType(EnumDefinition& typeInfo);

  // creates a variable. The type should be one given by createEnumType
  void addVariable(EnumType type, VarName name);

  bool variableExists(VarName name);
  EnumType getVariableType(VarName name); // integer type of a variable
  unsigned getVariableNumber(VarName name);
  EnumDefinition getEnumDefinition(EnumType type);

  EnumType getEnumType(EnumValue name); // integer type of a constant

  // gets the numerical value of the string value, for example, if the type 0 is
  // [first second third], getEnumValue(0, "third") will return 2.
  unsigned getEnumValue(EnumType type, EnumValue val);

  // the opposite of getEnumValue, gets the string value that corresponds to a 
  // numerical value
  EnumValue getEnumValueName(EnumType type, unsigned number);

  // How many elements there are in a type
  unsigned getEnumTypeSize(EnumType type);

  long enumValueTimes;
  
private:
  string myName;
  StateName myInitState;
  bool hasInitState;
  StateList myStates;
  vector<StateName> myImplicitStates;
  FinalStateList myFinalStates;
  GateList myGates;
  TransitionList myTransitions;
  FinalTransitionList myFinalTransitions;
  vector<Expression*> myInitialAssignments;

  PropList myStateProps;
  vector<PropType> myPropTypes;

  FinalStateList myInitStates;
  PropList initialSwitchOns;

  vector<string> myTypeDefNames;
  vector<EnumType> myTypeDefTypes;

  // each pair of these gives a legal gate,typedef -combination
  // if a gate name is not in the list, only unparameterized version is allowed
  // if the name is there, only the typedefs it has here are allowed
  vector<GateName> myLegalGateInfoGates;
  vector<vector<EnumType> > myLegalGateInfoTypes;

  vector<bool> myVariableUsed, myPropsUsed, myGatesUsed, myOriginalGatesUsed;
  
  vector<EnumDefinition> myEnumTypes;
  VarList myVariableNames;
  vector<EnumType> myVariableTypes;

  bool debug;

  long unnamedStateCount;
};

#endif
