// Parallel composition operation
// ------------------------------

#ifndef OPERATION_PAR_COMP_HH
#define OPERATION_PAR_COMP_HH

#include "MultipleInputOperation.hh"

class OperationParComp: public MultipleInputOperation
{
 public:
    OperationParComp(int col, int yCoord, MainDrawingWindow& dWindow);


 protected:
    virtual TVTItem* clone(int newVerticalOffset, MainDrawingWindow& dWindow);

 private:
    OperationParComp(const OperationParComp&);
    OperationParComp& operator=(const OperationParComp&);
};

#endif
