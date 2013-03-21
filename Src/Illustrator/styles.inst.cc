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



#include "styles.hh"

#ifndef MAKEDEPEND
#include <vector>
#include <fstream>
#endif



const int BUFFER_SIZE = 1024 ;
static char s_buffer[BUFFER_SIZE+1];

bool
IlStyles::readFromFile(const string &styleFileName)
{
  ifstream file(styleFileName.c_str());
  if( ! file )
    {
      return( false );
    }
  char delim;
  for( file >> delim ; file ; file >> delim )
    {
      file.getline(s_buffer, BUFFER_SIZE, delim);
      string actionName(s_buffer);
      string color;
      file >> color;
      if( !file )
        {
          return( false );
        }
      file.getline(s_buffer, BUFFER_SIZE);
      char *start;
      for( start = s_buffer ; *start && *start != '"' ; ++start )
        ;
      char *tail;
      for( tail = start+1; *start && *tail && *tail != '"' ; ++tail )
        ;
      if( *start == '"' && *tail == '"' )
        {
          *(tail)='\0';
          ++start;
          string dashpat(start);
          insert(make_pair(actionName,make_pair(color,dashpat)));
        }
      else
        {
          insert(make_pair(actionName,make_pair(color,string(""))));
        }
    }
  return( true );
}

const string &
IlStyles::color(const string &action)
{
  static const char *vareja[]
    = { "#D0D0D0",
        "#FF0000", "#0000FF", "#00E000",
        "#8010FF", "#E0E000", "#008000",
        "#FFA040", "#00A0FF", "#E060E0",
        "#FFA0FF", "#A0FF00", "#000080",
        "#800000", "#80FF80", "#A030A0"};
  static vector<string> varit(vareja, vareja+16);
  static unsigned current_color=0;
  static string default_dash[]={"", "xx ", "xxxx x ", "", "x "};
  static unsigned whichdash = 0;

  const_iterator pos = find(action);
  if( pos == end() )
    {
      unsigned color_idx = current_color % varit.size();
      ++ current_color;
      pos=insert(make_pair(action,
                           make_pair(varit[color_idx],
                                     default_dash[(whichdash++)%5]))).first;
    }
  assert( pos != end() && "action not in stylefile");
  return( (*pos).second.first );
}

const string &
IlStyles::dash(const string &action) const
{
  static const string default_dash("");

  const_iterator pos = find(action);
  //assert( pos != end() && "action not in stylefile");
  if( pos == end() )
    {
      return default_dash;
    }
  return( (*pos).second.second );
}


const string &
IlStyles::color( int action ) const
{
  AlternateMap::const_iterator pos = d_map.find(action);
  assert( pos != d_map.end() && "action not in stylefile");
  return( (*pos).second.first );
}

const string &
IlStyles::dash( int action ) const
{
  AlternateMap::const_iterator pos = d_map.find(action);
  assert( pos != d_map.end() && "action not in stylefile");
  return( (*pos).second.second );
}



