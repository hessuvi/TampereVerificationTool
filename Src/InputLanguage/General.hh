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
// Input language compiler: General.hh
// by Timo Kivelä
//
// This file has some general constants, type definitions and
// small classes used by other input language compiler
// classes. The methods of those small classes are all implemented
// in General.cc

#ifndef IL_GENERAL_HH
#define IL_GENERAL_HH

#include <string>
#include <vector>
#include <algorithm> // find
#include <iostream>

// The amount of different operators.
#define OPERATORS 17

using std::string;
using std::vector;
using std::find;
using std::cout;
using std::endl;

// Later other data types might be added
#define EXPR_TYPES 4
enum ExpressionValueType
{
  UNKNOWN_VALUE = 0,
  LOGICAL,
  PROP,
  ENUM
};

enum OperatorType
{
  UNKNOWN_OPERATOR = 0,
  INC,
  DEC,
  EQUALS,
  NOTEQUAL,
  GREATERTHAN,
  LESSTHAN,
  AND,
  OR,
  NOT,
  INC_NO_WRAP,
  DEC_NO_WRAP,
  GREATER_EQUAL,
  LESS_EQUAL
};

enum PropType
{
  NORMALPROP = 0,
  STICKYPROP,
  CUTPROP
};

class Transition;
class State;
class FinalState;
class FinalTransition;
class Expression;

// Type definitions to make the program look neater and also
// to make it easier to change some of the containers for example
// from vector to map
typedef string GateName;
typedef string StateName;
typedef string FinalStateName;
typedef string PropName;
typedef string VarName;
typedef string EnumValue;
typedef unsigned EnumType;

typedef vector<EnumValue> TypeDef;
typedef vector<EnumValue> EnumDefinition;
typedef vector<Transition*> TransitionList;
typedef vector<FinalTransition*> FinalTransitionList;
typedef vector<State*> StateList;
typedef vector<GateName> GateList;
typedef vector<FinalState*> FinalStateList;
typedef vector<FinalState*> FinalStateStack;
typedef vector<PropName> PropList;
typedef vector<VarName> VarList;
typedef vector<EnumValue> VarValueList;
typedef vector<FinalTransition*> FinalTransitionList;

// type tells us whether this is a logical value or an enum value
// then look the value from the appropriate fields. Don't trust
// the other values: if it's an enum value, "logical" might be uninitialized
struct ExpressionValue
{
  ExpressionValueType type;
  bool logical;
  EnumType enumType;
  unsigned enumValue;
};

// describes a transition of the input language
struct Transition
{
  // the positions below mean the position of the corresponding ! or ?
  // in the parameter list. For example, if an action is 
  // a?x!y?z, then the position of ?x is 0 and ?z is 2.

  State* fromState;
  State* toState;
  vector<GateName> gates;
  Expression* condition;

  vector<Expression*> assignments;

  // if the assignment is a true assignment, these two will be used to store
  // the results of evaluating it
  vector<VarName*> assignLeftSides; 
  vector<unsigned*> assignRightSides;

  vector<Expression*> exclamations; // The stuff after an "!"
  vector<unsigned> exclamationPositions;
  VarList questions; // the stuff after a "?"
  vector<unsigned> questionPositions;
  VarList deletes; // variables that are deleted in this transition
  PropList noSwitches;
  VarList handled; // variables that have been mentioned in conds, they don't need to stay same
};

// describes a transition after the transformation, with no bells and whistles
struct FinalTransition
{
  FinalState* fromState;
  FinalState* toState;
  GateName gate;
};

// Some helper functions used here and there
class Utils
{
public:
  static bool hasApostrophe(const string& s);
  static string removeApostrophe(const string& s);
  static OperatorType getOperatorType(string operName);

  static GateName addVarToGate(GateName gate, string var, unsigned position);
  static EnumValue getVarFromGate(GateName gate, unsigned position);

  static const string exprTypes[EXPR_TYPES];
  static const string operators[OPERATORS];
};

// An error class
class ILError
{
public:
  ILError(string theMsg)
  {
    msg = theMsg;
  }

  ~ILError() {}

  string msg;
private:
};

// holds a final state's information
class FinalState
{
public:
  // A pointer to the underlying more complex state
  FinalState(State* mother);
  
  // name is constructed by mother's name and the 
  // variable values in this state
  string getName();

  void setInitialStateness(bool on);
  bool isInitialState();

  void enableVariable(VarName name, unsigned value);

  // is the variable enabled in this state
  bool hasVariable(const VarName& varName);
  unsigned getVariableValue(const VarName& varName);

  // gives all the enabled variables
  void getVariables(VarList& list);

  // after executing this, whomToGive will have the same
  // enabled variables that this one has
  void giveSameEnabledVariablesToAnother(FinalState* whomToGive);

  void addTransition(FinalTransition* tr);
  void getTransitions(FinalTransitionList& list);

  void setProp(PropName prop, bool on);
  bool getProp(PropName prop);
  PropList getOnProps();

  State* getMother();

  void setReachable();
  bool isReachable();

private:
  bool myInitialstateness;
  bool myReachability;
  VarList myVars;
  vector<unsigned> myVarValues;
  State* myMother;
  FinalTransitionList myTransitions;
  PropList myOnProps;
};

// holds an underlying states information
class State
{
public:
  State();
  void setName(StateName newName);
  StateName getName();

  // property changers
  void addSwitchOff(string prop);
  void addSwitchOn(string prop);
  void addHereOff(string prop, Expression* e);
  void addHereOn(string prop, Expression* e);
  void getSwitchOffs(PropList& list);
  void getSwitchOns(PropList& list);
  void getHereOffs(PropList& list);
  void getHereOns(PropList& list);
  vector<Expression*> getHereOffConditions();
  vector<Expression*> getHereOnConditions();

  // property methods
  void setProp(PropName propName, bool on);
  void setOnProps(PropList& list);
  void getOnProps(PropList& list);

  // final state methods
  void getFinalStates(FinalStateList& list);
  void addFinalState(FinalState* newState);
  void removeFinalStates();

  // setting and reading variables
  bool hasVariable(const VarName& varName);
  void addVariable(VarName varName);
  bool isDeletedHere(VarName varName);
  void getVariables(VarList& list);
  void setVariables(VarList& list);

  // setting and reading transitions that leave from this state
  void addTransition(Transition* tr);
  void getTransitions(TransitionList& list);

private:
  string myName;
  PropList mySwitchOffs, mySwitchOns, myHereOffs, myHereOns, myOnProps;
  vector<Expression*> myHereOffConditions, myHereOnConditions;
  TransitionList myTransitions;
  VarList myDeletes;
  VarList myEnabledVariables;
  FinalStateList myFinalStates;
};
#endif
