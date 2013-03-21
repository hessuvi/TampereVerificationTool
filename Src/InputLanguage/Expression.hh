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
// Input language compiler: Expression.hh
// by Timo Kivelä
//
// This class represents a node of an "expression tree". The node
// can be either an operator or a leaf with data. For example the
// expression "x != y" in a tree form would look like:
//
//         !=
//        |  |
//        x  y
//
// There, the "!=" node is an operator, x and y are leafs.
// An expression node knows how to evaluate itself, if given the 
// FinalStates (for evaluating variables) and the Process (for evaluating
// constants and information about enum types).

#ifndef IL_EXPRESSION_HH
#define IL_EXPRESSION_HH

#include "General.hh"
#include "Process.hh"

class FinalState;
class Process;
class Transition;

class Expression
{
public:
  Expression();
  ~Expression();

  // the string representation of this node and the subtree.
  string toString();

  // If data is added, operator/operands are invalidated, this becomes a leaf
  void addData(string s);

  // If the expression is set to be an operator, the data is invalidated.
  void setOperator(OperatorType oper);

  // Operands are added from left to right
  void addOperand(Expression* newOperand);

  // Calculates the value of the expression. Gets the values
  // of the variables from the states given. Process is needed to get information
  // on the enum types. The last parameter is normally NULL, but sometimes transition
  // contains local variables that can be referenced in the expression. Then variables
  // that are not found in the states will be looked for in this transition.
  ExpressionValue evaluate(FinalState* from, 
			   FinalState* to, 
			   Process* process, 
			   Transition* tr = NULL,
			   unsigned gateNumber = 0);

  // Sees which variables appear in this expression and it's subexpressions and
  // adds their names (without ') to the transition's handled-list
  void markHandledVars(Transition* tr);

  // sees if this is a "real" assignment, which means that it is of the form 
  // x' = <expression>, where x is any variable and <expression> doesn't contain
  // any ':d variables. If it is, ls will contain x and rs the value of the right
  // side, evaluated in the final state "from".
  void isRealAssignment(FinalState* from, Process* process, VarName*& ls, unsigned*& rs);

  bool isLeaf();

  // only applicable to leaf nodes
  string getData();

  // only applicable to operator nodes
  vector<Expression*> getOperands();
  OperatorType getOperator();

  bool hasApostrophes();

  static long evaluations;

private:
  ExpressionValue evaluateLeaf(FinalState* from, 
			       FinalState* to, 
			       Process* process,
			       Transition* tr = NULL,
			       unsigned gateNumber = 0);

  unsigned getVariableValue(FinalState* from,
			     FinalState* to,
			     Process* process,
			     Transition* tr,
			     unsigned gateNumber,
			     unsigned enumType);

  vector<Expression*> myOperands;
  string myData;
  OperatorType myOperator;
  bool myLeafness;

  bool cached, myConstantness;
  unsigned cachedValue;
  unsigned cachedType;
};

#endif

