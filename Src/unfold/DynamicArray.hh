#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

#include <iostream>


template <class Element>
class DynamicArray
{
 public:
    DynamicArray() : tables( 1, new Element[ table_size ] ),
                     next_i( 0 ), end_i( table_size ) { }
    ~DynamicArray()
    { for ( unsigned i = 0; i < tables.size(); ++i ) { delete[] tables[i]; } }
    DynamicArray( const DynamicArray& src ) { *this = src; }
    DynamicArray& operator=( const DynamicArray& src )
    {
        if ( &src != this )
        { 
            for ( unsigned i = 0; i < tables.size(); ++i )
            { delete[] tables[i]; }
            
            tables.resize( src.tables.size() );
            next_i = src.next_i;
            end_i = src.end_i;
            
            tables[0] = new Element[ end_i ];
        
            for ( unsigned i = 1; i < tables.size(); ++i )
            { tables[i] = tables[i-1] + table_size; }
            
            for ( unsigned i = 0; i < next_i; ++i )
            { tables[0][i] = src[i]; }
        }

        return *this;
    }

    inline bool empty() const { return !next_i; }
    inline unsigned size() const { return next_i; }

#define GET_ELEM( x )  ( tables[ (x) / table_size ][ (x) % table_size ] )

    inline const Element& operator[] ( unsigned i ) const
    {
        if ( i >= size() )
        { std::cerr << "BUG in DynamicArray::operator[](i) const: "
              "size() == " << size() << ", i == " << i << std::endl; }

        return GET_ELEM( i );
    }

    inline Element& operator[] ( unsigned i )
    {
        if ( i >= size() )
        { std::cerr << "BUG in DynamicArray::operator[](i): "
              "size() == " << size() << ", i == " << i << std::endl; }

        return GET_ELEM( i );
    }

    inline Element& push_back()
    {
        if ( next_i == end_i ) { make_bigger(); }
        Element& el = GET_ELEM( next_i );
        ++next_i;
        return el;
    }

    inline void push_back( const Element& el )
    {
        if ( next_i == end_i ) { make_bigger(); }
        GET_ELEM( next_i ) = el;
        ++next_i;
    }     

    inline Element& back()
    {
        if ( !size() )
        { std::cerr << "BUG in DynamicArray::back(): size() == 0" <<
              std::endl; }

        const unsigned i = next_i - 1; return GET_ELEM( i );
    }

    inline void remove_back()
    {
        if ( !size() )
        { std::cerr << "BUG in DynamicArray::remove_back(): size() == 0" <<
              std::endl; }
        
        --next_i;
    }
    
#undef GET_ELEM

 private:
    static const unsigned table_size = 65536;

    void make_bigger()
    {
        // The size is increased by table_size:
        tables.push_back( new Element[ table_size ] );
        end_i += table_size;
    }

    std::vector<Element*> tables;
    unsigned next_i;
    unsigned end_i;
};


#endif
