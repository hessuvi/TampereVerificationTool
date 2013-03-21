#include "LSTSEditorMainDrawingWindow.hh"
#include "SystemTools.hh"
#include "constants.hh"

#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>

namespace LayoutGenerator
{
    struct Node
    {
        int x, y;
    };

    struct NodePtrComparison
    {
        bool operator()(const Node* node1, const Node* node2) const
        {
            return node1->x == node2->x ?
                node1->y < node2->y : node1->x < node2->x;
        }
    };

    struct Arc
    {
        unsigned nodeIndex1, nodeIndex2;
        double length;
    };

    std::vector<Node> nodes;
    std::vector<Arc> arcs;

    std::vector<Node> allNodeCoords;
    std::set<Node*, NodePtrComparison> nodeLocationLookup;
    typedef std::set<Node*, NodePtrComparison>::iterator NodeLookupIter;
    unsigned bestFoundLayoutUgliness;

    long currentRandomGeneratorSeed = 1234;

    void generateGraphLayout();

    void randomizeNodes();
    unsigned getUglinessValue();
    unsigned arcProximityToNodeUgliness(unsigned arcInd, unsigned nodeInd);
    bool arcsCross(unsigned arc1Ind, unsigned arc2Ind);
    void makeSwapTrimming();
}

namespace
{
    template<typename T>
    void freeVector(std::vector<T>& v)
    {
        std::vector<T> tmp;
        v.swap(tmp);
    }
}

// -----------------
// Layout generation
// -----------------
void LayoutGenerator::randomizeNodes()
{
    tools::setSeed(currentRandomGeneratorSeed++);

    const unsigned size = LayoutGenerator::allNodeCoords.size();

    for(unsigned i = 0; i < size; ++i)
    {
        unsigned randIndex = i+unsigned(tools::randomValue()*(size-i));
        if(randIndex > i && randIndex < size)
        {
            LayoutGenerator::Node tmp =
                LayoutGenerator::allNodeCoords[randIndex];
            LayoutGenerator::allNodeCoords[randIndex] =
                LayoutGenerator::allNodeCoords[i];
            LayoutGenerator::allNodeCoords[i] = tmp;
        }
    }

    std::copy(LayoutGenerator::allNodeCoords.begin(),
              LayoutGenerator::allNodeCoords.begin() +
              LayoutGenerator::nodes.size(),
              LayoutGenerator::nodes.begin());
}

unsigned LayoutGenerator::arcProximityToNodeUgliness(unsigned arcInd,
                                                     unsigned nodeInd)
{
    const double v1x = nodes[nodeInd].x - nodes[arcs[arcInd].nodeIndex1].x;
    const double v1y = nodes[nodeInd].y - nodes[arcs[arcInd].nodeIndex1].y;
    const double v2x = nodes[nodeInd].x - nodes[arcs[arcInd].nodeIndex2].x;
    const double v2y = nodes[nodeInd].y - nodes[arcs[arcInd].nodeIndex2].y;

    const double lengthsDiff =
        std::sqrt(v1x*v1x + v1y*v1y) + std::sqrt(v2x*v2x + v2y*v2y) -
        arcs[arcInd].length;

#define MINDIST .075
#define MAXDIST .2

    if(lengthsDiff < MINDIST) return 1000;
    if(lengthsDiff > MAXDIST) return 0;

    return 1000 - unsigned(((lengthsDiff-MINDIST) / (MAXDIST-MINDIST)) * 1000);
#undef MINDIST
#undef MAXDIST
}

bool LayoutGenerator::arcsCross(unsigned arc1Ind, unsigned arc2Ind)
{
    const Node& node1 = nodes[arcs[arc1Ind].nodeIndex1];
    const Node& node2 = nodes[arcs[arc1Ind].nodeIndex2];
    const Node& node3 = nodes[arcs[arc2Ind].nodeIndex1];
    const Node& node4 = nodes[arcs[arc2Ind].nodeIndex2];
    const double p1x = node1.x;
    const double p1y = node1.y;
    const double p2x = (node1.x == node2.x) ? node2.x+.001 : node2.x;
    const double p2y = node2.y;
    const double p3x = node3.x;
    const double p3y = node3.y;
    const double p4x = (node3.x == node4.x) ? node4.x+.001 : node4.x;
    const double p4y = node4.y;

    const double k1 = (p2y-p1y)/(p2x-p1x);
    const double k2 = (p4y-p3y)/(p4x-p3x);
    const double k2k1diff = k2-k1;
    if(std::fabs(k2k1diff) < 1e-5) return false;

    const double x = (p1y - p3y + k2*p3x - k1*p1x)/k2k1diff;

    const bool xIsInsideArc1 =
        (p1x > p2x) ? (x < p1x && x > p2x) : (x > p1x && x < p2x);
    const bool xIsInsideArc2 =
        (p3x > p4x) ? (x < p3x && x > p4x) : (x > p3x && x < p4x);

    return xIsInsideArc1 && xIsInsideArc2;
}

