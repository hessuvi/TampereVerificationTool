/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#include "StringSet.hh"


StringTree::StringTree() : coil_array(), number_of_strings( 0 ) { }

bool
StringTree::find( const std::string& str )
{
    if ( coil_array.empty() ) { return false; }
    
    coil_array.reset();
    
    for ( unsigned i = 0; i < str.size(); ++i )
    {
        if ( match( str[ i ] ) ) { return false; }
    }
    
    return !match( string_end );
}

bool
StringTree::remove( const std::string& str )
{
    if ( coil_array.empty() ) { return false; }
    
    coil_array.reset();
    
    for ( unsigned i = 0; i < str.size(); ++i )
    {
        if ( match( str[ i ] ) ) { return false; }
    }
    
    if ( match( string_end ) ) { return false; }
    
    coil_array.WriteElement( 17 );
    --number_of_strings;
    return true;
}

void
StringTree::clear()
{
    number_of_strings = 0;
    coil_array.clear();
}


// iterator

StringSet::iterator::iterator( StringTree& string_set,
          StateOfIterator s_of_it ) :
    coil_array( &string_set.coil_array ),
    state_of_iterator( s_of_it )
{
    if ( s_of_it == back ) { last(); return; }
    first();
}

StringSet::iterator::
~iterator()
{
    for ( unsigned i = 0; i < dfs_stack.size(); ++i ) 
    { delete dfs_stack[i]; }
}

StringSet::iterator&
StringSet::iterator::
operator=( const iterator& it )
{
    if ( &it != this )
    {
        coil_array = it.coil_array;
        current_string = it.current_string;
        string_end_i = it.string_end_i;
        dfs_stack.resize( it.dfs_stack.size() );
        for ( unsigned i = 0; i < dfs_stack.size(); ++i )
        { dfs_stack[i] = it.dfs_stack[i]->clone(); }
        state_of_iterator = it.state_of_iterator;
    }

    return *this;
}

char_t
StringSet::iterator::
smallest( char_t cc, int str_i )
{
    while ( isLink( cc ) )
    {
        unsigned coil_start_i = coil_array->read_link( cc );
        char_t key = (*coil_array)[coil_start_i];
        
        coil_array->enter_coil( coil_start_i );
        
        if ( !biggerValuesInRight( cc ) ) // Pienemmät oikealla.
        {
            dfs_push( str_i, cc, LESSER );
            cc = coil_array->enter_next_branch();
            continue;
        }
        
        if ( !hasTwoChildren( cc ) )
        {
            dfs_push( str_i, cc, EQUAL );
            return key;
        }
                
        dfs_push( str_i, cc, LESSER );
                
        // Pienemmät vasemmalla.
        coil_start_i = coil_array->read_left_link();
        cc = coil_array->enter_next_branch();
    }
            
    return cc;
}  
        
char_t
StringSet::iterator::
biggest( char_t cc, int str_i )
{
    while ( isLink( cc ) )
    {
        unsigned coil_start_i = coil_array->read_link( cc );
        char_t key = (*coil_array)[coil_start_i];
                
        coil_array->enter_coil( coil_start_i );
                
        if ( biggerValuesInRight( cc ) ) // Suuremmat oikealla.
        {
            dfs_push( str_i, cc, GREATER );
            cc = coil_array->enter_next_branch();
            continue;
        }
                
        if ( !hasTwoChildren( cc ) )
        {
            dfs_push( str_i, cc, EQUAL );
            return key;
        }
                
        dfs_push( str_i, cc, GREATER );
                
        // Suuremmat vasemmalla.
        coil_start_i = coil_array->read_left_link();
        cc = coil_array->enter_next_branch();
    }
    
    return cc;
}  
        
void
StringSet::iterator::
first()
{
    current_string.erase();
    if ( coil_array->empty() ) { state_of_iterator = end; return; }
            
    coil_array->reset();
            
    char_t c = coil_array->next_char();
            
    read_string( c );
            
    string_end_i = coil_array->current_char_i();
    state_of_iterator = begin;
}
        
void
StringSet::iterator::
last()
{
    current_string.erase();
    if ( coil_array->empty() ) { state_of_iterator = front; return; }
            
    coil_array->reset();
            
    char_t c = coil_array->next_char();
            
    read_string2( c );
            
    string_end_i = coil_array->current_char_i();
            state_of_iterator = back;
}
        
void
StringSet::iterator::
read_string( char_t c )
{
    while ( c != string_end )
    {
        if ( isLink( c ) )
        { c = smallest( c, current_string.size() - 1 ); }
        if ( c == string_end ) { break; }
        if ( c == 17 ) { next(); return; }
        current_string += c;
        c = coil_array->next_char();
    }
}
        
