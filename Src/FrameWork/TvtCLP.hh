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


#ifdef CIRC_TVTCLP_HH_
#error "Include recursion"
#endif

#ifndef ONCE_TVTCLP_HH_
#define ONCE_TVTCLP_HH_
#define CIRC_TVTCLP_HH_

#include "CommandLineParser.hh"
#include "InOutStream.hh"

#ifndef MAKEDEPEND
#include <vector>
#include <string>
#include <ctime>
#endif


// Class declaration:

class TvtCLP: public CommandLineParser
{
public:
    TvtCLP(const std::string& programDescription,
           bool allowInputFromStdin=false);

    virtual ~TvtCLP();

    bool parseCommandLine(int& argc, char**& argv);
    bool parseCommandLine(std::vector<std::string>& commandLine);

    inline const std::vector<std::string>& getCommandLine() const;

    // 0 means any amount;
    inline void setRequiredFilenamesCnt(unsigned min, unsigned max=0);

    inline unsigned getFilenamesCnt() const;
    inline const std::string& getInputFilename(unsigned index) const;
    InStream& getInputStream(unsigned index = 0);
    OutStream& getOutputStream(unsigned index = 1);

    inline const std::string& getCommentString() const;

    inline bool getForce() { return force; }

    void help(const std::string& programName);

//===========================================================================
 protected:
    void printProgramDescription();
    void printUsageNotice(const std::string& programName);
    void printStdinStdoutNotice();
    void printCopyrightNotice();


//===========================================================================
//===========================================================================
 private:
    bool logFileHandler(const std::string&);
    bool forceOptionHandler(const std::string&);
    bool commentOptionHandler(const std::string&);
    bool outputformatOptionHandler(const std::string&);
    bool stdinOptionHandler(const std::string&);
    bool nonOptionHandler(const std::string&);
    bool versionOptionHandler(const std::string&);

    bool copySectionOptionHandler(const std::string&);
    bool extractSectionOptionHandler(const std::string&);

    bool allowInterruptedOptionHandler(const std::string&);

    void giveInputStreamsTo(OutStream& os);

    void initializeOutputStream(OutStream& os, const std::string&);

    enum FILETYPE { NONE=0, INPUT, OUTPUT };

    std::vector<std::string> fullCommandLine;
    std::vector<std::string> fileNames;
    std::vector<Stream*> fileStreams;
    std::vector<FILETYPE> fileTypes;

    bool allowInputFromStdin, force, allowInterrupted;
    std::string commentString;
    std::string progDescr;
    unsigned requiredFilenamesCntMin, requiredFilenamesCntMax;

    std::time_t startingTime;

    std::vector<std::string> copySectionNames;
    std::vector<std::string> extractSectionNames;
    std::vector<std::string> extractFileNames;

    // Disable copying:
    TvtCLP (const TvtCLP&);
    TvtCLP& operator=(const TvtCLP&);
};


// Implementations
//===========================================================================
inline const std::vector<std::string>& TvtCLP::getCommandLine() const
{
    return fullCommandLine;
}

inline void TvtCLP::setRequiredFilenamesCnt(unsigned minimum,
                                            unsigned maximum)
{
    requiredFilenamesCntMin = minimum;
    requiredFilenamesCntMax = maximum;
}

inline unsigned TvtCLP::getFilenamesCnt() const
{
    return fileNames.size();
}

inline const std::string& TvtCLP::getInputFilename(unsigned index) const
{
    static std::string none;

    if(index < fileNames.size())
        return fileNames[index];
    else
        return none;
}

inline const std::string& TvtCLP::getCommentString() const
{
    return commentString;
}


#undef CIRC_TVTCLP_HH_
#endif
