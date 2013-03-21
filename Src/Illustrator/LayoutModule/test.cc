#include "Layouter.hh"
#include "../lts_draw_write.hh"

#include <vector>
#include <cstdlib>

namespace
{
    Layouter::State statesData[] =
    {
        { 0, 1, { 0, 0 } },
        { 1, 1, { 0, 0 } },
        { 2, 0, { 0, 0 } }
    };

    Layouter::Transition transData[] =
    {
        { 1, std::vector<Layouter::Coordinates>() },
        { 2, std::vector<Layouter::Coordinates>() }
    };
}

int main()
{
    std::vector<Layouter::State>
        states(statesData,
               statesData+sizeof(statesData)/sizeof(statesData[0]));

    std::vector<Layouter::Transition>
        transitions(transData,
                    transData+sizeof(transData)/sizeof(transData[0]));

    Layouter::calculateLayout(states, transitions);

    LTS_Draw_Write output(std::cout);

    for(unsigned i = 0; i < states.size(); ++i)
        output.TulostaTila(Point(states[i].coords.x, states[i].coords.y),
                           "", "#000000", "#FFFFFF", 0,
                           std::list<std::string>());

    for(unsigned i = 0; i < transitions.size(); ++i)
    {
        std::list<Point> pisteet;
        for(unsigned j = 0; j < transitions[i].bendPoints.size(); ++j)
            pisteet.push_back(Point(transitions[i].bendPoints[j].x,
                                    transitions[i].bendPoints[j].x));
        output.TulostaTapahtuma(pisteet, "", "#000000", "");
    }

    return EXIT_SUCCESS;
}
