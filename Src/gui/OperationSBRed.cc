#include "OperationSBRed.hh"

OperationSBRed::OperationSBRed(int col, int yCoord,
                               MainDrawingWindow& dWindow):
    Operation(col, yCoord, dWindow, "SBRed")
{
}

TVTItem* OperationSBRed::clone(int newVerticalOffset,
                               MainDrawingWindow& dWindow)
{
    return new OperationSBRed(getColumn(), newVerticalOffset, dWindow);
}
