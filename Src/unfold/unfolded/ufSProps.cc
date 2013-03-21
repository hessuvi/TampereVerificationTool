#include "ufSProps.hh"
#include "VarTable.hh"
#include "compiler/ps.hh"


lsts_index_t
ufSProps::lsts_numberOfStatePropsToWrite() { return numberOfStateProps(); }

void
ufSProps::lsts_WriteStateProps( iStatePropsAP& pipe )
{
    for ( unsigned prop_i = 0; prop_i < numberOfStateProps(); ++prop_i )
    {
        pipe.lsts_StartPropStates( state_prop_names[prop_i] );
        
        const std::vector<unsigned>& states = state_props[prop_i];

        for( unsigned i = 0; i < states.size(); ++i )
        { pipe.lsts_PropState( states[i] ); }
    
        pipe.lsts_EndPropStates( state_prop_names[prop_i] );
    }

}

void
ufSProps::AddStateProp( const std::string& prop_name )
{
    //    check_claim( ps::var_map.add( prop_name, ps::var_i2type_i.size() ),
    //                            its.errorMsg( prop_name,
    //                        "redefination of the state proposition" ) );

    ps::addNewName( prop_name, ps::var_names );
    VarTable::AddVariable( prop_name, logic_exp, false );

    state_prop_names.push_back( prop_name );
    state_props.push_back( std::vector<unsigned>() );
}

std::vector< std::string > ufSProps::state_prop_names;
std::vector< std::vector<unsigned> > ufSProps::state_props;
