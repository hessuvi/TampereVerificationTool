#include "TVTItem.hh"
#include "StringQueryDialog.hh"
#include "constants.hh"

#include <sstream>

// Popup menu data
// ---------------
const GtkItemFactoryEntry TVTItem::resultLSTSDefaultPopupMenuItems[] =
{
    { "/Build", NULL, GtkItemFactoryCallback(buildCallBack), 0,
      "<StockItem>", GTK_STOCK_EXECUTE },
    { "/View", NULL, GtkItemFactoryCallback(viewCallBack), 0,
      "<StockItem>", "tvt-view" },
    { "/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/Connect output", NULL, GtkItemFactoryCallback(connectOutputCallBack),0,
      "<StockItem>", "tvt-connect-output" },
    { "/Disconnect output", NULL,
      GtkItemFactoryCallback(disconnectOutputCallBack), 0,
      "<StockItem>", "tvt-disconnect-output" },
    { "/sep2", NULL, NULL, 0, "<Separator>", NULL },
    { "/Set name...", NULL, GtkItemFactoryCallback(setNameCallBack), 0,
      "<StockItem>", "tvt-set-name" }
};

const GtkItemFactoryEntry TVTItem::mainPartDefaultPopupMenuItems[] =
{
    { "/Copy item", "", GtkItemFactoryCallback(copyCallBack), 1,
      "<StockItem>", GTK_STOCK_COPY },
    { "/Remove item", NULL, GtkItemFactoryCallback(removeCallBack), 1,
      "<StockItem>", "tvt-remove" }
};

void TVTItem::updateMainPartMenuSensitivity(const PopupMenu&) const
{
}

void TVTItem::updateResultingLSTSMenuSensitivity(const PopupMenu& menu) const
{
    menu.setSensitivity("/Build", resultStatesAmount == ~0U);
    menu.setSensitivity("/Disconnect output", !outputConnections.empty());
}


// Constructor and destructor
// --------------------------
TVTItem::TVTItem(int col, int yCoord, MainDrawingWindow& dWindow,
                 const PopupMenu::MenuItems& mainPartItems):
    inputConnection(0),
    parentDWindow(dWindow),
    column(col), verticalOffset(yCoord),
    resultStatesAmount(~0U), resultTransitionsAmount(~0U),
    resultLSTSInfo(dWindow), resultName(dWindow),
    traverseFlag(false), dimmed(false)
{
    static unsigned long itemIDCounter = 0;

    itemID = itemIDCounter++;

    setResultAmountsText();
    resultName.setUpwardsAlignment();

    PopupMenu::MenuItems items(TableRange(resultLSTSDefaultPopupMenuItems));
    resultingLSTSPopupMenu.createMenu("<ResultingLSTSMenu>", items, this);

    items = mainPartItems;
    items.insert(items.end(), TableRange(mainPartDefaultPopupMenuItems));
    mainPartPopupMenu.createMenu("<MainPartMenu>", items, this);
}

TVTItem::~TVTItem() {}


// Connect output to the input of 'target'
// ---------------------------------------
bool TVTItem::connectOutputTo(TVTItem* target)
{
    if(target->column <= column) return false;
    if(!target->connectInput(this)) return false;
    outputConnections.push_back(target);
    return true;
}

bool TVTItem::isConnectedTo(TVTItem* target)
{
    for(unsigned i = 0; i < outputConnections.size(); ++i)
    {
        if(outputConnections[i] == target)
            return true;
    }
    return false;
}

void TVTItem::disconnectOutputFrom(TVTItem* target)
{
    target->disconnectInput(this);

    // We can assume there will never be so many output connections that
    // a linear search will be a speed problem:
    for(ociter i = outputConnections.begin();
        i != outputConnections.end(); ++i)
    {
        if(*i == target)
        {
            outputConnections.erase(i);
            break;
        }
    }
}

