#include "MainDrawingWindow.hh"
#include "InputLanguageSource.hh"
#include "LSTSEditorSource.hh"
#include "OperationSBRed.hh"
#include "OperationParComp.hh"
#include "StatusBar.hh"
#include "LineDrag.hh"
#include "constants.hh"

#include <iostream>
#include <cstdlib>



//===========================================================================
// MainDrawingWindow
//===========================================================================

namespace
{
    enum OperationID
    {
        INPUTLANGUAGESOURCE, LSTSEDITORSOURCE,
        PARCOMP, SBREDUCTION, LSTS2AG
    };
}


// ---------------
// Popup menu data
// ---------------
const GtkItemFactoryEntry MainDrawingWindow::sourceColumnPopupMenuItems[] =
{
    { "/New input language source...", NULL,
      GtkItemFactoryCallback(newSourceProcessCallBack), INPUTLANGUAGESOURCE,
      NULL, NULL },
    { "/New graphical lsts editor...", NULL,
      GtkItemFactoryCallback(newSourceProcessCallBack), LSTSEDITORSOURCE,
      NULL, NULL },

    { "/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/Unremove item", NULL, GtkItemFactoryCallback(unremoveItemCallBack), 0,
      NULL, NULL }
};

const GtkItemFactoryEntry MainDrawingWindow::operationColumnPopupMenuItems[] =
{
    { "/NewOperation/Parallel composition", NULL,
      GtkItemFactoryCallback(newOperationCallBack), PARCOMP, NULL, NULL },
    { "/NewOperation/SB reduction", NULL,
      GtkItemFactoryCallback(newOperationCallBack), SBREDUCTION, NULL, NULL },
    { "/NewOperation/LSTS to AG", NULL,
      GtkItemFactoryCallback(newOperationCallBack), LSTS2AG, NULL, NULL },

    { "/Insert column", NULL, GtkItemFactoryCallback(insertColumnCallBack), 4,
      NULL, NULL },
    { "/Remove column", NULL, GtkItemFactoryCallback(insertColumnCallBack), 0,
      NULL, NULL },

    { "/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/Unremove item", NULL, GtkItemFactoryCallback(unremoveItemCallBack), 1,
      NULL, NULL }
};



// --------------------------
// Constructor and destructor
// --------------------------
MainDrawingWindow::MainDrawingWindow():
    pixmap(0), drawingArea(0), drawingGC(0), colorMap(0), statusBar(0),
    pixmapWidth(0), pixmapHeight(0),
    columns(5),
    draggedItem(0), itemBeingConnected(0), connectionLine(0)
{
}

MainDrawingWindow::~MainDrawingWindow()
{
    for(unsigned i=0; i<items.size(); ++i)
        delete items[i];

    for(unsigned i=0; i<itemsToBeDeleted.size(); ++i)
        delete itemsToBeDeleted[i];
}


// ---------------------
// Drawing color setting
// ---------------------
void MainDrawingWindow::setDrawingColor(unsigned red, unsigned green,
                                        unsigned blue)
{
    GdkColor color = { 0, red, green, blue };
    gdk_colormap_alloc_color(colorMap, &color, FALSE, TRUE);
    gdk_gc_set_foreground(drawingGC, &color);
}


// ----------
// Redraw all
// ----------
void MainDrawingWindow::redrawAll()
{
    for(int i = 0; i < columns; ++i)
    {
        if(i == 0)
            setDrawingColor(SOURCE_LSTS_COLUMN_COLOR);
        else if(i%2 == 1)
            setDrawingColor(LSTS_COLUMN_COLOR);
        else
            setDrawingColor(OPERATION_COLUMN_COLOR);

        gdk_draw_rectangle(pixmap, drawingGC, TRUE,
                           i*COLUMN_WIDTH, 0, COLUMN_WIDTH, pixmapHeight);
    }

    for(unsigned i=0; i<items.size(); ++i)
        items[i]->draw(*this);
}


