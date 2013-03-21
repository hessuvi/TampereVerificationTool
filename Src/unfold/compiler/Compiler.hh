#ifndef COMPILER_HH
#define COMPILER_HH

#include "itstream.hh"
#include "uf_config.hh"


class Compiler
{
 public:
    Compiler();
    ~Compiler();

    void CompileFile( ff::file_source& fs, class FoldedLsts& to );
};


#endif
