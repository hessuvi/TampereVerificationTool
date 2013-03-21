#ifndef FOLDED_LSTS
#define FOLDED_LSTS

#include "uf_types.hh"

#include "DynamicArray.hh"
#include "compiler/uf_config.hh"

#include "folded/fdState.hh"
#include "folded/fdTransition.hh"

#include <string>
#include <vector>
#include <iostream>


class FoldedLsts
{
 public:
    FoldedLsts( class ufWriter& );
    ~FoldedLsts();

    // Compile time methods:
    fdState& GetState( unsigned state_i );

    void SetUnfoldedLsts( class UnfoldedLsts& );

    // Run time methods:
    unsigned numberOfFoldedStates() const { return states.size(); }

    void ConstructUfdInitState();
    void ConstructUfdState();

    bool isDeadLock( unsigned fd_state_i ) const
    { return states[fd_state_i].trs.empty(); }

    static class Alphabet& GiveAlphabet();

    // For debugging:
    void Print();

 private:
    void CreateUfdChildren();
    void CreateUfdState( fdTransition& tr, unsigned ufd_start_state_n );
    void EvalPostcondition( fdTransition& tr, unsigned ufd_state_n );

    // Methods for brute force evaluation:
    bool stepNewVariables();
    bool incNewVariable( unsigned var_i );

    void CheckOutputs( fdTransition& tr, unsigned& ufd_action_number );

    DynamicArray<fdState> states;

    class UnfoldedLsts* ufd_lsts;
    class ufWriter& writer;
};


#endif
