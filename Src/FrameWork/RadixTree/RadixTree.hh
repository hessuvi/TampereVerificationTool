#ifndef RADIX_TREE_HH
#define RADIX_TREE_HH

#include "CoilArray.hh"

#include <string>
#include <vector>
#include <iostream>


class RadixTree
{
 public:
    RadixTree( unsigned keySize ) :
        coil_array(), siz( 0 ), key_size( keySize ), key_seq( 0 ),
        adds( 0 ), rev_reads( 0 )
    {
        std::cout << "RadixTree::key_size == " << keySize << std:: endl;
    }


    ~RadixTree()
    {
        std::cout << "RadixTree::siz == " << siz << std::endl;
        std::cout << "RadixTree::adds == " << adds << std::endl;   
        std::cout << "RadixTree::rev_reads == " << rev_reads << std::endl;   
    }

         
    // Returns false if the key exists already. The last reference
    // parameter gives out the identity of the stored key.
    bool add( const std::vector<unsigned char>& key, unsigned, unsigned& key_id )
    {
        ++adds;

        /*
        unsigned char C = key[key_size - 1];
        unsigned char* k = const_cast<unsigned char*>( key );
        k[key_size - 1] = k[0]; k[0] = C;
        */

        //coil_array.debug();


        if ( !siz )
        {
            siz = 1; coil_array.add_link( 0 ); coil_array.add_link( 0 );
            AddBackWithData( key, 0 ); key_id = 1;         
            
            //  std::cout << "AFT" << std::endl; coil_array.debug();

            /*
            unsigned char C = key[key_size - 1];
            unsigned char* k = const_cast<unsigned char*>( key );
            k[key_size - 1] = k[0]; k[0] = C;
            */

            return true;
        }

        coil_array.full_reset();
        
        for ( unsigned i = 0; i < key_size; ++i )
        {
            //            if ( key[i] > 255 ) { std::cout << "BUGI" << std::endl; }

            const unsigned back_link = coil_array.get_branch_id();
            const int r = match( key[i] );
            if ( r )
            {
                if ( r == left_link )
                { coil_array.make_left_link(); }
                else
                { coil_array.MakePartialChoiceStructure( r ); }
                
                AddBackWithData( key, back_link, i );

                key_id = ++siz;

                //  std::cout << "AFT" << std::endl; coil_array.debug();

                /*
                unsigned char C = key[key_size - 1];
                unsigned char* k = const_cast<unsigned char*>( key );
                k[key_size - 1] = k[0]; k[0] = C;
                */

                return true;
            }
        }

        key_id = coil_array.get_branch_id();

        /*
        C = key[key_size - 1];
        k[key_size - 1] = k[0]; k[0] = C;
        */

        return false;
    }

    unsigned readData( unsigned loc_i ) const { return loc_i; }

    //  const unsigned*
    void readKey( unsigned key_id, std::vector<unsigned char>& stateNumbers ) const
    {
        key_seq = &stateNumbers;
 ReadKey( key_id );
    ++rev_reads;

    /*    unsigned char C = key_seq[key_size - 1];
    key_seq[key_size - 1] = key_seq[0]; key_seq[0] = C;
    */
    }
    //    return &(key_seq[0]); }

    unsigned size() const { return siz; }

    unsigned getAllocatedMemoryAmount() const
    { return coil_array.memoryUsed(); }

    //    std::vector<unsigned char>& GiveKeySeq() { return key_seq; }

 private:
    typedef unsigned char byte_t;

    int match( unsigned char x ) const
    {
	int c = coil_array.next_key_byte();
        for ( ; coil_array.is_curr_sp_byte();
              c = coil_array.enter_next_branch() )
        {
            int key = coil_array.read_link( c );
           
            //std::cout << "KEY==" << (char)key << std::endl;


#define r_isp_smallerOnRight( X )  ( 2 - ( ( X ) & 4 ) )
#define r_hasTwoChildren( X )  ( ( X ) & 8 )
//#define r_SetTwoChildren( X )  X |= 8
            //#define r_biggerValuesInRight( X )  ( ( X ) & 4 )


            key -= x; key *= r_isp_smallerOnRight( c );

            // Right branch, is x greater than key?
            if ( key > 0 ) { continue; }
            
            coil_array.enter_coil();

            // Left branch, is x lesser than key?
            if ( key )
	    {
                if ( r_hasTwoChildren( c ) )
                { coil_array.read_left_link(); continue; }
                return left_link;
            }
            
            // The node itself, x == key.
	    if ( r_hasTwoChildren( c ) )
	    { coil_array.read_left_link(); coil_array.enter_coil(); }
              
	    return 0;
	}
            
	c -= x; return c;
    }

