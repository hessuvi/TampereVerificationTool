#include "LSTSEditorMainDrawingWindow.hh"
#include "StatusBar.hh"
#include "ActionNameEditor.hh"
#include "LineDrag.hh"
#include "constants.hh"

// ---------------
// Popup menu data
// ---------------
const GtkItemFactoryEntry
LSTSEditorMainDrawingWindow::backgroundPopupMenuItems[] =
{
    { "/New state", NULL, GtkItemFactoryCallback(newStateCallBack), 0,
      NULL, NULL },
    { "/Change action of selected transition", NULL,
      GtkItemFactoryCallback(changeActionOfTransitionCallBack), 0,NULL,NULL },
    { "/Remove selected transition", NULL,
      GtkItemFactoryCallback(removeTransitionCallBack), 0, NULL, NULL },
    { "/Unselect transition", NULL,
      GtkItemFactoryCallback(unselectTransitionCallBack), 0, NULL, NULL }
};

const GtkItemFactoryEntry
LSTSEditorMainDrawingWindow::backgroundPopupMenuILangItems[] =
{
    { "/Edit selected transition properties", NULL,
      GtkItemFactoryCallback(editTransitionPropertiesCallBack), 0,NULL,NULL }
};

const GtkItemFactoryEntry LSTSEditorMainDrawingWindow::statePopupMenuItems[] =
{
    { "/Create transition (ctrl+click)", NULL,
      GtkItemFactoryCallback(createNewTransitionCallBack), 0, NULL, NULL },
    { "/Add selected stateprop", NULL,
      GtkItemFactoryCallback(addStatePropCallBack), 0, NULL, NULL },
    { "/Remove selected stateprop", NULL,
      GtkItemFactoryCallback(addStatePropCallBack), 1, NULL, NULL },
    { "/Make initial state", NULL,
      GtkItemFactoryCallback(makeInitialStateCallBack), 0, NULL, NULL },
    { "/Rearrange transitions to self", NULL,
      GtkItemFactoryCallback(rearrangeTransitionsToSelfCallBack),0,NULL,NULL},
    { "/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/Remove state", NULL, GtkItemFactoryCallback(removeStateCallBack), 0,
      NULL, NULL },
};


unsigned LSTSEditorMainDrawingWindow::transitionIdCounter = 1;


// --------------------------
// Constructor and destructor
// --------------------------
LSTSEditorMainDrawingWindow::LSTSEditorMainDrawingWindow(bool iLangEditor):
    thisIsAnILangEditor(iLangEditor),
    parent(0), statusBar(0),
    drawingArea(0), pixmap(0),
    drawingGC(0), highlightedTransitionGC(0), colorMap(0),
    pixmapWidth(0), pixmapHeight(0), origoX(0), origoY(0),
    clickedX(0), clickedY(0),
    eventListener(0),
    initialStateNumber(0), clickedStateIter(states.end()),
    selectedActionNumber(~0U), selectedStatePropNumber(~0U),
    stateBeingDragged(0),
    transitionCreationStartState(0), transitionCreationLine(0),
    selectedTransitionsHolder(0), stateWithSelectedTransition(0),
    selectedTransitionIsBeingDragged(false),
    dataHasBeenModified(false)
{
}

LSTSEditorMainDrawingWindow::~LSTSEditorMainDrawingWindow()
{
    if(pixmap) g_object_unref(pixmap);
    if(drawingGC) g_object_unref(drawingGC);
    if(highlightedTransitionGC) g_object_unref(highlightedTransitionGC);
    if(colorMap) g_object_unref(colorMap);
    if(transitionCreationLine) delete transitionCreationLine;
}


