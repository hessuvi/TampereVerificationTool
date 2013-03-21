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

#include "Expression.hh"

long Expression::evaluations = 0;

Expression::Expression()
  : cached(false)
{
}

Expression::~Expression()
{
  if (!myLeafness)
    {
      vector<Expression*>::iterator o = myOperands.begin();
      while (o != myOperands.end())
	{
	  delete *o;
	  myOperands.erase(o);
	  o = myOperands.begin();
	}
    }
}

void Expression::addData(string s)
{
  myLeafness = true;
  myData = s;
  cached = false;
}

void Expression::setOperator(OperatorType oper)
{
  myOperator = oper;
  cached = false;
}

void Expression::addOperand(Expression* newOperand)
{
  myLeafness = false;
  cached = false;
  myOperands.push_back(newOperand);
}

string Expression::toString()
{
  if (myLeafness)
    {
      return myData;
    }
  else
    {
      string tmp[2];
      for (unsigned i = 0; i < myOperands.size(); ++i)
	{
	  tmp[i] = myOperands[i]->toString();
	}

      string result = "";
      switch (myOperator)
	{
	case INC:
	  result += "INC( ";
	  result += tmp[0];
	  result += " )";
	  break;
	case DEC:
	  result += "DEC( ";
	  result += tmp[0];
	  result += " )";
	  break;
	case EQUALS:
	  result += tmp[0];
	  result += " = ";
	  result += tmp[1];
	  break;
	case NOTEQUAL:
	  result += tmp[0];
	  result += " != ";
	  result += tmp[1];
	  break;
	case GREATERTHAN:
	  result += tmp[0];
	  result += " > ";
	  result += tmp[1];
	  break;
	case LESSTHAN:
	  result += tmp[0];
	  result += " < ";
	  result += tmp[1];
	  break;
	case GREATER_EQUAL:
	  result += tmp[0];
	  result += " >= ";
	  result += tmp[1];
	  break;
	case LESS_EQUAL:
	  result += tmp[0];
	  result += " <= ";
	  result += tmp[1];
	  break;
	case AND:
	  result += tmp[0];
	  result += " /\\ ";
	  result += tmp[1];
	  break;
	case OR:
	  result += tmp[0];
	  result += " \\/ ";
	  result += tmp[1];
	  break;
	case NOT:
	  result += "-";
	  result += tmp[0];
	  break;
	case INC_NO_WRAP:
	  result += "INC_NO_WRAP( ";
	  result += tmp[0];
	  break;
	case DEC_NO_WRAP:
	  result += "DEC_NO_WRAP( ";
	  result += tmp[0];
	  break;
	default:
	  break;
	}

      return result;
    }
}

unsigned Expression::getVariableValue(FinalState* from,
				       FinalState* to,
				       Process* process,
				       Transition* tr,
				       unsigned gateNumber,
				       unsigned enumType)
{
  unsigned enumValue;

  if (Utils::hasApostrophe(myData))
    {
      string temp = Utils::removeApostrophe(myData);
      
      if (to->hasVariable(temp))
	{
	  enumValue = to->getVariableValue(temp);
	}
      else
	{
	  // the last chance to find the variable's value is from the transition.
	  // sometimes variables are created with ? and then deleted in the same
	  // transition. Here we search for those.
	  if (tr)
	    {
	      bool found = false;
	      vector<unsigned>::iterator qpos = tr->questionPositions.begin();
	      for (VarList::iterator q = tr->questions.begin(); 
		   q != tr->questions.end();
		   ++q)
		{
		  if (*q == temp)
		    {
		      enumValue = process->
			getEnumValue(enumType, 
				     Utils::getVarFromGate(tr->gates[gateNumber], *qpos));
		      found = true;
		    }
		  
		  ++qpos;
		}
	      if (!found)
		{
		  string errStr = "ERROR: ";
		  errStr += myData;
		  errStr += " cannot be evaluated. Might be an uninitialized variable?";
		  throw ILError(errStr);
		}
	    }
	  else
	    {
	      string errStr = "ERROR: ";
	      errStr += myData;
	      errStr += " cannot be evaluated. Might be an uninitialized variable?";
	      throw ILError(errStr);
	    }
	}
    }
  else
    {
      if (from->hasVariable(myData))
	{
	  enumValue = from->getVariableValue(myData);
	}
      else
	{
	  // we get here if for example a variable is used but is not active in
	  // the state. Consider this an error.
	  string errStr = "ERROR: ";
	  errStr += myData;
	  errStr += " cannot be evaluated. Might be an uninitialized variable?";
	  throw ILError(errStr);
	}
    }

  return enumValue;
}

