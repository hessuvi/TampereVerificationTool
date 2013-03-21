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

#include "fparser.hh"

#include <iostream>
#include <vector>
#include <cctype>

using namespace std;

// Redefine getc() so that it caches scripting commands:
// ----------------------------------------------------
inline int ITokenStream::tvt_getc()
{
    while(true)
    {
        if(evaluatedValueIndex != ~0U)
        {
            int c = evaluatedValue[evaluatedValueIndex];
            if(evaluatedValue[++evaluatedValueIndex] == '\0')
            {
                evaluatedValueIndex = ~0U;
            }
            return c;
        }

        int c = getc(char_pipe_in);
        if(c=='#')
        {
            int c2 = getc(char_pipe_in);
            if(c2=='{')
            {
                ParseScriptCommand();
            }
            else
            {
                ungetc(c2, char_pipe_in);
                return c;
            }
        }
        else
        {
            return c;
        }
    }
}

inline void ITokenStream::tvt_ungetc(int c)
{
    if(evaluatedValueIndex != ~0U)
    {
        --evaluatedValueIndex;
    }
    else
    {
        ungetc(c, char_pipe_in);
    }
}

inline string ITokenStream::scriptErrorMsg(const string& msg)
{
    return Exs::catString("Script error near line ",
                          line+isEndOfLineChar(peek_char),
                          string(": ")+msg);
}

// Reads the scripting command.
// ---------------------------
// Return value:
//  0: Invalid
//  1: SET
//  2: LABEL
//  3: CB
//  4: EVAL
//  5: PRINT
//  6: SCAN
unsigned ITokenStream::readScriptCommand()
{
    switch(tvt_getc())
    {
      case 'S':
          {
              int c = tvt_getc();
              if(c=='E' && tvt_getc()=='T') return 1;
              else if(c=='C' && tvt_getc()=='A' && tvt_getc()=='N') return 6;
              return 0;
          }
      case 'L':
          if(tvt_getc()!='A' || tvt_getc()!='B' ||
             tvt_getc()!='E' || tvt_getc()!='L') return 0;
          return 2;
      case 'C':
          if(tvt_getc()!='B') return 0;
          return 3;
      case 'E':
          if(tvt_getc()!='V' || tvt_getc()!='A' || tvt_getc()!='L')
              return 0;
          return 4;
      case 'P':
          if(tvt_getc()!='R' || tvt_getc()!='I' ||
             tvt_getc()!='N' || tvt_getc()!='T') return 0;
          return 5;
    }
    return 0;
}

static inline string getEvalErrorMsg(int n)
{
    switch(n)
    {
      case 1: return "Division by zero";
      case 2: return "sqrt of negative value";
      case 3: return "log of negative value";
      case 4: return "Trigonometric error";
    }
    return "";
}

double ITokenStream::evaluateExpression(const string& expr)
{
    static vector<double> values;
    static string vars;

    values.clear();
    vars.erase();

    for(map<string,double>::iterator i = scriptVariables.begin();
        i != scriptVariables.end();)
    {
        vars += i->first;
        values.push_back(i->second);
        ++i;
        if(i != scriptVariables.end()) vars += ',';
    }
    if(values.empty()) values.push_back(0);

    FunctionParser fp;
    int res = fp.Parse(expr, vars);
    check_claim(res < 0,
                scriptErrorMsg
                (Exs::catString
                 (string(fp.ErrorMsg())+" (at expression position ",
                  res, ").")));

    double value = fp.Eval(&values[0]);
    check_claim(fp.EvalError() == 0,
                scriptErrorMsg(getEvalErrorMsg(fp.EvalError())+
                               " when evaluating expression."));

    return value;
}

// Reads the input until the given char is found and returns the read
// chars (not including the given one):
// ------------------------------------------------------------------
inline string ITokenStream::readScriptUntil(char endc)
{
    string res;
    char c;
    while((c=tvt_getc()) != endc)
    {
        check_claim(!isError() && !isEndOfLineChar(c),
                    scriptErrorMsg("Unterminated script command."));
        if(c!=' ') res += c;
    }
    return res;
}

static inline int doubleToInt(double d)
{
    return d<0 ? -int((-d)+.5) : int(d+.5);
}

void ITokenStream::ParseScriptCommand()
{
    switch(readScriptCommand())
    {
      case 0:
          check_claim(false, scriptErrorMsg("Invalid scripting command."));

      case 1:
          {
              string var;
              char ch; while((ch=tvt_getc())==' ');
              while(isalpha(ch)) { var+=ch; ch=tvt_getc(); }
              check_claim(var.size(),
                          scriptErrorMsg("Syntax error after SET."));
              check_claim(ch=='=',
                          scriptErrorMsg(string("SET ")+var+" must be "
                                         "followed by '='."));
              scriptVariables[var] = evaluateExpression(readScriptUntil('}'));
              break;
          }

      case 2:
          {
              string labelname = readScriptUntil('}');
              labels[labelname] = Label(ftell(char_pipe_in), line, chars);
              break;
          }

      case 3:
          {
              string labelname = readScriptUntil(':');
              if(doubleToInt(evaluateExpression(readScriptUntil('}'))) != 0)
              {
                  map<string, Label>::iterator iter = labels.find(labelname);
                  check_claim(iter != labels.end(),
                              scriptErrorMsg("Label not found."));
                  check_claim(fseek(char_pipe_in, iter->second.filepos,
                                    SEEK_SET) == 0,
                              scriptErrorMsg
                              (string("Can't seek input: ")+strerror(errno)));
                  line = iter->second.line;
                  chars = iter->second.chars;
              }
              break;
          }

      case 4:
          {
              int value = doubleToInt
                  (evaluateExpression(readScriptUntil('}')));
              sprintf(evaluatedValue, "%i", value);
              evaluatedValueIndex = 0;

              break;
          }

      case 5:
          std::cerr << evaluateExpression(readScriptUntil('}')) << std::endl;
          break;

      case 6:
          readScriptUntil('}');
          long curPos = ftell(char_pipe_in);
          int c;
          unsigned tmpline = (line==0?1:line), tmpchars=chars;
          while((c=getc(char_pipe_in)) != EOF)
          {
              ++tmpchars;
              if(isEndOfLine(c)) { ++tmpline; tmpchars=0; }
              else if(c=='#')
              {
                  if((c=getc(char_pipe_in))=='{' &&
                     (c=getc(char_pipe_in))=='L' &&
                     (c=getc(char_pipe_in))=='A' &&
                     (c=getc(char_pipe_in))=='B' &&
                     (c=getc(char_pipe_in))=='E' &&
                     (c=getc(char_pipe_in))=='L')
                  {
                      string labelname = readScriptUntil('}');
                      labels[labelname] = Label(ftell(char_pipe_in),
                                                tmpline, tmpchars);
                  }
                  else
                      ungetc(c, char_pipe_in);
              }
          }
          fseek(char_pipe_in, curPos, SEEK_SET);
          break;
    }
}