// -------------------
// Main initialization
// -------------------
GtkWidget* LSTSEditorMainDrawingWindow::initialize
(GtkWindow* parentWindow, ActionNameEditor* editor, StatusBar* mainStatusBar)
{
    // Initialize pointers:
    parent = parentWindow;
    actionNameEditor = editor;
    statusBar = mainStatusBar;

    actionNameEditor->registerAttributeChangeCallBack(this);

    // Initialize drawing elements:
    drawingArea = gtk_drawing_area_new();
    drawingGC = gdk_gc_new(gtk_widget_get_root_window(GTK_WIDGET(parent)));
    highlightedTransitionGC =
        gdk_gc_new(gtk_widget_get_root_window(GTK_WIDGET(parent)));
    colorMap = gdk_colormap_get_system();

    gdk_gc_set_line_attributes(drawingGC, 2, GDK_LINE_SOLID,
                               GDK_CAP_NOT_LAST, GDK_JOIN_MITER);

    GdkColor hlcolor = { 0, LSTS_EDITOR_TRANSITION_HIGHLIGHT_COLOR };
    gdk_colormap_alloc_color(colorMap, &hlcolor, FALSE, TRUE);
    gdk_gc_set_foreground(highlightedTransitionGC, &hlcolor);
    gdk_gc_set_line_attributes(highlightedTransitionGC, 6, GDK_LINE_SOLID,
                               GDK_CAP_NOT_LAST, GDK_JOIN_MITER);

    // Create scrolled window:
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),
                                          drawingArea);

    // Set up event handlers:
    gtk_widget_set_events(drawingArea,
                          GDK_EXPOSURE_MASK |
                          GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_POINTER_MOTION_MASK |
                          GDK_POINTER_MOTION_HINT_MASK |
                          GDK_LEAVE_NOTIFY_MASK);

    g_signal_connect(GTK_OBJECT(drawingArea), "configure_event",
                     G_CALLBACK(configureEvent), this);
    g_signal_connect(GTK_OBJECT(drawingArea), "expose_event",
                     G_CALLBACK(exposeEvent), this);

    g_signal_connect(GTK_OBJECT(drawingArea), "button_press_event",
                     G_CALLBACK(mouseButtonPressEvent), this);
    g_signal_connect(GTK_OBJECT(drawingArea), "button_release_event",
                     G_CALLBACK(mouseButtonReleaseEvent), this);
    g_signal_connect(GTK_OBJECT(drawingArea), "motion_notify_event",
                     G_CALLBACK(motionEvent), this);
    g_signal_connect(GTK_OBJECT(drawingArea), "leave_notify_event",
                     G_CALLBACK(leaveEvent), this);

    initializeLSTSData();

    return scrolled;
}

// -----------------
// Delete everything
// -----------------
void LSTSEditorMainDrawingWindow::deleteEverything()
{
    states.clear();
    transitionsHolders.clear();
    selectedActionNumber = ~0U;
    selectedStatePropNumber = ~0U;
    stateBeingDragged = 0;
    transitionCreationStartState = 0;
    if(transitionCreationLine)
    {
        delete transitionCreationLine;
        transitionCreationLine = 0;
    }
    selectedTransitionsHolder = 0;
    stateWithSelectedTransition = 0;
    selectedTransitionIsBeingDragged = false;

    initializeLSTSData();

    refreshAll();

    actionNameEditor->deleteAll();
}

// ----------------------
// Main drawing functions
// ----------------------
void LSTSEditorMainDrawingWindow::setDrawingColor(unsigned red, unsigned green,
                                                  unsigned blue)
{
    GdkColor color = { 0, red, green, blue };
    gdk_colormap_alloc_color(colorMap, &color, FALSE, TRUE);
    gdk_gc_set_foreground(drawingGC, &color);
}

void LSTSEditorMainDrawingWindow::recalculatePixmapSize()
{
    int minX = states.begin()->x(), maxX = states.begin()->x();
    int minY = states.begin()->y(), maxY = states.begin()->y();
    StatesIterator iter = states.begin();
    for(++iter; iter != states.end(); ++iter)
    {
        const int x = iter->x(), y = iter->y();
        if(x < minX) minX = x;
        else if(x > maxX) maxX = x;
        if(y < minY) minY = y;
        else if(y > maxY) maxY = y;
    }
    minX -= LSTS_EDITOR_MARGINS_SIZE;
    maxX += LSTS_EDITOR_MARGINS_SIZE;
    minY -= LSTS_EDITOR_MARGINS_SIZE;
    maxY += LSTS_EDITOR_MARGINS_SIZE;

    const int width = maxX - minX;
    const int height = maxY - minY;

    if(width != pixmapWidth || height != pixmapHeight)
    {
        if(pixmap) g_object_unref(pixmap);

        pixmap = gdk_pixmap_new(drawingArea->window, width, height, -1);
        gtk_widget_set_size_request(drawingArea, width, height);
        pixmapWidth = width;
        pixmapHeight = height;
    }

    origoX = -minX;
    origoY = -minY;
}