void
StringSet::iterator::
read_string2( char_t c )
{
    while ( c != string_end )
    {
        if ( isLink( c ) )
        { c = biggest( c, current_string.size() - 1 ); }
        if ( c == string_end ) { break; }
        if ( c == 17 ) { next(); return; }
        current_string += c;
        c = coil_array->next_char();
    }
}
      
void
StringSet::iterator::
next()
{
    int str_i;
    char_t cc;
    Option option;
            
    state_of_iterator = other;
            
    if ( !dfs_pop( str_i, cc, option ) )
    {
        current_string.erase();
        string_end_i = coil_array->current_char_i();
        state_of_iterator = end;
        return;
    }
            
    current_string.erase( str_i + 1 );
    char_t c;
            
    if ( option == LESSER )
    {
        dfs_push( str_i, cc, EQUAL );
                
        c = coil_array->current_char();
        if ( c == 17 ) { next(); return; }
                
        if ( hasTwoChildren( cc ) )
        {
            unsigned coil_start_i = coil_array->read_left_link();
            coil_array->enter_coil( coil_start_i );
            current_string += c; c = coil_array->current_char();
        }
                
    }
    else if ( option == EQUAL )
    {
        if ( !hasTwoChildren( cc ) &&
             !biggerValuesInRight( cc ) ) { next(); return; }
                
        dfs_push( str_i, cc, GREATER );
                
        if ( biggerValuesInRight( cc ) )
        { c = coil_array->enter_next_branch( cc ); }
        else
        {
            coil_array->read_left_link();
            c = coil_array->enter_next_branch();
        }
                
        if ( c == 17 ) { next(); return; }
    }
    else { next(); return; }
            
    read_string( c );
    string_end_i = coil_array->current_char_i();
}
        
void
StringSet::iterator::
previous()
{
    int str_i;
    char_t cc;
    Option option;
            
    state_of_iterator = other;
            
    if ( !dfs_pop( str_i, cc, option ) )
    {
        current_string.erase();
        string_end_i = coil_array->current_char_i();
        state_of_iterator = front;
        return;
    }
            
    current_string.erase( str_i + 1 );
    char_t c;
            
    if ( option == GREATER )
    {
        dfs_push( str_i, cc, EQUAL );
        
        c = coil_array->current_char();
        if ( c == 17 ) { previous(); return; }
                
        if ( hasTwoChildren( cc ) )
        {
            unsigned coil_start_i = coil_array->read_left_link();
            coil_array->enter_coil( coil_start_i );
            current_string += c; c = coil_array->current_char();
        }
                
    }
    else if ( option == EQUAL )
    {
        if ( !hasTwoChildren( cc ) &&
             biggerValuesInRight( cc ) ) { previous(); return; }
                
        dfs_push( str_i, cc, LESSER );
                
        if ( !biggerValuesInRight( cc ) )
        { c = coil_array->enter_next_branch( cc ); }
        else
        {
            coil_array->read_left_link();
            c = coil_array->enter_next_branch();
        }
                
        if ( c == 17 ) { previous(); return; }
    }
    else { previous(); return; }
            
    read_string2( c );
    string_end_i = coil_array->current_char_i();
}
        
unsigned
StringSet::iterator::
dfs_push( int str_i, char_t c, Option option )
{
    dfs_stack.push_back( new DfsStackElem );
    DfsStackElem& sel = *dfs_stack.back();
    coil_array->QueryState( sel.ci, sel.base_i, sel.coil_end_i,
                            sel.coils );
    sel.str_i = str_i;
    sel.c = c;
    sel.option = option;
    
    return dfs_stack.size() - 1;
}
        
bool
StringSet::iterator::
dfs_pop( int& str_i, char_t& c, Option& option )
{
    if ( dfs_stack.empty() ) { return false; }
    
    const DfsStackElem& sel = *dfs_stack.back();
    coil_array->SetState( sel.ci, sel.base_i, sel.coil_end_i,
                          sel.coils );
    str_i = sel.str_i;
    c = sel.c;
    option = sel.option;
            
    delete dfs_stack.back(); dfs_stack.pop_back();
    return true;
}

StringSet::iterator::DfsStackElem*
StringSet::iterator::DfsStackElem::
clone()
{
    DfsStackElem& el = *(new DfsStackElem);
    el.ci = ci;
    el.base_i = base_i;
    el.coil_end_i = coil_end_i;
    el.coils = coils;

    el.str_i = str_i;
    el.c = c;
    el.option = option;
    return &el;
}
