// Base class for TVT operations (which take an input and produce an output)
// -------------------------------------------------------------------------
//   Knows its input source (overrideable by inherited classes) and draws
// the base main graphic (an operation symbol) and the name of the operation.

#ifndef OPERATION_HH
#define OPERATION_HH

#include "TVTItem.hh"
#include "MainDrawingWindow.hh"

class Operation: public TVTItem
{
 public:
    Operation(int col, int yCoord, MainDrawingWindow& dWindow,
              const char* operationName);



//------------------------------------------------------------
 protected:
//------------------------------------------------------------
    virtual int mainPartWidth() const;
    virtual int mainPartHeight() const;
    virtual void drawMainPart(MainDrawingWindow& dWindow,
                              int col, int yCoord) const;

    virtual void updateMainPartMenuSensitivity(const PopupMenu&) const;

    void drawInputConnection(MainDrawingWindow& dWindow,
                             int inputX, int inputY,
                             TVTItem* inputItem) const;

    const MainDrawingWindow::Text& getOperationName() const
    { return operationName; }

//------------------------------------------------------------
 private:
//------------------------------------------------------------
    MainDrawingWindow::Text operationName;

    static const GtkItemFactoryEntry operationPopupMenuItems[];
    static void optionsCallBack(gpointer, guint, GtkWidget*);
    static void moveRightCallBack(gpointer, guint, GtkWidget*);
    static void moveLeftCallBack(gpointer, guint, GtkWidget*);
    static void changeItemDimCallBack(gpointer, guint, GtkWidget*);
    static void changeSubtreeDimCallBack(gpointer, guint, GtkWidget*);

    Operation(const Operation&);
    Operation& operator=(const Operation&);
};

#endif
