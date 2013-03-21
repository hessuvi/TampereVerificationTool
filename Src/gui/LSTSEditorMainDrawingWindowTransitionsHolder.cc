#include "LSTSEditorMainDrawingWindow.hh"
#include "DrawingTools.hh"

#include <cmath>


// -----------------
// TransitionsHolder
// -----------------
LSTSEditorMainDrawingWindow::TransitionsHolder::TransitionsHolder():
    selectedTransition(~0U)
{}

LSTSEditorMainDrawingWindow::TransitionsHolder::TransitionsHolder(State* s1,
                                                                  State* s2):
    selectedTransition(~0U)
{
    if(s1->number() < s2->number()) { state[0] = s1; state[1] = s2; }
    else { state[0] = s2; state[1] = s1; }
}

void LSTSEditorMainDrawingWindow::TransitionsHolder::draw
(GdkPixmap* pixmap,
 ActionNameEditor* editor,
 int origoX, int origoY) const
{
    const GdkPoint coord[2] =
        { { state[0]->x()+origoX, state[0]->y()+origoY },
          { state[1]->x()+origoX, state[1]->y()+origoY } };
    GdkPoint points[3];

    const double midx = x(), midy = y();
    double dirx = state[0]->y() - state[1]->y();
    double diry = state[1]->x() - state[0]->x();
    const double len = std::sqrt(dirx*dirx+diry*diry);
    dirx = LSTS_EDITOR_TRANSITIONS_SPACING*dirx/len;
    diry = LSTS_EDITOR_TRANSITIONS_SPACING*diry/len;

    for(unsigned i = 0; i < transitions.size(); ++i)
    {
        const unsigned startStateIndex = transitions[i].startStateIndex;
        points[0] = coord[startStateIndex];
        points[2] = coord[!startStateIndex];

        const double offset = double(i)-(transitions.size()-1)/2.0;
        const int trmidx = tools::double2int(midx+dirx*offset);
        const int trmidy = tools::double2int(midy+diry*offset);
        transitions[i].midx = trmidx;
        transitions[i].midy = trmidy;
        points[1].x = trmidx+origoX;
        points[1].y = trmidy+origoY;

        const int trminx = trmidx - LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2;
        const int trminy = trmidy - LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2;
        const int trmaxx = trmidx + LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2;
        const int trmaxy = trmidy + LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2;
        if(i==0)
        {
            minHandlerX = trminx; minHandlerY = trminy;
            maxHandlerX = trmaxx; maxHandlerY = trmaxy;
        }
        else
        {
            if(trminx < minHandlerX) minHandlerX = trminx;
            if(trminy < minHandlerY) minHandlerY = trminy;
            if(trmaxx > maxHandlerX) maxHandlerX = trmaxx;
            if(trmaxy > maxHandlerY) maxHandlerY = trmaxy;
        }

        GdkGC* gc = editor->getActionGC(transitions[i].actionNumber);
        tools::drawArrow(pixmap, gc, points, 3, LSTS_EDITOR_STATE_RADIUS);
        gdk_draw_arc(pixmap, gc, TRUE,
                     points[1].x - LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
                     points[1].y - LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
                     LSTS_EDITOR_TRANSITION_HANDLE_SIZE,
                     LSTS_EDITOR_TRANSITION_HANDLE_SIZE, 0, 64*360);
    }
}

bool LSTSEditorMainDrawingWindow::TransitionsHolder::addTransition
(const Transition& t) const
{
    for(unsigned i = 0; i < transitions.size(); ++i)
        if(transitions[i] == t) return false;
    transitions.push_back(t);
    return true;
}

unsigned LSTSEditorMainDrawingWindow::TransitionsHolder::getTransitionIndex
(int coordX, int coordY) const
{
    if(coordX < minHandlerX || coordX > maxHandlerX ||
       coordY < minHandlerY || coordY > maxHandlerY)
        return ~0U;

    for(unsigned i = 0; i < transitions.size(); ++i)
    {
        const int tx = transitions[i].midx, ty = transitions[i].midy;
        if(coordX >= tx - (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2-1) &&
           coordX <= tx + (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2+1) &&
           coordY >= ty - (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2-1) &&
           coordY <= ty + (LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2+1))
            return i;
    }
    return ~0U;
}

