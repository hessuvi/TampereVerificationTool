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

#include "Reader.hh"

Reader::Reader(InStream& source,
	       Process* process,
	       bool debugMode)
  : myProcess(process), debug(debugMode)
{
  // true = case sensitive identifiers
  ts = new ITokenStream(source, true);
  
  ts->SetPunctuation("->#->#*#:#[#]#'#:=#(#)#=#!=#<#>#!#?#;#-#\\/#/\\#>=#<=#/#%#..");

  // false = case insensitive reserved words
  ts->SetReservedWords("PROCESS#GATES#STATE_PROPS#IS#VARIABLES#ENDPROC#INIT#ASSIGN#NO_SWITCH#"
		       "SWITCH_OFF#SWITCH_ON#HERE_OFF#HERE_ON#DELETE#INC#DEC#TAU#AND#OR#NOT#"
		       "INC_NO_WRAP#DEC_NO_WRAP#TYPEDEF#EVENT_PARAMS#TRUE#FALSE#BOOLEAN",
		       false);
}

Reader::~Reader()
{
  delete ts;
}

void Reader::readFile()
{
  handleProcess();
}

void Reader::handleProcess()
{
  const TT::Token* t;

  // ["TYPEDEF" <type_def>*]
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "TYPEDEF")
    {
      match(TT::RESERVED_WORD, "TYPEDEF");
      while (ts->Peek().isIdentifier())
	{
	  handleType_def();
	}
    }

  // "PROCESS" name
  match(TT::RESERVED_WORD, "PROCESS");
  t = match(TT::IDENTIFIER);
  myProcess->setProcessName(t->stringData());
  
  // ["GATES" name*]
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "GATES")
    {
      match(TT::RESERVED_WORD, "GATES"); // should always succeed
      do
	{
	  t = &ts->Peek();
	  if (t->isIdentifier())
	    {
	      t = &ts->Get();
	      if (!myProcess->gateExists(t->stringData()))
		{
		  myProcess->addGate(t->stringData());
		}
	      else
		{
		  string s = "Error: two gates cannot have the same name: ";
		  s += t->stringData();
		  throw ILError(s);
		}
	    }
	  else 
	    {
	      break;
	    }
	} 
      while (true);
    }
  
  // ["STATE_PROPS" <state_prop>*]
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "STATE_PROPS")
    {
      match(TT::RESERVED_WORD, "STATE_PROPS");

      bool noMoreStateProps = false;
      do 
        {
          if (ts->Peek().isIdentifier() || 
	      (ts->Peek().isPunctuation() && 
	       (ts->Peek().stringData() == "/" || ts->Peek().stringData() == "%")
	       )
	      )
            {
	      PropType* type;
	      type = new PropType;
	      PropName prop = handleState_prop(true, type);
              myProcess->addStateProp(prop, *type);
	      delete type;
            }
          else
            {
              noMoreStateProps = true;
            } 
	} 
      while (!noMoreStateProps);
    }
  
  // "IS"
  match(TT::RESERVED_WORD, "IS");

  // ["VARIABLES" <variable_def>*]
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "VARIABLES")
    {
      match(TT::RESERVED_WORD, "VARIABLES");
      // variable definition begins either with an identifier or a colon
      while (ts->Peek().isIdentifier() || 
	     (ts->Peek().isPunctuation() && ts->Peek().stringData() == ":"))
	{
	  handleVariable_def();
	}
    }

  // ["EVENT_PARAMS" <param_def>*]
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "EVENT_PARAMS")
    {
      match(TT::RESERVED_WORD, "EVENT_PARAMS");
      while (ts->Peek().isIdentifier())
	{
	  handleParam_def();
	}
    }

  // [<init_part>]
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "INIT")
    {
      handleInit_part();
    }
  else
    {
      std::cerr << "Warning: No initialization, init state will be random." << endl;
    }

  // <state>*
  while (ts->Peek().isPunctuation() && ts->Peek().stringData() == "*")
    {
      handleState();
    }

  // "ENDPROC"
  match(TT::RESERVED_WORD, "ENDPROC");

  myProcess->checkImplicitStates();
  
  // done!
}