void LSTSEditorMainDrawingWindow::redrawPixmap()
{
    gdk_draw_rectangle(pixmap, drawingArea->style->white_gc, TRUE,
                       0, 0, pixmapWidth, pixmapHeight);

    for(TransitionsHoldersIter iter = transitionsHolders.begin();
        iter != transitionsHolders.end(); ++iter)
    {
        iter->draw(pixmap, actionNameEditor, origoX, origoY);
    }

    if(selectedTransitionsHolder)
        selectedTransitionsHolder->drawSelectedTransition
            (pixmap, actionNameEditor, highlightedTransitionGC,
             origoX, origoY);
    else if(stateWithSelectedTransition)
        stateWithSelectedTransition->drawSelectedTransition
            (pixmap, actionNameEditor, highlightedTransitionGC,
             origoX, origoY);

    setDrawingColor(MAIN_FG_COLOR);
    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        if(iter->number() == initialStateNumber)
        {
            setDrawingColor(LSTS_EDITOR_INITIAL_STATE_BORDER_COLOR);
            iter->draw(pixmap, drawingGC, actionNameEditor, origoX, origoY);
            setDrawingColor(MAIN_FG_COLOR);
        }
        else
            iter->draw(pixmap, drawingGC, actionNameEditor, origoX, origoY);
    }
}

void LSTSEditorMainDrawingWindow::clearArea(int x, int y,
                                            int width, int height)
{
    gdk_draw_rectangle(pixmap, drawingArea->style->white_gc, TRUE,
                       x, y, width, height);
}

void LSTSEditorMainDrawingWindow::refreshArea(int x, int y,
                                              int width, int height)
{
    if(!pixmap) return;

    if(width == 0 && height == 0)
    {
        width = pixmapWidth;
        height = pixmapHeight;
    }
    gdk_draw_pixmap(drawingArea->window,
                    drawingArea->style->fg_gc[GTK_WIDGET_STATE(drawingArea)],
                    pixmap,
                    x, y, x, y, width, height);
}

void LSTSEditorMainDrawingWindow::refreshAll()
{
    recalculatePixmapSize();
    redrawPixmap();
    refreshArea();
}


// --------------------
// Initialize LSTS data
// --------------------
void LSTSEditorMainDrawingWindow::initializeLSTSData()
{
    State::resetStateNumberCounter();
    initialStateNumber = 0;
    states.push_back(State());
    dataHasBeenModified = true;
    if(eventListener)
        eventListener->stateCreated(states.back().number(), true);
}

// --------------
// Event handlers
// --------------
gboolean LSTSEditorMainDrawingWindow::configureEvent
(GtkWidget*, GdkEventConfigure*, LSTSEditorMainDrawingWindow* instance)
{
    if(!instance->backgroundPopupMenu.hasBeenCreated())
    {
        PopupMenu::MenuItems items(TableRange(backgroundPopupMenuItems));
        if(instance->thisIsAnILangEditor)
            items.insert(items.end(),
                         TableRange(backgroundPopupMenuILangItems));
        instance->backgroundPopupMenu.createMenu("<LSTSEditorBGMenu>",
                                                 items, instance);
    }
    if(!instance->statePopupMenu.hasBeenCreated())
    {
        PopupMenu::MenuItems items(TableRange(statePopupMenuItems));
        instance->statePopupMenu.createMenu("<LSTSEditorStateMenu>",
                                            items, instance);
    }

    //instance->refreshAll();
    instance->recalculatePixmapSize();
    instance->redrawPixmap();

    return FALSE;
}

gboolean LSTSEditorMainDrawingWindow::exposeEvent
(GtkWidget*, GdkEventExpose* event, LSTSEditorMainDrawingWindow* instance)
{
    instance->refreshArea(event->area.x, event->area.y,
                          event->area.width, event->area.height);

    return FALSE;
}

