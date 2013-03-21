#include <cstdlib>
#include <string>
#include <iostream>

#include "../RadixTree.hh"


typedef std::vector<unsigned char> key_type;


key_type&
GenRandomKey( unsigned key_len )
{
    static key_type key;
    key.resize( key_len, ' ' );

    for ( unsigned i = 0; i < key_len; ++i )
    {
        double r = drand48();
        unsigned char val = static_cast<unsigned char>( 256 * r ); 
        /* if ( val == 0 ) { std::cout << "0" << std::endl; }
        else if ( val == 255 ) { std::cout << "255" << std::endl; }
        */
        key[i] = val;
    }

    //    std::cout << key << std::endl;

    return key;
}


int
main()
{
    std::cout << "Testing KeyCoDec's functions with 1 000 000 random keys."
              << std::endl;
    std::cout << "The test takes about one minute to run."
              << std::endl;
    
    std::string key( "sadfjaslkdjfapfw0rw0+9r3dfasdjfasldjfkdasf93" );

    RadixTree< std::string > codec( key.size() );

    for ( unsigned round = 0; round < 2000000; ++round )
    {
        codec.Encode( reinterpret_cast<const unsigned char*>( key.c_str() ) );
        codec.Decode();
        
        const std::vector<unsigned char>& key_seq = codec.GiveKeySeq();
        
        for ( unsigned i = 0; i < key.size(); ++i )
        {
            if ( key_seq[i] != key[i] )
            {
                std::cerr << "A bug in KeyCoDec class!!" << std::endl;
                return EXIT_FAILURE;
            }
        }

    }

    //return EXIT_SUCCESS;


    srand48( 555 );
    for ( unsigned klen = 1; klen < 51; ++klen )
    {
        RadixTree< std::vector<unsigned char> > codec( klen );

        for ( unsigned round = 0; round < 20000; ++round )
        {
            key_type key( GenRandomKey( klen ) );
            codec.Encode( &( key[0] ) );
            codec.Decode();
            
            const std::vector<unsigned char>& key_seq = codec.GiveKeySeq();
            
            for ( unsigned i = 0; i < key.size(); ++i )
            {
                if ( key_seq[i] != key[i] )
                {
                    std::cerr << "A bug in KeyCoDec class!!" << std::endl;
                    return EXIT_FAILURE;
                }
            }
            
        }
    }

    std::cout << "All seems to be ok." << std::endl;
    return EXIT_SUCCESS;
}