PropName Reader::handleState_prop(bool firstTime, PropType* type)
{
  const TT::Token* t = &ts->Peek();

  if (firstTime &&
      t->isPunctuation() && 
      ( t->stringData() == "/" ||
	t->stringData() == "%" 
      ) 
     )
    {
      t = match(TT::PUNCTUATION);
      if (t->stringData() == "/")
	{
	  *type = STICKYPROP;
	}
      else if (t->stringData() == "%")
	{
	  *type = CUTPROP;
	}
      
      t = match(TT::IDENTIFIER);
      return t->stringData();
    }
  else
    {
      if (firstTime) *type = NORMALPROP;

      t = match(TT::IDENTIFIER);
      return t->stringData();
    }
}

void Reader::handleType(vector<string>& enumType)
{
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "BOOLEAN")
    {
      match(TT::RESERVED_WORD, "BOOLEAN");

      enumType.push_back("FALSE");
      enumType.push_back("TRUE");
      return;
    }

  match(TT::PUNCTUATION, "[");
  
  bool dots = false;
  const TT::Token* t;
  
  while (ts->Peek().isIdentifier() || ts->Peek().isNumber())
    {
      if (ts->Peek().isIdentifier()) 
	{
	  t = match(TT::IDENTIFIER);
	}
      else 
	{
	  t = match(TT::NUMBER);
	}

      if (dots)
	{
	  if (t->isNumber())
	    {
	      int first = atoi(enumType[enumType.size()-1].c_str());
	      int last = atoi(t->stringData().c_str());
	      first++; last--;

	      if (first > last)
		{
		  throw ILError("ERROR: Invalid use of the .. notation.");
		}
	      else
		{
		  for (int number = first; number <= last; ++number)
		    {
		      std::ostringstream temp;
		      temp << number;
		      enumType.push_back(temp.str());
		    }
		}
	    }
	  else
	    {
	      throw ILError("ERROR: The .. notation can only be used with numbers.");
	    }

	  dots = false;
	}

      enumType.push_back(t->stringData());

      if (ts->Peek().isPunctuation() && ts->Peek().stringData() == "..")
	{
	  match(TT::PUNCTUATION, "..");

	  if (t->isNumber())
	    {
	      dots = true;
	    }
	  else
	    {
	      throw ILError("ERROR: The .. notation can only be used with numbers.");
	    }
	}
    }

  if (enumType.size() == 0)
    {
      std::cerr << "Warning: an empty type defined." << endl;
    }
  match(TT::PUNCTUATION, "]");
}

void Reader::handleType_def()
{
  const TT::Token* t = &ts->Peek();
  
  if (t->isIdentifier())
    {
      t = match(TT::IDENTIFIER);

      string typeName = t->stringData();
      
      match(TT::PUNCTUATION, ":");

      vector<string> enumType;

      handleType(enumType);

      myProcess->addTypeDef(typeName, enumType);
    }
}

void Reader::handleVariable_def()
{
  const TT::Token* t = &ts->Peek();
  vector<string> varNames;

  while (t->isIdentifier())
    {
      t = match(TT::IDENTIFIER);
      varNames.push_back(t->stringData());
      t = &ts->Peek();
    }

  if (varNames.size() == 0)
  {
    std::cerr << "Warning: type defined, but no variables of the type created." << endl;
  }

  match(TT::PUNCTUATION, ":");
  
  EnumType typeCode;
  vector<string> enumType;

  // maybe it's an existing type
  if (ts->Peek().isIdentifier())
    {
      t = match(TT::IDENTIFIER);
      typeCode = myProcess->getTypeDefNumber(t->stringData());
    }
  else
    {
      handleType(enumType);
      typeCode = myProcess->createEnumType(enumType);
    }

  for (unsigned i = 0; i < varNames.size(); ++i)
    {
      myProcess->addVariable(typeCode, varNames[i]);
    }
}

