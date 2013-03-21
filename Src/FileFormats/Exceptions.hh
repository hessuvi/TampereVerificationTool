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

#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH

#include <string>


#ifndef check_claim

#define check_claim(claim, msg)  if( !(claim) ) { Exs::Throw( msg ); }
#define checkClaim(claim, msg, exp)  if( !(claim) ) { Exs::Throw( msg, exp ); }

#endif

#define warn_if_fails(claim, msg)  if( !(claim) ) { Exs::PrintWarning( msg ); }

namespace Exs
{
    struct Exception { };

    void Throw( const std::string& msg, Exception* = 0 );

    void PrintError( const std::string& msg );
    void PrintWarning( const std::string& msg );

    std::string catString( const std::string& start, unsigned val,
                           const std::string& end );
}


#endif
