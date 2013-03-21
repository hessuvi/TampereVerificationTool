#include "LSTSEditorMainDrawingWindow.hh"
#include "DrawingTools.hh"

#include <algorithm>
#include <cmath>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// -----------
// State class
// -----------
unsigned LSTSEditorMainDrawingWindow::State::stateNumberCounter = 0;


LSTSEditorMainDrawingWindow::State::State(int ix, int iy):
    xCoord(ix), yCoord(iy), selectedTransition(0), isBeingRemoved(false)
{
    stateNumber = stateNumberCounter++;
}

void LSTSEditorMainDrawingWindow::State::resetStateNumberCounter
(unsigned value)
{
    stateNumberCounter = value;
}

void LSTSEditorMainDrawingWindow::State::calculateCoordinates
(TransitionToSelf& tr, double angle)
{
    const double s = std::sin(angle);
    const double c = std::cos(angle);

    tr.coord1X = tools::double2int(LSTS_EDITOR_TRANSITION_TO_SELF_SIZE*c -
                                   s*LSTS_EDITOR_TRANSITION_TO_SELF_WIDTH/2.0);
    tr.coord1Y = tools::double2int(LSTS_EDITOR_TRANSITION_TO_SELF_SIZE*s +
                                   c*LSTS_EDITOR_TRANSITION_TO_SELF_WIDTH/2.0);
    tr.coord2X = tools::double2int(LSTS_EDITOR_TRANSITION_TO_SELF_SIZE*c +
                                   s*LSTS_EDITOR_TRANSITION_TO_SELF_WIDTH/2.0);
    tr.coord2Y = tools::double2int(LSTS_EDITOR_TRANSITION_TO_SELF_SIZE*s -
                                   c*LSTS_EDITOR_TRANSITION_TO_SELF_WIDTH/2.0);
    tr.centerX = (tr.coord1X+tr.coord2X)/2;
    tr.centerY = (tr.coord1Y+tr.coord2Y)/2;
}


bool LSTSEditorMainDrawingWindow::State::addTransitionToSelf
(unsigned id, unsigned actionNumber, bool calculateOrientation, int angle)
{
    for(unsigned i = 0; i < transitionsToSelf.size(); ++i)
        if(transitionsToSelf[i].actionNumber == actionNumber)
            return false;

    if(!calculateOrientation)
    {
        transitionsToSelf.push_back(TransitionToSelf(id, actionNumber));
        if(angle < 0)
        {
            transitionsToSelf.back().centerX = 0;
            transitionsToSelf.back().centerY = 0;
        }
        else
        {
            calculateCoordinates(transitionsToSelf.back(), angle*M_PI/180.0);
        }
        return true;
    }

    double newTransitionAngle = 0;

    if(!transitionsHolders.empty() || !transitionsToSelf.empty())
    {
        std::vector<double> angles;
        angles.reserve(transitionsHolders.size()+transitionsToSelf.size());

        for(unsigned i = 0; i< transitionsHolders.size(); ++i)
        {
            const double vx = transitionsHolders[i]->x() - x();
            const double vy = transitionsHolders[i]->y() - y();
            angles.push_back(std::atan2(vy, vx));
        }
        for(unsigned i = 0; i < transitionsToSelf.size(); ++i)
        {
            if(transitionsToSelf[i].centerX != 0)
            {
                const double vx = transitionsToSelf[i].centerX;
                const double vy = transitionsToSelf[i].centerY;
                angles.push_back(std::atan2(vy, vx));
            }
        }

        if(angles.size() == 1)
            newTransitionAngle = angles[0]+M_PI;
        else
        {
            std::sort(angles.begin(), angles.end());

            double biggestGap = (angles[0]+2*M_PI) - angles.back();
            newTransitionAngle = (angles[0]+2*M_PI + angles.back())/2;
            for(unsigned i = 1; i < angles.size(); ++i)
            {
                const double gap = angles[i] - angles[i-1];
                if(gap > biggestGap)
                {
                    biggestGap = gap;
                    newTransitionAngle = (angles[i]+angles[i-1])/2;
                }
            }
        }
    }

    transitionsToSelf.push_back(TransitionToSelf(id, actionNumber));
    calculateCoordinates(transitionsToSelf.back(), newTransitionAngle);

    return true;
}