void Reader::handleParam_def()
{
  if (ts->Peek().isIdentifier())
    {
      const TT::Token* t = match(TT::IDENTIFIER);
      
      GateName gateName = t->stringData();

      vector<EnumType> paramTypes;
      while (ts->Peek().isPunctuation() && ts->Peek().stringData() == "<")
	{
	  match(TT::PUNCTUATION, "<");

	  if (ts->Peek().stringData() == "BOOLEAN")
	    {
	      match(TT::RESERVED_WORD, "BOOLEAN");
	      paramTypes.push_back(0);
	    }
	  else
	    {
	      t = match(TT::IDENTIFIER);
	      paramTypes.push_back(myProcess->getTypeDefNumber(t->stringData()));
	    }

	  match(TT::PUNCTUATION, ">");

	}

      myProcess->addGateParams(gateName, paramTypes);
    }
}

void Reader::handleInit_part()
{
  vector<Expression*> assigns;
  
  // "INIT"
  match(TT::RESERVED_WORD, "INIT");
  
  // ["[" <assignment>* "]" ]
  if (ts->Peek().isPunctuation() && ts->Peek().stringData() == "[")
    {
      match(TT::PUNCTUATION, "[");
      
      while ( !(ts->Peek().isPunctuation() && ts->Peek().stringData() == "]") )
	{
	  Expression* e = handleAssignment();

	  // check that its types are correct and that it doesn't have references
	  // to the from-state variables and that it returns a boolean value
	  ExpressionValue val;
	  val.type = LOGICAL;
	  checkExpression(e, myProcess, false, true, &val);

	  assigns.push_back(e);
	  match(TT::PUNCTUATION, ";");
	}

      match(TT::PUNCTUATION, "]");
    }
  
  // [SWITCH_ON name*]
  if (ts->Peek().isReservedWord() && ts->Peek().stringData() == "SWITCH_ON")
    {
      match(TT::RESERVED_WORD, "SWITCH_ON");
      
      while (ts->Peek().isIdentifier())
	{
	  myProcess->addInitialSwitchOn(handleState_prop());
	  //	  myProcess->addInitialNoSwitch(match(TT::IDENTIFIER)->stringData());
	}
    }
  
  // ["->" name]
  if (ts->Peek().isPunctuation() && ts->Peek().stringData() == "->")
    {
      match(TT::PUNCTUATION, "->");
      
      const TT::Token* t = match(TT::IDENTIFIER);
      myProcess->setInitState(t->stringData());
    }
  
  // now add the initial assignments.
  for (unsigned i = 0; i < assigns.size(); ++i)
    {
      myProcess->addInitialAssignment(assigns[i]);
    }
}

