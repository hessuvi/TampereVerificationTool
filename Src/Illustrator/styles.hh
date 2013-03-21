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

Contributor(s): Heikki Virtanen.
*/



#ifndef ILLUS_STYLES_HH
#define ILLUS_STYLES_HH
#ifndef MAKEDEPEND
#include <utility>
#include <string>
#include <map>
#endif



#include <cassert>
using namespace std;

class IlStyleItem: public pair<string,string>
{
public:
  IlStyleItem() {};
  IlStyleItem(const pair<string,string> &p): pair<string,string>(p) {};
  IlStyleItem(const string &col, const string &dashPattern)
    : pair<string,string>(col, dashPattern ) {};
  const string &color() { return first ; };
  const string &dash()  { return second ; };
};

class IlStyles: public map<string, IlStyleItem>
{
public:
  bool readFromFile(const string &styleFileName);
  
  const string &color(const string &action);
  const string &dash(const string &action) const;

  template <class ITER>
  void buildMap(ITER start, ITER end);
  const string &color( int action ) const;
  const string &dash( int action ) const;
private:
  typedef map<int,IlStyleItem> AlternateMap;
  AlternateMap d_map;
};



#endif // ILLUS_STYLES_HH
