// Base class for TVT items (source LSTS files, operations, etc)
// -------------------------------------------------------------
//   These are items which show up in the drawing window.
//   Items consist of two parts: The main item graphic (the source file,
// operation, etc) and its resulting LSTS graphic (which is drawn in
// the next column).
//   TVTItem knows its location in the drawing window. Derived classes
// only need to define the width and height of their main item graphic
// and offer a method to draw it (TVTItem takes care of handling and
// drawing the resulting LSTS graphic).


#ifndef TVTITEM_HH
#define TVTITEM_HH

#include "MainDrawingWindow.hh"
#include "PopupMenu.hh"
#include "constants.hh"

#include <vector>

class TVTItem
{
 public:
    TVTItem(int col, int yCoord, MainDrawingWindow& dWindow,
            const PopupMenu::MenuItems& mainPartPopupMenuItems);
    virtual ~TVTItem();

    void draw(MainDrawingWindow& dWindow) const;
    void dragVertically(int offsetChange, class MainDrawingWindow& dWindow);
    void moveFollowersBy(int offsetChange);

    inline bool coordinatesAreInsideItem(int x, int y) const;

    inline const PopupMenu& getPopupMenu(int x, int y) const;


    // Returns false if there was already something connected to the input,
    // else true:
    bool connectOutputTo(TVTItem* target);
    bool isConnectedTo(TVTItem* target);
    void disconnectOutputFrom(TVTItem* target);
    void disconnectEverything();


    int getColumn() const { return column; }
    void setColumn(int newColumn) { column = newColumn; }
    int getVerticalOffset() const { return verticalOffset; }
    inline int getMaximumY() const;


    struct Coords
    {
        int x, y;
        Coords(int ix=0, int iy=0): x(ix), y(iy) {}
    };

    inline Coords getOutputConnectionCoords() const;

    bool isDimmed() const { return dimmed; }

    TVTItem* createCopy(int newVerticalOffset);

//------------------------------------------------------------
 protected:
//------------------------------------------------------------
    unsigned long getItemID() const { return itemID; }

    virtual int mainPartWidth() const = 0;
    virtual int mainPartHeight() const = 0;
    virtual void drawMainPart(MainDrawingWindow& dWindow,
                              int col, int yCoord) const = 0;

    virtual TVTItem* clone(int newVerticalOffset,
                           MainDrawingWindow& dWindow) = 0;

    void drawLSTSSymbol(MainDrawingWindow&, int xCoord, int yCoord,
                        int size) const;

    virtual bool connectInput(TVTItem* inputItem);
    virtual void disconnectInput(TVTItem* inputItem);
    virtual void disconnectAllInput();


    virtual void updateMainPartMenuSensitivity(const PopupMenu&) const;
    virtual void updateResultingLSTSMenuSensitivity(const PopupMenu&) const;

    void pullLeft();
    virtual bool canBeMovedLeft() const;
    void changeDimOnThisItem();
    void changeDimOnThisSubtree();

    inline void callRefreshAll();

    typedef std::vector<TVTItem*>::iterator ociter;
    std::vector<TVTItem*> outputConnections;
    TVTItem* inputConnection;

//------------------------------------------------------------
 private:
//------------------------------------------------------------
    unsigned long itemID;

    MainDrawingWindow& parentDWindow;
    int column, verticalOffset;
    unsigned resultStatesAmount, resultTransitionsAmount;
    MainDrawingWindow::Text resultLSTSInfo;
    MainDrawingWindow::Text resultName;
    PopupMenu mainPartPopupMenu, resultingLSTSPopupMenu;
    bool traverseFlag, dimmed;

    int drawResultingLSTSPart(MainDrawingWindow& dWindow) const;
    void setResultAmountsText();

    inline bool coordinatesAreInsideMainPart(int x, int y) const;
    inline bool coordinatesAreInsideResultingLSTS(int x, int y)const;

    void resetTraverseFlags();
    void leftPullRecursion();
    void moveFollowersRecursion(int offsetChange);
    void changeDimRecursion(bool dim);

    static const GtkItemFactoryEntry resultLSTSDefaultPopupMenuItems[];
    static const GtkItemFactoryEntry mainPartDefaultPopupMenuItems[];
    static void buildCallBack(gpointer, guint, GtkWidget*);
    static void viewCallBack(gpointer, guint, GtkWidget*);
    static void connectOutputCallBack(gpointer, guint, GtkWidget*);
    static void disconnectOutputCallBack(gpointer, guint, GtkWidget*);
    static void removeCallBack(gpointer, guint, GtkWidget*);
    static void copyCallBack(gpointer, guint, GtkWidget*);
    static void setNameCallBack(gpointer, guint, GtkWidget*);

    TVTItem(const TVTItem&);
    TVTItem& operator=(const TVTItem&);
};


inline bool TVTItem::coordinatesAreInsideMainPart(int x, int y) const
{
    const int width = mainPartWidth(), height = mainPartHeight();
    const int minx = column*COLUMN_WIDTH + COLUMN_WIDTH/2 - width/2;
    const int maxx = minx + width;
    const int miny = verticalOffset - height/2;
    const int maxy = miny + height;

    return x >= minx && x <= maxx && y >= miny && y <= maxy;
}

inline bool
TVTItem::coordinatesAreInsideResultingLSTS(int x, int y) const
{
    const int minx =
        (column+1)*COLUMN_WIDTH + COLUMN_WIDTH/2 - RESULT_LSTS_GRAPHIC_WIDTH/2;
    const int maxx = minx + RESULT_LSTS_GRAPHIC_WIDTH;
    const int miny = verticalOffset - RESULT_LSTS_GRAPHIC_HEIGHT/2;
    const int maxy = miny + RESULT_LSTS_GRAPHIC_HEIGHT;

    return x >= minx && x <= maxx && y >= miny && y <= maxy;
}

inline bool TVTItem::coordinatesAreInsideItem(int x, int y) const
{
    return
        coordinatesAreInsideMainPart(x, y) ||
        coordinatesAreInsideResultingLSTS(x, y);
}

inline const PopupMenu& TVTItem::getPopupMenu(int x, int y) const
{
    if(coordinatesAreInsideResultingLSTS(x, y))
    {
        updateResultingLSTSMenuSensitivity(resultingLSTSPopupMenu);
        return resultingLSTSPopupMenu;
    }
    updateMainPartMenuSensitivity(mainPartPopupMenu);
    return mainPartPopupMenu;
}

inline void TVTItem::callRefreshAll()
{
    parentDWindow.refreshAll();
}


inline TVTItem::Coords TVTItem::getOutputConnectionCoords() const
{
    return Coords((column+1)*COLUMN_WIDTH+COLUMN_WIDTH/2 +
                  RESULT_LSTS_GRAPHIC_WIDTH/2,
                  verticalOffset);
}

inline int TVTItem::getMaximumY() const
{
    return verticalOffset+mainPartHeight()/2;
}

#endif