// ---------------
// Pixmap resizing
// ---------------
void MainDrawingWindow::resizePixmap(int xSize, int ySize)
{
    if(xSize != pixmapWidth || ySize != pixmapHeight)
    {
        if(pixmap != 0)
        {
            g_object_unref(G_OBJECT(pixmap));
        }

        pixmap = gdk_pixmap_new(drawingArea->window, xSize, ySize, -1);
        gtk_widget_set_size_request(drawingArea, xSize, ySize);
        pixmapWidth = xSize;
        pixmapHeight = ySize;
    }
}

void MainDrawingWindow::recalculatePixmapSize()
{
    int maxColumn = 2, maxHeight = DAREA_MIN_HEIGHT;

    for(unsigned i=0; i<items.size(); ++i)
    {
        if(items[i]->getColumn() >= maxColumn)
            maxColumn = items[i]->getColumn()+2;
        if(items[i]->getMaximumY() > maxHeight-DAREA_BOTTOM_MARGIN)
            maxHeight = items[i]->getMaximumY() + DAREA_BOTTOM_MARGIN;
    }

    columns = maxColumn+1;
    resizePixmap((maxColumn+1)*COLUMN_WIDTH, maxHeight);
}

void MainDrawingWindow::refreshAll()
{
    if(items.empty())
    {
        items.push_back(new OperationParComp(2, 160, *this));
        for(unsigned i = 0; i < 5; ++i)
        {
            items.push_back(new LSTSEditorSource(0, 32+64*i, *this, "Jee"));
            items.back()->connectOutputTo(items[0]);
            items.back()->connectOutputTo(items[0]);
        }
        items.push_back(new OperationSBRed(4, 160, *this));
        items[0]->connectOutputTo(items.back());
    }

    recalculatePixmapSize();
    redrawAll();
    refreshArea();
}


// ----------------------------
// Main initialization function
// ----------------------------
GtkWidget* MainDrawingWindow::initialize(GtkWindow* parentWindow,
                                         StatusBar& mainStatusBar)
{
    if(pixmap != 0 || drawingArea != 0 || drawingGC != 0 || colorMap != 0)
    {
        std::cerr <<
            "Fatal error: MainDrawingWindow initialized more than once.\n";
        std::abort();
    }

    parent = parentWindow;
    statusBar = &mainStatusBar;

    drawingArea = gtk_drawing_area_new();
    drawingGC = gdk_gc_new(gtk_widget_get_root_window(GTK_WIDGET(parent)));
    colorMap = gdk_colormap_get_system();

    GtkWidget* scrolled = gtk_scrolled_window_new(NULL,NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),
                                          drawingArea);

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

    return scrolled;
}


// --------------------------------------------------------------------
// Connect the output of the given item to the input of a selected item
// --------------------------------------------------------------------
void MainDrawingWindow::connectItemOutput(TVTItem* sourceItem,
                                          int lineStartX, int lineStartY)
{
    itemBeingConnected = sourceItem;
    connectionLine = new LineDrag(drawingArea, pixmap, lineStartX, lineStartY);
    itemIsBeingDisconnected = false;

    statusBar->setText("Left-click: select destination, right-click: cancel");
}

// -------------------------------------------------------------------------
// Disconnect the output of the given item from the input of a selected item
// -------------------------------------------------------------------------
void MainDrawingWindow::disconnectItemOutput(TVTItem* sourceItem,
                                             int lineStartX, int lineStartY)
{
    itemBeingConnected = sourceItem;
    connectionLine = new LineDrag(drawingArea, pixmap,
                                  lineStartX, lineStartY, true);
    itemIsBeingDisconnected = true;

    statusBar->setText("Left-click: select destination, right-click: cancel");
}



// ---------------
// Signal handlers
// ---------------
gboolean MainDrawingWindow::configureEvent(GtkWidget*,
                                           GdkEventConfigure*,
                                           MainDrawingWindow* instance)
{
    instance->refreshAll();

    if(!instance->sourceColumnPopupMenu.hasBeenCreated())
    {
        PopupMenu::MenuItems items(TableRange(sourceColumnPopupMenuItems));
        instance->sourceColumnPopupMenu.createMenu("<SourceColumnMenu>",
                                                   items, instance);
    }
    if(!instance->operationColumnPopupMenu.hasBeenCreated())
    {
        PopupMenu::MenuItems items(TableRange(operationColumnPopupMenuItems));
        instance->operationColumnPopupMenu.createMenu("<OperationColumnMenu>",
                                                      items, instance);
    }

    return TRUE;
}

