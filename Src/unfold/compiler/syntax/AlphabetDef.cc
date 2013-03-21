#include "AlphabetDef.hh"

#include "VarTable.hh"
#include "compiler/ps.hh"
#include "FoldedLsts.hh"
#include "Alphabet.hh"
#include "itx.hh"


AlphabetDef::AlphabetDef()
{
    Alphabet& alpha = FoldedLsts::GiveAlphabet();

    std::cerr << "alphabet 1!" << std::endl;

    std::string gate;
    while ( itx::opt_getGate( gate ) )
    {
        ps::addNameIfNeeded( gate, ps::gate_names );
        alpha.AddGate( gate );
        
        std::cerr << "alphabet 2!" << gate << std::endl;

        unsigned num_of_pars = 0;
        while ( it.opt_get( ff::punct( "<" ) ) )
        {
            std::string name;
            if ( itx::opt_getName( name ) ) { }
            else if ( it.opt_get( ff::rword ) ) { name = *ff::rword; }
            else { it.error( "a variable or type name expected" ); }

            ps::NameStore* ns = ps::findName( name );
            if ( !ns ) { it.error( "a variable or type name expected" ); }

            unsigned type_i = 0;
            if ( ns == &ps::type_names ) { type_i = ns->last_i(); }
            else if ( ns == &ps::var_names )
            { type_i = ps::GiveVarTypeI( ns->last_i() ); }
            else { ns->Misplaced( name ); }

            alpha.AddParType( *uf_types::types[type_i] );
            it >> ff::punct( ">" );
            ++num_of_pars;
        }

        fdTransition::UpdateMaxNumberOfPars( num_of_pars );
        std::string elem_name;
        if ( !alpha.EndSubset( elem_name ) )
        { it.error( elem_name, "duplicate action subset" ); }

        //ap.uf_ActionSubset( elem_name );
    }

    //    alpha.Print();
}
