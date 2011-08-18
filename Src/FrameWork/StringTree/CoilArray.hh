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

#include "iCoilArray.hh"
#include "CharArray.hh"
#include <string>

/* Kontrollimerkin bitit:
   
   1. & 2. bitti: linkkiluvun koko (0: 1 merkkiä, 3: 4 merkkiä)
   3. bitti: kummassa haarassa suuremmat luvut (1: oikeassa, 0: vasemmassa)
   4. bitti: ovatko molemmat haarat käytössä? (0: eivät, 1: ovat)
   
   Kontrollimerkin tunnistaa siitä, että se on maksimissaan luku 15.
*/

#define isLink( X )  ( ( X ) <= 15 )
#define hasTwoChildren( X )  ( ( X ) & 8 )
#define SetTwoChildren( X )  X |= 8
#define biggerValuesInRight( X )  ( ( X ) & 4 )
#define isp_smallerOnRight( X )  ( 2 - ( ( X ) & 4 ) )

typedef unsigned char char_t;

#define link_size_part( X )  ( ( X ) & 3 )

#define END_CHAR  16
#define NUMBER_OF_END_CHARS  5
#define COIL_ARRAY_SIZE 5


class CoilArray : public iCoilArray<CharArray>
{
 public:
    typedef unsigned char elem_t;
    
    CoilArray() :
      iCoilArray<CharArray>()
    {
      for ( unsigned i = 0; i < COIL_ARRAY_SIZE; ++i ) {
	leq_five_c[i] = 0;
	}
    }

    ~CoilArray() {
      }
    
    inline bool isUsual( unsigned siz )
    {
        if ( siz > COIL_ARRAY_SIZE )
        {
            if ( elements.empty() ) { return false; }
            reset(); return true;
        }

        --siz;
        unsigned& iref = leq_five_c[siz];
        if ( iref ) { reset( iref ); return true; }
        iref = elements.size() - 1; return false;
    }
     
    inline void clear() { elements.clear(); }
    inline void push_back( elem_t el ) { elements.push_back( el ); }

    inline void AddBack( const std::string& str, unsigned i = 0 )
    {
        for ( ; i < str.size(); ++i ) { elements.push_back( str[ i ] ); }
        EndString();
    }

    inline void EndString()
    {
        for ( unsigned i = 0; i < NUMBER_OF_END_CHARS; ++i )
        { elements.push_back( END_CHAR ); }
    }

    inline elem_t next_char()
    {
        ++ci;
        if ( ci == coil_end_i ) { return_from_coil(); }
        link_control_i = ci;
        return elements[ ci ];
    }

    void MakeBranch( int right_has_bigger_values )
    {
        // ci osoittaa ensimmäiseen väärään merkkiin.
        right_has_bigger_values >>= 29;
        right_has_bigger_values &= 4;
        
        const unsigned link_i = elements.size() - base_i;
        unsigned link_size_minus_one = size_minus_one( link_i );

        unsigned roller = link_size_minus_one * 8;
        link_size_minus_one |= right_has_bigger_values;
        elements.push_back( elements[ci] );
        elements[ci] = link_size_minus_one;
      
        //int endMarkFound = 0;
        //unsigned foo = 1;

        while ( true )
        {
            // Kopioidaan merkit.
            const elem_t c = next_char();
            elements.push_back( c );
            elements[ci] = link_i >> roller;
            if ( !roller ) { break; }
            roller -= 8;
        }
        // ci osoittaa viimeiseen linkkimerkkiin.
    }

#define REP( x )  c = next_char(); elements.push_back( c ); elements[ci] = x

    void make_left_link()
    {
        SetTwoChildren( elements[link_control_i] );

        // ci osoittaa ensimmäiseen väärään merkkiin.
        unsigned i = elements.size();
        i -= base_i;
        char_t c;

        if ( i & 0xFFFFC000 )
        {
            if ( i & 0xFFC00000 ) { i |= 0xC0000000; REP( i >> 24 ); }
            else { i |= 0x800000; }
            REP( i >> 16 );
            REP( i >> 8 );
        }
        else if ( i & 0xFFC0 ) { i |= 0x4000; REP( i >> 8 ); }
        REP( i );
        // ci osoittaa viimeiseen linkkimerkkiin.
    }

    inline elem_t enter_next_branch()
    {
        SetCursor( next_branch_i );
        set_base_i( ci );
        link_control_i = ci;
        return elements[ci];
    }

    inline elem_t& enter_next_branch( elem_t cc )
    {
        cc &= 3; ci += cc; ci += 2;
        SetCursor( ci );
        set_base_i( ci );
        return elements[ci];
    }

    void WriteElement( const elem_t& el ) { elements[ci] = el; }
    void WriteElement( unsigned i, const elem_t& el ) { elements[i] = el; }

    unsigned link_control_i;

    unsigned leq_five_c[COIL_ARRAY_SIZE];
};
