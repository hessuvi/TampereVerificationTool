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


#ifdef CIRC_COMMANDLINEPARSER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_COMMANDLINEPARSER_HH_
#define ONCE_COMMANDLINEPARSER_HH_
#define CIRC_COMMANDLINEPARSER_HH_

#ifndef MAKEDEPEND
#include <vector>
#include <string>
#endif

// Class declaration:

class CommandLineParser
{
public:
    CommandLineParser(const std::string& optionSeparators="",
                      bool errorOnUnknownOption=true,
                      bool ignoreNonOptions=true);

    virtual bool parseCommandLine(int& argc, char**& argv);
    virtual bool parseCommandLine(std::vector<std::string>& commandLine);

    virtual ~CommandLineParser();


protected:
    // Methods for derived classes:
    // ---------------------------
    template<typename Derived>
    inline void setOptionHandler
    (const std::string& option,
     bool (Derived::*optionHandler)(const std::string&),
     bool exactMatch=true,
     const std::string& description="",
     const std::string& additionalParameters="",
     bool additionalParametersAreSeparate=true,
     bool hiddenOption=false);

    std::string getNextParameter();
    const std::string& peekNextParameter();
    bool nextParameterIsOption();
    const std::string& getProgramName();

    virtual void help(const std::string& programName)=0;

    void printOptionsHelp(unsigned indentation = 20/*,
                          std::ostream& = std::cerr*/);

    virtual bool unknownOptionHandler(const std::string&);
    virtual bool nonOptionHandler(const std::string&);



//---------------------------------------------------------------------------
private:
//---------------------------------------------------------------------------
    // Method ptr handling stuff:
    class HandlerPtrInterface
    {
     public:
        virtual bool dispatch(CommandLineParser* t, const std::string&) = 0;
    };

    template<typename Derived>
    class HandlerPtr: public HandlerPtrInterface
    {
     public:
        inline HandlerPtr(bool (Derived::*p)(const std::string&)): hptr(p) {}
        inline bool dispatch(CommandLineParser* t, const std::string& s)
        {
            return (static_cast<Derived*>(t)->*hptr)(s);
        }

     private:
        bool (Derived::*hptr)(const std::string&);
    };

    template<typename Derived>
    static inline
    HandlerPtr<Derived>* makeHandler(bool (Derived::*p)(const std::string&))
    {
        return new HandlerPtr<Derived>(p);
    }

    // Member variables:
    std::string optionSeparators;
    bool errorOnUnknownOption, ignoreNonOptions;

    struct OptionHandler
    {
        std::string option;
        HandlerPtrInterface* handler;
        bool exactMatch;
        std::string description, additionalParameters;
        bool additionalParametersAreSeparate, hidden;
    };
    std::vector<OptionHandler> handlers;

    std::vector<std::string>::iterator paramPtr;
    std::vector<std::string>* commandLine;
    char** newargv;

    static inline unsigned min(unsigned n1,unsigned n2)
    { return n1<n2 ? n1 : n2; }
    static inline unsigned min(unsigned n1,unsigned n2,unsigned n3)
    { return n1<n2 ? min(n1,n3) : min(n2,n3); }

    static inline bool isEqual(const std::string& s1, const std::string& s2)
    {
        for(unsigned i=0; i<s1.size(); ++i)
        {
            if(i == s2.size() || s1[i] != s2[i]) return false;
        }
        return true;
    }

    // Disable copying:
    CommandLineParser (const CommandLineParser&);
    CommandLineParser& operator=(const CommandLineParser&);
};


// Set the command line option handler
//===========================================================================
template<typename Derived>
inline void
CommandLineParser::setOptionHandler
(const std::string& option,
 bool (Derived::*oHandler)(const std::string&),
 bool exactMatch,
 const std::string& description,
 const std::string& additionalParameters,
 bool additionalParametersAreSeparate,
 bool hiddenOption)
{
    OptionHandler newHandler=
    {
        option, makeHandler(oHandler), exactMatch, description,
        additionalParameters, additionalParametersAreSeparate,
        hiddenOption
    };

    handlers.push_back(newHandler);
}

#undef CIRC_COMMANDLINEPARSER_HH_
#endif
