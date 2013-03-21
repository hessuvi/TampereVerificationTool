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
// Input language compiler: reader.hh
// by Timo Kivelä
// 
// This class parses an input language file and writes
// the data to a Process-class.

#ifndef TVT_INPUT_LANGUAGE_READER_HH
#define TVT_INPUT_LANGUAGE_READER_HH

#include "TransitionsContainer.hh"
#include "FileFormat/Lexical/constants.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "InOutStream.hh"

#include "General.hh"
#include "Process.hh"
#include "Expression.hh"

class Reader
{
public:
  // The data will be read from the source and written to the process.
  Reader(InStream& source, Process* process, bool debugMode = false);
  ~Reader();
  
  // This does the actual parsing
  void readFile();

  // get an error message with current line number
  string error(string msg);

private:
  // handle-methods are used in parsing the input language according to 
  // the BNF-specification
  void handleProcess();
  PropName handleState_prop(bool firstTime = false, PropType* type = NULL);
  void handleType(vector<string>& enumType);
  void handleType_def();
  void handleVariable_def();
  void handleParam_def();
  void handleInit_part();
  void handleState();
  void handleTransition(string currentState, bool& hasExplicitTarget, string& targetName);
  Expression* handleAssignment();
  Expression* handleCondition();

  Expression* handleExpression();
  Expression* handleAndLevel();
  Expression* handleNotLevel();
  Expression* handleTerm();
  Expression* handleFactor();

  const TT::Token* match(TT::TokenType expectedType, const string& data = "");

  // this checks that the given expression has no type conflicts. Also, if
  // fromVarsAllowed is false, checks that the expression has no references to fromstate vars and
  // if toVarsAllowed is false, checks that it has no references to tostate variables.
  void checkExpression(Expression* e,
		       Process* process,
		       bool fromVarsAllowed = true, 
		       bool toVarsAllowed = true,
		       ExpressionValue* value = NULL);

  // checks whether the gate's parameters are of a correct type, according to the ACTION_PARAMS
  void checkParameterTypes(vector<Expression*> e,
			   vector<unsigned> ePos,
			   vector<VarName> q,
			   vector<unsigned> qPos,
			   GateName gate);

  // helper function for checkExpression's recursive checking
  void checkExpressionHelper(Expression* e,
			     Process* process,
			     bool fromVarsAllowed,
			     bool toVarsAllowed,
			     ExpressionValue& value);

  Process* myProcess;
  //TransitionsContainer transitions;
  //GateList gates;
  //StateList states;
  InStream* file;
  ITokenStream* ts;

  bool debug;
};

#endif
