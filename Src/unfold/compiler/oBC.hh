#ifndef oBC_HH
#define oBC_HH

#include "uf_config.hh"
#include "sp_codes.hh"
#include "ps.hh"

#include <string>
#include <vector>


namespace oBC
{
    extern std::vector<byte_t> byte_code;

    inline void Reset() { byte_code.clear(); }

    inline unsigned GiveByteCodeSize() { return byte_code.size(); }

    inline void putByte( byte_t byte ) { byte_code.push_back( byte ); }
    inline void setByte( unsigned i, byte_t byte ) { byte_code[i] = byte; }

    inline unsigned reserveByte() { putByte( 0 ); return byte_code.size() - 1; }
    
    inline void
    ReserveUsedVarTable( unsigned number_of_vars )
    {
        if ( byte_code.size() ) { return; }
        for ( unsigned i = 0; i < number_of_vars; ++i ) { oBC::putByte( 0 ); }
    }

    inline void PutIntLiteral( int i ) { putByte( 'l' ); oBC::putByte( i ); }
    inline void PutBoolLiteral( bool b ) { oBC::PutIntLiteral( b ); }

    // Marking variables to byte code:
    inline void MarkOldIntVar( int index ) { putByte( 'o' ); putByte( index ); }
    inline void MarkNewIntVar( int index ) { putByte( 'n' ); putByte( index ); }

    inline void PutStringLiteral( const std::string& str )
    { putByte( ps::AddStringLiteral( str ) ); }

    // Returns an index pointing at next to reserved link:
    inline unsigned ReserveLink() { putByte( 0 ); return byte_code.size(); }

    // loc_i refers to an index pointing at next to reserved link:
    inline void SetLink( unsigned loc_i, unsigned value )
    { setByte( --loc_i, value ); }
    // Creates a link pointing at the end of the byte code:
    inline void SetLink( unsigned loc_i )
    { SetLink( loc_i, GiveByteCodeSize() - loc_i ); }

    inline void MarkVarUsed( unsigned var_i ) { byte_code[var_i] = 55; }

    // New stuff:
    inline byte_t* GiveCopy()
    {
        const unsigned siz = byte_code.size();
        byte_t* copy = new byte_t[ siz ];
        for ( unsigned i = 0; i < siz; ++i )
        {
            copy[i] = byte_code[i];

            
            std::cerr << static_cast<int>( copy[i] );
            if ( copy[i] > 32 )
            { std::cerr << "(" << static_cast<char>( copy[i] ) << ")"; }
            std::cerr << " "; 
        }
        std::cerr << " ###" << std::endl;
        
        return copy;
    }
}


#endif
