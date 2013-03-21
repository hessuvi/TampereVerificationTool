#include "ufStateProps.hh"

#include "unfolded/ufSProps.hh"
#include "folded/fdState.hh"

#include "itx.hh"

void
ufStateProps::comp( fdState& init_state )
{
    const bool opt_par = it.opt_get( ff::punct( "{" ) );

    std::string name;
    while ( itx::opt_getName( name ) )
    {
        ufSProps::AddStateProp( name );
        fdState::state_of_var st_of_var = fdState::exists_not;
        init_state.AddStateOfVar( st_of_var );

        it >> ff::opt_punct( "," );
    }

    if ( opt_par ) { it >> ff::punct( "}" ); }
}