void Reader::handleState()
{
  const TT::Token* t;

  // "*"
  match(TT::PUNCTUATION, "*");

  // name
  t = match(TT::IDENTIFIER);
  string stateName = t->stringData();

  myProcess->addState(stateName);

  // ["SWITCH_OFF" name*]
  // ["SWITCH_ON" name*]
  // ["HERE_OFF" name*]
  // ["HERE_ON" name*]

  // reading these is kinda similar, so let's do it with a loop
  // first two (switch) are simple: if we find the code word, read all identifiers
  // and possible cut etc:s and add them with addPropChange
  // the last two (here) are a bit more complicated: they might have a condition
  // after the list of prop names, so we don't just add them directly, but put them
  // into a list while we don't know the condition and add them later on.
  string names[] = {"SWITCH_OFF", "SWITCH_ON", "HERE_OFF", "HERE_ON"};
  for (unsigned temp = 0; temp < 4; ++temp)
    {
      if (ts->Peek().isReservedWord() && ts->Peek().stringData() == names[temp])
	{
	  match(TT::RESERVED_WORD, names[temp]);

	  while (ts->Peek().isIdentifier())
	    {
	      if (temp == 0 || temp == 1)
		{
		  myProcess->addPropChange(stateName, 
					   names[temp], 
					   handleState_prop());
		}
	      else
		{
		  Expression* e = NULL;
		  PropName prop = handleState_prop();

		  if (ts->Peek().stringData() == "[")
		    {
		      match(TT::PUNCTUATION, "[");
		      e = handleExpression();

		      // check that it has no type conflicts, returns a boolean value and
		      // doesn't use to-state variables
		      ExpressionValue val;
		      val.type = LOGICAL;
		      checkExpression(e, myProcess, true, false, &val);

		      match(TT::PUNCTUATION, "]");
		    }
	      
		  myProcess->addPropChange(stateName, names[temp], prop, e);
		}
	    }
	}
    }
   
  bool hasExplicitTarget = true;

  // ":" <transition>*
  match(TT::PUNCTUATION, ":");

  while ( (!(ts->Peek().isPunctuation() && ts->Peek().stringData() == "*") && 
	   !(ts->Peek().isReservedWord() && ts->Peek().stringData() == "ENDPROC"))
	 || !hasExplicitTarget)
    {
      if (ts->Peek().isEndOfFile())
	{
	  string e = "Error: unexpected end of file. Did you forget the 'endproc'?";
	  throw ILError(e);
	}

      string targetName;
      handleTransition(stateName, hasExplicitTarget, targetName);
      if (!hasExplicitTarget) stateName = targetName;
    }
}

