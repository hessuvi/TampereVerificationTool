#include "SystemTools.hh"

#include <sys/stat.h>

bool tools::fileExists(const char* filename)
{
    struct stat fileStat;
    if(stat(filename, &fileStat) < 0) return false;
    return S_ISREG(fileStat.st_mode);
}
