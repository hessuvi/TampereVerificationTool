/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// RemoveDuplicates() removes all duplicate components from a sorted
//   vector.

#ifndef REMOVE_DUPLICATES_HH
#define REMOVE_DUPLICATES_HH

#include <vector>
#include "config.hh"


template<typename object_type>
static lsts_index_t
RemoveDuplicates( std::vector<object_type>& objects )
{
    typedef typename std::vector<object_type>::size_type vec_size_t;

    vec_size_t from_i = 1;
    vec_size_t to_i = 0;
    
    while ( from_i < objects.size() )
    {
        if ( objects[ from_i ] != objects[ to_i ] )
        {
            ++to_i;
            objects[ to_i ] = objects[ from_i ];
        }
        
        ++from_i;
    }

    --from_i;
    lsts_index_t removed_amount = from_i - to_i;
        
    while ( from_i != to_i )
    {
        objects.pop_back();
        --from_i;
    }

    return removed_amount;
}


#endif
