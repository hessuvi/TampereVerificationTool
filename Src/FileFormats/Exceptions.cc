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

#include "Exceptions.hh"
#include "LogWrite.hh"

#ifdef NO_EXCEPTIONS
#include <cstdlib>
#endif

using std::string;


namespace
{
    const string error_prefix( "**Error: " );
    const string warning_prefix( "++Warning: " );
}


void
Exs::Throw( const std::string& msg, Exception* exc )
{
    PrintError( msg );

#ifdef NO_EXCEPTIONS
    std::exit( 1 );
#endif

    if ( exc ) { throw exc; }
    throw Exception();
}

void
Exs::PrintError( const std::string& msg )
{ LogWrite::writeNeatly( error_prefix + msg ); }

void
Exs::PrintWarning( const std::string& msg )
{ LogWrite::writeNeatly( warning_prefix + msg ); }

string
Exs::catString( const string& start, unsigned val, const string& end )
{
    string res_str;

    if ( !val ) { return start + "0" + end; }

    while ( val )
    {
        res_str += '0' + val % 10;
        val /= 10;
    }
    
    for ( unsigned i = 0; i < res_str.size() / 2; ++i )
    {
        unsigned j = res_str.size() - i - 1;
        char c = res_str[ j ];
        res_str[ j ] = res_str[i];
        res_str[i] = c;
    }
    
    return start + res_str + end;
}
