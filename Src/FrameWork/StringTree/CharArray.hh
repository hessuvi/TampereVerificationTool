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

// * When the array is full and its size < table_size, the size is doubled.
//   If size >= table_size, the size is increased by table_size.

#ifndef CHARARRAY_HH
#define CHARARRAY_HH

#include <vector>


class CharArray
{
 public:
    typedef unsigned char char_t;

    CharArray();
    ~CharArray();
    CharArray( const CharArray& src );
    CharArray& operator=( const CharArray& src );

    const char_t* const CreatePlainCharArray() const;

    inline bool empty() const { return !next_i; }
    inline unsigned size() const { return next_i; }

#define GET_CHAR( x )  ( tables[ (x) / table_size ][ (x) % table_size ] )

    inline const char_t operator[] ( unsigned i ) const
    { return GET_CHAR( i ); }
    inline char_t& operator[] ( unsigned i ) { return GET_CHAR( i ); }

    inline void push_back( char_t el )
    {
        if ( next_i == end_i ) { make_bigger(); }
        GET_CHAR( next_i ) = el;
        ++next_i;
    }     

#undef GET_CHAR

    inline void clear() { next_i = 0; }

 private:
    void CopyTables( const CharArray& src );
    void make_bigger();

    static const unsigned table_size = 65536;

    std::vector<char_t*> tables;
    unsigned next_i;
    unsigned end_i;
};


#endif
