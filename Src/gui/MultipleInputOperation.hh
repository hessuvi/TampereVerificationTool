// Base class for TVT operations with more than one input
// ------------------------------------------------------

#ifndef MULTIPLE_INPUT_OPERATION_HH
#define MULTIPLE_INPUT_OPERATION_HH

#include "Operation.hh"

#include <vector>

class MultipleInputOperation: public Operation
{
 public:
    MultipleInputOperation(int col, int yCoord, MainDrawingWindow& dWindow,
                           const char* operationName);




//------------------------------------------------------------
 protected:
//------------------------------------------------------------
    virtual int mainPartWidth() const;
    virtual int mainPartHeight() const;
    virtual void drawMainPart(MainDrawingWindow& dWindow,
                              int col, int yCoord) const;

    virtual bool connectInput(TVTItem* inputItem);
    virtual void disconnectInput(TVTItem* inputItem);
    virtual void disconnectAllInput();

    virtual bool canBeMovedLeft() const;

    std::vector<TVTItem*> inputConnections;


//------------------------------------------------------------
 private:
//------------------------------------------------------------
    mutable MainDrawingWindow::Text inputNumber;

    MultipleInputOperation(const MultipleInputOperation&);
    MultipleInputOperation& operator=(const MultipleInputOperation&);
};

#endif
