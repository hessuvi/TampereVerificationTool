#ifndef ALPHABET_HH
#define ALPHABET_HH

#include "uf_types.hh"
#include "VarTable.hh"

#include "StringTree/StringMap.hh"
#include "LSTS_File/ActionNamesAP.hh"

#include <string>
#include <vector>


class ActionSubset
{
 public:
    ActionSubset( const std::string& gate_name, unsigned first_num )
        : gate( gate_name ), first_n( first_num ) { }
    
    unsigned AddParType( uf_types::Type& typ )
    {
        par_types.push_back( &typ );
        subs.push_back( typ.lower_bound );
        unsigned siz = typ.upper_bound - typ.lower_bound + 1;
        for ( unsigned i = 0; i < coeffs.size(); ++i ) { coeffs[i] *= siz; }
        coeffs.push_back( 1 );
        return siz;
    }
    
    std::string CreateElemName()
    {
        std::string elem_name( gate );
        for ( unsigned i = 0; i < par_types.size(); ++i )
        {
            elem_name += "<";
            elem_name += par_types[i]->GiveName();
            elem_name += ">";
        }
        return elem_name;
    }
    
    void Print( iActionNamesAP& pipe );
    
    unsigned CalcActionNumber( const std::vector<unsigned>& pars ) const
    {
        unsigned ordinal_number = first_n;
        for ( unsigned i = 0; i < par_types.size(); ++i )
        { ordinal_number += (pars[i] - subs[i]) * coeffs[i]; }
        return ordinal_number;
    }

 private:
    std::string gate;
    std::vector<uf_types::Type*> par_types;
    std::vector<unsigned> subs;
    std::vector<unsigned> coeffs;
    unsigned first_n;
};


class Alphabet : public oActionNamesAP
{
 public:
    Alphabet() : subset_size( 0 ), action_cnt( 0 )
    {
        subsets.push_back( ActionSubset( "TAU", 0 ) );
        subset_map.add( "TAU", 0 );
    }

    unsigned AddGate( const std::string& gate )
    {
        subsets.push_back( ActionSubset( gate, action_cnt + 1 ) );
        subset_size = 1;
        return subsets.size() - 1;
    }

    void AddParType( uf_types::Type& typ )
    { subset_size *= subsets.back().AddParType( typ ); }

    bool EndSubset( std::string& elem_name )
    {
        elem_name = subsets.back().CreateElemName();
        action_cnt += subset_size;
        return subset_map.add( elem_name, subsets.size()-1 );
    }

    const ActionSubset* GiveActionSubset( const std::string& elem_name ) const
    {
        unsigned i = 0;
        if ( !subset_map.find( elem_name, i ) ) { return 0; }
        return &subsets[i];
    }

    // LSTS-library methods:
    virtual lsts_index_t lsts_numberOfActionNamesToWrite();
    virtual void lsts_WriteActionNames( iActionNamesAP& );

 private:
    std::vector<ActionSubset> subsets;
    StringMap<unsigned> subset_map;

    unsigned subset_size;
    unsigned action_cnt;
};


#endif