bool LSTSEditorMainDrawingWindow::TransitionsHolder::
coordinatesInsideTransitionHandle(int coordX, int coordY) const
{
    return getTransitionIndex(coordX, coordY) != ~0U;
}

unsigned LSTSEditorMainDrawingWindow::TransitionsHolder::selectTransition
(int clickX, int clickY) const
{
    unsigned index = getTransitionIndex(clickX, clickY);
    if(index != ~0U)
    {
        selectedTransition = index;
        return transitions[index].ID;
    }
    return 0;
}

void LSTSEditorMainDrawingWindow::TransitionsHolder::drawSelectedTransition
(GdkPixmap* pixmap, ActionNameEditor* editor, GdkGC* highlightGC,
 int origoX, int origoY) const
{
    const GdkPoint coord[2] =
        { { state[0]->x()+origoX, state[0]->y()+origoY },
          { state[1]->x()+origoX, state[1]->y()+origoY } };
    GdkPoint points[3];

    const unsigned startStateIndex =
        transitions[selectedTransition].startStateIndex;
    points[0] = coord[startStateIndex];
    points[1].x = transitions[selectedTransition].midx + origoX;
    points[1].y = transitions[selectedTransition].midy + origoY;
    points[2] = coord[!startStateIndex];

    tools::drawArrow(pixmap, highlightGC, points, 3, LSTS_EDITOR_STATE_RADIUS,
                     false);
    GdkGC* gc =
        editor->getActionGC(transitions[selectedTransition].actionNumber);
    tools::drawArrow(pixmap, gc, points, 3, 0);
    gdk_draw_arc(pixmap, gc, TRUE,
                 points[1].x - LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
                 points[1].y - LSTS_EDITOR_TRANSITION_HANDLE_SIZE/2,
                 LSTS_EDITOR_TRANSITION_HANDLE_SIZE,
                 LSTS_EDITOR_TRANSITION_HANDLE_SIZE, 0, 64*360);
}

bool LSTSEditorMainDrawingWindow::TransitionsHolder::
canChangeSelectedTransitionActionNumber(unsigned actionNumber) const
{
    Transition test(actionNumber,
                    transitions[selectedTransition].startStateIndex);
    for(unsigned i = 0; i < transitions.size(); ++i)
    {
        if(transitions[i] == test) return false;
    }
    return true;
}

void LSTSEditorMainDrawingWindow::TransitionsHolder::
changeSelectedTransitionActionNumber(unsigned actionNumber) const
{
    transitions[selectedTransition].actionNumber = actionNumber;
}

void LSTSEditorMainDrawingWindow::TransitionsHolder::
removeSelectedTransition() const
{
    transitions.erase(transitions.begin()+selectedTransition);
    if(transitions.empty()) disconnect(0);
}

void LSTSEditorMainDrawingWindow::TransitionsHolder::removeTransition
(unsigned actionNumber, EventNotify* eventListener) const
{
    for(unsigned i = 0; i < transitions.size();)
    {
        Transition& t = transitions[i];
        if(t.actionNumber == actionNumber)
        {
            if(eventListener)
                eventListener->transitionDeleted(t.ID);
            transitions.erase(transitions.begin()+i);
            continue;
        }
        if(t.actionNumber > actionNumber)
            --t.actionNumber;
        ++i;
    }
    if(transitions.empty()) disconnect(0);
}

void LSTSEditorMainDrawingWindow::TransitionsHolder::disconnect
(EventNotify* eventListener) const
{
    if(eventListener)
        for(unsigned i = 0; i < transitions.size(); ++i)
        {
            eventListener->transitionDeleted(transitions[i].ID);
        }

    state[0]->removeTransitionsHolder(*this);
    state[1]->removeTransitionsHolder(*this);
    transitions.clear();
}
