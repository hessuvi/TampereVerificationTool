#include "CharArray.hh"

#define init_size  65536 //32

#define DELETE_TABLES  for ( unsigned i = 0; i < tables.size(); ++i ) \
                       { delete[] tables[i]; }


spCharArray::spCharArray() :
    tables( 1, new char_t[ init_size ] ),
    next_i( 0 ),
    end_i( init_size ) { tables[0][0] = 0; }

spCharArray::~spCharArray() { DELETE_TABLES; }

spCharArray::spCharArray( const spCharArray& src ) :
    
    tables( src.tables.size(), 0 ),
    next_i( src.next_i ),
    end_i( src.end_i )

{ CopyTables( src ) ; }

spCharArray&
spCharArray::operator=( const spCharArray& src )
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

const spCharArray::char_t* const
spCharArray::CreatePlainCharArray() const
{
    char_t* const c_arr = new char_t[ size() ];
    for ( unsigned i = 0; i < size(); ++i )
    { c_arr[i] = operator[]( i ); }
    return c_arr;
}


// PRIVATE:

inline void    
spCharArray::CopyTables( const spCharArray& src )
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
spCharArray::make_bigger()
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
