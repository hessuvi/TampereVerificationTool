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

#ifdef CIRC_RENUM_HH_
#error "Include recursion"
#endif

#ifndef ONCE_RENUM_HH_
#define ONCE_RENUM_HH_
#define CIRC_RENUM_HH_

#include <vector>
#include "error_handling.hh"
#include <iostream>
using namespace std;
template<typename number_t>
class Renumber
{
 public:
    typedef std::vector<number_t> vector_t;
    typedef typename vector_t::size_type vector_size_t;
    typedef typename vector_t::iterator vector_iterator_t;

    Renumber() : next( 0 ) { }

    void SetOldMaxNumber( number_t m )
    {
        remap.clear();
        remap.resize( m + 1, 0 );
        next = 0;
    }

    number_t AddNumber( number_t n )
    {
        check_claim( n < remap.size(), "Renumber::AddNumber(): "
                     "index out of bounds" );

        number_t& new_number = remap[ n ];

        if ( new_number ) { return new_number; }
    
        ++next;
        new_number = next;

        return new_number;
    }

    void SetNewAmount( number_t a ) { next = a; }

    number_t newNumber( number_t old ) const
    {
        if ( old >= static_cast<number_t>( remap.size() ) )
        {
            // Invalid old number (perharps removed).
            return 0;
        }

        return remap[ old ];
    }
    
    number_t oldAmount() const { return remap.size() - 1; }
    number_t newAmount() const { return next; }
    
    bool hasNumberingChanged() const
    {
        for ( vector_size_t i = 0; i < remap.size(); ++i )
        {
            if ( i != remap[ i ] ) { return true; }
        }

        return false;
    }

    vector_size_t
    RenumberVector( vector_t& numbers )
    {
        vector_iterator_t i = numbers.begin();
        vector_iterator_t f = i;

        // { begin <= i <= f }
        while ( f != numbers.end() )
        {
            // { begin <= i <= f < end }
            *i = newNumber( *f );
            if ( *i ) { ++i; }
            ++f;
            // { begin <= i <= f <= end }
        }

        // { begin <= i <= f = end }

        vector_size_t removed_elements = numbers.end() - i;
        if ( i != numbers.end() ) { numbers.erase( i, numbers.end() ); }

        return removed_elements;
    }

 private:
    number_t next;
    vector_t remap;
};


#undef CIRC_RENUM_HH_
#endif
