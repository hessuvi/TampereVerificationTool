#include "Variables.hh"

#include "VarTable.hh"
#include "folded/fdState.hh"
#include "compiler/ps.hh"
#include "itx.hh"


Variables::Variables( fdState& init_state )
{
    std::string name;
    while ( itx::opt_getName( name ) )
    {
        std::vector<std::string> varNames;
        do
        {
            varNames.push_back( name );
            ps::addNewName( name, ps::var_names );
            // Comma allowed but not mandatory here:
            it >> ff::opt_punct( "," );
        } while ( itx::opt_getName( name ) );

        it >> ff::punct(":");

        unsigned type_i = 0;
        
        // A named user-defined type?
        if ( itx::opt_getName( name ) )
        { type_i = ps::checkName( name, ps::type_names ); }
        // Or a built-in type?
        else if ( it.opt_get( ff::rword ) )
        { type_i = ps::checkName( *ff::rword, ps::type_names ); }
        // It's an unnamed type.
        else { type_i = ParseType(); }

        int default_value = -77777;
        fdState::state_of_var st_of_var = fdState::exists_not;

        // Has a default value?
        if ( it.opt_get( ff::punct( "=" ) ) )
        {
            it >> default_value;
            st_of_var = fdState::exists;
        }

        it >> ff::opt_punct( ";" );

        // Adding the variables:
        for ( unsigned i = 0; i < varNames.size(); ++i )
        {
            VarTable::AddVariable( varNames[i], type_i, default_value );
            init_state.AddStateOfVar( st_of_var );
            // ap.uf_Variable( varNames[i], "" );
        }
    }

}