void Reader::handleTransition(string currentState, bool& hasExplicitTarget, string& targetName)
{
  const TT::Token* t;
  Expression* cond = NULL;
  vector<Expression*> assignments;
  vector<Expression*> excls;
  vector<string> quests;

  // ["[" <condition>"]"
  if (ts->Peek().isPunctuation() && ts->Peek().stringData() == "[")
    {
      match(TT::PUNCTUATION, "[");
      cond = handleCondition();
      
      // check that it has no type conflicts, evaluates to a boolean value
      // and doesn't use to-state variables
      ExpressionValue val;
      val.type = LOGICAL;
      checkExpression(cond, myProcess, true, false, &val);

      match(TT::PUNCTUATION, "]");
      match(TT::PUNCTUATION, "->");
    }

  string gate = "";
  vector<unsigned> questpos, exclpos;
  PropList noSwitches;

  // (name ( "!" expression | "?" name "'" )* | "TAU")
  if (ts->Peek().isIdentifier())
    {
      t = match(TT::IDENTIFIER);
      gate = t->stringData();

      unsigned position = 0;
      // ?!-stuff
      while (ts->Peek().isPunctuation() &&
	     (ts->Peek().stringData() == "?" || ts->Peek().stringData() == "!"))
	{
	  if (ts->Peek().stringData() == "?")
	    {
	      match(TT::PUNCTUATION, "?");
	      if (ts->Peek().isIdentifier())
		{
		  quests.push_back(match(TT::IDENTIFIER)->stringData());
		  questpos.push_back(position++);

		  match(TT::PUNCTUATION, "'");
		}
	      else
		{
		  string s = "Identifier expected after \"?\", ";
		  s += ts->Peek().stringData();
		  s += " found.";
		  throw ILError(s);
		}
	    }
	  else if (ts->Peek().stringData() == "!")
	    {
	      match(TT::PUNCTUATION, "!");
	      
	      excls.push_back(handleCondition());

	      // check that it has no type conflicts (NOT and returns an enumeration value)
	      //ExpressionValue val;
	      //val.type = ENUM;
	      checkExpression(excls[excls.size()-1], myProcess, true, true, NULL); //&val);

	      exclpos.push_back(position++);
	    }
	}

      // now check that the gate can get this kind of parameters according to the ACTION_PARAMS
      checkParameterTypes(excls, exclpos, quests, questpos, gate);

    }
  else // if no gate was named, we'd love to see the word TAU here
    {
      match(TT::RESERVED_WORD, "TAU");
    }

  // [ <assignment>* ]
  if (ts->Peek().isPunctuation() && ts->Peek().stringData() == "[")
    {
      match(TT::PUNCTUATION, "[");
      while (!(ts->Peek().isPunctuation() && ts->Peek().stringData() == "]"))
	{
	  assignments.push_back(handleAssignment());
	  
	  // check that it has no type conflicts and returns a boolean value
	  ExpressionValue val;
	  val.type = LOGICAL;
	  checkExpression(assignments[assignments.size()-1], myProcess, true, true, &val);

	  match(TT::PUNCTUATION, ";");
	}

      match(TT::PUNCTUATION, "]");
    }

  // ["NO_SWITCH" name*]
  t = &ts->Peek();
  if (t->isReservedWord() && t->stringData() == "NO_SWITCH")
    {
      match(TT::RESERVED_WORD);
      while (ts->Peek().isIdentifier())
	{
	  noSwitches.push_back(handleState_prop());
	}
    }

  // ["DELETE" name*]
  VarList deletes;
  t = &ts->Peek();
  if (t->isReservedWord() && t->stringData() == "DELETE")
    {
      match(TT::RESERVED_WORD, "DELETE");
      while (ts->Peek().isIdentifier())
	{
	  deletes.push_back(match(TT::IDENTIFIER)->stringData());
	}
    }

  // "->" name
  match(TT::PUNCTUATION, "->");

  string targetState;

  if (ts->Peek().isIdentifier() && myProcess->gateExists(ts->Peek().stringData()))
    {
      hasExplicitTarget = false;
      targetState = myProcess->getNewUnnamedState();
    }
  else
    {
      hasExplicitTarget = true;
      t = match(TT::IDENTIFIER);
      targetState = t->stringData();
    }

  // if the target state doesn't exist yet, create it
  if (!myProcess->stateExists(targetState))
    {
      myProcess->addState(targetState, false);
    }

  targetName = targetState;
  
  myProcess->addTransition(currentState, 
			   targetState, 
			   gate, 
			   cond, 
			   assignments,
			   deletes,
			   excls,
			   exclpos,
			   quests,
			   questpos,
			   noSwitches);
}

Expression* Reader::handleAssignment()
{
  Expression* e = handleExpression();
  if (!e) std::cerr << "Internal warning: Returning NULL assignment" << endl;
  return e;
}

Expression* Reader::handleCondition()
{
  Expression* e = handleExpression();
  if (!e) std::cerr << "Internal warning: Returning NULL condition" << endl;
  return e;
}

Expression* Reader::handleExpression()
{
  Expression* newExpr = new Expression;

  Expression* first = handleAndLevel();

  const TT::Token* t = &(ts->Peek());

  if ( (t->isPunctuation() && t->stringData() == "\\/") || 
       (t->isReservedWord() && t->stringData() == "OR") )
    {
      t = &(ts->Get());
      newExpr->setOperator(Utils::getOperatorType(t->stringData()));
      newExpr->addOperand(first);
      newExpr->addOperand(handleExpression());
    }
  else
    {
      // the whole expression was just an and-level expression
      delete newExpr;
      newExpr = first;
    }

  return newExpr;
}

Expression* Reader::handleAndLevel()
{
  Expression* newExpr = new Expression;

  Expression* first = handleNotLevel();

  const TT::Token* t = &(ts->Peek());

  if ( (t->isPunctuation() && t->stringData() == "/\\") || 
       (t->isReservedWord() && t->stringData() == "AND") )
    {
      t = &(ts->Get());
      newExpr->setOperator(Utils::getOperatorType(t->stringData()));
      newExpr->addOperand(first);
      newExpr->addOperand(handleAndLevel());
    }
  else
    {
      // the whole expression was just a not-level expression
      delete newExpr;
      newExpr = first;
    }

  return newExpr;
}

