#ifndef FD_TRANSITION_HH
#define FD_TRANSITION_HH

#include "uf_types.hh"


class fdTransition
{
 public:
    fdTransition();
    
    unsigned action_n; unsigned dest_state_n;

    const class ActionSubset* action_subset;    

    byte_t* bc_precond;
    byte_t* bc_postcond; bool is_postcond_real;

    unsigned line_n;

    struct Output
    {
        Output() : par_type( 0 ), bc_output( 0 ), par_i( 0 ) { }
        Output( uf_types::Type& typ, byte_t* bc, unsigned pi ) 
            : par_type( &typ ), bc_output( bc ), par_i( pi ) { }

        const uf_types::Type* par_type;
        byte_t* bc_output;
        unsigned par_i;
    };
    std::vector<Output> outputs;

    bool isValidOutputPar( int val, unsigned par_i )
    {
        uf_types::Type& typ = *par_types[par_i];
        return val >= typ.lower_bound && val <= typ.upper_bound;
    }

    struct Input
    {
        Input() : par_type( 0 ), par_i( 0 ), var_i( 0 ) { }
        Input( uf_types::Type& typ, unsigned pi, unsigned vi ) 
            : par_type( &typ ), par_i( pi ), var_i( vi ) { }

        const uf_types::Type* par_type;
        unsigned par_i;
        unsigned var_i;
    };
    std::vector<Input> inputs;


    std::vector<uf_types::Type*> par_types;


    void AddInput( unsigned par_i, unsigned var_i );
    void AddOutput( unsigned par_i, unsigned type_i, byte_t* bc );

    const uf_types::Type& GiveParType( unsigned i ) const
    { return *par_types[i]; }

    static void UpdateMaxNumberOfPars( unsigned par_n );
    static unsigned GiveMaxNumberOfPars() { return max_number_of_parameters; }

 private:
    void AddParType( uf_types::Type& typ ) { par_types.push_back( &typ ); }

    static unsigned max_number_of_parameters;
};


#endif
