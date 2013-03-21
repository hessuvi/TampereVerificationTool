#ifndef TYPE_STRUCTURE_HH
#define TYPE_STRUCTURE_HH

#include <string>


class TypeStructure
{
 public:
    virtual ~TypeStructure();

    void ParseIntType( int lBound, const std::string& name );
    void ParseEnumType( const std::string& name );
    unsigned ParseType( std::string name = "" );
    bool ParseTypeDefs();
};


#endif
