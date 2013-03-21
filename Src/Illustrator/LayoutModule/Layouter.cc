#include "Layouter.hh"

void Layouter::calculateLayout(std::vector<State>& states,
                               std::vector<Transition>& transitions)
{
    for(unsigned i = 0; i < states.size(); ++i)
    {
        states[i].coords.x = i;
        states[i].coords.y = i;

        for(unsigned j = 0; j < states[i].transitionsAmount; ++j)
        {
            unsigned trInd = states[i].transitionsStartIndex + j;
            transitions[trInd].bendPoints.push_back(states[i].coords);
        }
    }

    for(unsigned i = 0; i < transitions.size(); ++i)
    {
        Transition& tr = transitions[i];
        tr.bendPoints.push_back(states[tr.endStateIndex].coords);
    }
}
