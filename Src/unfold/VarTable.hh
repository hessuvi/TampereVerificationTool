#ifndef VAR_TABLE_HH
#define VAR_TABLE_HH

#include "uf_types.hh"


class VarTable
{
 public:
    static unsigned size() { return number_of_variables; }

    static std::vector<int> new_variables;
    static std::vector<uf_types::Type*> var_types;

    static unsigned number_of_used_variables;
    static std::vector<int*> used_variables;
    static std::vector<uf_types::Type*> types_of_used_variables;

    static void AddVariable( const std::string& name, unsigned type_i,
                             unsigned default_value );
    static void Init();

    static void SetUsedTypes( const std::vector<uf_types::Type*>& types );
    static const std::string& GenAction( const std::string& gate );

    static inline void SetUsedVars( const byte_t* bc_used_vars )
    {
        unsigned uv_i = 0;
        for ( unsigned i = 0; i < size(); ++i )
        {
            if ( !bc_used_vars[i] ) { continue; }

            new_variables[i] = var_types[i]->lower_bound;
            used_variables[uv_i] = &new_variables[i];
            types_of_used_variables[uv_i] = var_types[i];
            ++uv_i;
        }
        number_of_used_variables = uv_i;
    }
    
    static inline bool calcNextCombination()
    {
        unsigned var_i = number_of_used_variables - 1;
        
        while ( ++*used_variables[var_i] >
                types_of_used_variables[var_i]->upper_bound )
        {
            *used_variables[var_i] =
                types_of_used_variables[var_i]->lower_bound;
            if ( !var_i ) { return false; }
            --var_i;
        }
        
        return true;
    }

 private:
    static unsigned number_of_variables;
};


#endif