Expression* Reader::handleNotLevel()
{
  Expression* newExpr = new Expression;

  const TT::Token* t = &(ts->Peek());

  if ( (t->isPunctuation() && t->stringData() == "-") || 
       (t->isReservedWord() && t->stringData() == "NOT") )
    {
      t = &(ts->Get());
      newExpr->setOperator(Utils::getOperatorType(t->stringData()));
      newExpr->addOperand(handleNotLevel());
    }
  else
    {
      // the whole expression was just a term
      delete newExpr;
      newExpr = handleTerm();
    }

  return newExpr;
}

Expression* Reader::handleTerm()
{
  Expression* newExpr = new Expression;
  
  Expression* first = handleFactor();

  const TT::Token* t = &(ts->Peek());

  if (t->isPunctuation() && (t->stringData() == "=" || t->stringData() == "!=" || 
			     t->stringData() == ">" || t->stringData() == "<" ||
			     t->stringData() == ">=" || t->stringData() == "<=" ))
    {
      t = match(TT::PUNCTUATION);
      newExpr->setOperator(Utils::getOperatorType(t->stringData()));
      newExpr->addOperand(first);
      newExpr->addOperand(handleFactor());
    }
  else
    {
      // the whole term was just a factor
      delete newExpr;
      newExpr = first;
    }

  return newExpr;
}

Expression* Reader::handleFactor()
{
  Expression* newExpr = new Expression;

  const TT::Token* t = &(ts->Peek());

  if (t->isPunctuation() && t->stringData() == "(")
    {
      t = match(TT::PUNCTUATION, "(");
      delete newExpr;
      newExpr = handleExpression();
      t = match(TT::PUNCTUATION, ")");
    }
  else
    {
      if (t->isReservedWord() && 
	  ( t->stringData() == "INC" || t->stringData() == "DEC" ||
	    t->stringData() == "INC_NO_WRAP" || t->stringData() == "DEC_NO_WRAP")
	  )
	{
	  t = match(TT::RESERVED_WORD);
	  newExpr->setOperator(Utils::getOperatorType(t->stringData()));
	  newExpr->addOperand(handleFactor());
	}
      else if (t->isReservedWord() && ( t->stringData() == "FALSE" || t->stringData() == "TRUE"))
	{
	  t = match(TT::RESERVED_WORD);
	  newExpr->addData(t->stringData());
	}
      else
	{
	  string s;
	  // a constant/variable name
	  t = &ts->Peek();
	  if (t->isIdentifier())
	    {
	      t = match(TT::IDENTIFIER);
	      s = t->stringData();
	      t = &(ts->Peek());
	      if (t->stringData() == "'")
		{
		  t = match(TT::PUNCTUATION, "'");
		  // ' after a variable name should be joined to that variable
		  s += "'";
		}
	    }
	  else if (t->isNumber())
	    {
	      t = match(TT::NUMBER);
	      s = t->stringData();
	    }
	  else
	    {
	      string errMsg = "Error: constant or variable name expected.";
	      throw ILError(errMsg);
	    }
	  
	  newExpr->addData(s);
	}
    }

  return newExpr;
}

const TT::Token* Reader::match(TT::TokenType expectedType, 
				     const string& data)
{
  string error = "";

  if (data == "")
    {

      error += TT::TokenTypeNames[expectedType];
      error += " expected.";

      return &ts->Get(error, expectedType);
    }
  else
    {
      error += "\"";
      error += data;
      error += "\" expected.";

      const TT::Token* t = &ts->Get(error, expectedType, data);
      return t;
    }

  // we can't get here ever because of the if .. else both have returns, 
  // but the compiler fails to see that. This removes the warning.
  return NULL;
}

string Reader::error(string msg)
{
  return ts->errorMsg(msg);
}

