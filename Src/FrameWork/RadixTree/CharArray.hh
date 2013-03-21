// * When the array is full and its size < table_size, the size is doubled.
//   If size >= table_size, the size is increased by table_size.

#ifndef SP_CHAR_ARRAY_HH
#define SP_CHAR_ARRAY_HH

#include "TVTvec.hh"

#include <vector>


class spCharArray
{
 public:
    typedef unsigned char char_t;

#define init_size  65536 //32

#define DELETE_TABLES  for ( unsigned i = 0; i < tables.size(); ++i ) \
                       { delete[] tables[i]; }


spCharArray() :
    tables( 1, new char_t[ init_size ] ),
    next_i( 0 ),
    end_i( init_size ) { tables[0][0] = 0; }

~spCharArray() { DELETE_TABLES; }

    inline bool empty() const { return !next_i; }
    inline unsigned size() const { return next_i; }
    unsigned memoryUsed() const
    { return tables.size() * ( sizeof(char_t*) + table_size*sizeof(char_t) ) +
          links.allocatedMem(); }

#define GET_CHAR( x )  ( tables[ (x) / table_size ][ (x) % table_size ] )

    inline const char_t operator[] ( unsigned i ) const
    { return GET_CHAR( i ); }
    inline char_t& operator[] ( unsigned i ) { return GET_CHAR( i ); }

    inline void push_back( char_t el )
    {
        if ( next_i == end_i ) {     tables.push_back( new char_t[ table_size ] );
        end_i += table_size; }
        GET_CHAR( next_i ) = el;
        ++next_i;
    }     

#undef GET_CHAR

    inline void clear() { next_i = 0; }

    inline unsigned links_size() const { return links.size(); }
    inline unsigned get_link( unsigned i ) const { return links[i]; }
    inline void add_link( unsigned i ) { return links.push_back( i ); }

 private:
    static const unsigned table_size = 65536;

    std::vector<char_t*> tables;
    unsigned next_i;
    unsigned end_i;

    //std::vector<unsigned> links;
    TVTvec<unsigned, 65536> links;
};


#endif
