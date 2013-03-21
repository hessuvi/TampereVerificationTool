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
// Input language compiler: Writer.hh
// by Timo Kivelä
// 
// This class gets the data in a Process-class, removes the 
// variables and other features supported by the input language
// and converts them into a similarly behaving LSTS and then writes
// the data to an LSTS-file.

#ifndef INPUTLANGUAGECOMPILER_HH
#define INPUTLANGUAGECOMPILER_HH

#include "TransitionsContainer.hh"
#include "StateProps.hh"
#include "LSTS_File/ActionNamesStore.hh"
#include "LSTS_File/oLSTS_File.hh"

#include "General.hh"
#include "Process.hh"

class Writer
{
public:
  // Parameters: 
  // process = The process to convert into LSTS
  // debugMode = if true, prints out extra information
  // unusedGates = if false, only writes used action names
  Writer(Process* process, bool debugMode = false, bool unusedGates = true);
  ~Writer();

  // Do the conversion
  void convertToLSTS();

  // Write the actual file
  //  void writeOutputFile(string outputFilename);
  void writeOutputFile(OutStream& output);

private:
  void calculateGatenames();
  void calculateVariables();
  void recursiveVariables(State* state, Transition* trans, VarList varsToAdd);
  void getCreatedVariables(VarList& list, Transition* tr);
  void createVariablesRecursion(VarList& list, Expression* expr);

  void splitStates();

  bool checkPrecondition(Transition* tr, FinalState* from);
  void evaluateRealAssignments(Transition* tr, FinalState* from);

  bool createTransition(FinalState* fromfstate, 
			FinalState* tofstate, 
			Transition* trans,
			GateName* gate);
  void addTransitions();

  bool checkConditions(Transition* tr, FinalState* from, FinalState* to, unsigned gateNumber);

  //  bool temp1(Transition* tr, FinalState* from, FinalState* to, unsigned gateNumber,
  //	    VarList& alreadyProcessed, ExpressionValue& e);
  //  bool temp2(Transition* tr, FinalState* from, FinalState* to, unsigned gateNumber,
  //	     VarList& alreadyProcessed, ExpressionValue& e);


  void unfoldQuestions(Transition* tr);
  bool checkQuestions(Transition* tr, GateName gate, FinalState* to);

  void updateOnPropList(PropList& list, Transition* tr);
  void calculateStateProps();
  void recursiveSwitch(State* state, PropName prop, bool on, bool switchHere);

  void findInitStates();
  void findReachableStates();

  void markUsedGates();

  void resetBeenBits();
  void setBeenBit(FinalState* state);
  void setBeenBit(State* state);
  bool beenHere(FinalState* state);
  bool beenHere(State* state);

  Process* myProcess;
  ActionNamesStore* actionStore;
  TransitionsContainer* transitions;
  StatePropsContainer* stateProps;
  FinalStateName startState;
  FinalStateList beenBitsF;
  StateList beenBits;

  oLSTS_File olsts;
  Header* hd;

  bool debug, unused;
};

#endif

