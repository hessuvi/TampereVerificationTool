/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen.
*/

#ifdef CIRC_LOGWRITE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_LOGWRITE_HH_
#define ONCE_LOGWRITE_HH_
#define CIRC_LOGWRITE_HH_

#include "config.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <string>
#endif

class LogWrite
{
public:
    static inline std::ostream& getOStream() { return *getOutputStream(); }

    static void setLogFile(const std::string& logfile);
    static const std::string& setProgramName(const std::string& name = "");

    static const std::string getDateTime();
    static const std::string getInfoString();

    static std::string neatString( const std::string&,
                                   unsigned max_line_length =
                                   MAX_CHARS_IN_LINE );
    static void writeNeatly( const std::string& );

private:
    static std::ostream* getOutputStream(std::ostream* os = 0);
    //static std::ostream* outputStream;
    //static std::string programName;
};

#undef CIRC_LOGWRITE_HH_
#endif