ExpressionValue Expression::evaluateLeaf(FinalState* from, 
					 FinalState* to, 
					 Process* process,
					 Transition* tr,
					 unsigned gateNumber)
{
  ExpressionValue val;

  if (cached)
    {
      // is a variable that has been evaluated before
      val.type = ENUM;
      val.enumType = cachedType;

      val.enumValue = getVariableValue(from, to, process, tr, gateNumber, val.enumType);
      return val;
    }

  if (myData == "FALSE" || myData == "TRUE")
    {
      val.type = LOGICAL;
      if (myData == "FALSE")
	{ 
	  val.logical = false;
	}
      else
	{
	  val.logical = true;
	}
      return val;
    }

  val.type = ENUM;
  val.enumType = process->getEnumType(myData);

  if (val.enumType != 0) // was found, is a constant
    {
      cached = true;
      myConstantness = true;
      val.enumValue = process->getEnumValue(val.enumType, myData);
    }
  else // was not found, is a variable or a state prop
    {
      myConstantness = false;

      PropList props;
      process->getStateProps(props);
      if (props.end() != find(props.begin(), props.end(), myData))
	{
	  val.type = PROP;

	  cached = false; // we don't support caching state props

	  // is a state prop
	  if (Utils::hasApostrophe(myData))
	    {
	      string temp = Utils::removeApostrophe(myData);
	      val.logical = to->getProp(temp);
	    }
	  else
	    {
	      val.logical = from->getProp(myData);
	    }
	}
      else
	{
	  // is a variable
	  string temp = myData;
	  if (Utils::hasApostrophe(myData))
	    {
	      temp = Utils::removeApostrophe(myData);
	    }

	  val.enumType = process->getVariableType(temp);
	  val.enumValue = getVariableValue(from, to, process, tr, gateNumber, val.enumType);

	  if (val.enumType == 0)
	    {
	      val.type = LOGICAL;
	      if (val.enumValue == 0)
		{
		  val.logical = false;
		}
	      else
		{
		  val.logical = true;
		}
	    }
	  else
	    {
	      cached = true;
	    }
	}
    }

  //  cout << "Leaf evaluated." << endl;

  cachedValue = val.enumValue;
  cachedType = val.enumType;

  return val;
}

