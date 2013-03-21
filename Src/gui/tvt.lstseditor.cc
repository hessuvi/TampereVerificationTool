#include "LSTSEditor.hh"
#include "SystemTools.hh"

#include <iostream>
#include <cstdlib>
#include <cstring>

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    bool overrideCheck = false;
    int argIndex = 1;
    if(argIndex < argc &&
       std::strcmp(argv[argIndex], "--override-check") == 0)
    {
        overrideCheck = true;
        ++argIndex;
    }

    if(argIndex >= argc)
    {
        std::cerr << "Usage: " << argv[0]
                  << " [<options>] <LSTS file name>" << std::endl;
        return EXIT_FAILURE;
    }

    LSTSEditor editor("", argv[argIndex]);

    if(tools::fileExists(argv[argIndex]))
    {
        if(editor.readLSTSandRun(overrideCheck))
        {
            std::cerr << "The input LSTS is too big to be edited in a "
                "feasible way.\n"
                "(You can override this check at your own risk using the "
                "option\n--override-check)" << std::endl;
            return EXIT_FAILURE;
        }
    }
    else
        editor.run(true);

    return EXIT_SUCCESS;
}
