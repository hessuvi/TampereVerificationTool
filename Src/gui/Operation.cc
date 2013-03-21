#include "Operation.hh"
#include "constants.hh"

// Menu items
// ----------
const GtkItemFactoryEntry Operation::operationPopupMenuItems[] =
{
    { "/Options...", NULL, GtkItemFactoryCallback(optionsCallBack), 1,
      NULL, NULL },
    { "/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/Push to next column",NULL, GtkItemFactoryCallback(moveRightCallBack),1,
      "<StockItem>", "tvt-push-to-next-column" },
    { "/Pull to prev column", NULL, GtkItemFactoryCallback(moveLeftCallBack),1,
      "<StockItem>", "tvt-pull-to-prev-column" },
    { "/Move to prev column", NULL, GtkItemFactoryCallback(moveLeftCallBack),0,
      "<StockItem>", "tvt-move-to-prev-column" },
    { "/sep2", NULL, NULL, 0, "<Separator>", NULL },
    { "/Change item dim", NULL,
      GtkItemFactoryCallback(changeItemDimCallBack), 0, NULL, NULL },
    { "/Change subtree dim", NULL,
      GtkItemFactoryCallback(changeSubtreeDimCallBack), 0, NULL, NULL },
    { "/sep3", NULL, NULL, 0, "<Separator>", NULL }
};

void Operation::updateMainPartMenuSensitivity(const PopupMenu& menu) const
{
    bool yes = canBeMovedLeft();
    menu.setSensitivity("/Pull to prev column", yes);
    menu.setSensitivity("/Move to prev column", yes);
}


// Contructor
// ----------
Operation::Operation(int col, int yCoord,
                     MainDrawingWindow& dWindow,
                     const char* operationName):
    TVTItem(col, yCoord, dWindow,
            PopupMenu::MenuItems(TableRange(operationPopupMenuItems))),
    operationName(dWindow, operationName)
{}


// Main graphic
// ------------
int Operation::mainPartWidth() const
{
    return OPERATION_GRAPHIC_WIDTH;
}

int Operation::mainPartHeight() const
{
    return OPERATION_GRAPHIC_HEIGHT;
}

void Operation::drawInputConnection(MainDrawingWindow& dWindow,
                                    int inputX, int inputY,
                                    TVTItem* inputItem) const
{
    Coords srcCoords = inputItem->getOutputConnectionCoords();
    if(getColumn() > inputItem->getColumn()+2)
    {
        Coords midCoords((inputItem->getColumn()+2)*
                         COLUMN_WIDTH+COLUMN_WIDTH/2,
                         inputY);
        dWindow.line(srcCoords.x, srcCoords.y, midCoords.x, midCoords.y);
        srcCoords = midCoords;
    }
    dWindow.line(srcCoords.x, srcCoords.y, inputX, inputY);
}

void Operation::drawMainPart(MainDrawingWindow& dWindow,
                             int col, int yCoord) const
{
    MainDrawingWindow::Point v[4];
    const int midx = col*COLUMN_WIDTH+COLUMN_WIDTH/2;

    v[0].x = midx+OPERATION_GRAPHIC_WIDTH/2 - OPERATION_GRAPHIC_HEIGHT/2;
    v[0].y = yCoord-OPERATION_GRAPHIC_HEIGHT/2;
    v[1].x = midx-OPERATION_GRAPHIC_WIDTH/2;
    v[1].y = v[0].y;
    v[2].x = v[1].x;
    v[2].y = v[1].y+OPERATION_GRAPHIC_HEIGHT;
    v[3].x = v[0].x;
    v[3].y = v[2].y;

    dWindow.lines(v, 4);
    dWindow.circle(v[0].x, yCoord, OPERATION_GRAPHIC_HEIGHT, -90, 180);
    dWindow.text(col*COLUMN_WIDTH, yCoord-DRAWING_WINDOW_FONT_SIZE/2,
                 operationName);

    if(inputConnection)
        drawInputConnection(dWindow, midx-OPERATION_GRAPHIC_WIDTH/2, yCoord,
                            inputConnection);
}


// Popup menu callback functions
// -----------------------------
void Operation::optionsCallBack(gpointer, guint, GtkWidget*)
{
}

void Operation::moveRightCallBack(gpointer itemInstancePtr, guint refreshView,
                                  GtkWidget* widget)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(itemInstancePtr);
    Operation* opInst = static_cast<Operation*>(instance);

    instance->setColumn(instance->getColumn()+2);

    for(unsigned i = 0; i < opInst->outputConnections.size(); ++i)
    {
        TVTItem* outputConnection = opInst->outputConnections[i];
        if(outputConnection->getColumn() == instance->getColumn())
        {
            moveRightCallBack(outputConnection, 0, widget);
        }
    }

    if(refreshView)
        opInst->callRefreshAll();
}

void Operation::moveLeftCallBack(gpointer itemInstancePtr, guint pull,
                                 GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(itemInstancePtr);
    Operation* opInst = static_cast<Operation*>(instance);

    if(pull)
        opInst->pullLeft();
    else
        instance->setColumn(instance->getColumn()-2);

    opInst->callRefreshAll();
}

void Operation::changeItemDimCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);
    Operation* opInst = static_cast<Operation*>(instance);

    opInst->changeDimOnThisItem();
}

void Operation::changeSubtreeDimCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);
    Operation* opInst = static_cast<Operation*>(instance);

    opInst->changeDimOnThisSubtree();
}