// Mouse click
// -----------
gboolean LSTSEditorMainDrawingWindow::mouseButtonPressEvent
(GtkWidget*, GdkEventButton* press, LSTSEditorMainDrawingWindow* instance)
{
    instance->clickedX = int(press->x) - instance->origoX;
    instance->clickedY = int(press->y) - instance->origoY;

    // If creating a new transition and right-click happens:
    if(instance->transitionCreationStartState && press->button == 3)
    {
        instance->cancelNewTransitionCreation();
        instance->redrawPixmap();
        instance->refreshArea();
        return TRUE;
    }

    // Check if a state was clicked:
    // ----------------------------
    for(StatesIterator iter = instance->states.begin();
        iter != instance->states.end(); ++iter)
    {
        if(iter->coordinatesInsideState(instance->clickedX,
                                        instance->clickedY))
        {
            instance->clickedStateIter = iter;

            // If creating a new transition:
            if(instance->transitionCreationStartState && press->button == 1)
            {
                delete instance->transitionCreationLine;
                instance->transitionCreationLine = 0;
                instance->createNewTransition
                    (instance->transitionCreationStartState, &(*iter));
                instance->transitionCreationStartState = 0;
            }

            // If ctrl+click, start creating a new transition:
            else if(press->state & GDK_CONTROL_MASK)
            {
                if(instance->selectedActionNumber != ~0U)
                    instance->startCreatingNewTransition(iter);
            }

            // If left-click, start dragging:
            else if(press->button == 1)
            {
                instance->stateBeingDragged = &(*iter);
                instance->draggedStateOriginalX =
                    instance->stateBeingDragged->x();
                instance->draggedStateOriginalY =
                    instance->stateBeingDragged->y();
                if(instance->stateBeingDragged->number() ==
                   instance->initialStateNumber)
                    instance->setDrawingColor
                        (LSTS_EDITOR_INITIAL_STATE_BORDER_COLOR);
                else
                    instance->setDrawingColor(MAIN_FG_COLOR);
            }

            // If right-click, open popup menu:
            else if(press->button == 3)
            {
                instance->updateStateMenuSensitivity();
                instance->statePopupMenu.popup(int(press->x_root),
                                               int(press->y_root),
                                               press->button, press->time);
            }

            return TRUE;
        }
    }

    // Check if a transition handle was clicked:
    // ----------------------------------------
    bool transitionWasSelected = false;
    for(TransitionsHoldersIter iter = instance->transitionsHolders.begin();
        iter != instance->transitionsHolders.end(); ++iter)
    {
        transitionWasSelected =
            iter->selectTransition(instance->clickedX, instance->clickedY);
        if(transitionWasSelected)
        {
            instance->selectedTransitionsHolder = &(*iter);
            instance->stateWithSelectedTransition = 0;
            instance->redrawPixmap();
            instance->refreshArea();
            instance->actionNameEditor->selectActionName
                (iter->getSelectedTransition().actionNumber);
            if(instance->eventListener)
                instance->eventListener->transitionSelected
                    (iter->getSelectedTransition().ID);

            // No "return TRUE;" because even if the selection is made,
            // the background popup menu should still open:
            break;
        }
    }

    if(!transitionWasSelected)
    {
        for(StatesIterator iter = instance->states.begin();
            iter != instance->states.end(); ++iter)
        {
            if(iter->selectTransition(instance->clickedX, instance->clickedY))
            {
                instance->selectedTransitionsHolder = 0;
                instance->stateWithSelectedTransition = &(*iter);
                instance->redrawPixmap();
                instance->refreshArea();
                instance->actionNameEditor->selectActionName
                    (iter->getSelectedTransitionActionNumber());

                if(press->button == 1)
                    instance->selectedTransitionIsBeingDragged = true;

                break;
            }
        }
    }

    // The background was clicked:
    // --------------------------
    if(press->button == 3)
    {
        instance->updateBackgroundMenuSensitivity();
        instance->backgroundPopupMenu.popup(int(press->x_root),
                                            int(press->y_root),
                                            press->button, press->time);
    }

    return TRUE;
}

// Mouse release
// -------------
gboolean LSTSEditorMainDrawingWindow::mouseButtonReleaseEvent
(GtkWidget*, GdkEventButton* press, LSTSEditorMainDrawingWindow* instance)
{
    if(instance->stateBeingDragged)
    {
        if(press->button == 1 &&
           instance->stateBeingDragged->x() == instance->draggedStateOriginalX
           &&
           instance->stateBeingDragged->y() == instance->draggedStateOriginalY)
        {
            instance->stateBeingDragged->
                selectStateProp(instance->clickedX, instance->clickedY,
                                instance->actionNameEditor);
            if(instance->eventListener)
                instance->eventListener->stateSelected
                    (instance->stateBeingDragged->number());
        }

        if(instance->stateBeingDragged->x() != instance->draggedStateOriginalX
           ||
           instance->stateBeingDragged->y() != instance->draggedStateOriginalY)
        {
            instance->dataHasBeenModified = true;
        }

        instance->stateBeingDragged = 0;
        instance->refreshAll();
    }

    if(instance->selectedTransitionIsBeingDragged)
    {
        instance->selectedTransitionIsBeingDragged = false;
        instance->redrawPixmap();
        instance->refreshArea();
        instance->dataHasBeenModified = true;
    }

    return TRUE;
}

