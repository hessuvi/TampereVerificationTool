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

#include "CommandLineParser.hh"

#include "error_handling.hh"

#include <iostream>
using std::string;
using std::vector;
using std::cerr;
using std::endl;

// Constructor and destructor
//===========================================================================
CommandLineParser::CommandLineParser(const string& optionSeps,
                                     bool errOnUnknownOp,
                                     bool igNonOps):
    optionSeparators(optionSeps),
    errorOnUnknownOption(errOnUnknownOp),
    ignoreNonOptions(igNonOps),
    commandLine(0), newargv(0)
{
}

CommandLineParser::~CommandLineParser()
{
    for(unsigned i=0; i<handlers.size(); ++i)
    {
        delete handlers[i].handler; handlers[i].handler=0;
    }
    if(newargv) { delete[] newargv; newargv=0; }
}

// Parse command-line
//===========================================================================
// The version which takes argc and argv directly:
bool CommandLineParser::parseCommandLine(int& argc, char**& argv)
{
    bool ret;

    vector<string> cline(argv, argv+argc);
    ret = parseCommandLine(cline);

    if(ret && cline.size() != static_cast<unsigned>(argc))
    {
        newargv = new char*[cline.size()];
        unsigned ind=0;
        for(unsigned i=0; i<cline.size(); i++, ind++)
        {
            while(cline[i] != argv[ind]) ind++;
            newargv[i] = argv[ind];
        }
        argv = newargv;
        argc = cline.size();
    }
    return ret;
}

// The version which takes a vector:
bool CommandLineParser::parseCommandLine(vector<string>& cLine)
{
    paramPtr = cLine.begin()+1;
    this->commandLine = &cLine;

    string tmp;
    bool erased;;

    while(paramPtr != cLine.end())
    {
        // This should probably be removed...
        if(*paramPtr == "--help" || *paramPtr == "-h")
        {
            help(cLine[0]);
            paramPtr = cLine.erase(paramPtr);
            return false;
        }

        // Check for user-given options:
        erased = false;
        if(optionSeparators.empty() ||
           (!paramPtr->empty() &&
            optionSeparators.find((*paramPtr)[0]) != string::npos)
          )
        {
            if(optionSeparators.empty())
                tmp = *paramPtr;
            else
                tmp = paramPtr->substr(1);
            unsigned i;
            for(i=0; i<handlers.size(); i++)
            {
                // Check if the option has a handler:
                if((handlers[i].exactMatch && handlers[i].option == tmp) ||
                   (!handlers[i].exactMatch &&
                    isEqual(handlers[i].option, tmp))
                  )
                {
                    // Call the handler:
                    if(handlers[i].handler->dispatch(this, *paramPtr) == false)
                        return false;
                    // Remove the parameter from the command line:
                    paramPtr = cLine.erase(paramPtr);
                    erased = true;
                    break;
                }
            }
            if(i==handlers.size())
            {
                if(errorOnUnknownOption)
                {
		    write_message( string("Error: Unknown option: ") +
				   *paramPtr);
		    return false;
                }
                else
                {
                    // Call the default handler for unknown option:
                    if(!unknownOptionHandler(*paramPtr)) return false;
                    // Remove from command line:
                    paramPtr = cLine.erase(paramPtr);
                    erased = true;
                }
            }
        }
        else if(!ignoreNonOptions)
        {
            // Call the default handler for non-option:
            if(!nonOptionHandler(*paramPtr)) return false;
            // Remove from command line:
            paramPtr = cLine.erase(paramPtr);
            erased = true;
        }

        if(!erased) paramPtr++;
    }

    this->commandLine=0;
    return true;
}

// Helper methods:
//===========================================================================
string CommandLineParser::getNextParameter()
{
    if(commandLine==0 || paramPtr+1 == commandLine->end())
        return "";

    string ret = *(paramPtr+1);
    commandLine->erase(paramPtr+1);
    return ret;
}

const string& CommandLineParser::peekNextParameter()
{
    static const string empty("");
    if(commandLine==0 || paramPtr+1 == commandLine->end())
        return empty;

    return *(paramPtr+1);
}

bool CommandLineParser::nextParameterIsOption()
{
    if(commandLine==0 || paramPtr+1 == commandLine->end() ||
       optionSeparators.empty() ||
       optionSeparators.find((*(paramPtr+1))[0]) != string::npos)
        return false;
    return true;
}

const string& CommandLineParser::getProgramName()
{
    return (*commandLine)[0];
}

static inline void CLP_addIndentationSpaces(string& line, unsigned indentation)
{
    while(line.size() < indentation-1)
    {
        line += ' ';
    }
}

void CommandLineParser::printOptionsHelp(unsigned indentation)
{
    string line;

    cerr << "Options:" << endl;
    if(optionSeparators.size() > 1)
    {
        cerr << "  An option can start with any of these characters:";
        for(unsigned i=0; i<optionSeparators.size(); i++)
        {
            cerr << " " << optionSeparators[i];
        }
        cerr << endl;
    }

    cerr << " -h" << endl;
    line = " --help";
    CLP_addIndentationSpaces(line, indentation);
    cerr << line << "This text." << endl;

    for(unsigned i=0; i<handlers.size(); i++)
    {
        if(handlers[i].option == "" or
           handlers[i].hidden) continue;

        line=" ";
        if(optionSeparators.size() > 0)
        {
            line += optionSeparators[0];
        }
        line += handlers[i].option;
        if(handlers[i].additionalParameters != "")
        {
            if(handlers[i].additionalParametersAreSeparate)
            {
                line += ' ';
            }
            line += handlers[i].additionalParameters;
        }
        if(handlers[i].description == "")
        {
            cerr << line << endl;
        }
        else
        {
            if(line.size() >= indentation-1)
            {
                cerr << line << endl;
                line = "";
            }
            unsigned descInd=0;
            while(descInd < handlers[i].description.size())
            {
                CLP_addIndentationSpaces(line, indentation);

                unsigned nDescInd = descInd+77-indentation;
                if(nDescInd+1 < handlers[i].description.size())
                {
                    while(nDescInd > descInd &&
                          handlers[i].description[nDescInd] != ' ')
                    {
                        nDescInd--;
                    }
                    if(nDescInd == descInd)
                    {
                        nDescInd = descInd+77-indentation;
                        do
                        {
                            ++nDescInd;
                        }
                        while(nDescInd < handlers[i].description.size() &&
                              handlers[i].description[nDescInd] != ' ');
                    }
                    line += handlers[i].description.substr(descInd,
                                                           nDescInd-descInd);
                }
                else line += handlers[i].description.substr(descInd);

                cerr << line << endl;
                line = "";
                descInd = nDescInd+1;
            } // while
        } // else
    } // for
}

bool CommandLineParser::unknownOptionHandler(const string&) { return true; }
bool CommandLineParser::nonOptionHandler(const string&) { return true; }