gboolean MainDrawingWindow::exposeEvent(GtkWidget*,
                                        GdkEventExpose* event,
                                        MainDrawingWindow* instance)
{
    instance->refreshArea(event->area.x, event->area.y,
                          event->area.width, event->area.height);
    return FALSE;
}

// ------------------
// Mouse button click
// ------------------
gboolean MainDrawingWindow::mouseButtonPressEvent(GtkWidget*,
                                                  GdkEventButton* press,
                                                  MainDrawingWindow* instance)
{
    const int x = int(press->x), y = int(press->y);

    if(x > instance->pixmapWidth || y > instance->pixmapHeight) return TRUE;

    // Get clicked TVTItem:
    // -------------------
    TVTItem* clickedItem = 0;
    for(unsigned i=0; i<instance->items.size(); ++i)
    {
        if(instance->items[i]->coordinatesAreInsideItem(x, y))
        {
            clickedItem = instance->items[i];
            break;
        }
    }

    // If a TVTItem is being connected:
    // -------------------------------
    if(instance->itemBeingConnected)
    {
        bool endConnection = false;

        if(press->button == 1 && clickedItem)
        {
            if(instance->itemIsBeingDisconnected)
            {
                if(instance->itemBeingConnected->isConnectedTo(clickedItem))
                {
                    instance->itemBeingConnected->
                        disconnectOutputFrom(clickedItem);
                    endConnection = true;
                }
            }
            else if(instance->itemBeingConnected->connectOutputTo(clickedItem))
                endConnection = true;
        }

        if(press->button == 3)
            endConnection = true;

        if(endConnection)
        {
            instance->itemBeingConnected = 0;
            delete instance->connectionLine;
            instance->connectionLine = 0;
            instance->statusBar->clear();
            instance->refreshAll();
        }
        return TRUE;
    }

    // If making a copy of an item:
    // ---------------------------
    if(instance->draggedItem)
    {
        if(press->button == 3)
            instance->removeItem(instance->draggedItem);
        instance->draggedItem = 0;
        instance->refreshAll();
        instance->statusBar->clear();
        return TRUE;
    }

    // If left button was pressed, start dragging:
    // ------------------------------------------
    if(press->button == 1 && clickedItem)
    {
        instance->draggedItem = clickedItem;
        instance->draggedItemOriginalY = clickedItem->getVerticalOffset();
        instance->draggedItemCurrentY = y;
        if(press->state & GDK_CONTROL_MASK)
            instance->dragRecursively = true;
        else
            instance->dragRecursively = false;
    }

    // If right button was pressed, popup context menu:
    // -----------------------------------------------
    else if(press->button == 3)
    {
        if(clickedItem)
        {
            clickedItem->getPopupMenu(x, y).
                popup(int(press->x_root), int(press->y_root),
                      press->button, press->time);
        }
        else
        {
            instance->clickedColumn = x/COLUMN_WIDTH;
            instance->clickedYCoord = y;

            if(instance->clickedColumn == 0)
            {
                instance->updateSourceColumnMenuSensitivity();
                instance->sourceColumnPopupMenu.popup(int(press->x_root),
                                                      int(press->y_root),
                                                      press->button,
                                                      press->time);
            }
            else if(instance->clickedColumn%2 == 0)
            {
                instance->updateOperationColumnMenuSensitivity();
                instance->operationColumnPopupMenu.popup(int(press->x_root),
                                                         int(press->y_root),
                                                         press->button,
                                                         press->time);
            }
        }
    }

    return TRUE;
}


