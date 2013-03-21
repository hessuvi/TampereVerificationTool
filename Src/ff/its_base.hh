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

#ifndef FF_ITS_BASE_HH
#define FF_ITS_BASE_HH

#include "icstream.hh"

#include <string>
#include <vector>
#include <exception>

namespace ff
{
    struct token_base
    {
        virtual ~token_base() { }
        
        virtual unsigned opt_get( struct rword_type& ) { return 0; }
        virtual unsigned opt_get( struct id_type& ) { return 0; }
        virtual unsigned opt_get( struct punct_type& ) { return 0; }
        virtual unsigned opt_get( struct str_type& ) { return 0; }
        virtual unsigned opt_get( struct unsig_type& ) { return 0; }
        virtual unsigned opt_get( struct integ_type& ) { return 0; }
        virtual unsigned opt_get( struct eoi_type& ) { return 0; }

        virtual unsigned opt_get( unsigned& ) { return 0; }
        virtual unsigned opt_get( int& ) { return 0; }
        virtual unsigned opt_get( std::string& ) { return 0; }

        virtual unsigned opt_get( const struct rword_type& ) { return 0; }
        virtual unsigned opt_get( const struct id_type& ) { return 0; }
        virtual unsigned opt_get( const struct punct_type& ) { return 0; }

        template <class Its, class Token>
        Its& read( Its& it, Token& tok ) const { it.get( tok ); return it; }
    };

    // The root class of itstream.
    struct its_root
    {
        its_root() : tb( 0 ), direct( 0 ) { }
        inline void set_peek( token_base& peek_token ) { tb = &peek_token; }
        inline void unset_peek() { tb = direct; }

        token_base* tb;
        token_base* direct;

        // For setting exception's messages:
        void set_ex_msg( const std::string& msg );
        void set_ex_msg( const std::string& token, const std::string& msg );
    };

    // Token-related classes:

    struct str_data_token : public token_base
    {
        const std::string& operator*() const { return data; }
        const std::string& as_string() const { return data; }

        bool operator==( const std::string& sc ) const { return sc == data; }
        bool operator!=( const std::string& sc ) const { return sc != data; }

        std::string data;
    };

    struct cs_str_data_token : public str_data_token
    {
        cs_str_data_token() : cs( true ) { }
        bool operator==( const std::string& sc ) const;
        bool operator!=( const std::string& sc ) const
        { return !operator==( sc ); }
        bool cs;
    };
    
    template <class Base>
    struct peek_base : public Base
    {
        peek_base( its_root& ir ) : itr( ir ) { }
     protected:
        inline unsigned ok( std::string& sc )
        { Base::data.swap( sc ); itr.unset_peek(); return 1; }
        
        its_root& itr;
    };

#define assignments( R_TYPE ) \
  const R_TYPE& operator=( const std::string& newd ) \
  { data = newd; return *this; } \
  const R_TYPE& operator=( const char* newd ) { data = newd; return *this; } \
  const R_TYPE& operator()( const std::string& newd ) \
  { data = newd; return *this; } \
  const R_TYPE& operator()( const char* newd ) { data = newd; return *this; }

    struct rword_type : public peek_base<cs_str_data_token>
    {
        static const char* name() { return "reserved word"; } 
        rword_type( its_root* ir = 0 ) : peek_base<cs_str_data_token>( *ir ) { }
        assignments( rword_type )
        unsigned operator==( const std::string& List ) const;
        bool operator!=( const std::string& sc ) const {return !operator==(sc);}
     private:
        unsigned opt_get( rword_type& word )
        { word.cs = cs; return ok( word.data ); }
        unsigned opt_get( const rword_type& word );
    };

    struct id_type : public peek_base<cs_str_data_token>
    {
        static const char* name() { return "identifier"; } 
        id_type( its_root* ir = 0 ) : peek_base<cs_str_data_token>( *ir ) { }
        assignments( id_type )
     private:
        unsigned opt_get( id_type& id ) { id.cs = cs; return ok( id.data ); }
        unsigned opt_get( const id_type& id )
        { if ( id != data ) { return 0; } itr.unset_peek(); return true; }
    };

