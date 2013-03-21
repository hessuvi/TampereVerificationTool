#include "fdState.hh"

#include "VarTable.hh"
#include "compiler/ps.hh"


static void
catListItem( std::string& str, const std::string& item,
             unsigned item_i, unsigned list_size )
{
    if ( item_i )
    {
        if ( item_i < list_size - 1 ) { str += ", "; }
        else { str += " and "; }
    }

    str += item;
}


std::vector<fdState::state_of_var> fdState::suggested_vars;

fdState::fdState() : vars( VarTable::size(), exists_not ),
                     vars_set( false ) { }
void
fdState::CheckVars( const std::string& end_st_name )
{
    if ( !vars_set ) { vars = suggested_vars; vars_set = true; return; }

    std::vector<unsigned> missing;
    std::vector<unsigned> extra;
    
    for ( unsigned i = 0; i < vars.size(); ++i )
    {
        if ( suggested_vars[i] >= exists && vars[i] >= exists ||
             suggested_vars[i] < exists && vars[i] < exists ) { continue; }
        
        if ( vars[i] >= exists ) { missing.push_back( i ); }
        else { extra.push_back( i ); }
    }
    
    if ( missing.empty() && extra.empty() ) { return; }
    
    std::string str( "the variables that are valid after executing "
                     "the current transition differ from "
                     "the existing variables of the end state: " );
    
    if ( missing.size() )
    {
        for ( unsigned i = 0; i < missing.size(); ++i )
        { catListItem( str, ps::GiveVarName( missing[i] ),
                       i, missing.size() ); }
        if ( missing.size() >= 2 ) { str += " are missing"; }
        else { str += " is missing"; }
    }
    
    if ( extra.size() )
    {
        if ( missing.size() ) { str += " and "; }
        for ( unsigned i = 0; i < extra.size(); ++i )
        { catListItem( str, ps::GiveVarName( extra[i] ), i, extra.size() ); }
        str += " should be deleted";
    }
        
    std::cerr << end_st_name << ", " << str << std::endl;
}
