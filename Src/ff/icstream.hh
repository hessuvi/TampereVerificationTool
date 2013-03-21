/*
  icstream is a class template that abstracts file level details from input.
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

#ifndef FF_ICSTREAM_HH
#define FF_ICSTREAM_HH

#include <string>
#include <cstdio>
#include <iostream>

inline std::string& operator<<( std::string& str, int val )
{
    if ( val < 0 ) { str += '-'; val = -val; };
    unsigned i = 0;
    int tmp_val = val; do { tmp_val /= 10; ++i; } while ( tmp_val );
    i += str.size(); str.resize( i );
    do { str[--i] = '0' + val % 10; val /= 10; } while ( val );
    return str;
}

namespace ff
{
    struct Cursor
    {
        Cursor() : line( 1 ), column( 0 ) { }
        void operator+=( int c )
        { if ( c == '\n' ) { ++line; column = 0; } else { ++column; } }

        int line; int column;
    };

    class file_source
    {
     public:
        file_source( FILE* is, std::string filename = "" ) :
            source_type( c_istream ), cis( is ), name( filename ),
            cursor( 0 ), rec( 0 ) { }

        void record( std::string& rec_str ) { rec = &rec_str; }
        void stop_recording() { rec = 0; }
        
        enum source_t { c_istream, std_string };
        const source_t source_type;
        union { const std::string* str; FILE* cis; };
        const std::string name;
        Cursor* cursor;
        std::string* rec;
    };

    struct input_error : public std::exception
    {
        input_error() { }
        input_error( const std::string& filename, Cursor&, const char* msg );
        virtual ~input_error() throw() { }
        const char* what() const throw() { return rep_str.c_str(); }
        static std::string rep_str;
    };
    
    template <class istream_t>
    class icstream
    {
     public:
        icstream( file_source& src ) : is( src ), peek_char( is.get() ),
                              fs( src ) { src.cursor = &cursor; }
        int peek() const { return peek_char; }
        int get() { int c = peek_char;
                    if ( fs.rec && c != EOF ) { (*fs.rec) += c; }
                    cursor += c; peek_char = is.get(); return c; }
        void skip() { get(); }

        // Skips white space and comments and peeks the next character:
        int sws_peek() { comb_out(); return peek(); }

     private:
        void c_eof( bool block_comment )
        {
            if ( block_comment )
            { saved_cursor.column += 2;
              throw input_error( fs.name, saved_cursor,
                                 "`#(', endless block comment" ); }
        }

        void skip_comments()
        {
            unsigned com_lvl = 0;
            while ( true )
            { skip(); // Skipped char is always '#'.
              if ( peek() == '(' ) { skip(); ++com_lvl; }
              else // A line comment.
              {
                  for ( int c = get(); c != '\n'; c = get() )
                  { if ( c == EOF ) { c_eof( com_lvl ); return; } }
                  if ( !com_lvl ) { return; }
              }
              while ( peek() != '#' )
              {
                  if ( peek() == ')' )
                  { skip();
                    if ( peek() == '#' )
                    { skip(); --com_lvl; if ( !com_lvl ) { return; } }
                  }
                  else if ( get() == EOF ) { c_eof( com_lvl ); return; }
              }
            }
        }

        void comb_out()
        {
            while ( true )
            {
                while ( isspace( peek() ) ) { skip(); }
                if ( peek() != '#' ) { return; }
                saved_cursor = cursor; skip_comments();
            }
        }

        istream_t is; int peek_char; Cursor cursor; Cursor saved_cursor;
        file_source& fs;
    };
    
    class string_istream
    {
     public:
        string_istream( file_source src ) : istr( *src.str ), next_i( 0 ) { }
        int get() { return ( next_i == istr.size() ) ? (EOF) :
                            ( static_cast<unsigned char>( istr[next_i++] ) ); }
     private:
        const std::string& istr; unsigned next_i;
    };
    
    class c_istream
    {
     public:
        c_istream( file_source src ) : cis( src.cis ) { }
        int get() { return getc( cis ); }
     private:
        FILE* cis;
    };


    typedef icstream<c_istream> c_icstream;
    typedef icstream<string_istream> str_icstream;

}

#endif
