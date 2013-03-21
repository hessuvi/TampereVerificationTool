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

#include "CharArray.hh"

#define init_size  65536 //32

#define DELETE_TABLES  for ( unsigned i = 0; i < tables.size(); ++i ) \
                       { delete[] tables[i]; }


CharArray::CharArray() :
    tables( 1, new char_t[ init_size ] ),
    next_i( 0 ),
    end_i( init_size ) { tables[0][0] = 0; }

CharArray::~CharArray() { DELETE_TABLES; }

CharArray::CharArray( const CharArray& src ) :
    
    tables( src.tables.size(), 0 ),
    next_i( src.next_i ),
    end_i( src.end_i )

{ CopyTables( src ) ; }

CharArray&
CharArray::operator=( const CharArray& src )
{
    if ( &src != this )
    {
        DELETE_TABLES;
        
        tables.resize( src.tables.size() );
        next_i = src.next_i;
        end_i = src.end_i;

        CopyTables( src );
    }
    
    return *this;
}

const CharArray::char_t* const
CharArray::CreatePlainCharArray() const
{
    char_t* const c_arr = new char_t[ size() ];
    for ( unsigned i = 0; i < size(); ++i )
    { c_arr[i] = operator[]( i ); }
    return c_arr;
}


// PRIVATE:

inline void    
CharArray::CopyTables( const CharArray& src )
{
    // Only one table is needed (may be bigger than usually):
    tables[0] = new char_t[ end_i ];
    
    // Setting table pointers:
    for ( unsigned i = 1; i < tables.size(); ++i )
    { tables[i] = tables[i-1] + table_size; }
    
    // Copying contents of source array:
    for ( unsigned i = 0; i < next_i; ++i )
    { tables[0][i] = src[i]; }
}

void
CharArray::make_bigger()
{
    //    if ( end_i >= table_size )
    //{
        // The size is increased by table_size:
        tables.push_back( new char_t[ table_size ] );
        end_i += table_size;
        //    return;
        //}
        /*
    // The size is doubled:
    char_t* const old_table = tables[0];
    char_t* const new_table = new char_t[ 2*end_i ];
    
    for ( unsigned i = 0; i < end_i; ++i )
    { new_table[i] = old_table[i]; }

    end_i *= 2;
    
    delete[] old_table;
    tables[0] = new_table;*/
}
