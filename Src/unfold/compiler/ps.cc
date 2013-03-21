#include "ps.hh"


std::vector<unsigned> ps::var_i2type_i;

std::vector<std::string> ps::string_literals;
std::vector<std::string> ps::var_strings;


unsigned
ps::checkName( const std::string& name, NameStore& store )
{
    NameStore* st = findName( name );
    if ( st == &store ) { return store.last_i(); }
    if ( !st )
    { it.error( name, std::string( "undefined " ) + store.GiveName() ); }
    return st->Misplaced( name );
}

unsigned
ps::addNewName( const std::string& name, NameStore& store, bool alias_of_prev )
{
    NameStore* st = findName( name );
    if ( !st ) { store.add( name, alias_of_prev ); return store.last_i(); }
    if ( st == &store )
    { it.error( name, std::string( "redefination of the " ) +
                store.GiveName() ); }
    return st->Misplaced( name );
}

bool
ps::addNameIfNeeded( const std::string& name, NameStore& store )
{
    NameStore* st = findName( name );
    if ( !st ) { store.add( name ); return true; }
    if ( st == &store ) { return false; }
    return st->Misplaced( name );
}

ps::NameStore*
ps::findName( const std::string& name )
{
    if ( type_names.find( name ) ) { return &type_names; }
    if ( var_names.find( name ) ) { return &var_names; }
    if ( constants.find( name ) ) { return &constants; }
    if ( state_names.find( name ) ) { return &state_names; }
    if ( gate_names.find( name ) ) { return &gate_names; }
    if ( state_prop_names.find( name ) ) { return &state_prop_names; }

    return 0;
}


// ps::NameStore

ps::NameStore ps::type_names( "type name" );
ps::NameStore ps::var_names( "variable name" );
ps::NameStore ps::constants( "constant" );
ps::NameStore ps::state_names( "state name" );
ps::NameStore ps::gate_names( "gate name" );
ps::NameStore ps::state_prop_names( "state proposition name" );


ps::NameStore::NameStore( const char* name ) : store_name( name ), i( 0 ),
                                               number_of_uniques( 0 ) { }
unsigned
ps::NameStore::Misplaced( const std::string& name )
{
    std::string str( "misplaced " ); str += GiveName();
    it.error( name, str );
    return 0;
}

bool
ps::NameStore::add( const std::string& name, bool alias_of_prev )
{
    if ( !alias_of_prev ) { i = number_of_uniques++; }
    return name_map.add( name, i );
}
