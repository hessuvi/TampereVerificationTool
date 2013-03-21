#include "fdTransition.hh"
#include "compiler/ps.hh"


unsigned fdTransition::max_number_of_parameters = 0;


fdTransition::fdTransition() : action_n( 0 ), dest_state_n( 0 ),
                               action_subset( 0 ),
                               bc_precond( 0 ), bc_postcond( 0 ),
                               is_postcond_real( false ), line_n( 0 ) { }
void
fdTransition::AddInput( unsigned par_i, unsigned var_i )
{
    uf_types::Type& typ = ps::GiveVarType( var_i );
    AddParType( typ );
    inputs.push_back( Input( typ, par_i, var_i ) );
}

void
fdTransition::AddOutput( unsigned par_i, unsigned type_i, byte_t* bc )
{
    uf_types::Type& typ = *uf_types::types[type_i];
    AddParType( typ );
    outputs.push_back( Output( typ, bc, par_i ) );
}

void
fdTransition::UpdateMaxNumberOfPars( unsigned par_n )
{
    if ( par_n > max_number_of_parameters )
    { max_number_of_parameters = par_n; }
}