ExpressionValue Expression::evaluate(FinalState* from, 
				     FinalState* to, 
				     Process* process,
				     Transition* tr,
				     unsigned gateNumber)
{
  //      if (from && to)
  //    {
  //      cout << "Evaluating expression: " << toString() << " in transition " 
  //           << from->getName() << " -> " << to->getName() << endl;
  //    }

  if (cached)
    {
      if (myConstantness)
	{
	  ExpressionValue val;
	  val.type = ENUM;
	  val.enumType = cachedType;
	  val.enumValue = cachedValue;
	  return val;
	}
      else
	{
	  // is a variable that has been evaluated before
	  return evaluateLeaf(from, to, process, tr, gateNumber);
	}
    }

  ++evaluations;
  
  ExpressionValue val, tmp[2];
  if (myLeafness)
    {
      val = evaluateLeaf(from, to, process, tr, gateNumber);
    }
  else
    {
      for (unsigned i = 0; i < myOperands.size(); ++i)
        {
	  //	  cout << "Recursive evaluation of " << toString() << endl;
	  tmp[i] = myOperands[i]->evaluate(from, to, process, tr, gateNumber);
	  //	  cout << "Returned to tmp[" << i << "]:" << tmp[i].enumValue << endl;
        }

      //      cout << "Applying operator " << myOperator << endl;
      unsigned s;
      bool wrap = false, equal = false;

      switch (myOperator)
	{
	case INC:
	  wrap = true;
	case INC_NO_WRAP:
	  //	  if (tmp[0].type != ENUM)
	  // {
	  //    string e = "Error: tried to apply INC to a non-enum value";
	  //    throw ILError(e);
	  //  }

	  val.type = ENUM;
	  val.enumType = tmp[0].enumType;
	  val.enumValue = ++(tmp[0].enumValue);
	  s = process->getEnumTypeSize(val.enumType);
	  if (val.enumValue > s)
	    {
	      if (wrap)
		{
		  val.enumValue = 1; // wrap around
		}
	      else
		{
		  string e = "Error: tried to increase out of enumeration range";
		  throw ILError(e);
		}
	    }
	  break;

	case DEC:
	  wrap = true;
	case DEC_NO_WRAP:
	  //	  if (tmp[0].type != ENUM)
	  //  {
	  //    string e = "Error: tried to apply DEC to a non-enum value";
	  //   throw ILError(e);
	  //  }

	  val.type = ENUM;
	  val.enumType = tmp[0].enumType;
	  val.enumValue = tmp[0].enumValue - 1;
	  if (val.enumValue == 0)
	    { 
	      if (wrap)
		{
		  val.enumValue = process->getEnumTypeSize(val.enumType);
		}
	      else // if would wrap but not allowed, this will evaluate to false
		{
		  string e = "Error: tried to decrease out of enumeration range";
		  throw ILError(e);
		}
	    }
	  break;

	case EQUALS:
	  //	  if (tmp[0].type != ENUM || tmp[1].type != ENUM)
	  // {
	  //    string e = "Error evaluating operator '=': invalid parameter types";
	  //    throw ILError(e);
	  //  }

	  val.type = LOGICAL;

	  if (tmp[0].type == LOGICAL)
	    {
	      if (tmp[0].logical == tmp[1].logical)
		{
		  val.logical = true;
		}
	      else
		{
		  val.logical = false;
		}
	      break;
	    }

	  if (tmp[0].enumType == tmp[1].enumType && tmp[0].enumValue == tmp[1].enumValue)
	    {
	      val.logical = true;
	    }
	  else
	    {
	      val.logical = false;
	    }
	  break;

	case NOTEQUAL:
	  //  if (tmp[0].type != ENUM || tmp[1].type != ENUM)
	  //  {
	  //    string e = "Error evaluating operator '!=': invalid parameter types";
	  //    throw ILError(e);
	  //  }

	  val.type = LOGICAL;

	  if (tmp[0].type == LOGICAL)
	    {
	      if (tmp[0].logical != tmp[1].logical)
		{
		  val.logical = true;
		}
	      else
		{
		  val.logical = false;
		}
	      break;
	    }

	  if (tmp[0].enumType == tmp[1].enumType && tmp[0].enumValue != tmp[1].enumValue)
	    {
	      val.logical = true;
	    }
	  else
	    {
	      val.logical = false;
	    }
	  break;

	case GREATER_EQUAL:
	  equal = true;
	case GREATERTHAN:
	  //  if (tmp[0].type != ENUM || tmp[1].type != ENUM)
	  //  {
	  //    string e;
	  //    if (!equal) e = "Error evaluating operator '>': invalid parameter types";
	  //    if (equal) e = "Error evaluating operator '>=': invalid parameter types";
	  //    throw ILError(e);
	  //  }

	  val.type = LOGICAL;
	  if (tmp[0].enumType == tmp[1].enumType && 
	      (!equal && tmp[0].enumValue > tmp[1].enumValue) ||
	      (equal && tmp[0].enumValue >= tmp[1].enumValue) )
	    {
	      val.logical = true;
	    }
	  else
	    {
	      val.logical = false;
	    }
	  break;

	case LESS_EQUAL:
	  equal = true;
	case LESSTHAN:
	  //  if (tmp[0].type != ENUM || tmp[1].type != ENUM)
	  //  {
	  //    string e;
	  //    if (!equal) e = "Error evaluating operator '<': invalid parameter types";
	  //    if (equal) e = "Error evaluating operator '<=': invalid parameter types";
	  //    throw ILError(e);
	  //  }

	  val.type = LOGICAL;
	  if (tmp[0].enumType == tmp[1].enumType && 
	      (!equal && tmp[0].enumValue < tmp[1].enumValue) || 
	      (equal && tmp[0].enumValue <= tmp[1].enumValue) )
	    {
	      val.logical = true;
	    }
	  else
	    {
	      val.logical = false;
	    }
	  break;
	case AND:
	  //  if ((tmp[0].type != LOGICAL && tmp[0].type != PROP) || 
	  //    (tmp[1].type != LOGICAL && tmp[1].type != PROP))
	  //  {
	  //    string e = "Error evaluating and operator 'AND': invalid parameter types";
	  //    throw ILError(e);
	  //  }

	  val.type = LOGICAL;
	  if (tmp[0].logical && tmp[1].logical)
	    {
	      val.logical = true;
	    }
	  else
	    {
	      val.logical = false;
	    }
	  break;
	case OR:
	  //  if ( (tmp[0].type != LOGICAL && tmp[0].type != PROP) || 
	  //     (tmp[1].type != LOGICAL && tmp[1].type != PROP))
	  //  {
	  //    string e = "Error evaluating operator 'OR': invalid parameter types";
	  //    throw ILError(e);
	  //  }

	  val.type = LOGICAL;
	  if ( tmp[0].logical || tmp[1].logical)
	    {
	      val.logical = true;
	    }
	  else
	    {
	      val.logical = false;
	    }
	  break;
	case NOT:
	  //  if ( tmp[0].type != LOGICAL && tmp[0].type != PROP)
	  //  {
	  //    string e = "Error evaluating operator 'NOT': invalid parameter types";
	  //    throw ILError(e);
	  //  }

	  val.type = LOGICAL;
	  if (!tmp[0].logical)
	    {
	      val.logical = true;
	    }
	  else
	    {
	      val.logical = false;
	    }
	  break;

	default:
	  break;
	}
    }

  //      cout << "Evaluation of " << toString() << " done: ";
  //     if (val.type == LOGICAL)
  //      {
  //      if (val.logical == true) cout << "true";
  //       else cout << "false";
  //     }
  //    else cout << val.enumType << ":" << val.enumValue;
  //    cout << endl;
  
  return val;
}