unsigned LayoutGenerator::getUglinessValue()
{
    unsigned ugliness = 0;

    for(unsigned arcInd = 0; arcInd < arcs.size(); ++arcInd)
    {
        Arc& arc = arcs[arcInd];
        const double vx = nodes[arc.nodeIndex1].x - nodes[arc.nodeIndex2].x;
        const double vy = nodes[arc.nodeIndex1].y - nodes[arc.nodeIndex2].y;
        arc.length = std::sqrt(vx*vx + vy*vy);
    }

    for(unsigned arcInd = 0; arcInd < arcs.size(); ++arcInd)
    {
        const Arc& arc = arcs[arcInd];

        for(unsigned nodeInd = 0;
            nodeInd < nodes.size(); ++nodeInd)
        {
            if(nodeInd != arc.nodeIndex1 && nodeInd != arc.nodeIndex2)
            {
                ugliness += arcProximityToNodeUgliness(arcInd, nodeInd);
            }
        }

        for(unsigned arcInd2 = arcInd+1; arcInd2 < arcs.size(); ++arcInd2)
        {
            const Arc& arc2 = arcs[arcInd2];
            if(arc.nodeIndex1 != arc2.nodeIndex1 &&
               arc.nodeIndex1 != arc2.nodeIndex2 &&
               arc.nodeIndex2 != arc2.nodeIndex1 &&
               arc.nodeIndex2 != arc2.nodeIndex2 &&
               arcsCross(arcInd, arcInd2))
            {
                //ugliness += 100;
                ugliness += 400;
            }
        }

        const double vx = nodes[arc.nodeIndex1].x - nodes[arc.nodeIndex2].x;
        const double vy = nodes[arc.nodeIndex1].y - nodes[arc.nodeIndex2].y;
        //ugliness += unsigned(.01 + 10*std::sqrt(vx*vx + vy*vy)) - 10;
        ugliness += unsigned(.01 + 140*std::sqrt(vx*vx + vy*vy)) - 140;
    }

    return ugliness;
}

void LayoutGenerator::generateGraphLayout()
{
    //const int squareSize = int(std::ceil(std::sqrt(nodes.size())))+4;
    const int squareSize = int(std::ceil(std::sqrt(double(nodes.size()))))*2+2;
    allNodeCoords.resize(squareSize*squareSize);

    unsigned index = 0;
    for(int x = 0; x < squareSize; ++x)
    {
        for(int y = 0; y < squareSize; ++y)
        {
            allNodeCoords[index].x = x;
            allNodeCoords[index].y = y;
            ++index;
        }
    }

    std::vector<Node> bestFoundLayout;
    bestFoundLayoutUgliness = ~0U;

    const unsigned tries = 2000+(nodes.size() > 50 ? 5000 : nodes.size()*100);

    for(unsigned i = 0; i < tries; ++i)
    {
        randomizeNodes();

        if(i == 0)
        {
            bestFoundLayout = nodes;
            bestFoundLayoutUgliness = getUglinessValue();
        }
        else
        {
            unsigned ugliness = getUglinessValue();
            if(ugliness < bestFoundLayoutUgliness)
            {
                makeSwapTrimming();
                bestFoundLayout = nodes;
                //bestFoundLayoutUgliness = ugliness;
            }
        }
    }

    nodes = bestFoundLayout;
    freeVector(bestFoundLayout);
    freeVector(allNodeCoords);

    //makeSwapTrimming();
}

