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

Contributor(s): 
*/

#include "StringTree/static_vector.hh"


template <class VectorType>
class iCoilArray
{
 public:
    typedef unsigned char elem_t;
    
    iCoilArray() : ci( static_cast<unsigned>( -1 ) ),
                   coil_end_i( static_cast<unsigned>( -1 ) ),
                   base_i( 0 ) { }

    inline elem_t operator[]( unsigned i ) const { return elements[i]; }
    inline bool empty() const { return elements.empty(); }
    inline unsigned size() const { return elements.size(); }

    // Asettaa suhteellisen linkin pohja-arvon:
    inline void set_base_i( const unsigned b_i ) { base_i = b_i; }

    inline void reset( unsigned i = static_cast<unsigned>( -1 ) ) const
    {
        base_i = 2;
        SetCursor( i );
    }

    inline void Reset() const
    {
        coil_end_i = static_cast<unsigned>( -1 );
        coils.reset();
    }

    inline unsigned GiveCursor() const { return ci; }
    inline void SetCursor( unsigned i ) const { Reset(); ci = i; }
    
    inline void enter_coil( unsigned coil_start_i ) const
    {
        next_char(); // ci osoittaa nyt linkki‰ seuraavaan merkkiin.

        coils.push( coil_end_i ); // Ed. mahd. kiemuran loppu talteen.
        coils.push( ci ); // Kursori talteen.

        ci = coil_start_i; // Siirryt‰‰n uuden kiemuran alkuun.
        coil_end_i = next_branch_i; // Uuden kiemuran loppu.
    }

    inline void return_from_coil() const
    { ci = coils.pop(); coil_end_i = coils.pop(); }
    inline elem_t current_char() const { return elements[ci]; }
    inline unsigned current_char_i() const { return ci; }

    inline elem_t next_char() const
    {
        ++ci;
        if ( ci == coil_end_i ) { return_from_coil(); }
        return elements[ ci ];
    }

    inline unsigned size_minus_one( unsigned i ) const
    {
        if ( i & 0xFFFF0000 )
        {
            if ( i & 0xFF000000 ) { return 3; }
            else { return 2; }
        }
        else if ( i & 0xFF00 ) { return 1; }

        return 0;
    }

    inline elem_t enter_next_branch() const
    {
        SetCursor( next_branch_i );
        set_base_i( ci );
        return elements[ci];
    }

    inline elem_t enter_next_branch( elem_t cc ) const
    {
        cc &= 3; ci += cc; ci += 2;
        SetCursor( ci );
        set_base_i( ci );
        return elements[ci];
    }

    inline unsigned read_link( unsigned link_size_minus_one ) const
    {
        link_size_minus_one &= 3;
        next_branch_i = link_size_minus_one; next_branch_i += 2;
        
        unsigned link_i = next_char();
        
        for ( ; link_size_minus_one; --link_size_minus_one )
        {
            link_i <<= 8;
            link_i |= next_char();
        }

        link_i += base_i;
        next_branch_i += link_i;

        return link_i;
    }

    inline unsigned read_left_link() const
    {
        unsigned link_i = next_char();
        unsigned link_size_minus_one = link_i >> 6;
        next_branch_i = link_size_minus_one; ++next_branch_i;
        
        for ( link_i &= 0x3F; link_size_minus_one; --link_size_minus_one )
        {
            link_i <<= 8;
            link_i |= next_char();
        }

        link_i += base_i;
        next_branch_i += link_i;

        return link_i;
    }

    // With 5 as the maximum link size, the maximum number of coils
    // will be 2:
    static const unsigned maxNumberOfCoils = 2;
    typedef static_number_vector<unsigned, 2*maxNumberOfCoils> coils_t;

    inline void SetState( unsigned cI, unsigned baseI, unsigned coilEndI,
                          const coils_t& new_coils ) const
    {
        ci = cI;
        base_i = baseI;
        coil_end_i = coilEndI;
        coils = new_coils;
    }
    
    inline void QueryState( unsigned& cI, unsigned& baseI, unsigned& coilEndI,
                            coils_t& ref_coils ) const
    {
        cI = ci;
        baseI = base_i;
        coilEndI = coil_end_i;
        ref_coils = coils;
    }

 protected:
    VectorType elements;

    mutable unsigned ci;
    mutable unsigned coil_end_i;
    mutable unsigned base_i;
    mutable unsigned next_branch_i;

    mutable coils_t coils;
};