// --------------------
// Mouse button release
// --------------------
gboolean
MainDrawingWindow::mouseButtonReleaseEvent(GtkWidget*,
                                           GdkEventButton*,
                                           MainDrawingWindow* instance)
{
    if(instance->draggedItem)
    {
        if(instance->dragRecursively)
            instance->draggedItem->moveFollowersBy
                (instance->draggedItem->getVerticalOffset() -
                 instance->draggedItemOriginalY);

        instance->draggedItem = 0;
        instance->refreshAll();
    }
    return TRUE;
}


// --------------
// Mouse movement
// --------------
gboolean MainDrawingWindow::motionEvent(GtkWidget*,
                                        GdkEventMotion* event,
                                        MainDrawingWindow* instance)
{
    int x, y;
    GdkModifierType state;
    static const TVTItem* lastHoveredItem = 0;

    // Get hover coordinates:
    // ---------------------
    if(event->is_hint)
        gdk_window_get_pointer(event->window, &x, &y, &state);
    else
    {
        x = int(event->x);
        y = int(event->y);
    }

    // If we are dragging an item:
    // --------------------------
    if(instance->draggedItem)
    {
        instance->draggedItem->dragVertically(y-instance->draggedItemCurrentY,
                                              *instance);
        instance->draggedItemCurrentY = y;
    }

    // If we are connecting a TVTItem to another:
    // -----------------------------------------
    else if(instance->itemBeingConnected)
    {
        instance->connectionLine->lineTo(x, y);
    }

    // Update statusbar text:
    // ---------------------
    else
    {
        // If outside the pixmap, clear text:
        if(x > instance->pixmapWidth || y > instance->pixmapHeight)
        {
            instance->statusBar->clear();
            return TRUE;
        }

        // If hovering the same item as last time, no need to change anything:
        bool hoveringAnItem =
            lastHoveredItem && lastHoveredItem->coordinatesAreInsideItem(x, y);
        // If not hovering the same item, check if hovering another one:
        if(!hoveringAnItem)
        {
            for(unsigned i = 0; i < instance->items.size(); ++i)
            {
                const TVTItem* item = instance->items[i];
                if(item->coordinatesAreInsideItem(x, y))
                {
                    instance->statusBar->setText(ITEM_HOVER_TEXT);
                    lastHoveredItem = item;
                    hoveringAnItem = true;
                    break;
                }
            }
        }
        // If not, print hover text for columns:
        if(!hoveringAnItem)
        {
            lastHoveredItem = 0;
            if((x/COLUMN_WIDTH)%2 == 0)
                instance->statusBar->setText(COLUMN_HOVER_TEXT);
            else
                instance->statusBar->clear();
        }
    }

    return TRUE;
}

// -----------------
// Mouse leave event
// -----------------
gboolean MainDrawingWindow::leaveEvent(GtkWidget*, GdkEventCrossing*,
                                       MainDrawingWindow* instance)
{
    if(!instance->itemBeingConnected)
        instance->statusBar->clear();
    return TRUE;
}


// -----------------------------
// Popup menu callback functions
// -----------------------------
void MainDrawingWindow::newSourceProcessCallBack(gpointer ptr, guint srcID,
                                                 GtkWidget*)
{
    MainDrawingWindow* instance = reinterpret_cast<MainDrawingWindow*>(ptr);

    StringQueryDialog query;
    const char* processName =
        query.run(instance->parent, "Process name", "",
                  "Name of the new source process:");

    if(!processName) return;

    TVTItem* item = 0;

    switch(srcID)
    {
      case INPUTLANGUAGESOURCE:
          item = new InputLanguageSource(instance->clickedColumn,
                                         instance->clickedYCoord, *instance,
                                         processName);
          break;

      case LSTSEDITORSOURCE:
          item = new LSTSEditorSource(instance->clickedColumn,
                                      instance->clickedYCoord, *instance,
                                      processName);
          break;
    }

    if(item)
    {
        instance->items.push_back(item);
        instance->refreshAll();
    }
}

