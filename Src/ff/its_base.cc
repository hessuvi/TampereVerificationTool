/*
  itstream is a library for reading tokens from an input stream.
  Copyright (C) 2005 Timo Erkkilä {timo.erkkila@tut.fi}
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "its_base.hh"
#include "gen.hh"

#include <algorithm>

namespace ff
{
    // General:
    inline bool greaterLength( const std::string& s1, const std::string& s2 )
    { return s1.length() > s2.length(); }
    
    inline unsigned find_string( const std::string& str,
                                 const std::vector<std::string>& vec )
    { for ( unsigned i = 0; i < vec.size(); )
      { if ( vec[i++] == str ) { return i; } } return 0; }
    
    inline bool case_ins_compare( const std::string& a, const std::string& b )
    {
        if ( a.size() != b.size() ) { return false; }
        for ( unsigned i = 0; i < a.size(); ++i )
        { if ( tolower( static_cast<unsigned char>( a[i] ) ) !=
               tolower( static_cast<unsigned char>( b[i] ) ) )
        { return false; } }
        return true;
    }

    // cs_str_data_token:
    bool cs_str_data_token::operator==( const std::string& sc ) const
    { return cs ? ( sc == data ) : case_ins_compare( sc, data ); }

    // rword_type:
    unsigned rword_type::operator==( const std::string& List ) const
    {
        if ( cs ) { return is_str_in_list( data, List ); }
        return is_tolower_str_in_list( data, List );
    }
    unsigned rword_type::opt_get( const rword_type& word )
    {
        unsigned found_n = operator==( word.data );
        if ( !found_n ) { return 0; }
        data.swap( const_cast<std::string&>( word.data ) );
        itr.unset_peek(); return found_n;
    }

    // its_base:
    void its_base::set_puncts( const char** array, unsigned siz )
    {
        puncts.assign( &array[0], &array[siz / sizeof( char* )] );
        std::sort( puncts.begin(), puncts.end(), greaterLength );
    }
    
    void its_base::set_puncts( const char* ps )
    {
        for ( puncts.clear(); *ps; )
        {
            if ( *ps == ' ' ) { ++ps; continue; }
            std::string str; str = *ps;
            while ( *++ps && *ps != ' ' ) { str += *ps; }
            puncts.push_back( str );
        }
        std::sort( puncts.begin(), puncts.end(), greaterLength );
    }

    void create_msg_prefix( const file_source& fs )
    {
        std::string& rep_str = input_error::rep_str;
        if ( fs.name.size() ) { rep_str = fs.name; }
        else { rep_str = "<no filename>"; }
        rep_str += ": ";

        if ( fs.cursor )
        {
            rep_str += "L"; rep_str << fs.cursor->line; rep_str += ", C";
            rep_str << fs.cursor->column; rep_str += ": ";
        }
    }

    // inner_punct_type:
    unsigned inner_punct_type::opt_get( const punct_type& punc )
    {
        unsigned found_n = is_str_in_list( ps->back(), punc.data );
        if ( !found_n ) { return 0; }
        ps->back().swap( const_cast<std::string&>( punc.data ) );
        ps->pop_back(); if ( ps->empty() ) { itr.unset_peek(); }
        return found_n;
    }
    unsigned inner_punct_type::opt_get( int& i )
    {
        if ( ps->size() == 1 && ps->back() == "-" )
        { unsigned u;
          if ( itr.direct->opt_get( u ) )
          { i = -u; ps->pop_back(); itr.unset_peek(); return 1; } }
        i = 0; return 0;
    }
    unsigned inner_punct_type::opt_get( integ_type& i )
    { return inner_punct_type::opt_get( i.data ); }

    // its_base:
    unsigned its_base::is_rword( const std::string& sc ) const
    {
        if ( s_rword.cs ) { return ff::find_string( sc, rwords ); }
        std::string tc( sc ); for ( unsigned i = 0; i < tc.size(); ++i )
                              { tc[i] = tolower( tc[i] ); }
        return ff::find_string( tc, rwords );
    }
    void its_base::error( const std::string& msg ) const
    { throw input_error( fs->name, *(fs->cursor), msg.c_str() ); }
    void its_base::error( const std::string& token, const std::string& msg )
        const
    { std::string str( "`" ); str += token + "', " + msg; error( str ); }
}