void LSTSEditorMainDrawingWindow::State::rearrangeTransitionsToSelf
(bool onlyThoseWithoutOrientation)
{
    std::vector<TransitionToSelf> temp(transitionsToSelf);
    transitionsToSelf.clear();
    if(onlyThoseWithoutOrientation)
    {
        for(unsigned i = 0; i < temp.size(); ++i)
        {
            if(temp[i].centerX != 0)
                transitionsToSelf.push_back(temp[i]);
        }
        for(unsigned i = 0; i < temp.size(); ++i)
        {
            if(temp[i].centerX == 0)
                addTransitionToSelf(temp[i].ID, temp[i].actionNumber);
        }
    }
    else
    {
        for(unsigned i = 0; i < temp.size(); ++i)
            addTransitionToSelf(temp[i].ID, temp[i].actionNumber);
    }
}

void LSTSEditorMainDrawingWindow::State::addTransitionsHolder
(const TransitionsHolder& holder)
{
    for(unsigned i = 0; i < transitionsHolders.size(); ++i)
        if(transitionsHolders[i] == &holder)
            return;
    transitionsHolders.push_back(&holder);
}

void LSTSEditorMainDrawingWindow::State::removeTransitionsHolder
(const TransitionsHolder& holder)
{
    if(!isBeingRemoved)
    {
        for(unsigned i = 0; i < transitionsHolders.size(); ++i)
        {
            if(transitionsHolders[i] == &holder)
            {
                transitionsHolders.erase(transitionsHolders.begin()+i);
                return;
            }
        }
    }
}

void LSTSEditorMainDrawingWindow::State::addStateProp(unsigned stateProp)
{
    for(unsigned i = 0; i < stateProps.size(); ++i)
        if(stateProps[i] == stateProp) return;

    stateProps.push_back(stateProp);
}

void LSTSEditorMainDrawingWindow::State::removeStateProp(unsigned stateProp,
                                                         bool renumber)
{
    for(unsigned i = 0; i < stateProps.size();)
    {
        if(renumber && stateProps[i] > stateProp)
        {
            --stateProps[i];
            ++i;
        }
        else if(stateProps[i] == stateProp)
        {
            stateProps.erase(stateProps.begin()+i);
            if(!renumber) return;
        }
        else ++i;
    }
}

void LSTSEditorMainDrawingWindow::State::draw(GdkPixmap* pixmap,
                                              GdkGC* gc,
                                              ActionNameEditor* editor,
                                              int origoX, int origoY,
                                              bool drawTransitions) const
{
    if(drawTransitions)
    {
        for(unsigned i = 0; i < transitionsToSelf.size(); ++i)
        {
            GdkPoint p[] = { { x()+origoX, y()+origoY },
                             { 0, 0 }, { 0, 0 },
                             { x()+origoX, y()+origoY } };

            const TransitionToSelf& tr = transitionsToSelf[i];
            p[1].x = tr.coord1X + p[0].x;
            p[1].y = tr.coord1Y + p[0].y;
            p[2].x = tr.coord2X + p[0].x;
            p[2].y = tr.coord2Y + p[0].y;
            GdkGC* gc = editor->getActionGC(tr.actionNumber);
            tools::drawArrow(pixmap, gc, p, 4, LSTS_EDITOR_STATE_RADIUS);
            gdk_draw_arc
                (pixmap, gc, TRUE,
                 tr.centerX+x()+origoX-LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
                 tr.centerY+y()+origoY-LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
                 LSTS_EDITOR_TRANSITION_HANDLE_SIZE,
                 LSTS_EDITOR_TRANSITION_HANDLE_SIZE, 0, 360*64);
        }
    }

    if(!stateProps.empty())
    {
        int startAngle = 0, angleStep = 64*360/stateProps.size();
        for(unsigned i = 0; i < stateProps.size(); ++i)
        {
            gdk_draw_arc(pixmap, editor->getStatePropGC(stateProps[i]), TRUE,
                         origoX+x()-LSTS_EDITOR_STATE_RADIUS,
                         origoY+y()-LSTS_EDITOR_STATE_RADIUS,
                         LSTS_EDITOR_STATE_RADIUS*2,
                         LSTS_EDITOR_STATE_RADIUS*2,
                         startAngle, angleStep);
            startAngle += angleStep;
        }
    }

    gdk_draw_arc(pixmap, gc, FALSE,
                 origoX+x()-LSTS_EDITOR_STATE_RADIUS,
                 origoY+y()-LSTS_EDITOR_STATE_RADIUS,
                 LSTS_EDITOR_STATE_RADIUS*2, LSTS_EDITOR_STATE_RADIUS*2,
                 0, 64*360);
}

