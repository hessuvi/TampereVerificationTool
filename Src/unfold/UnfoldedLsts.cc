#include "UnfoldedLsts.hh"

#include "FileFormat/Lexical/OTokenStream.hh"
#include "unfolded/ufWriter.hh"


UnfoldedLsts::UnfoldedLsts( FoldedLsts& flsts ) :
    number_of_states( 0 ), folded_lsts( flsts )
{ flsts.SetUnfoldedLsts( *this ); }

void
UnfoldedLsts::Unfold( ufWriter& writer )
{
    // Adding the init state:
    folded_lsts.ConstructUfdInitState();
 
    // Depth-first-search:
    while ( dfs_stack.give_top() ) { folded_lsts.ConstructUfdState(); }   

    // End routines:
    writer.SetStateCnt( number_of_states );
}

// **PRECOND**: 0 <= fd_tate_i <= states.size()-1
// Returns: number_of_states => 1 or
//          0 which means the state exists
bool
UnfoldedLsts::createState
( unsigned fd_state_number, const std::vector<int>& new_variables,
  unsigned& ufd_state_number,
  const std::vector<fdState::state_of_var>& states_of_vars )
{
    // Encoding the folded state number:
    oTokenStream::number2string( fd_state_number, id_string );
    
    // Encoding integer values of variables:
    for ( unsigned i = 0; i < new_variables.size(); ++i )
    {
        if ( states_of_vars[i] <= fdState::exists_not )
        { id_string += "N"; continue; }

        int val = new_variables[i];
        if ( val < 0 ) { id_string += "i-"; val = -val; }
        else { id_string += "i"; }
        oTokenStream::number2string( val, number_string );
        id_string += number_string;
    }
        
    // Is the state a new one?
    if ( !state_ids.add_if_needed( id_string, number_of_states + 1,
                                   ufd_state_number ) ) { return false; }
    // Creating a new state:
    ufd_state_number = ++number_of_states;

    if ( !folded_lsts.isDeadLock( fd_state_number ) )
    {
        ufdState* new_st; dfs_stack.get_new( new_st ); dfs_stack.push( new_st );
        new_st->fd_state_number = fd_state_number;
        new_st->ufd_state_number = ufd_state_number;
        new_st->variables = new_variables;
    }

    return true;
}
