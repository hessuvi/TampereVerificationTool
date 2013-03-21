#include "LSTSEditorSource.hh"
#include "constants.hh"

LSTSEditorSource::LSTSEditorSource(int col, int yCoord,
                                   MainDrawingWindow& dWindow,
                                   const char* name):
    SourceProcess(col, yCoord, dWindow, name),
    editor(name)
{
}

namespace
{
    template<int size>
    void drawMouseCursorSymbol(MainDrawingWindow& dWindow, int x, int y)
    {
        MainDrawingWindow::Point v[7];

        v[0].x = x;
        v[0].y = y;
        v[1].x = x+size/2;
        v[1].y = y+size/2;
        v[2].x = v[1].x-size/5;
        v[2].y = v[1].y+size/9;
        v[3].x = v[2].x+size/4;
        v[3].y = v[2].y+size/2;
        v[6].x = x;
        v[6].y = v[1].y+(v[1].x-x)/2;
        v[5].x = x+size/5;
        v[5].y = v[6].y-size/9;
        v[4].x = v[5].x+size/4;
        v[4].y = v[5].y+size/2;

        dWindow.setDrawingColor(SOURCE_LSTS_GRAPHIC_BG_COLOR);
        dWindow.polygon(v, 7, true);
        //dWindow.setDrawingColor(SOURCE_LSTS_GRAPHIC_TEXTLINE_COLOR);
        dWindow.setDrawingColor(MAIN_FG_COLOR);
        dWindow.polygon(v, 7);
    }
}

void LSTSEditorSource::drawMainPart(MainDrawingWindow& dWindow,
                                    int col, int yCoord) const
{
    SourceProcess::drawMainPart(dWindow, col, yCoord);

    const unsigned midx = col*COLUMN_WIDTH+COLUMN_WIDTH/2;

    dWindow.setDrawingColor(SOURCE_LSTS_GRAPHIC_TEXTLINE_COLOR);
    drawLSTSSymbol(dWindow, midx, yCoord, SOURCE_LSTS_GRAPHIC_WIDTH);
    drawMouseCursorSymbol<12>(dWindow, midx, yCoord-4);
    //dWindow.setDrawingColor(MAIN_FG_COLOR);
}

void LSTSEditorSource::edit()
{
    editor.run();
}

void LSTSEditorSource::hasBeenRenamed(const char* newName)
{
    editor.changeLSTSName(newName);
}

TVTItem* LSTSEditorSource::clone(int newVerticalOffset,
                                 MainDrawingWindow& dWindow)
{
    return new LSTSEditorSource(getColumn(), newVerticalOffset,
                                dWindow, getSourceProcessName());
}
