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

Contributor(s): Juha Nieminen, Timo Erkkilä, Terhi Kilamo, Heikki Virtanen.
*/

#include <iostream>
#include "FileFormat/Lexical/ITokenStream.hh"
#include "InOutStream.hh"

// The command line parser of TVT:
#include "TvtCLP.hh"

#include "General.hh"
#include "Writer.hh"
#include "Reader.hh"
#include "Process.hh"

class InpCLP: public TvtCLP
{
  static const char* const description;
  bool debugMode, unused;

public:
  InpCLP()
    : TvtCLP(description), debugMode(false), unused(true)
  {
    setOptionHandler("d", &InpCLP::debugHandler, false,
		     "Debug mode. Writes information about what the program "
		     "is doing to stdout. Also creates a state proposition for "
		     "each state and sets it on in the state. Thus you can more "
		     "easily manually verify that the output is correct. ");

    setOptionHandler("u", &InpCLP::unusedHandler, false,
		     "Don't write unused action names. By default, all mentioned "
		     "gates and their legal combinations of parameter values are "
		     "written to the resulting LSTS file. If this option is specified, "
		     "only those actions and combinations of parameter values that are "
		     "used in the reachable part of the process are written.");
  }

  bool getDebugMode()
  {
    return debugMode;
  }

  bool getUnused()
  {
    return unused;
  }

private:
  bool debugHandler(const string& param)
  {
    if (param == "d" || param != "d")
      {
	cout << "Debug mode on" << endl;
	debugMode = true;
	return true;
      }
    else
      {
	return false;
      }
  }

  bool unusedHandler(const string& param)
  {
    if (param == "u" || param != "u")
      {
	unused = false;
	return true;
      }
    else
      {
	return false;
      }
  }
};

const char* const InpCLP::description="TVT input language compiler.\n"
                                      "Converts a TVT input language file to an LSTS file.";


int main(int argc, char* argv[])
{
  InpCLP options;

  if ( !options.parseCommandLine(argc, argv))
    {
      return 1;
    }

  Process process(options.getDebugMode());

  Writer compiler(&process, options.getDebugMode(), options.getUnused());
  Reader reader(options.getInputStream(), &process, options.getDebugMode());

  try
    {
      reader.readFile();
    }
  catch(ILError e)
    {
      std::cerr << reader.error(e.msg) << endl;

      return 1;
    }
  
  try
    {
      compiler.convertToLSTS();
      compiler.writeOutputFile(options.getOutputStream());
      if (options.getDebugMode()) cout << "All done, cleaning up." << endl;
    }
  catch (ILError e)
    {
      std::cerr << e.msg << endl;

      return 1;
    }

  return 0;
}
