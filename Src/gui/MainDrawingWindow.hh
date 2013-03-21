#ifndef MAIN_DRAWING_WINDOW_HH
#define MAIN_DRAWING_WINDOW_HH

#include "PopupMenu.hh"
#include "TextDraw.hh"

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#include <vector>
#endif

//===================
// MainDrawingWindow
//===================
class MainDrawingWindow
{
 public:
    typedef GdkPoint Point;
    typedef TextDraw Text;

    MainDrawingWindow();
    ~MainDrawingWindow();

    GtkWidget* initialize(GtkWindow* parentWindow,
                          class StatusBar& mainStatusBar);

    void setDrawingColor(unsigned red, unsigned green, unsigned blue);

    inline void line(int x1, int y1, int x2, int y2);
    inline void lines(Point* points, unsigned amount);
    inline void rectangle(int x, int y, int w, int h, bool filled=false);
    inline void polygon(Point* points, unsigned amount, bool filled=false);
    inline void circle(int x, int y, int radius,
                       int startAngle = 0, int degrees = 360);
    inline void text(int x, int y, const Text& t);

    inline void refreshArea(int x=0, int y=0, int width=0, int height=0);

    // Recalculates and refreshes the whole drawing area:
    void refreshAll();


    void connectItemOutput(class TVTItem* sourceItem,
                           int lineStartX, int lineStartY);
    void disconnectItemOutput(class TVTItem* sourceItem,
                              int lineStartX, int lineStartY);
    void copyItem(class TVTItem* item);
    void removeItem(class TVTItem* item);

    bool queryDialog(const char* title, const char* question);


    operator GtkWindow*() { return parent; }


//----------------------------------------------------------------------
 private:
//----------------------------------------------------------------------
    GtkWindow* parent;
    GdkPixmap* pixmap;
    GtkWidget* drawingArea; // GtkDrawingArea*
    GdkGC* drawingGC;
    GdkColormap* colorMap;
    class StatusBar* statusBar;
    int pixmapWidth, pixmapHeight;
    int columns;

    std::vector<class TVTItem*> items;
    std::vector<class TVTItem*> itemsToBeDeleted;

    int clickedColumn, clickedYCoord;

    class TVTItem* draggedItem;
    int draggedItemOriginalY, draggedItemCurrentY;
    bool dragRecursively;

    class TVTItem* itemBeingConnected;
    class LineDrag* connectionLine;
    bool itemIsBeingDisconnected;

    PopupMenu sourceColumnPopupMenu, operationColumnPopupMenu;

    void resizePixmap(int xSize, int ySize);
    void recalculatePixmapSize();
    void redrawAll();

    static gboolean configureEvent(GtkWidget*, GdkEventConfigure*,
                                   MainDrawingWindow*);
    static gboolean exposeEvent(GtkWidget*, GdkEventExpose*,
                                MainDrawingWindow*);
    static gboolean mouseButtonPressEvent(GtkWidget*, GdkEventButton*,
                                          MainDrawingWindow*);
    static gboolean mouseButtonReleaseEvent(GtkWidget*, GdkEventButton*,
                                            MainDrawingWindow*);
    static gboolean motionEvent(GtkWidget*, GdkEventMotion*,
                                MainDrawingWindow*);
    static gboolean leaveEvent(GtkWidget*, GdkEventCrossing*,
                               MainDrawingWindow*);

    static const GtkItemFactoryEntry sourceColumnPopupMenuItems[];
    static const GtkItemFactoryEntry operationColumnPopupMenuItems[];
    void updateSourceColumnMenuSensitivity();
    void updateOperationColumnMenuSensitivity();
    static void newSourceProcessCallBack(gpointer, guint, GtkWidget*);
    static void newOperationCallBack(gpointer, guint, GtkWidget*);
    static void insertColumnCallBack(gpointer, guint, GtkWidget*);
    static void unremoveItemCallBack(gpointer, guint, GtkWidget*);

    MainDrawingWindow(const MainDrawingWindow&);
    MainDrawingWindow& operator=(const MainDrawingWindow&);
};




//========================================================================
// Inline methods
//========================================================================
inline void MainDrawingWindow::line(int x1, int y1, int x2, int y2)
{
    gdk_draw_line(pixmap, drawingGC, x1, y1, x2, y2);
}

inline void MainDrawingWindow::lines(Point* points, unsigned amount)
{
    gdk_draw_lines(pixmap, drawingGC, points, amount);
}

inline void MainDrawingWindow::rectangle(int x, int y, int w, int h,
                                         bool filled)
{
    gdk_draw_rectangle(pixmap, drawingGC, filled, x, y, w, h);
}

inline void MainDrawingWindow::polygon(Point* points, unsigned amount,
                                       bool filled)
{
    gdk_draw_polygon(pixmap, drawingGC, filled, points, amount);
}

inline void MainDrawingWindow::circle(int x, int y, int radius,
                                      int startAngle, int degrees)
{
    gdk_draw_arc(pixmap, drawingGC, FALSE, x-radius/2, y-radius/2,
                 radius, radius, startAngle*64, degrees*64);
}

inline void MainDrawingWindow::text(int x, int y, const Text& t)
{
    t.draw(pixmap, drawingGC, x, y);
}

inline void MainDrawingWindow::refreshArea(int x, int y,
                                           int width, int height)
{
    if(width == 0 && height == 0)
    {
        width = pixmapWidth;
        height = pixmapHeight;
    }
    gdk_draw_pixmap(drawingArea->window,
                    drawingArea->style->fg_gc[GTK_WIDGET_STATE(drawingArea)],
                    pixmap,
                    x, y, x, y, width, height);
}

#endif
