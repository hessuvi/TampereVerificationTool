#ifndef UF_CONFIG_HH
#define UF_CONFIG_HH

#include "itstream.hh"

#include <vector>


typedef int byte_t;
typedef std::vector<byte_t> byte_code_t;

typedef unsigned type_index_t;

// Let i be of type_index_type, then 0 <= i < var_types.size().
static const type_index_t logic_exp = 0;
static const type_index_t int_exp = 1;

extern ff::itstream it;

#endif