void Reader::checkExpression(Expression* e, 
			     Process* process,
			     bool fromVarsAllowed, 
			     bool toVarsAllowed,
			     ExpressionValue* value)
{
  ExpressionValue resultValue;
  checkExpressionHelper(e, process, fromVarsAllowed, toVarsAllowed, resultValue);

  if (value)
    {
      if (value->type != resultValue.type)
	{
	  // didn't return the correct type
	  string errMsg = "Error: expression ";
	  errMsg += e->toString();
	  errMsg += " doesn't evaluate to the expected type. (is ";
	  errMsg += Utils::exprTypes[resultValue.type];
	  errMsg += ", should be ";
	  errMsg += Utils::exprTypes[value->type];
	  errMsg += ").";
	  throw ILError(errMsg);
	}
    }
}

void Reader::checkParameterTypes(vector<Expression*> e,
				 vector<unsigned> ePos,
				 vector<VarName> q,
				 vector<unsigned> qPos,
				 GateName gate)
{
  vector<EnumType> results(e.size() + q.size());

  // first check exclamation marks (!)
  vector<unsigned>::iterator temp = ePos.begin();
  for(vector<Expression*>::iterator eIt = e.begin(); eIt != e.end(); ++eIt)
    {
      ExpressionValue resultValue;
      checkExpressionHelper(*eIt, myProcess, true, true, resultValue);

      if (resultValue.type == ENUM)
	{
	  results[*temp] = resultValue.enumType;
	}
      else if (resultValue.type == LOGICAL)
	{
	  results[*temp] = 0;
	}
      else
	{
	  string errStr = "ERROR: !-expression must return an enumeration or boolean value.";
	  throw ILError(errStr);
	}
      ++temp;
    }

  // then check question marks (?)
  temp = qPos.begin();
  for (vector<VarName>::iterator qIt = q.begin(); qIt != q.end(); ++qIt)
    {
      ExpressionValue resultValue;

      Expression* tempExpr = new Expression;
      tempExpr->addData(*qIt);

      checkExpressionHelper(tempExpr, myProcess, true, true, resultValue);

      delete tempExpr;

      if (resultValue.type == ENUM)
	{
	  results[*temp] = resultValue.enumType;
	}
      else if (resultValue.type == LOGICAL)
	{
	  results[*temp] = 0;
	}
      else 
	{
	  string errStr = "Internal error: Variable evaluation must return an enum or boolean.";
	  throw ILError(errStr);
	}
      ++temp;
    }

  if (!myProcess->checkGateParams(gate, results))
    {
      string errStr = "ERROR: Illegal combination of parameter types for gate ";
      errStr += gate;
      errStr += ".";
      throw ILError(errStr);
    }
}

