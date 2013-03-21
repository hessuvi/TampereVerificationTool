#ifndef UNFOLDED_LSTS
#define UNFOLDED_LSTS

// Local includes:
#include "FoldedLsts.hh"
#include "unfolded/DfsStack.hh"

// Library includes:
#include "StringTree/StringMap.hh"
#include <string>


class UnfoldedLsts
{
 public:
    UnfoldedLsts( FoldedLsts& flsts );

    void Unfold( class ufWriter& );

    // **PRECOND**: 0 <= fd_tate_i <= states.size()-1
    // Returns: number_of_states => 1 or
    //          0 which means the state exists
    bool createState
    ( unsigned fd_state_number, const std::vector<int>& new_variables,
      unsigned& ufd_state_number,
      const std::vector<fdState::state_of_var>& states_of_vars );

    StringMap<unsigned> state_ids;
    DfsStack dfs_stack;

 private:
    std::string id_string;
    std::string number_string;
    unsigned number_of_states;
    FoldedLsts& folded_lsts;
};


#endif
