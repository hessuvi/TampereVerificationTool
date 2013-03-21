// =================================================
// Helping functions which might be system-dependant
// =================================================

#ifndef SYSTEM_TOOLS_HH
#define SYSTEM_TOOLS_HH

#include <cstdlib>
#include <ctime>

namespace tools
{
    inline void setSeed(long value)
    {
        srand48(value);
    }

    inline double randomValue()
    {
        return drand48();
    }

    bool fileExists(const char* filename);
}

#endif
