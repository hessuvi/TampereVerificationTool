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

#include "TvtCLP.hh"

#include "LogWrite.hh"
#include "error_handling.hh"

using std::string;
using std::vector;
using std::endl;
using std::cerr;

// Constructor and destructor
//===========================================================================
TvtCLP::TvtCLP(const string& programDescription, bool allowInputFromStdin):
    CommandLineParser("-", true, false),
    allowInputFromStdin(allowInputFromStdin), force(false),
    allowInterrupted(false),
    progDescr(programDescription),
    requiredFilenamesCntMin(0), requiredFilenamesCntMax(0),
    startingTime(std::time(NULL))
{
    setOptionHandler("l", &TvtCLP::logFileHandler, true,
                     "", "<file>");
    setOptionHandler("-log", &TvtCLP::logFileHandler, true,
                     "Write all messages to <file>.", "<file>");
    setOptionHandler("-version", &TvtCLP::versionOptionHandler, true,
                     "Prints toolkit version which this program is part of.");
    setOptionHandler("f", &TvtCLP::forceOptionHandler);
    setOptionHandler("-force", &TvtCLP::forceOptionHandler, true,
                     "Overwrite output file if it exists.");
    setOptionHandler("c", &TvtCLP::commentOptionHandler, true,
                     "", "<text>");
    setOptionHandler("-comment", &TvtCLP::commentOptionHandler, true,
                     "Add text to the history section.", "<text>");

    setOptionHandler("-copy-section", &TvtCLP::copySectionOptionHandler, true,
                     "Copies the specified section to the result even "
                     "if it was an include section.", "<section name>");

    setOptionHandler("-extract-section", &TvtCLP::extractSectionOptionHandler,
                     true, "Writes the specified section of the result to the "
                     "specified include file.",
                     "<section name> <file name>");

    setOptionHandler("-allow-interrupted-lsts",
                     &TvtCLP::allowInterruptedOptionHandler,
                     true, "", "", true, true);

    /*
    setOptionHandler("OF=", &TvtCLP<T>::outputformatOptionHandler, false,
                     "", "<format>", false);
    string OFHelp = "Specify file format for <outputfile>. "
        "Valid values for <format> are:";
    for(unsigned i=0; i<output.formatNames.size(); i++)
    {
        OFHelp += " "; OFHelp += output.formatNames[i];
    }
    setOptionHandler("-output-format=", &TvtCLP<T>::outputformatOptionHandler,
                     false, OFHelp, "<format>", false);
    */

    setOptionHandler("", &TvtCLP::stdinOptionHandler);
}

TvtCLP::~TvtCLP()
{
    for(unsigned i=0; i<fileStreams.size(); i++)
    {
        delete fileStreams[i];
    }
}

// Public methods
//===========================================================================
bool TvtCLP::parseCommandLine(int& argc, char**& argv)
{
    return CommandLineParser::parseCommandLine(argc, argv);
}

bool TvtCLP::parseCommandLine(vector<string>& commandLine)
{
    LogWrite::setProgramName(commandLine[0]);

    // Take the command line:
    fullCommandLine = commandLine;
    // Call the parser:
    if(!CommandLineParser::parseCommandLine(commandLine))
        return false;
    // Check that the correct amount of file names were given:
    if((fileNames.size() == 0 && !allowInputFromStdin) ||
       ((requiredFilenamesCntMin!=0 &&
         fileNames.size()<requiredFilenamesCntMin) ||
        (requiredFilenamesCntMax!=0 &&
         fileNames.size()>requiredFilenamesCntMax)))
    {
        help(fullCommandLine[0]); return false;
    }

    if(!copySectionNames.empty())
    {
        for(unsigned i = 0; i < copySectionNames.size(); ++i)
        {
            OutStream::CopySection(copySectionNames[i]);
        }
    }

    if(!extractSectionNames.empty())
    {
        for(unsigned i = 0; i < extractSectionNames.size(); ++i)
        {
            OutStream::ExtractSection(extractSectionNames[i],
                                      extractFileNames[i]);
        }
    }

    return true;
}

void TvtCLP::giveInputStreamsTo(OutStream& os)
{
    for(unsigned i=0; i<fileStreams.size(); ++i)
    {
        if(fileTypes[i] == INPUT)
        { os.AddInStream( *(static_cast<InStream*>( fileStreams[i] )) ); }
    }
}

InStream& TvtCLP::getInputStream(unsigned index)
{
    static InStream standardInput("-");

    if(fileStreams.size() == 0)
    {
        fileStreams.resize(fileNames.size());
        fileTypes.resize(fileNames.size());
    }

    if(index >= fileNames.size())
    {
        if(allowInterrupted) standardInput.SetAllowInterruptedLSTS();
        return standardInput;
    }

    check_claim(fileStreams[index] == 0,
                string("Attempt to open file '")+fileNames[index]+"' twice.");

    InStream* is = new InStream(fileNames[index]);
    if(allowInterrupted) is->SetAllowInterruptedLSTS();
    fileStreams[index] = is;
    fileTypes[index] = INPUT;
    return *is;
}

