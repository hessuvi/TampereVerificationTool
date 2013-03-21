#ifndef UF_SPROPS_HH
#define UF_SPROPS_HH

#include "LSTS_File/StatePropsAP.hh"


class ufSProps : public oStatePropsAP
{
 public:
    virtual lsts_index_t lsts_numberOfStatePropsToWrite();
    virtual void lsts_WriteStateProps( iStatePropsAP& pipe );

    static void AddStateProp( const std::string& prop_name );

    static void AddStatePropState( unsigned state_prop_i, unsigned state )
    { state_props[state_prop_i].push_back( state ); }

    static unsigned numberOfStateProps() { return state_props.size(); }

    static void CheckStatePropositions( const std::vector<int>& variables,
                                        unsigned ufd_state_number )
    {
        for ( unsigned i = 0; i < numberOfStateProps(); ++i )
        {
            if ( variables[i] )
            { state_props[i].push_back( ufd_state_number ); }
        }
    }
            
 private:
    static std::vector<std::string> state_prop_names;
    static std::vector< std::vector<unsigned> > state_props;
};


#endif
