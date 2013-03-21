#ifndef SPI_COIL_ARRAY
#define SPI_COIL_ARRAY

#include "CharArray.hh"
#include "StringTree/static_vector.hh"
#include "TVTvec.hh"

#include <vector>


class spiCoilArray
{
 public:
    void debug()
    {
        for ( unsigned i = 0; i < 80; ++i ) { std::cout << i % 10; }
        std::cout << std::endl;
        
        for ( unsigned i = 0; i < elements.size(); ++i )
        {
            unsigned char c = elements[i];
            if ( c == 128 ) { std::cout << "E"; continue; }
            if ( c >= 32 && c <= 126 || c >= 161 )
            { std::cout << c; continue; }
            std::cout << "[" << static_cast<unsigned>( c ) << "]";
        }
        
        std::cout << std::endl << std::endl;
    }
    
    typedef unsigned char elem_t;
    
    spiCoilArray() : ci( static_cast<unsigned>( -1 ) ),
                     coil_end_i( static_cast<unsigned>( -1 ) ),
                     base_i( 0 ), back_link_i( 0 ) { }

    elem_t operator[]( unsigned i ) const { return elements[i]; }
    bool empty() const { return elements.empty(); }
    unsigned size() const { return elements.size(); }
    unsigned memoryUsed() const { return elements.memoryUsed(); }

    void change_back_link() { back_link_i = base_i; }
    unsigned give_back_link() const { return back_link_i; }

    void full_reset() const
    {
        bytes_in_branch = elements[0]; back_link_i = 0; Jump( 0 );
        base_i = 1;
        /* SB */ sb = 128;

        branch_id = 1;
    }

    /*
      void reset2( unsigned i ) const
      {
      //     SB  sb = 128;
      base_i = i; back_link_i = i;
      Jump( i - 1 );
      } */
    
    unsigned GiveCursor() const { return ci; }

    void Jump( unsigned i ) const
    { ci = i; coil_end_i = 0; coils.reset(); }
    
    void enter_coil( /*unsigned coil_start_i*/ ) const
    {
        next(); // ci osoittaa nyt linkki‰ seuraavaan merkkiin.

        coils.push( coil_end_i ); // Ed. mahd. kiemuran loppu talteen.
        coils.push( ci ); // Kursori talteen.

        ci = coil_start_i; // Siirryt‰‰n uuden kiemuran alkuun.
        coil_end_i = next_branch_i; // Uuden kiemuran loppu.
    }

    void return_from_coil() const
    { ci = coils.pop(); coil_end_i = coils.pop(); }
    elem_t current_char() const { return elements[ci]; }
    unsigned current_char_i() const { return ci; }

    /* SB */ elem_t is_curr_sp_byte() { return special_bytes & sb; }

    elem_t read_link( unsigned lcc ) const
    {
        link_control_i = ci;
        unsigned size_minus_one = lcc & 3;
        coil_start_i = -size_minus_one - 2;

        last_link = next();
        for ( ; size_minus_one; --size_minus_one )
        { last_link = ( last_link << 8 ) | next(); }
        last_link <<= 4; last_link |= lcc >> 4;

        next_branch_i = elements.get_link( last_link );
        coil_start_i += next_branch_i;

        return elements[coil_start_i];
    }

    void read_left_link() const
    {
        last_link = next();
        unsigned link_size_minus_one = last_link >> 6;
        coil_start_i = -link_size_minus_one - 2;
        
        for ( last_link &= 0x3F; link_size_minus_one; --link_size_minus_one )
        {
            last_link <<= 8;
            last_link |= next();
        }

        next_branch_i = elements.get_link( last_link );
        coil_start_i += next_branch_i;
    }

    // With 5 as the maximum link size, the maximum number of coils
    // will be 2:
    static const unsigned maxNumberOfCoils = 2;
    typedef static_number_vector<unsigned, 2*maxNumberOfCoils> coils_t;

    void StoreState()
    {
        Tci = ci;
        Tcoil_end_i = coil_end_i;
        Tbase_i = base_i;
        Tback_link_i = back_link_i;
        Tnext_branch_i = next_branch_i;
        Tcoils = coils;
    }

    void RestoreState()
    {
        ci = Tci;
        coil_end_i = Tcoil_end_i;
        base_i = Tbase_i;
        back_link_i = Tback_link_i;
        next_branch_i = Tnext_branch_i;
        coils = Tcoils;
    }

    unsigned get_branch_id() const { return branch_id; }
    unsigned get_last_read_link() const { return last_link; }
    unsigned get_link( unsigned i ) const
    { return elements.get_link( i ); }

    void add_link( unsigned i ) { elements.add_link( i ); }

    elem_t next() const
    {
        if ( ++ci == coil_end_i ) { return_from_coil(); }
        return elements[ci];
    }

 protected:
    spCharArray elements;

    mutable unsigned ci;
    mutable unsigned coil_end_i;
    mutable unsigned base_i;
    mutable unsigned back_link_i;

    mutable unsigned coil_start_i;
    mutable unsigned next_branch_i;

    mutable coils_t coils;

    mutable unsigned Tci;
    mutable unsigned Tcoil_end_i;
    mutable unsigned Tbase_i;
    mutable unsigned Tback_link_i;
    mutable unsigned Tnext_branch_i;
    mutable coils_t Tcoils;

    /* SB */ mutable unsigned csi; mutable elem_t sb;
    mutable elem_t special_bytes;

    mutable unsigned link_control_i;

    mutable elem_t bytes_in_branch; // **ST_LIMIT**

    mutable unsigned last_link;
    mutable unsigned branch_id;
};


#endif