// Mouse motion
// ------------
gboolean LSTSEditorMainDrawingWindow::motionEvent
(GtkWidget*, GdkEventMotion* event, LSTSEditorMainDrawingWindow* instance)
{
    int x, y;
    GdkModifierType buttonState;

    // Get hover coordinates:
    // ---------------------
    if(event->is_hint)
        gdk_window_get_pointer(event->window, &x, &y, &buttonState);
    else
    {
        x = int(event->x);
        y = int(event->y);
    }

    // If dragging a state:
    // -------------------
    State* state = instance->stateBeingDragged;
    if(state)
    {
        state->erase(instance->pixmap, instance->drawingArea->style->white_gc,
                     instance->origoX, instance->origoY);
        int minx = state->x() - LSTS_EDITOR_STATE_RADIUS - 1+instance->origoX;
        int maxx = state->x() + LSTS_EDITOR_STATE_RADIUS + 1+instance->origoX;
        int miny = state->y() - LSTS_EDITOR_STATE_RADIUS - 1+instance->origoY;
        int maxy = state->y() + LSTS_EDITOR_STATE_RADIUS + 1+instance->origoY;

        x -= instance->origoX;
        y -= instance->origoY;
        state->x(instance->draggedStateOriginalX + x - instance->clickedX);
        state->y(instance->draggedStateOriginalY + y - instance->clickedY);
        state->draw(instance->pixmap, instance->drawingGC,
                    instance->actionNameEditor,
                    instance->origoX, instance->origoY, false);

        int minx2 = state->x() - LSTS_EDITOR_STATE_RADIUS -1+instance->origoX;
        int maxx2 = state->x() + LSTS_EDITOR_STATE_RADIUS +1+instance->origoX;
        int miny2 = state->y() - LSTS_EDITOR_STATE_RADIUS -1+instance->origoY;
        int maxy2 = state->y() + LSTS_EDITOR_STATE_RADIUS +1+instance->origoY;

        if(minx2 < minx) minx = minx2;
        if(maxx2 > maxx) maxx = maxx2;
        if(miny2 < miny) miny = miny2;
        if(maxy2 > maxy) maxy = maxy2;

        instance->refreshArea(minx, miny, maxx-minx+1, maxy-miny+1);
        return TRUE;
    }

    // If creating a new transition:
    // ----------------------------
    if(instance->transitionCreationStartState)
    {
        instance->transitionCreationLine->lineTo(x, y);
        return TRUE;
    }

    // If dragging a transition:
    // ------------------------
    if(instance->selectedTransitionIsBeingDragged)
    {
        int minx, miny, maxx, maxy;
        instance->stateWithSelectedTransition->
            getBoundaries(minx, miny, maxx, maxy);
        instance->clearArea(minx+instance->origoX, miny+instance->origoY,
                            maxx-minx+1, maxy-miny+1);

        instance->stateWithSelectedTransition->
            getSelectedTransitionBoundaries(minx, miny, maxx, maxy);

        instance->stateWithSelectedTransition->
            reorientSelectedTransition(x - instance->origoX,
                                       y - instance->origoY);
        instance->stateWithSelectedTransition->
            drawSelectedTransition(instance->pixmap,
                                   instance->actionNameEditor, 0,
                                   instance->origoX, instance->origoY);
        instance->stateWithSelectedTransition->
            draw(instance->pixmap, instance->drawingGC,
                 instance->actionNameEditor,
                 instance->origoX, instance->origoY);

        int minx2, miny2, maxx2, maxy2;
        instance->stateWithSelectedTransition->
            getSelectedTransitionBoundaries(minx2, miny2, maxx2, maxy2);
        minx = std::min(minx, minx2); miny = std::min(miny, miny2);
        maxx = std::max(maxx, maxx2); maxy = std::max(maxy, maxy2);
        instance->refreshArea(minx+instance->origoX-2, miny+instance->origoY-2,
                              maxx-minx+5, maxy-miny+5);
        return TRUE;
    }

    // If nothing is being done, update status bar help:
    // ------------------------------------------------
    x -= instance->origoX;
    y -= instance->origoY;
    for(StatesIterator iter = instance->states.begin();
        iter != instance->states.end(); ++iter)
    {
        if(iter->coordinatesInsideState(x, y))
        {
            instance->statusBar->setText(LSTS_EDITOR_STATE_HOVER_TEXT);
            return TRUE;
        }
        if(iter->coordinatesInsideTransitionHandle(x, y))
        {
            instance->statusBar->setText
                (LSTS_EDITOR_TRANSITION_TO_SELF_HOVER_TEXT);
            return TRUE;
        }
    }
    for(TransitionsHoldersIter iter = instance->transitionsHolders.begin();
        iter != instance->transitionsHolders.end(); ++iter)
    {
        if(iter->coordinatesInsideTransitionHandle(x, y))
        {
            instance->statusBar->setText(LSTS_EDITOR_TRANSITION_HOVER_TEXT);
            return TRUE;
        }
    }
    instance->statusBar->setText(LSTS_EDITOR_BACKGROUND_HOVER_TEXT);

    return TRUE;
}

