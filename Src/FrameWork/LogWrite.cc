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

#include "LogWrite.hh"
#include "error_handling.hh"

#include <fstream>
#include <ctime>
#include <cstdio>
#include <cerrno>
#include <cstring> // strerror

//std::ostream* LogWrite::outputStream = &std::cerr;
//std::string LogWrite::programName;

std::ostream* LogWrite::getOutputStream(std::ostream* os)
{
    static std::ostream* outputStream = &std::cerr;

    if(os) outputStream = os;
    return outputStream;
}

const std::string& LogWrite::setProgramName(const std::string& name)
{
    static std::string programName;

    if(name.size()) programName = name;
    return programName;
}

void LogWrite::setLogFile(const std::string& logfile)
{
    static std::ofstream logstream;
    logstream.open(logfile.c_str(), std::ios::app);
    if(!logstream)
    {
        getOutputStream(&std::cerr);
        check_claim(false,
                    std::string("Can't open ")+logfile+": "+
                    std::strerror(errno));
    }
    getOutputStream(&logstream);
}

const std::string LogWrite::getDateTime()
{
    static char dateString[25];
    std::time_t t = std::time(NULL);
    std::tm *TM = localtime(&t);
    std::strftime(dateString, 25, "%d.%m.%Y %H:%M", TM);

    return std::string(dateString);
}

const std::string LogWrite::getInfoString()
{
    return getDateTime() + " " +setProgramName()+":\n ";
}

// Formats a string nicely so that there won't be lines longer than
// MAX_CHARS_IN_LINE in a resulting string.
std::string
LogWrite::neatString( const std::string& str, unsigned max_line_length )
{
    std::string result_str( str );
    unsigned col = 0;
    unsigned nl = 0;

    for ( unsigned i = 0; i < str.size(); ++i )
    {
        if ( str[ i ] == '\n' )
        {
            col = 0;
            continue;
        }

        if ( col > max_line_length )
        {
            result_str[ nl ] = '\n';
            col = i - nl;
        }

        if ( str[ i ] == ' ' ) { nl = i; }
        ++col;
    }

    return result_str;
}

void
LogWrite::writeNeatly( const std::string& msg )
{
    std::string str( getInfoString() );
    str += msg;
    getOStream() << neatString( str ) << std::endl;
}