void LSTSEditorMainDrawingWindow::State::drawSelectedTransition
(GdkPixmap* pixmap, ActionNameEditor* editor, GdkGC* highlightGC,
 int origoX, int origoY) const
{
    const TransitionToSelf& tr = transitionsToSelf[selectedTransition];
    GdkPoint p[] = { { x()+origoX, y()+origoY },
                     { x()+tr.coord1X+origoX, y()+tr.coord1Y+origoY },
                     { x()+tr.coord2X+origoX, y()+tr.coord2Y+origoY },
                     { x()+origoX, y()+origoY } };

    GdkGC* gc = editor->getActionGC(tr.actionNumber);
    if(highlightGC)
    {
        tools::drawArrow(pixmap, highlightGC, p, 4,
                         LSTS_EDITOR_STATE_RADIUS, false);
        tools::drawArrow(pixmap, gc, p, 4, 0);
    }
    else
        tools::drawArrow(pixmap, gc, p, 4, LSTS_EDITOR_STATE_RADIUS);

    gdk_draw_arc
        (pixmap, gc, TRUE,
         tr.centerX+x()+origoX-LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
         tr.centerY+y()+origoY-LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
         LSTS_EDITOR_TRANSITION_HANDLE_SIZE,
         LSTS_EDITOR_TRANSITION_HANDLE_SIZE, 0, 360*64);
}

void LSTSEditorMainDrawingWindow::State::erase(GdkPixmap* pixmap,
                                               GdkGC* gc,
                                               int origoX, int origoY) const
{
    gdk_draw_arc(pixmap, gc, TRUE,
                 origoX+x()-LSTS_EDITOR_STATE_RADIUS-2,
                 origoY+y()-LSTS_EDITOR_STATE_RADIUS-2,
                 LSTS_EDITOR_STATE_RADIUS*2+4, LSTS_EDITOR_STATE_RADIUS*2+4,
                 0, 64*360);
}

unsigned LSTSEditorMainDrawingWindow::State::getTransitionIndex
(int coordX, int coordY) const
{
    const int limit = LSTS_EDITOR_TRANSITION_TO_SELF_SIZE +
        LSTS_EDITOR_TRANSITION_HANDLE_SIZE;

    if(transitionsToSelf.size() > 1 &&
       (coordX < x()-limit || coordX > x()+limit ||
        coordY < y()-limit || coordY > y()+limit))
        return ~0U;

    for(unsigned i = 0; i < transitionsToSelf.size(); ++i)
    {
        const int cx = x() + transitionsToSelf[i].centerX;
        const int cy = y() + transitionsToSelf[i].centerY;

        if(coordX >= cx - (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2-1) &&
           coordX <= cx + (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2+1) &&
           coordY >= cy - (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2-1) &&
           coordY <= cy + (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2+1))
            return i;
    }
    return ~0U;
}

bool LSTSEditorMainDrawingWindow::State::coordinatesInsideTransitionHandle
(int coordX, int coordY) const
{
    return getTransitionIndex(coordX, coordY) != ~0U;
}

unsigned LSTSEditorMainDrawingWindow::State::selectTransition(int clickX,
                                                              int clickY)
{
    const unsigned index = getTransitionIndex(clickX, clickY);
    if(index != ~0U)
    {
        selectedTransition = index;
        return transitionsToSelf[index].ID;
    }

    return 0;
}

bool LSTSEditorMainDrawingWindow::State::
canChangeSelectedTransitionActionNumber(unsigned actionNumber) const
{
    for(unsigned i = 0; i < transitionsToSelf.size(); ++i)
        if(transitionsToSelf[i].actionNumber == actionNumber)
            return false;
    return true;
}

void LSTSEditorMainDrawingWindow::State::
changeSelectedTransitionActionNumber(unsigned actionNumber)
{
    transitionsToSelf[selectedTransition].actionNumber = actionNumber;
}

void LSTSEditorMainDrawingWindow::State::removeSelectedTransition()
{
    transitionsToSelf.erase(transitionsToSelf.begin() + selectedTransition);
}

void LSTSEditorMainDrawingWindow::State::reorientSelectedTransition(int dirx,
                                                                    int diry)
{
    if(dirx == x() && diry == y()) return;
    calculateCoordinates(transitionsToSelf[selectedTransition],
                         std::atan2(double(diry - y()), double(dirx - x())));
}