gboolean LSTSEditorMainDrawingWindow::leaveEvent
(GtkWidget*, GdkEventCrossing*, LSTSEditorMainDrawingWindow* instance)
{
    instance->statusBar->clear();
    return TRUE;
}


// -----------------------
// Menu sensitivity update
// -----------------------
void LSTSEditorMainDrawingWindow::updateBackgroundMenuSensitivity()
{
    backgroundPopupMenu.setSensitivity
        ("/Change action of selected transition",
         (selectedTransitionsHolder &&
          selectedTransitionsHolder->
          getSelectedTransition().actionNumber != selectedActionNumber &&
          selectedTransitionsHolder->
          canChangeSelectedTransitionActionNumber(selectedActionNumber)) ||
         (stateWithSelectedTransition &&
          stateWithSelectedTransition->
          canChangeSelectedTransitionActionNumber(selectedActionNumber)));
    backgroundPopupMenu.setSensitivity("/Remove selected transition",
                                       selectedTransitionsHolder ||
                                       stateWithSelectedTransition);
    backgroundPopupMenu.setSensitivity("/Unselect transition",
                                       selectedTransitionsHolder ||
                                       stateWithSelectedTransition);
    if(thisIsAnILangEditor)
        backgroundPopupMenu.setSensitivity
            ("/Edit selected transition properties",
             selectedTransitionsHolder || stateWithSelectedTransition);
}

void LSTSEditorMainDrawingWindow::updateStateMenuSensitivity()
{
    bool notInitialState = (clickedStateIter->number() != initialStateNumber);

    statePopupMenu.setSensitivity("/Add selected stateprop",
                                  selectedStatePropNumber != ~0U &&
                                  !clickedStateIter->
                                  hasStateProp(selectedStatePropNumber));
    statePopupMenu.setSensitivity("/Remove selected stateprop",
                                  clickedStateIter->
                                  hasStateProp(selectedStatePropNumber));
    statePopupMenu.setSensitivity("/Make initial state", notInitialState);
    statePopupMenu.setSensitivity("/Remove state", notInitialState);
    statePopupMenu.setSensitivity("/Create transition (ctrl+click)",
                                  selectedActionNumber != ~0U);
    statePopupMenu.setSensitivity("/Rearrange transitions to self",
                                  clickedStateIter->hasTransitions());
}

// --------------------
// Popup menu callbacks
// --------------------
void LSTSEditorMainDrawingWindow::newStateCallBack(gpointer ptr, guint,
                                                   GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    instance->states.push_back(State(instance->clickedX, instance->clickedY));
    instance->refreshAll();
    instance->dataHasBeenModified = true;

    if(instance->eventListener)
        instance->eventListener->
            stateCreated(instance->states.back().number(), false);
}

