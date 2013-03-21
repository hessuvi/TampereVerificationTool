#include "uf_types.hh"

#include "compiler/ps.hh"


// All types created are pointed at here:
std::vector<uf_types::Type*> uf_types::types;

std::vector<unsigned> uf_types::const_i2type_i;
std::vector<unsigned> uf_types::const_i2const_val;


void
uf_types::AddType( Type* typ, std::string alias )
{
    if ( ps::addNameIfNeeded( typ->GiveName(), ps::type_names ) )
    { types.push_back( typ ); }
    else { delete typ; typ = 0; } // The type existed already.
    
    if ( alias.size() ) { ps::addNewName( alias, ps::type_names, true ); }

    /*
    if ( typ )
    {
        std::cerr << " * type " << ps::type_names.last_i() << " "
                  << typ->GiveName() << ", alias " << alias
                  << " added. types.size() == " << types.size()
                  << std::endl;
    }
    else
    {
        std::cerr << " + alias " << alias << " added to type "
                  << ps::type_names.last_i() << std::endl;
    }
    */
}


// Enum

void
uf_types::Enum::AddConstant( const std::string& con )
{
    ps::addNewName( con, ps::constants );
    const_i2type_i.push_back( types.size() );
    const_i2const_val.push_back( number_of_values );
    constants.push_back( con );
    ++upper_bound; ++number_of_values;
}
