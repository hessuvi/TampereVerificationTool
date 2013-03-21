#include "MultipleInputOperation.hh"

#include <cstdio>

// Constructor
// -----------
MultipleInputOperation::MultipleInputOperation(int col, int yCoord,
                                               MainDrawingWindow& dWindow,
                                               const char* operationName):
    Operation(col, yCoord, dWindow, operationName),
    inputNumber(dWindow, "", 0, false)
{}


// Main graphic
// ------------
int MultipleInputOperation::mainPartWidth() const
{
    return OPERATION_GRAPHIC_WIDTH;
}

int MultipleInputOperation::mainPartHeight() const
{
    const int height =
        (inputConnections.size()+1)*MULT_INPUT_OPERATION_INPUTS_SPACING;
    return OPERATION_GRAPHIC_WIDTH > height ?
        OPERATION_GRAPHIC_WIDTH : height;
}

void MultipleInputOperation::drawMainPart(MainDrawingWindow& dWindow,
                                          int col, int yCoord) const
{
    MainDrawingWindow::Point v[4];
    const int midx = col*COLUMN_WIDTH+COLUMN_WIDTH/2;
    const int height = mainPartHeight();

    v[0].x = midx - OPERATION_GRAPHIC_WIDTH/2;
    v[0].y = yCoord - height/2;
    v[1].x = v[0].x;
    v[1].y = v[0].y + height;
    v[2].x = v[1].x + OPERATION_GRAPHIC_WIDTH;
    v[2].y = v[1].y - OPERATION_GRAPHIC_WIDTH/2;
    v[3].x = v[2].x;
    v[3].y = v[0].y + OPERATION_GRAPHIC_WIDTH/2;

    dWindow.polygon(v, 4);
    dWindow.text(col*COLUMN_WIDTH, yCoord-DRAWING_WINDOW_FONT_SIZE/2,
                 getOperationName());

    // Assumption: There will never be more than 10^10-1 input connections
    static char inputNumberStr[11];

    const int textX = midx - OPERATION_GRAPHIC_WIDTH/2;
    int textY =
        yCoord -
        MULT_INPUT_OPERATION_INPUTS_SPACING*inputConnections.size()/2 +
        MULT_INPUT_OPERATION_INPUTS_SPACING/2 -
        DRAWING_WINDOW_FONT_SIZE/2;

    for(unsigned i = 0; i < inputConnections.size(); ++i)
    {
        TVTItem* input = inputConnections[i];
        if(input)
        {
            std::sprintf(inputNumberStr, "%u", i+1);
            inputNumber.setText(inputNumberStr);
            dWindow.text(textX+2, textY, inputNumber);
            drawInputConnection(dWindow,
                                textX, textY+DRAWING_WINDOW_FONT_SIZE/2,
                                input);
        }
        textY += MULT_INPUT_OPERATION_INPUTS_SPACING;
    }
}

bool MultipleInputOperation::connectInput(TVTItem* inputItem)
{
    for(unsigned i=0; i<inputConnections.size(); ++i)
    {
        if(!inputConnections[i])
        {
            inputConnections[i] = inputItem;
            return true;
        }
    }

    inputConnections.push_back(inputItem);
    return true;
}

void MultipleInputOperation::disconnectInput(TVTItem* inputItem)
{
    for(unsigned i=inputConnections.size(); i > 0;)
    {
        --i;
        if(inputConnections[i] == inputItem)
        {
            inputConnections[i] = 0;
            if(i == inputConnections.size()-1)
            {
                while(!inputConnections.empty() &&
                      inputConnections.back() == 0)
                    inputConnections.pop_back();
            }
            break;
        }
    }
}

void MultipleInputOperation::disconnectAllInput()
{
    while(!inputConnections.empty())
        inputConnections.back()->disconnectOutputFrom(this);
}

bool MultipleInputOperation::canBeMovedLeft() const
{
    if(getColumn() == 2) return false;
    for(unsigned i = 0; i < inputConnections.size(); ++i)
        if(inputConnections[i] &&
           inputConnections[i]->getColumn() == getColumn()-2)
            return false;
    return true;
}
