#include "SourceProcess.hh"
#include "constants.hh"

const GtkItemFactoryEntry SourceProcess::sourceProcessPopupMenuItems[] =
{
    { "/Edit", NULL, GtkItemFactoryCallback(editCallBack), 1, NULL, NULL },
    { "/Rename...", NULL, GtkItemFactoryCallback(renameCallBack), 1,
      "<StockItem>", "tvt-rename" },
    { "/sep1", NULL, NULL, 0, "<Separator>", NULL }
};

SourceProcess::SourceProcess(int col, int yCoord,
                       MainDrawingWindow& dWindow, const char* name):
    TVTItem(col, yCoord, dWindow,
            PopupMenu::MenuItems(TableRange(sourceProcessPopupMenuItems))),
    sourceName(dWindow, name)
{
}

int SourceProcess::mainPartWidth() const
{
    return SOURCE_LSTS_GRAPHIC_WIDTH;
}

int SourceProcess::mainPartHeight() const
{
    return SOURCE_LSTS_GRAPHIC_HEIGHT;
}

void SourceProcess::hasBeenRenamed(const char*)
{
}

namespace
{
    void drawDocumentSymbol(MainDrawingWindow& dWindow,
                            int col, int yCoord)
    {
        MainDrawingWindow::Point v[5];
        const int midx = col*COLUMN_WIDTH+COLUMN_WIDTH/2;

        v[0].x = midx-SOURCE_LSTS_GRAPHIC_WIDTH/2;
        v[0].y = yCoord+SOURCE_LSTS_GRAPHIC_HEIGHT/2;
        v[1].x = v[0].x + SOURCE_LSTS_GRAPHIC_WIDTH;
        v[1].y = v[0].y;
        v[2].x = v[1].x;
        v[2].y = v[1].y - SOURCE_LSTS_GRAPHIC_HEIGHT;
        v[3].x = v[0].x + SOURCE_LSTS_GRAPHIC_FOLD_SIZE;
        v[3].y = v[2].y;
        v[4].x = v[0].x;
        v[4].y = v[3].y + SOURCE_LSTS_GRAPHIC_FOLD_SIZE;

        dWindow.setDrawingColor(SOURCE_LSTS_GRAPHIC_BG_COLOR);
        dWindow.polygon(v, 5, true);
        dWindow.setDrawingColor(MAIN_FG_COLOR);
        dWindow.polygon(v, 5);

        dWindow.line(v[3].x, v[3].y, v[3].x, v[4].y);
        dWindow.line(v[3].x, v[4].y, v[4].x, v[4].y);
    }
}

void SourceProcess::drawMainPart(MainDrawingWindow& dWindow,
                                 int col, int yCoord) const
{
    drawDocumentSymbol(dWindow, col, yCoord);

    dWindow.text(col*COLUMN_WIDTH, yCoord+SOURCE_LSTS_GRAPHIC_HEIGHT/2,
                 sourceName);
}


void SourceProcess::editCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);
    SourceProcess* spInst = static_cast<SourceProcess*>(instance);

    spInst->edit();
}

void SourceProcess::renameCallBack(gpointer ptr, guint, GtkWidget*)
{
    TVTItem* instance = reinterpret_cast<TVTItem*>(ptr);
    SourceProcess* spInst = static_cast<SourceProcess*>(instance);

    if(spInst->sourceName.askString("Name of the process"))
    {
        spInst->callRefreshAll();
        spInst->hasBeenRenamed(spInst->sourceName.getText());
    }
}
