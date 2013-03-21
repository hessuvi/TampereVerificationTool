// SBReduction operation
// ---------------------

#ifndef OPERATIONSBRED_HH
#define OPERATIONSBRED_HH

#include "Operation.hh"

class OperationSBRed: public Operation
{
 public:
    OperationSBRed(int col, int yCoord, MainDrawingWindow& dWindow);


//------------------------------------------------------------
 protected:
//------------------------------------------------------------
    virtual TVTItem* clone(int newVerticalOffset, MainDrawingWindow& dWindow);

//------------------------------------------------------------
 private:
//------------------------------------------------------------

    OperationSBRed(const OperationSBRed&);
    OperationSBRed& operator=(const OperationSBRed&);
};

#endif
