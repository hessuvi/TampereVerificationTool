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

// ------------------------------------------------------------------------
// Command line parser
// ------------------------------------------------------------------------
#include "cr_clp.hh"

#include <iostream>
using std::string;
using std::cout;
using std::cerr;
using std::endl;


bool USE_REGULAR_EXPRESSIONS = false;

const char* const CreateRulesCLP::description =
"  This program creates a default parallel composition rules file from a\n"
"series of LSTS files (to be used for making the parallel composition of\n"
"those files). Actions with same names synchronize.";


CreateRulesCLP::CreateRulesCLP(): TvtCLP(description), renameUsed(false)
{
    setOptionHandler("-rename", &CreateRulesCLP::renameHandler, true,
                     "Specify action hiding/renaming file. "
                     "                  " // KLUDGE!
                     "Write \"--help rename\" for more help.",
                     "<file>", true);
#ifndef NO_REGEXP
    setOptionHandler("-use-ere", &CreateRulesCLP::useereHandler, true,
                     "Handle strings in the hiding/renaming file as "
                     "Extended Regular Expressions. Write "
                     "\"--help use-ere\" for more help.");
#endif
}


bool CreateRulesCLP::rename() { return renameUsed; }
InStream& CreateRulesCLP::getRenameFile() { return renameFile; }


bool CreateRulesCLP::renameHandler(const string& param)
{
    string renameFileName = getNextParameter();
    if(renameFileName == "")
    {
        cerr << "File name expected after " << param << endl;
        return false;
    }
    renameUsed = true;
    renameFile.OpenInputFile(renameFileName);
    return true;
}

#ifndef NO_REGEXP
bool CreateRulesCLP::useereHandler(const string&)
{
    USE_REGULAR_EXPRESSIONS = true;
    return true;
}
#endif


void CreateRulesCLP::help( const string& programName )
{
    string nextParam = getNextParameter();

    if(nextParam == "rename")
    {
        cout <<
"  The hiding/renaming file contains groups of strings. Each groups is ended\n"
"with a semicolon (;).\n"
"  If there's one string in the group the program will hide the action with\n"
"that name. If there are two or more strings in a group the program will\n"
"rename the action specified in the first string to the action(s) specified\n"
"in the subsequent strings.\n";
#ifndef NO_REGEXP
        cout <<
"\n"
"  If the command line parameter --use-ere is specified, the first string of\n"
"the group will be handled as an Extended Regular Expression and the\n"
"subsequent strings as replacement strings. Write \"--help use-ere\" in the\n"
"command line for more help.\n";
#endif
    }
#ifndef NO_REGEXP
    else if(nextParam == "use-ere")
    {
        cout <<
"  Using Extended Regular Expressions\n"
"  -------------------------------------------------------------------------\n"
"\n"
"  When this flag is used in conjunction with the hiding/renaming file\n"
"(specified with the --rename option), the first string of each string group\n"
"will be used as an Extended Regular Expression and the subsequent strings\n"
"(i.e the resulting names of renaming) will be used as replacement patterns.\n"
"\n"
"  This can be used to hide or rename more than one action at a time.\n"
"  For example, the following line in the hiding/renaming file will hide all\n"
"actions which name begin with 'action' and are followed by one or more\n"
"digits:\n"
"\n"
"        \"action[0-9]+\";\n"
"\n"
"  The following will rename those actions to 'Test' followed by the same\n"
"number as the original had:\n"
"\n"
"        \"action([0-9]+)\" \"Test\\!1\";\n"
"\n"
"(Eg. an action named \"action321\" will be renamed to \"Test321\".)\n"
"\n"
"  This will hide every action:\n"
"\n"
"        \".*\";\n"
"\n"
"  Note that a '\\' character has to be quoted in \"TVT-style\", ie: '\\!'.\n"
"\n"
"  A quick explanation of Extended Regular Expressions (for a more extensive\n"
"explanation, see proper manual page (such as 'grep' or 'regex') or other\n"
"documentation):\n"
"  .   matches any character (ie. one character)\n"
"  *   matches 0 or more instances of the previous character\n"
"      (eg. \"a*\" matches \"\", \"a\", \"aa\", \"aaa\" and so on)\n"
"  +   like * but matches 1 or more instances of the previous character\n"
"  ?   like *, but matches 0 or 1 instance of the previous character\n"
"  ()  can be used for grouping (eg. \"(abc)+\" matches \"abc\", \"abcabc\", etc)\n"
"      They are also used for pattern replacement.\n"
"  |   choice (\"abc|def\" matches \"abc\" and \"def\" but nothing else)\n"
"  []  matches any of the character inside the brackets. A character can be\n"
"      a range in the form \"a-z\" (eg. \"[1-9A-Za-z]\"). If the first\n"
"      character inside the brackes is ^ then it will match any character\n"
"      not inside the brackets.\n"
"\n"
"  If the replacement string has a '\\!' followed by a digit (0-9), then\n"
"they will be replaced by the contents of the respective matching parentheses\n"
"in the ERE.\n"
"  '\\!0' is replaced by the whole matching string, '\\!1' is replaced with\n"
"the match inside the first pair of parentheses, '\\!2' with the match\n"
"inside the second pair of parentheses and so on.\n"
"  For example, if the ERE is \"action([0-9]+)\", the replacement string is\n"
"\"Test\\!1\" and the matching string is \"action321\", then the matching\n"
"string inside the parentheses will be \"321\" and thus the replacement is\n"
"\"Test321\".\n";
    }
#endif
    else
    {
        printProgramDescription();
        cerr << endl << "Usage:" << endl
             << " " << programName
             << " [<options>] [<output rulesfile> <inputfiles>]"
             << endl << endl;

        printOptionsHelp();
        printStdinStdoutNotice();
        printCopyrightNotice();
    }
}
