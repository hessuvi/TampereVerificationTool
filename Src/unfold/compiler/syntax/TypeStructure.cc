#include "TypeStructure.hh"

#include "uf_types.hh"
#include "itx.hh"


TypeStructure::~TypeStructure() { }

// A bounded integer type consists of an interval.
void
TypeStructure::ParseIntType( int lBound, const std::string& name )
{
    int uBound;
    it >> ff::punct( ".." ) >> uBound;

    uf_types::BoundedInt* new_type =
        new uf_types::BoundedInt( lBound, uBound );
    uf_types::AddType( new_type, name );
}

// An enum type consists of a set of identifiers.
void
TypeStructure::ParseEnumType( const std::string& name )
{
    uf_types::Enum* new_enum = new uf_types::Enum; 

    std::string c_name;
    while ( itx::opt_getName( c_name ) )
    {
        new_enum->AddConstant( c_name );
        // Comma allowed but not mandatory here:
        it >> ff::opt_punct( "," );
    }

    uf_types::AddType( new_enum, name );
}

// Parses an unknown unnamed type.
unsigned
TypeStructure::ParseType( std::string name )
{
    it >> ff::opt_punct( ":" ) >> ff::punct( "[" );

    // An integer type?
    if ( it.opt_get( ff::integ ) ) { ParseIntType( *ff::integ, name ); }
    else { ParseEnumType( name ); }

    it >> ff::punct( "]" );

    return uf_types::types.size() - 1;
}
