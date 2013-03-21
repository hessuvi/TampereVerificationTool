#ifndef UF_TYPE_DEFS_HH
#define UF_TYPE_DEFS_HH

#include "TypeStructure.hh"
#include "itx.hh"

struct ufTypeDefs : public TypeStructure
{
    ufTypeDefs()
    {
        std::string name;
        while ( itx::opt_getName( name ) ) { ParseType( name ); }
    }
};

#endif
