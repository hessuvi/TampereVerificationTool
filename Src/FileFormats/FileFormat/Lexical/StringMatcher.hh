/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// FILE_DES: StringMatcher.hh: FileFormats/FileFormat/Lexical
// Timo Erkkilä

// StringMatcher is a finite automaton that tests input characters against
//   set of strings it's been constructed with. StringMatcher knows a
//   longest matching string (if it exists) with each input.

// $Id: StringMatcher.hh 1.4.1.3 Wed, 04 Jun 2003 23:03:58 +0300 timoe $
// 
// $Log: ITokenStream.hh,v $
// Revision 1.1  1999/08/09 10:34:04  timoe
// Initial revision
//

#ifndef STRINGMATCHER_HH
#define STRINGMATCHER_HH

#include <vector>
#include <string>
#include "charconv.hh"


class StringMatcher
{
 public:
    StringMatcher() { ClearPatterns(); }

    void ClearPatterns()
    { states.clear(); states.push_back( State() ); Reset(); next_i = 1; }
    unsigned AddPattern( const std::string& pattern )
    {
        unsigned s = 0;
        unsigned i = 0;
        for ( unsigned t = 0;
              i < pattern.size() &&
                  ( t = states[ s ].Action( pattern[ i ], case_sensitive ) );
              ++i )
        {
            s = t;
        }
        
        for ( ; i < pattern.size(); ++i )
        {
            const unsigned new_state = states.size();
            Transition tr = { pattern[ i ], new_state };
            states[ s ].AddTransition( tr );
            states.push_back( State() );
            s = new_state;
        }

        states[ s ].SetAccState( next_i );
        return next_i++;
    }

    void Reset() { curr_state = 0; input_str.erase(); matched = 0;
                   case_sensitive = true; }
    bool input( char c )
    {
        curr_state = states[ curr_state ].Action( c, case_sensitive );

        if ( curr_state )
        {
            input_str += c;
            if ( states[ curr_state ].isAccState() )
            { matched = input_str.size(); }
        }
        
        return curr_state;
    }

    std::string longestMatch() const { return input_str.substr( 0, matched ); }
    std::string leftOver() const { return input_str.substr( matched ); } 
    const std::string& inputString() const { return input_str; } 
    unsigned isAccState() const { return states[ curr_state ].isAccState(); }

    void SetCaseSensitive( bool b ) { case_sensitive = b; }

 private:
    struct Transition
    {
        char c;
        unsigned dest;
    };
    
    class State
    {
     public:
        State() : acc_state( 0 ) { }
        void AddTransition( Transition tr ) { transitions.push_back( tr ); }
        void SetAccState( unsigned index ) { acc_state = index; }
        
        unsigned Action( char c, bool cs ) const
        {
            for( unsigned i = 0; i < transitions.size(); ++i )
            {
                if ( cs )
                {
                    if ( c == transitions[ i ].c )
                    { return transitions[ i ].dest; }
                }
                else if ( ToUpperCase( c ) ==
                          ToUpperCase( transitions[ i ].c ) )
                { return transitions[ i ].dest; }
            }
            return 0;
        }
        
        unsigned isAccState() const { return acc_state; }
        
     private:
        std::vector<Transition> transitions;
        unsigned acc_state;
    };

    
    std::vector<State> states;
    unsigned curr_state;
    std::string input_str;
    unsigned matched;
    bool case_sensitive;
    unsigned next_i;
};


#endif
