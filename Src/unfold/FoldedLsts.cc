#include "FoldedLsts.hh"

#include "Alphabet.hh"

#include "eval/iBC.hh"
#include "eval/ExpEvaluator.hh"
#include "compiler/ps.hh"

#include "UnfoldedLsts.hh"
#include "VarTable.hh"

#include "unfolded/ufWriter.hh"


// Internal:
namespace
{
    DfsStack* dfs_stack = 0;
    ufdState* curr_ufd_state = 0;

    ExpEvaluator exp_evaluator;

    Alphabet alphabet;

    // Current event parameters are stored here:
    std::vector<unsigned> curr_pars;
}


FoldedLsts::FoldedLsts( ufWriter& wr ) : writer( wr )
{ wr.GiveoLsts().AddActionNamesWriter( alphabet ); }

FoldedLsts::~FoldedLsts()
{
    for ( unsigned i = 0; i < states.size(); ++i )
    {
        std::vector<fdTransition>& trs = states[i].trs;
        for ( unsigned k = 0; k < trs.size(); ++k )
        {
            fdTransition& tr = trs[k];
            if ( tr.bc_precond ) { delete [] tr.bc_precond; }
            if ( tr.bc_postcond ) { delete [] tr.bc_postcond; }
            for ( unsigned j = 0; j < tr.outputs.size(); ++j )
            { delete [] tr.outputs[j].bc_output; }
        }
    }

}


// Compile time methods:

fdState&
FoldedLsts::GetState( unsigned state_i )
{
    if ( state_i >= states.size() )
    {
        states.push_back( fdState() );
        check_claim( state_i < states.size(),
                     "a bug in FoldedLsts::GetState(): state_i out of range" );
    }

    return states[state_i];
}

Alphabet& FoldedLsts::GiveAlphabet() { return alphabet; }

void
FoldedLsts::SetUnfoldedLsts( UnfoldedLsts& ufdLsts )
{ ufd_lsts = &ufdLsts; dfs_stack = &ufd_lsts->dfs_stack; }


// Run time methods:

void
FoldedLsts::ConstructUfdInitState()
{
    //    Print();


    curr_pars.resize( fdTransition::GiveMaxNumberOfPars() );

    dfs_stack->get_new( curr_ufd_state );
    dfs_stack->push( curr_ufd_state );
    dfs_stack->update();

    // Finding the INIT:
    ps::state_names.find( "init" );
    curr_ufd_state->fd_state_number = ps::state_names.last_i();

    // Initializing the curr_ufd_state:
    curr_ufd_state->ufd_state_number = 0;
    curr_ufd_state->variables = VarTable::new_variables;
 
    // Constructing unfolded initial states:
    ConstructUfdState();
}

void
FoldedLsts::ConstructUfdState()
{
    dfs_stack->pop( curr_ufd_state );

    CreateUfdChildren();
    dfs_stack->update();

    dfs_stack->recycle( curr_ufd_state );
}


// PRIVATE:
void
FoldedLsts::CreateUfdChildren()
{
    const unsigned fd_state_n = curr_ufd_state->fd_state_number;
    const unsigned ufd_state_number = curr_ufd_state->ufd_state_number;
    fdState& state = states[fd_state_n];

    writer.start_st_transitions( ufd_state_number );
    
    exp_evaluator.SetVariables( curr_ufd_state->variables,
                                VarTable::new_variables );
    
    // Adding children to the dfs stack:
    for ( unsigned i = 0; i < state.trs.size(); ++i )
    {
        // HMM??? :
        VarTable::new_variables = curr_ufd_state->variables;

        fdTransition& tr = state.trs[i];

        // Evaluating a possible precondition:
        if ( tr.bc_precond )
        {
            iBC::SetByteCode( tr.bc_precond );
            if ( !exp_evaluator.Eval() ) { continue; }
        }

        // Evaluating possible output expressions:
        for ( unsigned j = 0; j < tr.outputs.size(); ++j )
        {
            fdTransition::Output& op = tr.outputs[j];
            const unsigned par_i = op.par_i;
            iBC::SetByteCode( op.bc_output );
            curr_pars[par_i] = exp_evaluator.Eval();
            if ( !tr.isValidOutputPar( curr_pars[par_i], par_i ) )
            {
                std::string msg( "line " );
                msg = valueToMessage( msg, tr.line_n, ": parameter number " );
                msg = valueToMessage( msg, par_i+1, " out of range" );
                AbortWithMessage( msg );
            }
        }

        // Evaluating a possible postcondition:        
        if ( tr.bc_postcond )
        { EvalPostcondition( tr, ufd_state_number ); continue; }

        // Transition without postcondition:
        CreateUfdState( tr, ufd_state_number );
    }

    writer.end_st_transitions( ufd_state_number );

    ufSProps::CheckStatePropositions( curr_ufd_state->variables,
                                      ufd_state_number );
}

// This is the definite bottle-neck of the program -- brute-force-evaluation
// of expressions:
void
FoldedLsts::EvalPostcondition( fdTransition& tr, unsigned ufd_state_number )
{
    VarTable::SetUsedVars( tr.bc_postcond );
    iBC::SetByteCode( tr.bc_postcond, VarTable::size() );

    // What is an unreal postcondition?
    if ( !tr.is_postcond_real || exp_evaluator.Eval() )
    { CreateUfdState( tr, ufd_state_number ); }
    
    if ( !VarTable::number_of_used_variables ) { return; }

    while ( VarTable::calcNextCombination() )
    {
        iBC::SetByteCode( tr.bc_postcond, VarTable::size() );
        // Creating a state each time the condition evaluates true:        
        if ( !tr.is_postcond_real || exp_evaluator.Eval() )
        { CreateUfdState( tr, ufd_state_number ); }
    }

}        

// Creates a new unfolded state, checks the action name in case and
// adds a transition to the state.
void
FoldedLsts::CreateUfdState( fdTransition& tr, unsigned ufd_start_state_n )
{
    unsigned ufd_dest_state_n = 0;
    ufd_lsts->createState( tr.dest_state_n, VarTable::new_variables,
                           ufd_dest_state_n,
                           states[tr.dest_state_n].vars );
    
    // Is the created state an initial state?
    if ( !ufd_start_state_n )
    { writer.AddInitialState( ufd_dest_state_n ); return; }
    
    // Evaluating possible input expressions:
    for ( unsigned i = 0; i < tr.inputs.size(); ++i )
    {
        fdTransition::Input& i_exp = tr.inputs[i];
        curr_pars[i_exp.par_i] = VarTable::new_variables[i_exp.var_i];
    }

    unsigned ufd_action_n = tr.action_subset->CalcActionNumber( curr_pars );
    writer.add_transition( ufd_dest_state_n, ufd_action_n );
}


// For debugging:

void
FoldedLsts::Print()
{
    for ( unsigned i = 0; i < states.size(); ++i )
    {
        std::cerr << "*state" << i << ":" << std::endl;
        std::vector<fdTransition>& trs = states[i].trs;
        std::cerr << "   size == " << trs.size() << std::endl;
        for ( unsigned j = 0; j < trs.size(); ++j )
        {
            std::cerr << " +tr" << j << " : " << trs[j].dest_state_n
                      << std::endl;
        }
    }

}