void Expression::markHandledVars(Transition* tr)
{
  string varName;
  if (myLeafness)
    {
      if (Utils::hasApostrophe(myData))
	{
	  varName = Utils::removeApostrophe(myData);
	  if (tr->handled.end() == find(tr->handled.begin(), tr->handled.end(), varName))
	    {
	      tr->handled.push_back(varName);
	    }
	}
    }
  else
    {
      for (unsigned i = 0; i < myOperands.size(); ++i)
	{
	  myOperands[i]->markHandledVars(tr);
	}
    }
}

// sees if this is a "real" assignment, which means that it is of the form 
// x' = <expression>, where x is any variable and <expression> doesn't contain
// any ':d variables. If it is, ls will contain x and rs the value of the right
// side, evaluated in the final state "from".
void Expression::isRealAssignment(FinalState* from, 
				  Process* process, 
				  VarName*& ls, 
				  unsigned*& rs)
{
  if (!myLeafness && 
      myOperator == EQUALS && 
      !myOperands[1]->hasApostrophes() &&
      myOperands[0]->isLeaf() &&
      Utils::hasApostrophe(myOperands[0]->getData()))
    {
      rs = new unsigned;
      ExpressionValue tmp = myOperands[1]->evaluate(from, NULL, process);
      if (tmp.type == ENUM)
	{
	  *rs = tmp.enumValue;
	}
      else // result is a boolean
	{
	  *rs = tmp.logical;
	}

      ls = new VarName;
      *ls = Utils::removeApostrophe(myOperands[0]->getData());
    }
  else
    {
      ls = NULL;
      rs = NULL;
    }
}

bool Expression::hasApostrophes()
{
  if (myLeafness)
    {
      if (Utils::hasApostrophe(myData))
	{
	  return true;
	}
    }
  else
    {
      for (unsigned i = 0; i < myOperands.size(); ++i)
	{
	  if (myOperands[i]->hasApostrophes())
	    {
	      return true;
	    }
	}
    }

  return false;
}

bool Expression::isLeaf()
{
  return myLeafness;
}

string Expression::getData()
{
  return myData;
}

vector<Expression*> Expression::getOperands()
{
  return myOperands;
}

OperatorType Expression::getOperator()
{
  return myOperator;
}
