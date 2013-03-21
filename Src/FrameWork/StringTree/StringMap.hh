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

#ifndef STRINGMAP_HH
#define STRINGMAP_HH

#include "StringSet.hh"

/*
  Public interface of the StringMap<DataType> class template:

  StringMap();
  ~StringMap();
    
  bool add( const std::string& str, DataType data );
  // Adds the pair (string, data) to the map. Returns false if the
  // string was already in the map.

  bool find( const std::string& str, DataType& data );
  // Finds a string and assigns its satellite data to the second
  // parameter. Returns false if the string is not in the map.

  unsigned size();
  // Returns the number of pairs in the map.

*/

template<class DataType>
class StringMap : public StringTree
{
 public:
    bool // Returns true if the string wasn't in the tree already.
    add( const std::string& str, DataType data )
    {
        if ( coil_array.empty() )
        {
            AddBackWithData( str, data );
            ++number_of_strings;
            return true;
        }
	
        coil_array.reset();
         
        for ( unsigned i = 0; i < str.size(); ++i )
        {
            const int r = match( str[ i ] );
            if ( r )
            {
                if ( r == left_link ) { coil_array.make_left_link(); }
                else { coil_array.MakeBranch( r ); }
                AddBackWithData( str, data, i );
                ++number_of_strings;
                return true;
            }
        }

        const int r = match( string_end );
        if ( r )
        {
            if ( r == left_link ) { coil_array.make_left_link(); }
            else { coil_array.MakeBranch( r ); }
            AddBackWithData( "", data );
            ++number_of_strings;
            return true;
        }

        return false;
    }

    bool // Returns true if the string wasn't in the tree already.
    add_if_needed( const std::string& str, DataType data, DataType& read_data )
    {
        if ( coil_array.empty() )
        {
            AddBackWithData( str, data );
            ++number_of_strings;
            return true;
        }
	
        coil_array.reset();
        
        for ( unsigned i = 0; i < str.size(); ++i )
        {
            const int r = match( str[ i ] );
            if ( r )
            {
                if ( r == left_link ) { coil_array.make_left_link(); }
                else { coil_array.MakeBranch( r ); }
                AddBackWithData( str, data, i );
                ++number_of_strings;
                return true;
            }
        }
        
        const int r = match( string_end );
        if ( r )
        {
            if ( r == left_link ) { coil_array.make_left_link(); }
            else { coil_array.MakeBranch( r ); }
            AddBackWithData( "", data );
            ++number_of_strings;
            return true;
        }
        
        read_data = coil_array.next_char();
        for ( unsigned i = 1; i < sizeof( DataType ); ++i )
        {
            read_data <<= 8;
            read_data |= coil_array.next_char();
        }
        
        return false;
    }

    bool find( const std::string& str, DataType& read_data ) const
    {
        if ( coil_array.empty() ) { return false; }

        coil_array.reset();

        for ( unsigned i = 0; i < str.size(); ++i )
        {
            if ( match( str[ i ] ) ) { return false; }
        }
	
        if ( match( string_end ) ) { return false; }

        read_data = coil_array.next_char();
        for ( unsigned i = 1; i < sizeof( DataType ); ++i )
        {
            read_data <<= 8;
            read_data |= coil_array.next_char();
        }
        
        return true;
    }
    
    class iterator : public StringTree::iterator
    {
     public:
        iterator( StringTree& string_map, StateOfIterator s_of_it = begin ) :
            StringTree::iterator( string_map, s_of_it ) { }
        
        DataType target_data()
        {
            DataType read_data = coil_array->next_char();
            for ( unsigned i = 1; i < sizeof( DataType ); ++i )
            {
                read_data <<= 8;
                read_data |= coil_array->next_char();
            }
            return read_data;
        }

    };

 private:
    void AddBackWithData( const std::string& str, DataType data,
                          unsigned i = 0 )
    {
        for ( ; i < str.size(); ++i ) { coil_array.push_back( str[ i ] ); }
        coil_array.push_back( END_CHAR );
        unsigned roller = sizeof( DataType );
        i = roller;
        --roller; roller *= 8;

        do
        {
            coil_array.push_back( data >> roller );
            roller -= 8;
        } while ( roller );

        coil_array.push_back( data );
                
        for ( ; i < NUMBER_OF_END_CHARS - 1; ++i )
        { coil_array.push_back( END_CHAR ); }
    }

};


#endif
