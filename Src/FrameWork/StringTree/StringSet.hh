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

#ifndef STRINGSET_HH
#define STRINGSET_HH

#include "CoilArray.hh"

#include <string>
#include <vector>


class StringTree
{
 public:
    StringTree();
    // ~StringTree();
    // StringTree( const StringTree& ); 
    // StringTree& operator=( const StringTree& ); 
  
    inline bool // Returns true if the string wasn't in the tree already.
    add( const std::string& str )
    {
        unsigned i = 0;

        if ( coil_array.isUsual( str.size() ) )
        {
	    const int result = matchStr( str, i );
	    if ( !result ) { return false; }

	    if ( result == left_link ) { coil_array.make_left_link(); }
	    else { coil_array.MakeBranch( result ); }
	 }

	coil_array.AddBack( str, i );
	++number_of_strings;
	return true;
    }

    /*
    inline bool // Returns true if the string wasn't in the tree already.
    addX( const std::string& str )
    {
        std::string sTr( str );
        char c = sTr[0] & 1; str[0] &= ~1;
        for ( unsigned i = 1; i < str.size(); ++i )
        {
            if ( i % sizeof( char ) ) { c =<< 1; c |= sTr[i] & 1; }
            else { sTr.push_back( c ); c = sTr[i] & 1; }
            str[i] &= ~1;
        }

        unsigned i = 0;

        if ( coil_array.isUsual( str.size() ) )
        {
            const int result = matchStr( str, i );
            if ( !result ) { return false; }

            if ( result == left_link ) { coil_array.make_left_link(); }
            else { coil_array.MakeBranch( result ); }
	}

        coil_array.AddBack( str, i );
        ++number_of_strings;
        return true;
        }*/

    bool // Returns true if the string wasn't in the tree already.
    add2( const std::string& str )
    {
        if ( !coil_array.isUsual( str.size() ) )
        {
            coil_array.AddBack( str );
            ++number_of_strings;
            return true;
        }
        
        //        coil_array.reset();
        
        for ( unsigned i = 0; i < str.size(); ++i )
        {
            const int r = match( str[i] );
            if ( r )
            {
                if ( r == left_link ) { coil_array.make_left_link(); }
                else { coil_array.MakeBranch( r ); }
                coil_array.AddBack( str, i );
                ++number_of_strings;
                return true;
            }
        }
        
        const int r = match( string_end );
        if ( r )
        {
            if ( r == left_link )
            {
                coil_array.make_left_link();
            }
            else { coil_array.MakeBranch( r ); }
            coil_array.EndString();
            ++number_of_strings;
            return true;
        }
        
        return false;
    }
    
    
    bool find( const std::string& str );
    bool remove( const std::string& str );
    inline unsigned size() { return number_of_strings; }
    void clear();

    class iterator
    {
     public:
        enum StateOfIterator
        {
            end = 0,
            front,
            begin,
            back,
            other
        };
        
        iterator( StringTree& string_set,
                  StateOfIterator s_of_it = begin );
        ~iterator();
        iterator( const iterator& cit ) { *this = cit; }
        iterator& operator=( const iterator& cit );
        
        const std::string& operator*() const { return current_string; }
        iterator& operator++() { next(); return *this; }
        iterator& operator--() { previous(); return *this; }
        
        bool is_at_end() const { return !state_of_iterator; }
        bool is_at_front() const { return state_of_iterator == front; }
        
        bool operator!=( const iterator& it ) const
        { return string_end_i != it.string_end_i ||
              coil_array != it.coil_array; }
        bool operator==( const iterator& it ) const
        { return !operator!=( it ); }
        
        void delete_target()
        { coil_array->WriteElement( string_end_i, removed_string ); }
        
        // This operator does not have a function:
        // const HistoryNode* const operator->() const;

     protected:
        CoilArray* coil_array;

     private:
        char_t smallest( char_t cc, int str_i );
        char_t biggest( char_t cc, int str_i );
        void first();
        void last();
        
        void read_string( char_t c );
        void read_string2( char_t c );

        enum Option
        {
            LESSER,
            GREATER,
            EQUAL
        };
        
        void next();
        void previous();
        
        unsigned dfs_push( int str_i, char_t c, Option option );
        bool dfs_pop( int& str_i, char_t& c, Option& option );

        std::string current_string;
        unsigned string_end_i;
        
        struct DfsStackElem
        {
            DfsStackElem* clone();
            
            unsigned ci;
            unsigned base_i;
            unsigned coil_end_i;
            CoilArray::coils_t coils;
            unsigned str_i;
            unsigned char c;
            Option option;
        };
        
        std::vector<DfsStackElem*> dfs_stack;
        
        StateOfIterator state_of_iterator;
    };
    
 protected:
    int match( int x ) const
    {
	int c = coil_array.next_char();
        for ( ; isLink( c ); c = coil_array.enter_next_branch() )
        {
            unsigned coil_start_i = coil_array.read_link( c );
            int key = coil_array[coil_start_i];
            key -= x; key *= isp_smallerOnRight( c );
	    
            // Oikea haara?
            if ( key > 0 ) { continue; }
            
            coil_array.enter_coil( coil_start_i );
            
            // Vasen haara?
            if ( key )
	    {
		if ( hasTwoChildren( c ) )
		{ coil_array.read_left_link(); continue; }

		return left_link;
	    }

            // Solmu itse?
	    if ( hasTwoChildren( c ) )
	    {
		coil_start_i = coil_array.read_left_link();
		--coil_start_i;
		coil_array.enter_coil( coil_start_i );
	    }
                
	    return 0;
	}
            
	c -= x; return c;
    }

inline int
matchStr( const std::string& str, unsigned& i ) const
{
    char* chars = const_cast<char*>( str.c_str() );
    chars[str.size()] = string_end;
    for ( ; i <= str.size(); ++i )
    {
        int x = chars[i];
                
        for ( int c = coil_array.next_char();
              ;
              c = coil_array.enter_next_branch() )
        {
            if ( !isLink( c ) )
            {
                c -= x;
                if ( !c ) { break; }
		chars[str.size()] = 0;
                return c;
            }
            
            unsigned coil_start_i = coil_array.read_link( c );
            int key = coil_array[coil_start_i];
            key -= x; key *= isp_smallerOnRight( c );
            
            // Oikea haara?
            if ( key > 0 ) { continue; }
            
            coil_array.enter_coil( coil_start_i );
            
            // Vasen haara?
            if ( key )
            {
                if ( hasTwoChildren( c ) )
                { coil_array.read_left_link(); continue; }
                
                chars[str.size()] = 0;		    
                return left_link;
            }
            
            // Solmu itse.
            if ( hasTwoChildren( c ) )
            {
                coil_start_i = coil_array.read_left_link();
                --coil_start_i;
                coil_array.enter_coil( coil_start_i );
            }
            
            break;
        }
        
    }
    
    chars[str.size()] = 0;
    return 0;
}

    mutable CoilArray coil_array;
    unsigned number_of_strings;

    static const int left_link = 555;
    static const char_t string_end = 16;
    static const char_t removed_string = 17;

    friend class iterator;
};


typedef StringTree StringSet;


#endif