void TvtCLP::initializeOutputStream(OutStream& os, const string& cl)
{
    os.SetCommandLine(cl);
    if(commentString.size()) os.AddCommentLine(commentString);
    os.SetBeginTime(startingTime);
    giveInputStreamsTo(os);
}

OutStream& TvtCLP::getOutputStream(unsigned index)
{
    static OutStream standardOutput("-");
    static bool stdoutinitialized = false;

    string cl = fullCommandLine[0];
    for(unsigned i=1; i<fullCommandLine.size(); ++i)
    {
        if(fullCommandLine[i] == "--comment" ||
           fullCommandLine[i] == "-c")
        {
            ++i;
        }
        else
        {
            cl += " "; cl += fullCommandLine[i];
        }
    }

    if(!stdoutinitialized)
    {
        stdoutinitialized = true;
        initializeOutputStream(standardOutput, cl);
    }

    if(fileStreams.size() == 0)
    {
        fileStreams.resize(fileNames.size(), 0);
        fileTypes.resize(fileNames.size());
    }

    if(index >= fileNames.size())
        return standardOutput;

    check_claim(fileStreams[index] == 0,
                string("Attempt to open file '")+fileNames[index]+"' twice.");

    OutStream* os = new OutStream(fileNames[index], force);
    fileStreams[index] = os;
    fileTypes[index] = OUTPUT;

    initializeOutputStream(*os, cl);

    return *os;
}


// Private methods
//===========================================================================
// --log
// -----
bool TvtCLP::logFileHandler(const string& param)
{
    check_claim(peekNextParameter() != "",
                string("File name expected after ")+param);
    LogWrite::setLogFile(getNextParameter());
    return true;
}

// --version
// ---------
bool TvtCLP::versionOptionHandler(const string&)
{
    // PRCS generates the proper line:
    // $Format: "    cerr << \"$ProjectVersion$\" << endl;"$
    cerr << "0.946" << endl;

    return false;
}

// --force
// -------
bool TvtCLP::forceOptionHandler(const string&)
{
    force = true;
    OutStream::SetForce( force );
    return true;
}

// --comment
// ---------
bool TvtCLP::commentOptionHandler(const string& param)
{
    check_claim(peekNextParameter()!="",
                string("Text string expected after ")+param);
    commentString = getNextParameter();
    return true;
}

// --copy-section
bool TvtCLP::copySectionOptionHandler(const string& param)
{
    check_claim(peekNextParameter()!="",
                string("Section name expected after ")+param);
    copySectionNames.push_back(getNextParameter());
    return true;
}

// --extract-section
bool TvtCLP::extractSectionOptionHandler(const string& param)
{
    check_claim(peekNextParameter()!="",
                string("Section name expected after ")+param);
    extractSectionNames.push_back(getNextParameter());
    check_claim(peekNextParameter()!="",
                string("Section and file name expected after ")+param);
    extractFileNames.push_back(getNextParameter());
    return true;
}

bool TvtCLP::allowInterruptedOptionHandler(const string&)
{
    allowInterrupted = true;
    return true;
}

/*
// --output-format= -optio
// -----------------------
template<typename T>
bool TvtCLP<T>::outputformatOptionHandler(const string& param)
{
    string format;
    if(param.substr(0,4) == "-OF=")
        format = param.substr(4);
    else
        format = param.substr(16);

    unsigned i;
    for(i=0; i<output.formatNames.size(); i++)
    {
        if(output.formatNames[i] == format)
            break;
    }
    check_claim(i < output.formatNames.size(),
                string("Invalid output file format: ")+format);

    output.outputFormat = i+1;

    return true;
}
*/

// - option
// --------
bool TvtCLP::stdinOptionHandler(const string&)
{
    return nonOptionHandler("-");
}

// File names reading
// ------------------
bool TvtCLP::nonOptionHandler(const string& param)
{
    fileNames.push_back(param);
    return true;
}

// Help:
void TvtCLP::help(const string& programName)
{
    printProgramDescription();
    printUsageNotice(programName);
    printOptionsHelp();
    printStdinStdoutNotice();
    printCopyrightNotice();
}

void TvtCLP::printProgramDescription()
{
    cerr << progDescr << endl;
}

void TvtCLP::printUsageNotice(const string& programName)
{
    cerr << endl << "Usage:" << endl
         << " " << programName
         << " [<options>] <input file> [<output file>]" << endl << endl;
}

void TvtCLP::printStdinStdoutNotice()
{
    cerr << LogWrite::
        neatString( "\n"
          "Input file name can be replaced by '-' which means that input "
	  "is stdin. If output file name is not specified or it is '-', "
          "the output will be stdout.\n\n" );
}

void TvtCLP::printCopyrightNotice()
{
    cerr << "co(1999-2003): "
        "VARG research group at Tampere University of Technology,\n"
            "               Institute of Software Systems.\n";
}
