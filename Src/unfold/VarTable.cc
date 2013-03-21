#include "VarTable.hh"
#include "compiler/ps.hh"


std::vector<int> VarTable::new_variables;
std::vector<uf_types::Type*> VarTable::var_types;

unsigned VarTable::number_of_used_variables = 0;
std::vector<int*> VarTable::used_variables;
std::vector<uf_types::Type*> VarTable::types_of_used_variables;

unsigned VarTable::number_of_variables = 0;


namespace
{
    std::string tmp_strA;
    std::string tmp_strB;
}


void
VarTable::Init()
{
    var_types.resize( size() );
    for ( unsigned i = 0; i < size(); ++i )
    { var_types[i] = &ps::GiveVarType( i ); }
    
    used_variables.resize( size() );
    types_of_used_variables.resize( size() );
}

void
VarTable::AddVariable( const std::string& name, unsigned type_i,
                       unsigned default_value )
{
    ++number_of_variables;
    new_variables.push_back( default_value );
    ps::AddVar( name, type_i );
}

void
VarTable::SetUsedTypes( const std::vector<uf_types::Type*>& types )
{
    new_variables.resize( types.size() );
    used_variables.resize( types.size() );
    types_of_used_variables.resize( types.size() );

    for ( unsigned i = 0; i < types.size(); ++i )
    {
        new_variables[i] = types[i]->lower_bound;
        used_variables[i] = &new_variables[i];
        types_of_used_variables[i] = types[i];
    }
    number_of_used_variables = types.size();
}

const std::string&
VarTable::GenAction( const std::string& gate )
{
    tmp_strA = gate;

    for ( unsigned i = 0; i < number_of_used_variables; ++i )
    {
        types_of_used_variables[i]->ValueToString( new_variables[i], tmp_strB );
        tmp_strA += "<" + tmp_strB + ">";
    }

    return tmp_strA;
}