// Connect input
// -------------
bool TVTItem::connectInput(TVTItem* source)
{
    if(inputConnection != 0)
        inputConnection->disconnectOutputFrom(this);
    inputConnection = source;
    return true;
}

void TVTItem::disconnectInput(TVTItem*)
{
    inputConnection = 0;
}

void TVTItem::disconnectAllInput()
{
    if(inputConnection)
        inputConnection->disconnectOutputFrom(this);
}

// Disconnect every connection
// ---------------------------
void TVTItem::disconnectEverything()
{
    disconnectAllInput();

    for(unsigned i=0; i<outputConnections.size(); ++i)
    {
        outputConnections[i]->disconnectInput(this);
    }
    outputConnections.clear();
}


// Set text for resulting states and transitions amount:
// ----------------------------------------------------
void TVTItem::setResultAmountsText()
{
    std::stringstream t;

    if(resultStatesAmount == ~0U)
        t << '?';
    else
        t << resultStatesAmount;
    t << "\n";
    if(resultTransitionsAmount == ~0U)
        t << '?';
    else
        t << resultTransitionsAmount;

    resultLSTSInfo.setText(t.str().c_str());
}


// Draw resulting LSTS part:
// ------------------------
namespace
{
    void drawLSTSSymbolArrow(MainDrawingWindow& dWindow,
                             int x1, int y1,
                             int x2, int y2,
                             int arrowType)
    {
        dWindow.line(x1, y1, x2, y2);
        switch(arrowType)
        {
          case 1:
              dWindow.line(x2, y2, x2, y2-4);
              dWindow.line(x2, y2, x2+4, y2-2);
              break;

          case 2:
              dWindow.line(x2, y2, x2, y2-4);
              dWindow.line(x2, y2, x2-4, y2-2);
              break;

          case 3:
              dWindow.line(x2, y2, x2+1, y2-4);
              dWindow.line(x2, y2, x2+4, y2-1);
              break;

          case 4:
              dWindow.line(x2, y2, x2-2, y2-4);
              dWindow.line(x2, y2, x2+2, y2-4);
              break;
        }
    }
}

void TVTItem::drawLSTSSymbol(MainDrawingWindow& dWindow,
                             int xCoord, int yCoord,
                             int size) const
{
    const int y1 = yCoord-size/2;
    const int x2 = xCoord-(yCoord-y1)/2;
    const int x3 = xCoord+(yCoord-y1)/2;
    const int x4 = x3 - size*5/12, y4 = yCoord + size*5/12;
    const int x5 = x3, y5 = yCoord+size/2;

    dWindow.circle(xCoord, y1, RESULT_LSTS_SYMBOL_RADIUS);
    dWindow.circle(x2, yCoord, RESULT_LSTS_SYMBOL_RADIUS);
    drawLSTSSymbolArrow(dWindow,
                        xCoord - RESULT_LSTS_SYMBOL_RADIUS/2 + 1,
                        y1 + RESULT_LSTS_SYMBOL_RADIUS - 1,
                        x2 + RESULT_LSTS_SYMBOL_RADIUS/2 - 1,
                        yCoord - RESULT_LSTS_SYMBOL_RADIUS + 1, 1);
    dWindow.circle(x3, yCoord, RESULT_LSTS_SYMBOL_RADIUS);
    drawLSTSSymbolArrow(dWindow,
                        xCoord + RESULT_LSTS_SYMBOL_RADIUS/2 - 1,
                        y1 + RESULT_LSTS_SYMBOL_RADIUS - 1,
                        x3 - RESULT_LSTS_SYMBOL_RADIUS/2 + 1,
                        yCoord - RESULT_LSTS_SYMBOL_RADIUS + 1, 2);
    dWindow.circle(x4, y4, RESULT_LSTS_SYMBOL_RADIUS);
    drawLSTSSymbolArrow(dWindow,
                        x3 - RESULT_LSTS_SYMBOL_RADIUS + 1,
                        yCoord + RESULT_LSTS_SYMBOL_RADIUS - 1,
                        x4 + RESULT_LSTS_SYMBOL_RADIUS - 1,
                        y4 - RESULT_LSTS_SYMBOL_RADIUS + 1, 3);
    dWindow.circle(x5, y5, RESULT_LSTS_SYMBOL_RADIUS);
    drawLSTSSymbolArrow(dWindow,
                        x3, yCoord + RESULT_LSTS_SYMBOL_RADIUS - 1,
                        x5, y5 - RESULT_LSTS_SYMBOL_RADIUS + 1, 4);
}