void LSTSEditorMainDrawingWindow::changeActionOfTransitionCallBack
(gpointer ptr, guint, GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    if(instance->selectedTransitionsHolder)
        instance->selectedTransitionsHolder->
            changeSelectedTransitionActionNumber
            (instance->selectedActionNumber);
    else if(instance->stateWithSelectedTransition)
        instance->stateWithSelectedTransition->
            changeSelectedTransitionActionNumber
            (instance->selectedActionNumber);
    instance->redrawPixmap();
    instance->refreshArea();
    instance->dataHasBeenModified = true;

    if(instance->eventListener)
    {
        unsigned trID = instance->getSelectedTransitionID();
        if(trID)
            instance->eventListener->transitionActionChanged
                (trID, instance->selectedActionNumber);
    }
}

void LSTSEditorMainDrawingWindow::removeTransitionCallBack(gpointer ptr,
                                                           guint, GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    if(instance->eventListener)
    {
        unsigned trID = instance->getSelectedTransitionID();
        if(trID) instance->eventListener->transitionDeleted(trID);
    }

    if(instance->selectedTransitionsHolder)
        instance->selectedTransitionsHolder->removeSelectedTransition();
    else if(instance->stateWithSelectedTransition)
        instance->stateWithSelectedTransition->removeSelectedTransition();
    instance->selectedTransitionsHolder = 0;
    instance->stateWithSelectedTransition = 0;
    instance->redrawPixmap();
    instance->refreshArea();
    instance->dataHasBeenModified = true;
}

void LSTSEditorMainDrawingWindow::unselectTransitionCallBack(gpointer ptr,
                                                             guint, GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    if(instance->eventListener)
    {
        unsigned trID = instance->getSelectedTransitionID();
        if(trID) instance->eventListener->transitionUnselected(trID);
    }

    instance->selectedTransitionsHolder = 0;
    instance->stateWithSelectedTransition = 0;
    instance->redrawPixmap();
    instance->refreshArea();
}

void LSTSEditorMainDrawingWindow::editTransitionPropertiesCallBack
(gpointer ptr, guint, GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    if(instance->eventListener)
    {
        unsigned trID = instance->getSelectedTransitionID();
        if(trID) instance->eventListener->editTransitionProperties(trID);
    }
}


void LSTSEditorMainDrawingWindow::createNewTransitionCallBack(gpointer ptr,
                                                              guint,
                                                              GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    instance->startCreatingNewTransition(instance->clickedStateIter);
}

void LSTSEditorMainDrawingWindow::removeStateCallBack(gpointer ptr, guint,
                                                      GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    GtkWidget* dialog = gtk_message_dialog_new
        (instance->parent, GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
         "Are you sure you want to remove this\n"
         "state and all of its transitions?");
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm removal");
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if(result == GTK_RESPONSE_YES)
    {
        instance->removeState(instance->clickedStateIter);
    }
}

void LSTSEditorMainDrawingWindow::addStatePropCallBack(gpointer ptr,
                                                       guint remove,
                                                       GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    if(remove)
    {
        if(instance->eventListener)
            instance->eventListener->statePropRemovedFromState
                (instance->clickedStateIter->number(),
                 instance->selectedStatePropNumber);
        instance->clickedStateIter->removeStateProp
            (instance->selectedStatePropNumber, false);
    }
    else
    {
        if(instance->eventListener)
            instance->eventListener->statePropAddedToState
                (instance->clickedStateIter->number(),
                 instance->selectedStatePropNumber,
                 instance->actionNameEditor->getStatePropName
                 (instance->selectedStatePropNumber));
        instance->clickedStateIter->addStateProp
            (instance->selectedStatePropNumber);
    }
    instance->redrawPixmap();
    instance->refreshArea();
    instance->dataHasBeenModified = true;
}

void LSTSEditorMainDrawingWindow::makeInitialStateCallBack(gpointer ptr,
                                                           guint, GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    if(instance->eventListener)
        instance->eventListener->stateWasMadeInitial
            (instance->clickedStateIter->number());

    instance->initialStateNumber = instance->clickedStateIter->number();
    instance->redrawPixmap();
    instance->refreshArea();
    instance->dataHasBeenModified = true;
}

void LSTSEditorMainDrawingWindow::rearrangeTransitionsToSelfCallBack
(gpointer ptr, guint, GtkWidget*)
{
    LSTSEditorMainDrawingWindow* instance =
        reinterpret_cast<LSTSEditorMainDrawingWindow*>(ptr);

    instance->clickedStateIter->rearrangeTransitionsToSelf();
    instance->redrawPixmap();
    instance->refreshArea();
    instance->dataHasBeenModified = true;
}