void LSTSEditorMainDrawingWindow::State::getSelectedTransitionBoundaries
(int& minx, int& miny, int& maxx, int& maxy) const
{
    const TransitionToSelf& tr = transitionsToSelf[selectedTransition];
    minx = tools::min3(x(), x()+tr.coord1X, x()+tr.coord2X);
    miny = tools::min3(y(), y()+tr.coord1Y, y()+tr.coord2Y);
    maxx = tools::max3(x(), x()+tr.coord1X, x()+tr.coord2X);
    maxy = tools::max3(y(), y()+tr.coord1Y, y()+tr.coord2Y);
}

void LSTSEditorMainDrawingWindow::State::getBoundaries
(int& minx, int& miny, int& maxx, int& maxy) const
{
    minx = x() - LSTS_EDITOR_TRANSITION_TO_SELF_SIZE -
        LSTS_EDITOR_TRANSITION_HANDLE_SIZE;
    miny = y() - LSTS_EDITOR_TRANSITION_TO_SELF_SIZE -
        LSTS_EDITOR_TRANSITION_HANDLE_SIZE;
    maxx = x() + LSTS_EDITOR_TRANSITION_TO_SELF_SIZE +
        LSTS_EDITOR_TRANSITION_HANDLE_SIZE;
    maxy = y() + LSTS_EDITOR_TRANSITION_TO_SELF_SIZE +
        LSTS_EDITOR_TRANSITION_HANDLE_SIZE;
}


void LSTSEditorMainDrawingWindow::State::removeTransition
(unsigned actionNumber, EventNotify* eventListener)
{
    for(unsigned i = 0; i < transitionsToSelf.size();)
    {
        TransitionToSelf& tr = transitionsToSelf[i];
        if(tr.actionNumber == actionNumber)
        {
            if(eventListener)
                eventListener->transitionDeleted(tr.ID);
            transitionsToSelf.erase(transitionsToSelf.begin()+i);
            continue;
        }
        if(tr.actionNumber > actionNumber)
            --tr.actionNumber;
        ++i;
    }
}

void LSTSEditorMainDrawingWindow::State::selectStateProp
(int clickX, int clickY, ActionNameEditor* editor) const
{
    if(stateProps.empty()) return;

    double angle = -std::atan2(double(clickY-y()), double(clickX-x()));
    if(angle < 0) angle += 2*M_PI;
    unsigned index = unsigned(stateProps.size()*angle/(2*M_PI));
    if(index >= stateProps.size()) index = stateProps.size()-1;
    editor->selectStatePropName(stateProps[index]);
}

void LSTSEditorMainDrawingWindow::State::removeAllTransitionsToSelf
(EventNotify* eventListener)
{
    if(eventListener)
        for(unsigned i = 0; i < transitionsToSelf.size(); ++i)
        {
            eventListener->transitionDeleted(transitionsToSelf[i].ID);
        }
    transitionsToSelf.clear();
}


// Write transitions to LSTS file
// ------------------------------
#include "LSTS_File/TransitionsAP.hh"
#include <sstream>

void LSTSEditorMainDrawingWindow::State::writeTransitions
(iTransitionsAP& pipe, const std::vector<unsigned>& stateNumberLookup)
{
    std::ostringstream angleStr;

    unsigned thisStateNumber = stateNumberLookup[number()];
    pipe.lsts_StartTransitionsFromState(thisStateNumber);

    for(unsigned i = 0; i < transitionsToSelf.size(); ++i)
    {
        int angle =
            int(360.5 +
                180.0*(std::atan2(double(transitionsToSelf[i].centerY),
                                  double(transitionsToSelf[i].centerX)))/M_PI);
        if(angle >= 360) angle -= 360;
        angleStr.str("");
        angleStr << (angle+1);
        pipe.lsts_TransitionWithLayoutInfo(thisStateNumber, thisStateNumber,
                                           transitionsToSelf[i].actionNumber,
                                           angleStr.str());
    }

    for(unsigned i = 0; i < transitionsHolders.size(); ++i)
    {
        const State* destState = transitionsHolders[i]->getState(0);
        unsigned destStateIndex = 0;
        if(destState == this)
        {
            destState = transitionsHolders[i]->getState(1);
            destStateIndex = 1;
        }
        unsigned destStateNumber = stateNumberLookup[destState->number()];

        const std::vector<Transition>& transitions =
            transitionsHolders[i]->getTransitions();

        for(unsigned j = 0; j < transitions.size(); ++j)
        {
            if(transitions[j].startStateIndex != destStateIndex)
            {
                pipe.lsts_Transition(thisStateNumber, destStateNumber,
                                     transitions[j].actionNumber);
            }
        }
    }

    pipe.lsts_EndTransitionsFromState(thisStateNumber);
}
