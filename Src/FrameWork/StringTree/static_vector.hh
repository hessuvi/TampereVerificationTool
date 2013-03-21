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

// This is a safe and generic static array. Mainly for debugging use.

#ifndef STATIC_VECTOR_HH
#define STATIC_VECTOR_HH

#include "debug.hh"
#include <string>


template <class element_type, unsigned Size>
class static_vector
{
 public:
    inline static_vector( element_type el = element_type() ) { reset( el ); }
    inline static_vector( const static_vector& from ) { *this = from; } 
    inline static_vector& operator= ( const static_vector& from )
    {
        for ( unsigned i = 0; i < size; ++i )
        { elements[i] = from.elements[i]; }
        next_i = from.next_i;
        return *this;
    }

    inline void push( element_type el )
    {
        //        if ( next_i < size )
        //{
            elements[next_i] = el; ++next_i;
            return;
            //}

        //error( "an extra element pushed" );
    }

    inline element_type pop()
    {
        /*        if ( !next_i )
        {
            error( "an extra element popped" );
            }*/

        --next_i;
        element_type el = elements[next_i];
        elements[next_i] = 0;
        return el;
    }

    inline const element_type& operator[] ( unsigned i ) const
    { return get_el( i ); }

    inline element_type& operator[] ( unsigned i )
    { return const_cast<element_type&>( get_el( i ) ); }

    inline void reset( element_type el )
    {
        for ( unsigned i = 0; i < size; ++i )
        { elements[i] = el; }
        next_i = 0;
    }

    static const unsigned size = Size;

 private:
    inline const element_type& get_el( unsigned i ) const
    {
        /* if ( i >= size )
        { 
            DPRINT( "static_vector, size " << size <<
                    ": index (" << i << ") out of range" );
                    }*/
        return elements[i];
    }
  
    inline void error( std::string str ) const
    {
        DPRINT( "static_vector, size " << size << ": " << str );
        str = str;;
    }

    element_type elements[ size ];
    unsigned next_i;
};

template <typename element_type, unsigned Size>
class static_number_vector : public static_vector<element_type, Size>
{
 public:
    inline static_number_vector() : static_vector<element_type, Size>( 0 ) { }
    inline void reset() { static_vector<element_type, Size>::reset( 0 ); }
};


#endif
