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

inline int tvt_getc();
inline void tvt_ungetc(int c);
void ParseScriptCommand();
unsigned readScriptCommand();
inline std::string scriptErrorMsg(const std::string&);
double evaluateExpression(const std::string&);
inline std::string readScriptUntil(char endc);

//int dummy[1000];

std::map<std::string, double> scriptVariables;

struct Label
{
    long filepos;
    unsigned line, chars;

    inline Label(): filepos(0), line(0), chars(0) {}
    inline Label(long fp, unsigned l, unsigned c):
        filepos(fp), line(l), chars(c) {}
};

std::map<std::string, Label> labels;

char evaluatedValue[30];
unsigned evaluatedValueIndex;