int TVTItem::drawResultingLSTSPart(MainDrawingWindow& dWindow) const
{
    const int midx = (column+1)*COLUMN_WIDTH+COLUMN_WIDTH/2;
    const int x = midx-RESULT_LSTS_GRAPHIC_WIDTH/2;
    const int y = verticalOffset-RESULT_LSTS_GRAPHIC_HEIGHT/2;

    dWindow.rectangle(x, y,
                      RESULT_LSTS_GRAPHIC_WIDTH,
                      RESULT_LSTS_GRAPHIC_HEIGHT);

    drawLSTSSymbol(dWindow, midx, verticalOffset,
                   RESULT_LSTS_GRAPHIC_WIDTH);

    dWindow.text((column+1)*COLUMN_WIDTH,
                 verticalOffset+RESULT_LSTS_GRAPHIC_HEIGHT/2,
                 resultLSTSInfo);
    dWindow.text((column+1)*COLUMN_WIDTH,
                 verticalOffset-RESULT_LSTS_GRAPHIC_HEIGHT/2,
                 resultName);

    return x;
}

// Draw the entire item (main part graphic, resulting LSTS graphic and the
// line between them):
// ------------------------------------------------------------------------
void TVTItem::draw(MainDrawingWindow& dWindow) const
{
    if(dimmed)
        dWindow.setDrawingColor(DIMMED_FG_COLOR);
    else
        dWindow.setDrawingColor(MAIN_FG_COLOR);
    drawMainPart(dWindow, column, verticalOffset);
    const int x2 = drawResultingLSTSPart(dWindow);

    const int midx = column*COLUMN_WIDTH+COLUMN_WIDTH/2;
    const int x1 = midx+mainPartWidth()/2;
    dWindow.line(x1, verticalOffset, x2, verticalOffset);
}

// Called when the item is dragged vertically:
// ------------------------------------------
void TVTItem::dragVertically(int offsetChange, MainDrawingWindow& dWindow)
{
    int width = mainPartWidth()+1, height = mainPartHeight()+1;
    int x = column*COLUMN_WIDTH+COLUMN_WIDTH/2 - width/2;

    if(column == 0)
        dWindow.setDrawingColor(SOURCE_LSTS_COLUMN_COLOR);
    else
        dWindow.setDrawingColor(OPERATION_COLUMN_COLOR);
    dWindow.rectangle(x, verticalOffset-height/2, width, height, true);

    if(verticalOffset + offsetChange < 0)
        offsetChange = -verticalOffset;
    verticalOffset += offsetChange;

    dWindow.setDrawingColor(MAIN_FG_COLOR);
    drawMainPart(dWindow, column, verticalOffset);

    int y = verticalOffset-height/2;
    if(offsetChange > 0)
    {
        y -= offsetChange;
        height += offsetChange;
    }
    else
    {
        height -= offsetChange;
    }
    dWindow.refreshArea(x, y, width, height);
}

void TVTItem::moveFollowersBy(int offsetChange)
{
    resetTraverseFlags();
    moveFollowersRecursion(offsetChange);
}

void TVTItem::moveFollowersRecursion(int offsetChange)
{
    for(unsigned i = 0; i < outputConnections.size(); ++i)
    {
        TVTItem* outputConnection = outputConnections[i];
        if(!outputConnection->traverseFlag)
        {
            outputConnection->verticalOffset += offsetChange;
            if(outputConnection->verticalOffset < 0)
                outputConnection->verticalOffset = 0;
            outputConnection->traverseFlag = true;
            outputConnection->moveFollowersRecursion(offsetChange);
        }
    }
}