    struct punct_type : public str_data_token
    {
        static const char* name() { return "punctuator"; } 
        assignments( punct_type )
    };

    struct str_type : public str_data_token
    { static const char* name() { return "string"; } };

    struct inner_punct_type : public peek_base<str_data_token>
    {
        inner_punct_type( its_root* ir = 0 ) :
            peek_base<str_data_token>( *ir ), ps( 0 ) { }
        unsigned opt_get( punct_type& punc )
        { punc.data.swap( ps->back() ); ps->pop_back();
          if ( ps->empty() ) { itr.unset_peek(); } return true; }
        unsigned opt_get( const punct_type& punc );
        unsigned opt_get( int& );
        unsigned opt_get( struct integ_type& );

        std::vector<std::string>* ps;
    };

    struct unsig_type : public token_base
    {
        static const char* name() { return "unsigned integer"; }
        unsigned operator*() const { return data; }
        std::string as_string() { std::string str; str << data; return str; }
        unsigned data;
    };

    struct integ_type : public token_base
    {
        static const char* name() { return "integer"; }
        int operator*() const { return data; }
        std::string as_string() { std::string str; str << data; return str; }
        int data;
    };

    struct eoi_type : public str_data_token
    {
        static const char* name() { return "end of input"; }
        eoi_type() { data = name(); }
        unsigned opt_get( eoi_type& ) { return true; }
    };

    template <class Token>
    struct opt_token : public Token
    {
        opt_token<Token>() : result( 0 ) { }
        typedef opt_token<Token> this_t;

        const this_t& operator=( const std::string& newd )
        { Token::data = newd; return *this; }
        const this_t& operator=( const char* newd )
        { Token::data = newd; return *this; }
        const this_t& operator()( const std::string& newd )
        { Token::data = newd; return *this; }
        const this_t& operator()( const char* newd )
        { Token::data = newd; return *this; }

        template <class Its>
        Its& read( Its& it, const this_t& tok ) const
        { result = it.opt_get( tok ); it.res_val = result; return it; }

        template <class Its>
        Its& read( Its& it, this_t& tok ) const
        { result = it.opt_get( tok ); it.res_val = result; return it; }

        operator bool () const { return result; }
        operator unsigned () const { return result; }
     private:
        mutable unsigned result;
    };

    // The base class of itstream.
    struct its_base : public its_root
    {
        its_base() : s_punct(this), s_rword(this), s_id(this), res_val( 0 ),
                     fs( 0 ) { }

        void error( const std::string& msg ) const;
        void error( const std::string& token, const std::string& msg ) const;
        
        template <class Token> 
        void expected( Token&, const char* s_msg )
        { if ( s_msg ) { error( s_msg ); return; }
          error( std::string( Token::name() ) + " expected" ); }
        void expected( int&, const char* s_msg )
        { integ_type i; expected( i, s_msg );}
        void expected( const str_data_token& tok, const char* s_msg )
        { if ( s_msg ) { error( s_msg ); return; }
          error( std::string( "`" ) + tok.data + "'" + " expected" ); }

        // The methods for setting punctuators and reserved words:        
        void set_puncts( const char** array, unsigned size_in_bytes );
        void set_puncts( const char* ps );
        void set_rwords( const char** array, unsigned size_in_bytes )
        { rwords.assign( &array[0], &array[size_in_bytes/sizeof( char* )] ); }
        
        // The methods for handling the case sensitivity issues:
        void set_id_case_sens( bool b ) { s_id.cs = b; }
        void set_rword_case_sens( bool b ) { s_rword.cs = b; }
        bool id_case_sens() const { return s_id.cs; }
        bool rword_case_sens() const { return s_rword.cs; }

        // The members below are not meant for public use:
        unsigned is_rword( const std::string& id ) const;

        eoi_type s_eoi; inner_punct_type s_punct;
        rword_type s_rword; id_type s_id;
        unsigned res_val;
        std::vector<std::string> rwords;
        std::vector<std::string> puncts;
        file_source* fs;
    };
    
    struct error_msg_type
    { 
        error_msg_type& operator() ( const char* new_msg )
        { data = new_msg; return *this; }
        const char* data;
    };

    struct default_msg_type { };
}

#undef assignments
#endif