void MainDrawingWindow::updateSourceColumnMenuSensitivity()
{
    bool itemExists = false;
    for(unsigned i = 0; i < itemsToBeDeleted.size(); ++i)
    {
        if(itemsToBeDeleted[i]->getColumn() == 0)
        {
            itemExists = true;
            break;
        }
    }
    sourceColumnPopupMenu.setSensitivity("/Unremove item", itemExists);
}

void MainDrawingWindow::updateOperationColumnMenuSensitivity()
{
    bool canRemoveColumn = true;
    for(unsigned i = 0; i < items.size(); ++i)
    {
        if(items[i]->getColumn() == clickedColumn)
        {
            canRemoveColumn = false;
            break;
        }
    }
    operationColumnPopupMenu.setSensitivity("/Remove column", canRemoveColumn);

    bool itemExists = false;
    for(unsigned i = 0; i < itemsToBeDeleted.size(); ++i)
    {
        if(itemsToBeDeleted[i]->getColumn() > 0)
        {
            itemExists = true;
            break;
        }
    }
    operationColumnPopupMenu.setSensitivity("/Unremove item", itemExists);
}

void MainDrawingWindow::newOperationCallBack(gpointer ptr, guint opID,
                                             GtkWidget*)
{
    MainDrawingWindow* instance = reinterpret_cast<MainDrawingWindow*>(ptr);

    TVTItem* item = 0;

    switch(opID)
    {
      case PARCOMP:
          item = new OperationParComp(instance->clickedColumn,
                                      instance->clickedYCoord, *instance);
          break;
      case SBREDUCTION:
          item = new OperationSBRed(instance->clickedColumn,
                                    instance->clickedYCoord, *instance);
          break;
      case LSTS2AG: break;
    }

    if(item)
    {
        instance->items.push_back(item);
        instance->refreshAll();
    }
}

void MainDrawingWindow::insertColumnCallBack(gpointer ptr, guint offset,
                                             GtkWidget*)
{
    MainDrawingWindow* instance = reinterpret_cast<MainDrawingWindow*>(ptr);

    for(unsigned i = 0; i < instance->items.size(); ++i)
    {
        int col = instance->items[i]->getColumn();
        if(col >= instance->clickedColumn)
            instance->items[i]->setColumn(col+offset-2);
    }
    instance->refreshAll();
}


void MainDrawingWindow::unremoveItemCallBack(gpointer ptr, guint operation,
                                             GtkWidget*)
{
    MainDrawingWindow* instance = reinterpret_cast<MainDrawingWindow*>(ptr);

    for(unsigned i = instance->itemsToBeDeleted.size(); i > 0;)
    {
        --i;
        TVTItem* item = instance->itemsToBeDeleted[i];
        if((operation && item->getColumn() > 0) ||
           (!operation && item->getColumn() == 0))
        {
            instance->itemsToBeDeleted.erase
                (instance->itemsToBeDeleted.begin()+i);
            instance->items.push_back(item);
            instance->refreshAll();
            break;
        }
    }
}

// Copy a TVTItem
// --------------
void MainDrawingWindow::copyItem(TVTItem* item)
{
    TVTItem* copy = item->createCopy(item->getVerticalOffset() + 128);
    items.push_back(copy);
    draggedItem = copy;
    draggedItemOriginalY = draggedItemCurrentY = copy->getVerticalOffset();
    dragRecursively = false;
    copy->dragVertically(0, *this);
    statusBar->setText("Left-click: select location, right-click: cancel");
}

// Remove a TVTItem
// ----------------
void MainDrawingWindow::removeItem(TVTItem* item)
{
    for(unsigned i = 0; i < items.size(); ++i)
    {
        if(items[i] == item)
        {
            items.erase(items.begin()+i);
            item->disconnectEverything();
            //delete item;
            itemsToBeDeleted.push_back(item);
            refreshAll();
            break;
        }
    }
}

// -----------
// QueryDialog
// -----------
bool MainDrawingWindow::queryDialog(const char* title, const char* question)
{
    GtkWidget* dialog =
        gtk_message_dialog_new(parent, GTK_DIALOG_DESTROY_WITH_PARENT,
                               GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                               question);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return result == GTK_RESPONSE_YES;
}