    // Functions for searching the key backwards:
    void ReadKey( unsigned loc_i ) const
    {
        // Reading the tail of the key:
        loc_i = coil_array.get_link( loc_i );
        //        std::cout << "::ReadKey loc_i == " << loc_i << std::endl;
        unsigned bytes_read = 0; read_key_part( loc_i, bytes_read );
        if ( bytes_read == key_size ) { return; }

        // std::cout << "::ReadKey bytes_read == " << bytes_read << std::endl;

        // Reading the other parts of the key:
        while ( true )
        {
            loc_i = coil_array.get_link( read_data() );
            // std::cout << "::ReadKey *loc_i == " << loc_i << std::endl;
            unsigned bytes_left = bytes_read;
            read_key_part( loc_i, bytes_read );
            if ( bytes_read == key_size ) { return; }
            do { next_byte_in_branch(); } while ( --bytes_left ); 
        }
    }

    void read_key_part( unsigned from_i, unsigned& bytes_read ) const
    {
        byte_t first_byte = first_byte_in_branch( from_i );
        unsigned bytes_in_branch = coil_array.bytesInBranch();
        //        std::cout << "::read_key_part bytes_in_branch == " << bytes_in_branch << std::endl;
        unsigned i = key_size - bytes_in_branch;
        unsigned last = key_size - bytes_read;
        bytes_read += ( bytes_in_branch - bytes_read );

        (*key_seq)[i++] = first_byte;
        for ( ; i < last; ++i )
        { (*key_seq)[i] = next_byte_in_branch(); }
    }

    byte_t first_byte_in_branch( unsigned i ) const
    {
	const byte_t byte = coil_array.enter_branch( i );
        if ( coil_array.is_curr_sp_byte() )
        {
            const byte_t key = coil_array.read_link( byte );
            coil_array.enter_coil();

            // Jumping over a possible left link:
	    if ( r_hasTwoChildren( byte ) )
	    { coil_array.read_left_link(); coil_array.enter_coil(); }
                
	    return key;
	}
            
	return byte;
    }
    
    byte_t next_byte_in_branch() const
    {
	const byte_t byte = coil_array.next_key_byte();
        if ( coil_array.is_curr_sp_byte() )
        {
            //std::cout << "dflksjfljds" << std::endl;
            const byte_t key = coil_array.read_link( byte );
            coil_array.enter_coil();

            // Jumping over a possible left link:
	    if ( r_hasTwoChildren( byte ) )
	    { coil_array.read_left_link(); coil_array.enter_coil(); }
                
	    return key;
	}
            
	return byte;
    }
    
    void AddBackWithData( const std::vector<unsigned char>& end_of_key,
                          unsigned data, unsigned i = 0 )
    {
        // The byte containing the length of the end of the key:
        coil_array.push_back( key_size - i ); // **ST_LIMIT**

        // Storing the end of the key:
        coil_array.push_back( 0 );
        for ( unsigned sb = 1; true; )
        {
            coil_array.push_back( end_of_key[i] );
            if ( ++i >= key_size ) { break; }
            if ( ++sb == 9 ) { sb = 1; coil_array.push_back( 0 ); }
        }


        coil_array.push_back( data >> 24 );
        coil_array.push_back( data >> 16 );
        coil_array.push_back( data >> 8 );
        coil_array.push_back( data );

        /*
        // Storing the data:
        unsigned data_bytes = 1;
        while( true )
        {
            if ( data <= 127 )
            { coil_array.push_back( data | 128 ); break; }
            coil_array.push_back( data & 127 );
            data >>= 7; ++data_bytes;
        }

        // Adding dummy bytes if need be:
        while ( data_bytes < 4 ) { coil_array.push_back( 255 ); ++data_bytes; }
        */
    }

    unsigned read_data() const
    {
        unsigned data = coil_array.next();
        data <<= 8; data |= coil_array.next();
        data <<= 8; data |= coil_array.next();
        data <<= 8; data |= coil_array.next();
        return data;
        
        /*
        unsigned data = 0; unsigned shifts = 0;
        while ( true )
        {
            unsigned byte = coil_array.next();
            if ( byte > 127 )
            { data |= ( byte & 127 ) << shifts; return data; }
            data |= byte << shifts; shifts += 7;
        }
        return 0; // Never here. */
    }

    mutable spCoilArray coil_array;
    unsigned siz;
    mutable byte_t key_byte;

    static const int left_link = 555;

    const unsigned key_size;
    mutable std::vector<unsigned char>* key_seq;

    mutable unsigned adds;
    mutable unsigned rev_reads;
};


#endif
