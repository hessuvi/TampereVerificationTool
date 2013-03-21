#include "InputLanguageSource.hh"
#include "constants.hh"

InputLanguageSource::InputLanguageSource(int col, int yCoord,
                                         MainDrawingWindow& dWindow,
                                         const char* name):
    SourceProcess(col, yCoord, dWindow, name)
{
}

void InputLanguageSource::drawMainPart(MainDrawingWindow& dWindow,
                                       int col, int yCoord) const
{
    SourceProcess::drawMainPart(dWindow, col, yCoord);

    const int midx = col*COLUMN_WIDTH+COLUMN_WIDTH/2;
    const int x1 = midx-SOURCE_LSTS_GRAPHIC_WIDTH/2+6;
    const int x2 = midx+SOURCE_LSTS_GRAPHIC_WIDTH/2-4;
    const int y1 =
        yCoord - SOURCE_LSTS_GRAPHIC_HEIGHT/2 +
        SOURCE_LSTS_GRAPHIC_FOLD_SIZE + 4;
    const int y2 = yCoord + SOURCE_LSTS_GRAPHIC_HEIGHT/2 - 4;

    dWindow.setDrawingColor(SOURCE_LSTS_GRAPHIC_TEXTLINE_COLOR);
    for(int y = y1; y <= y2; y += 3)
    {
        dWindow.line(x1, y, x2, y);
    }
    dWindow.setDrawingColor(MAIN_FG_COLOR);
}

void InputLanguageSource::edit()
{
}

TVTItem* InputLanguageSource::clone(int newVerticalOffset,
                                    MainDrawingWindow& dWindow)
{
    return new InputLanguageSource(getColumn(), newVerticalOffset,
                                   dWindow, getSourceProcessName());
}
