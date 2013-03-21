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

#ifndef FF_ITSTREAM_HH
#define FF_ITSTREAM_HH

#include "icstream.hh"
#include "its_base.hh"

namespace ff
{
    namespace
    {
        // Tokens:
        rword_type rword; id_type id; punct_type punct;
        str_type str; unsig_type unsig; integ_type integ; eoi_type eoi;
        // Optional tokens:
        opt_token<rword_type> opt_rword; opt_token<id_type> opt_id;
        opt_token<punct_type> opt_punct; opt_token<str_type> opt_str;
        opt_token<unsig_type> opt_unsig; opt_token<integ_type> opt_integ;
        // Manipulators:
        error_msg_type error_msg; default_msg_type default_msg;
    }

    // Input token stream:
    class itstream : public its_base
    {
     public:
        itstream() : e_msg( 0 ) { }
        itstream( file_source& src ) : e_msg( 0 ) { open( src ); }
        ~itstream();

        void open( file_source& src );
        
        template <class Token>
        unsigned opt_get( Token& tok ) throw ( input_error )
        { return tb->opt_get( tok ); }

        template <class Token>
        Token& get( Token& tok ) throw ( input_error )
        { if ( !opt_get( tok ) ) { expected( tok, e_msg ); } return tok; }

        template <class Token>
        itstream& operator>>( Token& tok ) throw ( input_error )
        { return tok.read( *this, tok ); }

        itstream& operator>>( int& i ) throw ( input_error )
        { get( i ); return *this; }

        // Setting error messages:
        itstream& operator>>( error_msg_type& m )
        { e_msg = m.data; return *this; }
        itstream& operator>>( default_msg_type& ) { e_msg = 0; return *this; }

        // Was the last optional token read successfully?
        operator bool () const { return res_val; }

     private:
        const char* e_msg;
    };
}

    
#endif