void Reader::checkExpressionHelper(Expression* e,
				   Process* process,
				   bool fromVarsAllowed,
				   bool toVarsAllowed,
				   ExpressionValue& value)
{
  if (e->isLeaf())
    {
      string data = e->getData();

      if (data == "FALSE" || data == "TRUE")
	{
	  value.type = LOGICAL;
	  return;
	}

      // if the data is a variable with or without an '
      if (process->variableExists(data) ||
	  (Utils::hasApostrophe(data) && process->variableExists(Utils::removeApostrophe(data)))
	 )
	{
	  if (!fromVarsAllowed && !Utils::hasApostrophe(data))
	    {
	      // illegal use of fromstate variable
	      string errMsg = "Error: non-':d variables (";
	      errMsg += data;
	      errMsg += ") not allowed here.";
	      throw ILError(errMsg);
	    }
	  if (!toVarsAllowed && Utils::hasApostrophe(data))
	    {
	      // illegal use of tostate variable
	      string errMsg = "Error: ':d variables (";
	      errMsg += data;
	      errMsg += ") not allowed here.";
	      throw ILError(errMsg);
	    }

	  // ok, get and return the type
	  if (Utils::hasApostrophe(data))
	    {
	      data = Utils::removeApostrophe(data);
	    }

	  value.type = ENUM;
	  value.enumType = process->getVariableType(data);

	  // boolean type looks like an enum, but is actually logical
	  if (value.enumType == 0)
	    {
	      value.type = LOGICAL;
	    }

	  return;
	}
      else
	{
	  // it should be a constant or a proposition
	  unsigned enumType = process->getEnumType(data);
	  if (enumType != 0)
	    {
	      // it's a constant
	      value.type = ENUM;
	      value.enumType = enumType;
	    }
	  else
	    {
	      // it's a state proposition
	      PropList props;
	      myProcess->getStateProps(props);
	      if (props.end() == find(props.begin(), props.end(), data))
		{
		  string errMsg = "Error: Undefined identifier \"";
		  errMsg += data;
		  errMsg += "\".";
		  throw ILError(errMsg);
		}
	      else
		{
		  value.type = PROP;
		}
	    }
	}
    }
  else // not a leaf node, check the operands and then that they are of the correct type
    {
      vector<Expression*> opers = e->getOperands();
      ExpressionValue values[2]; // max 2 operands
      
      for (unsigned current = 0; current < opers.size(); current++)
	{
	  checkExpressionHelper(opers[current], 
				process, 
				fromVarsAllowed, 
				toVarsAllowed, 
				values[current]);
	}

      OperatorType o = e->getOperator();
      if (o == INC || o == DEC || o == INC_NO_WRAP || o == DEC_NO_WRAP)
	{
	  if (values[0].type == ENUM)
	    {
	      value.type = ENUM;
	      value.enumType = values[0].enumType;
	    }
	  else
	    {
	      string errMsg = "Error: The parameter of ";
	      errMsg += Utils::operators[o];
	      errMsg += " is of an incorrect type (should be enumeration).";
	      throw ILError(errMsg);
	    }
	}
      else if (o == EQUALS || o == NOTEQUAL || o == GREATERTHAN || o == LESSTHAN ||
	       o == GREATER_EQUAL || o == LESS_EQUAL)
	{
	  if (values[0].type == ENUM && values[1].type == ENUM)
	    {
	      if (values[0].enumType != values[1].enumType)
		{
		  string errMsg = "Error: The parameters of ";
		  errMsg += Utils::operators[o];
		  errMsg += " are of different type.";
		  throw ILError(errMsg);
		}
	      else
		{
		  value.type = LOGICAL;
		}
	    }
	  else if ( (o == EQUALS || o == NOTEQUAL) && 
		    values[0].type == LOGICAL && values[1].type == LOGICAL)
	    {
	      value.type = LOGICAL;
	    }
	  else
	    {
	      cout << "values0 = " << values[0].type << ", values1 = " << values[1].type << endl;

	      string errMsg = "Error: The parameters of ";
	      errMsg += Utils::operators[o];
	      errMsg += " are of different type.";
	      throw ILError(errMsg);
	    }
	}
      else if (o == AND || o == OR)
	{
	  if ( (values[0].type == LOGICAL || values[0].type == PROP) && 
	       (values[1].type == LOGICAL || values[1].type == PROP) )
	    {
	      value.type = LOGICAL;
	    }
	  else
	    {
	      string errMsg = "Error: Some of the parameters of ";
	      errMsg += Utils::operators[o];
	      errMsg += " are of an incorrect type (should be boolean or proposition).";
	      throw ILError(errMsg);
	    }
	}
      else if (o == NOT)
	{
	  if ( (values[0].type == LOGICAL || values[0].type == PROP))
	    {
	      value.type = LOGICAL;
	    }
	  else
	    {
	      string errMsg = "Error: Some of the parameters of ";
	      errMsg += Utils::operators[o];
	      errMsg += " are of an incorrect type (should be boolean or proposition).";
	    }
	}
    }
}

