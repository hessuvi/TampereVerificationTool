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

  When a wished token is not available: strings are set as empty ""
  and integers are set 0. For example, after failed opt_get( ff::id ),
  *ff::id yields an empty string.
*/

#include "itstream.hh"
#include "gen.hh"

#include <algorithm>

namespace ff
{
    template <class istream>
    class unread_token : public ff::token_base
    {
     public:
        unread_token( ff::file_source& src, ff::its_base& it ) :
            is( src ), itb( it ) { itb.s_punct.ps = &ps; }
        typedef unread_token<istream> this_t;

        // RESERVED WORD:
        unsigned opt_get( ff::rword_type& word )
        {
            if ( !read_id( word.data ) ) { return 0; }
            if ( itb.is_rword( word.data ) )
            { word.cs = itb.rword_case_sens(); return 1; }
            // The token is identifier that is stored as peek token:
            itb.set_peek( itb.s_id ); itb.s_id.data.swap( word.data );
            word.data.clear(); return 0;
        }
        unsigned opt_get( const ff::rword_type& word )
        {
            if ( !this_t::opt_get( itb.s_rword ) ) { return 0; } 
            unsigned found_n = ( itb.s_rword == word.data );
            if ( found_n )
            { itb.s_rword.data.swap( const_cast<std::string&>( word.data ) );
              return found_n; }
            itb.set_peek( itb.s_rword ); return 0;
        }

        // IDENTIFIER:
        unsigned opt_get( ff::id_type& id )
        {
            if ( !read_id( id.data ) ) { return 0; }
            if ( !itb.is_rword( id.data ) ) { return 1; }
            // The token is reserved word that is stored as peek token:
            itb.set_peek( itb.s_rword ); itb.s_rword.data.swap( id.data );
            id.data.clear(); return 0;
        }
        unsigned opt_get( const ff::id_type& id )
        {
            if ( !this_t::opt_get( itb.s_id ) ) { return 0; }
            if ( itb.s_id == id.data ) { return 1; }
            itb.set_peek( itb.s_id ); return 0;
        }

        // PUNCTUATOR:
        inline bool is_punct_char( int c )
        { return ispunct( c ) && c != '#' && c != '"'; }

        inline void ps_pop_to( std::string& str )
        { str.swap( ps.back() ); ps.pop_back(); }
        unsigned opt_get( ff::punct_type& punc )
        {
            if ( !is_punct_char(is.sws_peek()) )
            { punc.data.clear(); return 0; }
            read_punct( is.get() ); ps_pop_to( punc.data );
            if ( ps.size() ) { itb.set_peek( itb.s_punct ); }
            return 1;
        }
        unsigned opt_get( const ff::punct_type& punc )
        {
            if ( !is_punct_char( is.sws_peek() ) ) { return 0; }
            read_punct( is.get() );
            unsigned found_n = is_str_in_list( ps.back(), punc.data );
            if ( found_n )
            {
                ps.back().swap( const_cast<std::string&>( punc.data ) );
                ps.pop_back();
                if ( ps.size() ) { itb.set_peek( itb.s_punct ); }
                return found_n;
            }

            itb.set_peek( itb.s_punct ); return 0;
        }

        // STRING:
        unsigned opt_get( std::string& str )
        {
            str.clear();
            if ( is.sws_peek() != '"' ) { return 0; }
            is.skip();
            while ( is.peek() != '"' )
            { if ( is.peek() == EOF )
              { itb.error( "endless string (closing quote missing)" ); }
              str += is.get();
            }
            is.skip();
            return 1;
        }
        unsigned opt_get( ff::str_type& str ){return this_t::opt_get(str.data);}

        // UNSIGNED:
        unsigned opt_get( unsigned& i )
        {
            if ( !isdigit( is.sws_peek() ) ) { i = 0; return 0; }
            i = is.get() ^ 48;
            while ( isdigit( is.peek() ) ) { i *= 10; i += is.get() ^ 48; }
            return 1;
        }
        unsigned opt_get( ff::unsig_type& uns )
        { return this_t::opt_get( uns.data ); }

        // INTEGER:
        inline unsigned uns_opt_get( int& i )
        { unsigned u; unsigned b = opt_get( u ); i = u; return b; }

        unsigned opt_get( int& i )
        {
            if ( uns_opt_get( i ) ) { return 1; }
            if ( is.peek() == '-' )
            {
                is.skip(); read_punct( '-' );
                if ( ps.size() == 1 && ps.back().size() == 1 &&
                     uns_opt_get( i ) ) { ps.clear(); i = -i; return 1; }
                itb.set_peek( itb.s_punct );
            }
            return 0;
        }
        unsigned opt_get( ff::integ_type& i ) {return this_t::opt_get(i.data);}

        unsigned opt_get( ff::eoi_type& )
        { if ( is.sws_peek() != EOF ) { return 0; }
          itb.set_peek( itb.s_eoi ); return 1; }
        
     private:
        bool read_id( std::string& sc )
        {
            if ( !isalpha( is.sws_peek() ) && is.peek() != '_' )
            { sc.clear(); return false; }
            for ( sc = is.get(); isalnum( is.peek() ) || is.peek() == '_'; 
                  sc += is.get() ); return true;
        }

        const std::string* extract_punct( std::string& p_str, unsigned& pi )
        {
            for ( unsigned i = 0; i < itb.puncts.size(); ++i )
            {
                const std::string& c_punct = itb.puncts[i];
                if ( !p_str.compare( pi, c_punct.size(), c_punct ) )
                { pi += c_punct.size(); return &c_punct; }
            }
            return 0;
        }
   
        void read_punct( int first_char )
        {
            std::string p_str; p_str = first_char;
            while ( is_punct_char( is.peek() ) )
            { p_str += is.get(); }
            std::cout << "p_str: " << p_str << std::endl;
            ps.clear(); unsigned pi = 0;
            do 
            {
                const std::string* matched_punc = extract_punct( p_str, pi );
                if ( !matched_punc )
                { itb.error( p_str.substr( pi ), "bad punctuation" ); }
                ps.push_back( *matched_punc );
                std::cout << *matched_punc << std::endl;
            } while ( pi < p_str.size() );

            std::reverse( ps.begin(), ps.end() );
        }

        ff::icstream<istream> is;
        ff::its_base& itb;
        std::vector<std::string> ps;
    };
}

namespace ff
{
    // itstream
    itstream::~itstream() { delete direct; }
    
    void itstream::open( file_source& src )
    {
        delete direct; direct = new unread_token<c_istream>( src, *this );
        tb = direct; fs = &src;
    }
}
