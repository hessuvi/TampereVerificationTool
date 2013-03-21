// Base class for source process files (input language and raw lsts files)
// -----------------------------------------------------------------------
//   Knows the (abstract) name of the input process and draws the base
// main graphic (a folded paper).


#ifndef SOURCEPROCESS_HH
#define SOURCEPROCESS_HH


#include "TVTItem.hh"
#include "MainDrawingWindow.hh"

class SourceProcess: public TVTItem
{
 public:
    SourceProcess(int col, int yCoord,
                  MainDrawingWindow& dWindow, const char* name);


//------------------------------------------------------------
 protected:
//------------------------------------------------------------
    virtual int mainPartWidth() const;
    virtual int mainPartHeight() const;
    virtual void drawMainPart(MainDrawingWindow& dWindow,
                              int col, int yCoord) const = 0;

    virtual void edit() = 0;
    virtual void hasBeenRenamed(const char* newName);
    const char* getSourceProcessName() const { return sourceName.getText(); }

//------------------------------------------------------------
 private:
//------------------------------------------------------------
    MainDrawingWindow::Text sourceName;

    static const GtkItemFactoryEntry sourceProcessPopupMenuItems[];
    static void editCallBack(gpointer, guint, GtkWidget*);
    static void renameCallBack(gpointer, guint, GtkWidget*);

    SourceProcess(const SourceProcess&);
    SourceProcess& operator=(const SourceProcess&);
};

#endif