void LayoutGenerator::makeSwapTrimming()
{
    static const int coordChanges[][2] =
        { { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 },
          { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 },
          { -2 , 0 }, { -2 , -1 }, { -2 , -2 }, { -1 , -2 },
          { 0 , -2 }, { 1 , -2 }, { 2 , -2 }, { 2 , -1 },
          { 2 , 0 }, { 2 , 1 }, { 2 , 2 }, { 1 , 2 },
          { 0 , 2 }, { -1 , 2 }, { -2 , 2 }, { -2 , 1 }, // };

          { 0 , 10 }, { 0 , -10 },  { 10 , 0 }, { -10 , 0 },
          { 0 , 5 }, { 0 , -5 },  { 5 , 0 }, { -5 , 0 },
          { 5 , 5 }, { 5 , -5 },  { -5 , -5 }, { -5 , 5 }
        };

    for(unsigned nodeInd = 0; nodeInd < nodes.size(); ++nodeInd)
        nodeLocationLookup.insert(&nodes[nodeInd]);

    Node tempNode;
    Node* changedNode = &tempNode;

    std::vector<Node*> nodesToMove;
    std::vector<Node*> bestNodesToMove;

    bool gotBetter = true;

    for(unsigned i = 0; (i < 20) && gotBetter; ++i)
    {
        gotBetter = false;

        for(unsigned nodeInd = 0; nodeInd < nodes.size(); ++nodeInd)
        {
            const Node currentNode = nodes[nodeInd];
            unsigned bestChangeIndex = ~0U;
            unsigned bestUgliness = bestFoundLayoutUgliness;

            for(unsigned changeIndex = 0;
                changeIndex < sizeof(coordChanges)/sizeof(coordChanges[0]);
                ++changeIndex)
            {
                unsigned ugliness;
                const int changeX = coordChanges[changeIndex][0];
                const int changeY = coordChanges[changeIndex][1];

                nodesToMove.clear();
                nodesToMove.push_back(&nodes[nodeInd]);
                *changedNode = currentNode;
                while(true)
                {
                    changedNode->x += changeX;
                    changedNode->y += changeY;
                    NodeLookupIter otherNode =
                        nodeLocationLookup.find(changedNode);
                    if(otherNode != nodeLocationLookup.end())
                        nodesToMove.push_back(*otherNode);
                    else
                        break;
                }
                for(unsigned i = 0; i < nodesToMove.size(); ++i)
                {
                    nodesToMove[i]->x += changeX;
                    nodesToMove[i]->y += changeY;
                }
                ugliness = getUglinessValue();
                for(unsigned i = 0; i < nodesToMove.size(); ++i)
                {
                    nodesToMove[i]->x -= changeX;
                    nodesToMove[i]->y -= changeY;
                }

                if(ugliness < bestUgliness)
                {
                    bestChangeIndex = changeIndex;
                    bestUgliness = ugliness;
                    bestNodesToMove = nodesToMove;
                }
            }

            if(bestChangeIndex != ~0U)
            {
                gotBetter = true;
                bestFoundLayoutUgliness = bestUgliness;

                for(unsigned i = 0; i < bestNodesToMove.size(); ++i)
                {
                    nodeLocationLookup.erase(bestNodesToMove[i]);
                }
                for(unsigned i = 0; i < bestNodesToMove.size(); ++i)
                {
                    bestNodesToMove[i]->x += coordChanges[bestChangeIndex][0];
                    bestNodesToMove[i]->y += coordChanges[bestChangeIndex][1];
                    nodeLocationLookup.insert(bestNodesToMove[i]);
                }
            }
        }
    }

    nodeLocationLookup.clear();
}


namespace
{
    gboolean infoDialogExposeEvent(GtkWidget *widget, GdkEventExpose*)
    {
        gtk_dialog_response(GTK_DIALOG(widget), 0);
        return FALSE;
    }
}

// ----------------------------------------------
// LSTSEditorMainDrawingWindow::calculateLayout()
// ----------------------------------------------
void LSTSEditorMainDrawingWindow::calculateLayout()
{
    if(states.size() < 2) return;

    GtkWidget* dialog =
        gtk_message_dialog_new(parent,
                               GtkDialogFlags(GTK_DIALOG_MODAL |
                                              GTK_DIALOG_DESTROY_WITH_PARENT),
                               GTK_MESSAGE_INFO, GTK_BUTTONS_NONE,
                               "Please wait a moment.\n"
                               "Calculating some suitable layout.");
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    g_signal_connect(GTK_OBJECT(dialog), "expose-event",
                     G_CALLBACK(infoDialogExposeEvent), NULL);
    gtk_dialog_run(GTK_DIALOG(dialog));

    LayoutGenerator::nodes.resize(states.size());

    std::map<unsigned, unsigned> numberToIndex;
    unsigned index = 0;
    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        numberToIndex[iter->number()] = index++;
    }

    for(TransitionsHoldersIter iter = transitionsHolders.begin();
        iter != transitionsHolders.end(); ++iter)
    {
        LayoutGenerator::arcs.push_back(LayoutGenerator::Arc());
        LayoutGenerator::arcs.back().nodeIndex1 =
            numberToIndex[iter->getState(0)->number()];
        LayoutGenerator::arcs.back().nodeIndex2 =
            numberToIndex[iter->getState(1)->number()];
    }

    numberToIndex.clear();

    LayoutGenerator::generateGraphLayout();

    index = 0;
    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        iter->x(LayoutGenerator::nodes[index].x*
                LSTS_EDITOR_GENERATED_LAYOUT_STATES_SPACING);
        iter->y(LayoutGenerator::nodes[index].y*
                LSTS_EDITOR_GENERATED_LAYOUT_STATES_SPACING);
        ++index;
    }

    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        iter->rearrangeTransitionsToSelf();
    }

    freeVector(LayoutGenerator::nodes);
    freeVector(LayoutGenerator::arcs);

    gtk_widget_destroy(dialog);
    refreshAll();

    dataHasBeenModified = true;
}