// Pull this and all connected items to the previous column
// --------------------------------------------------------
void TVTItem::pullLeft()
{
    resetTraverseFlags();
    leftPullRecursion();
}

bool TVTItem::canBeMovedLeft() const
{
    return column > 2 &&
        (!inputConnection || inputConnection->column < column-2);
}

void TVTItem::leftPullRecursion()
{
    traverseFlag = true;
    if(canBeMovedLeft())
    {
        column -= 2;

        for(unsigned i=0; i<outputConnections.size(); ++i)
        {
            TVTItem* outputConnection = outputConnections[i];
            if(!outputConnection->traverseFlag)
                outputConnection->leftPullRecursion();
        }
    }
}

void TVTItem::resetTraverseFlags()
{
    traverseFlag = false;
    for(unsigned i=0; i<outputConnections.size(); ++i)
        outputConnections[i]->resetTraverseFlags();
}

// Change the dimmed status of this item (or entire subtree)
// ---------------------------------------------------------
void TVTItem::changeDimOnThisItem()
{
    dimmed = !dimmed;
    callRefreshAll();
}

void TVTItem::changeDimRecursion(bool dim)
{
    traverseFlag = true;
    dimmed = dim;
    for(unsigned i=0; i<outputConnections.size(); ++i)
    {
        TVTItem* outputConnection = outputConnections[i];
        if(!outputConnection->traverseFlag)
            outputConnection->changeDimRecursion(dim);
    }
}

void TVTItem::changeDimOnThisSubtree()
{
    resetTraverseFlags();
    changeDimRecursion(!dimmed);
    callRefreshAll();
}

// Make a copy of this TVTItem:
// ---------------------------
TVTItem* TVTItem::createCopy(int newVerticalOffset)
{
    TVTItem* copy = clone(newVerticalOffset, parentDWindow);
    if(inputConnection)
        inputConnection->connectOutputTo(copy);
    copy->resultStatesAmount = resultStatesAmount;
    copy->resultTransitionsAmount = resultTransitionsAmount;
    return copy;
}


// Popup menu callbacks:
// --------------------
#include <iostream>
void TVTItem::removeCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);
    if(instance->parentDWindow.queryDialog
       ("Confirm removal", "Are you sure you want to remove this item?"))
    {
        instance->parentDWindow.removeItem(instance);
    }
}

void TVTItem::copyCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);

    instance->parentDWindow.copyItem(instance);
}

void TVTItem::buildCallBack(gpointer, guint, GtkWidget*)
{
    std::cout << "build called" << std::endl;
}

void TVTItem::viewCallBack(gpointer, guint, GtkWidget*)
{
    std::cout << "view called" << std::endl;
}

void TVTItem::setNameCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);
    if(instance->resultName.askString("Name of result LSTS"))
        instance->callRefreshAll();
}

void TVTItem::connectOutputCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);
    const int x =
        (instance->column+1)*COLUMN_WIDTH+COLUMN_WIDTH/2 +
        RESULT_LSTS_GRAPHIC_WIDTH/2;
    instance->parentDWindow.connectItemOutput(instance,
                                              x, instance->verticalOffset);
}

void TVTItem::disconnectOutputCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);

    if(instance->outputConnections.size() == 1)
    {
        instance->disconnectOutputFrom(instance->outputConnections[0]);
        instance->callRefreshAll();
    }
    else
    {
        const int x =
            (instance->column+1)*COLUMN_WIDTH+COLUMN_WIDTH/2 +
            RESULT_LSTS_GRAPHIC_WIDTH/2;
        instance->parentDWindow.disconnectItemOutput(instance, x,
                                                     instance->verticalOffset);
    }
}