// ------------------------
// Graph handling functions
// ------------------------
// Remove state
// ------------
void LSTSEditorMainDrawingWindow::removeState(StatesIterator iter)
{
    iter->removeAllTransitionsHolders(transitionsHolders, eventListener);
    iter->removeAllTransitionsToSelf(eventListener);

    if(eventListener)
        eventListener->stateDeleted(iter->number());

    states.erase(iter);
    selectedTransitionsHolder = 0;
    stateWithSelectedTransition = 0;

    refreshAll();
    dataHasBeenModified = true;
}

// Create new transition
// ---------------------
void LSTSEditorMainDrawingWindow::startCreatingNewTransition(StatesIterator
                                                             iter)
{
    transitionCreationStartState = &(*iter);
    setDrawingColor(MAIN_FG_COLOR);
    transitionCreationLine =
        new LineDrag(drawingArea, pixmap,
                     transitionCreationStartState->x() + origoX,
                     transitionCreationStartState->y() + origoY);
}

void LSTSEditorMainDrawingWindow::cancelNewTransitionCreation()
{
    delete transitionCreationLine;
    transitionCreationLine = 0;
    transitionCreationStartState = 0;
}

void LSTSEditorMainDrawingWindow::createNewTransition(State* startState,
                                                      State* endState,
                                                      bool refreshWindow)
{
    bool transitionWasAdded = false;

    if(startState == endState)
    {
        transitionWasAdded =
            startState->addTransitionToSelf(transitionIdCounter++,
                                            selectedActionNumber);
    }
    else
    {
        TransitionsHoldersIter holder =
            transitionsHolders.insert
            (TransitionsHolder(startState, endState)).first;

        transitionWasAdded =
            holder->addTransition(Transition(transitionIdCounter++,
                                             selectedActionNumber,
                                             holder->getState(1)==startState));
        startState->addTransitionsHolder(*holder);
        endState->addTransitionsHolder(*holder);
    }

    if(refreshWindow)
    {
        redrawPixmap();
        refreshArea();
    }
    if(transitionWasAdded)
        dataHasBeenModified = true;

    if(transitionWasAdded && eventListener)
        eventListener->transitionCreated
            (transitionIdCounter-1,
             startState->number(), endState->number(), selectedActionNumber,
             actionNameEditor->getActionName(selectedActionNumber));
}

// Action name editor callbacks
// ----------------------------
void LSTSEditorMainDrawingWindow::actionNameSelect(unsigned actionNumber)
{
    selectedActionNumber = actionNumber;

    if(transitionCreationStartState && actionNumber == ~0U)
    {
        cancelNewTransitionCreation();
        redrawPixmap();
        refreshArea();
    }
}

void LSTSEditorMainDrawingWindow::actionNameAttributesChanged(unsigned)
{
    redrawPixmap();
    refreshArea();
}

void LSTSEditorMainDrawingWindow::actionNameRemoved(unsigned actionNumber)
{
    for(TransitionsHoldersIter iter = transitionsHolders.begin();
        iter != transitionsHolders.end(); ++iter)
    {
        iter->removeTransition(actionNumber, eventListener);
    }
    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        iter->removeTransition(actionNumber, eventListener);
    }

    if(eventListener)
        eventListener->actionNameDeleted(actionNumber);

    selectedTransitionsHolder = 0;
    stateWithSelectedTransition = 0;
    redrawPixmap();
    refreshArea();
}

void LSTSEditorMainDrawingWindow::statePropNameSelect(unsigned spNumber)
{
    selectedStatePropNumber = spNumber;
}

void LSTSEditorMainDrawingWindow::statePropNameChanged(unsigned spNumber)
{
    if(eventListener)
        eventListener->statePropRenamed(spNumber,
                                        actionNameEditor->getStatePropName
                                        (spNumber));
}

void LSTSEditorMainDrawingWindow::statePropNameAttributesChanged(unsigned)
{
    redrawPixmap();
    refreshArea();
}

void LSTSEditorMainDrawingWindow::statePropNameRemoved(unsigned spNumber)
{
    if(eventListener)
        eventListener->statePropRemoved(spNumber);

    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
        iter->removeStateProp(spNumber, true);
    redrawPixmap();
    refreshArea();
}
