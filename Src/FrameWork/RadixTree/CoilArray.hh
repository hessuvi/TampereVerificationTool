#ifndef SP_COIL_ARRAY
#define SP_COIL_ARRAY

#include "iCoilArray.hh"

#include <vector>
#include <string>

/* Linkin kontrollimerkin bitit:
   7. & 8. bitti: ei käytössä.
   5. & 6.: ei käytössä.
   4. bitti: ovatko molemmat haarat käytössä? (0: eivät, 1: ovat)
   3. bitti: kummassa haarassa suuremmat luvut (1: oikeassa, 0: vasemmassa)
   1. & 2. bitti: linkkiluvun koko (0: 1 merkkiä, 3: 4 merkkiä)
*/


class spCoilArray : public spiCoilArray
{
 public:
    typedef unsigned char elem_t;

    void clear() { elements.clear(); }
    void push_back( elem_t el ) { elements.push_back( el ); }

    /* SB */ void SetSpByte( unsigned i, unsigned b ) { elements[i] |= b; }

    unsigned size_minus_one( unsigned i ) const
    {
        if ( i & 0xFFFF0000 )
        {
            if ( i & 0xFF000000 ) { return 3; }
            else { return 2; }
        }
        else if ( i & 0xFF00 ) { return 1; }

        return 0;
    }

    void MakePartialChoiceStructure( int right_has_bigger_values )
    {
        /* SB */ SetSpByte( csi, sb );

        // ci osoittaa ensimmäiseen väärään merkkiin.
        right_has_bigger_values >>= 29;
        right_has_bigger_values &= 4;

        unsigned link_i = elements.links_size();
        elem_t foo = link_i & 15; link_i >>= 4; foo <<= 4;
        unsigned link_size_minus_one = size_minus_one( link_i );
        
        unsigned roller = link_size_minus_one * 8;

        link_size_minus_one |= right_has_bigger_values;
        elements.push_back( elements[ci] );
        elements[ci] = link_size_minus_one | foo;

        while ( true )
        {
            // Kopioidaan merkit.
            const elem_t c = next();
            elements.push_back( c );
            elements[ci] = link_i >> roller;
            if ( !roller ) { break; }
            roller -= 8;
        }
        // ci osoittaa viimeiseen linkkimerkkiin.

        elements.add_link( elements.size() );
    }

#define REPO( x )  c = next(); elements.push_back( c ); elements[ci] = x

    void make_left_link()
    {
        elements[link_control_i] |= 8; /* LCC */

        // ci osoittaa ensimmäiseen väärään merkkiin.
        unsigned i = elements.links_size();
        elem_t c; 

        if ( i & 0xFFFFC000 )
        {
            if ( i & 0xFFC00000 ) { i |= 0xC0000000; REPO( i >> 24 ); }
            else { i |= 0x800000; }
            REPO( i >> 16 );
            REPO( i >> 8 );
        }
        else if ( i & 0xFFC0 ) { i |= 0x4000; REPO( i >> 8 ); }
        REPO( i );
        // ci osoittaa viimeiseen linkkimerkkiin.

        elements.add_link( elements.size() );
    }

    elem_t next_key_byte() const
    {
        if ( ++ci == coil_end_i ) { return_from_coil(); }
        if ( sb <<= 1 ) { return elements[ci]; }
        /* SB */ csi = ci; sb = 1; special_bytes = elements[csi];
        if ( ++ci == coil_end_i ) { return_from_coil(); }
        return elements[ci];
    }

    // Enters a branch refferred by next_branch_i.
    elem_t enter_next_branch() const
    {
        Jump( next_branch_i );
        base_i = ci;

        bytes_in_branch = elements[ci];
        /* SB */ csi = ++ci; sb = 1; special_bytes = elements[csi];

        branch_id = last_link;
        return elements[++ci];
    }
    
    // Enters a branch referred by the parameter i.
    elem_t enter_branch( unsigned i ) const
    {
        Jump( i );
        base_i = ci;
        
        bytes_in_branch = elements[ci];
        /* SB */ csi = ++ci; sb = 1; special_bytes = elements[csi];

        branch_id = last_link;
        return elements[++ci];
    }

    elem_t bytesInBranch() const { return bytes_in_branch; }
};


#undef REPO
#endif
