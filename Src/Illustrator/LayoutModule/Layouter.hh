#ifndef LAYOUTER_HH
#define LAYOUTER_HH

#include <vector>

namespace Layouter
{
    typedef unsigned IndexType;

    struct Coordinates
    {
        int x, y;
    };

    struct State
    {
        IndexType transitionsStartIndex, transitionsAmount;
        Coordinates coords;
    };

    struct Transition
    {
        IndexType endStateIndex;
        std::vector<Coordinates> bendPoints;
    };

    void calculateLayout(std::vector<State>& states,
                         std::vector<Transition>& transitions);
}

#endif
