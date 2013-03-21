#include "OperationParComp.hh"

OperationParComp::OperationParComp(int col, int yCoord,
                                   MainDrawingWindow& dWindow):
    MultipleInputOperation(col, yCoord, dWindow, "PC")
{
}

TVTItem* OperationParComp::clone(int newVerticalOffset,
                                 MainDrawingWindow& dWindow)
{
    OperationParComp* copy = new OperationParComp(getColumn(),
                                                  newVerticalOffset,
                                                  dWindow);
    for(unsigned i = 0; i < inputConnections.size(); ++i)
        inputConnections[i]->connectOutputTo(copy);

    return copy;
}
