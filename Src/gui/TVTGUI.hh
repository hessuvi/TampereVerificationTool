#ifndef TVTGUI_HH
#define TVTGUI_HH

#include "MainDrawingWindow.hh"
#include "MainTextWindow.hh"
#include "StatusBar.hh"

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#endif

class TVTGUI
{
 public:
    TVTGUI(int& argc, char**& argv);

    void mainLoop();



//========================================================================
 private:
    MainTextWindow mainTextWindow;
    MainDrawingWindow mainDrawingWindow;
    GtkWidget* mainPanedWindow;
    GtkItemFactory* mainMenuItemFactory;
    StatusBar mainStatusBar;


    static void mainPanedWindowResize(GtkWidget* panedWindow,
                                      GtkAllocation*, gpointer);

    static GtkItemFactoryEntry menuItems[];
    static void menuQuitProgram();
    static void menuNewProjectCallback(gpointer, guint, GtkWidget*);
    static void menuOpenProjectCallback(gpointer, guint, GtkWidget*);
    static void menuEditCallback(gpointer, guint, GtkWidget*);
    void enableUndo(gboolean);
    void enableRedo(gboolean);

    TVTGUI(const TVTGUI&);
    TVTGUI& operator=(const TVTGUI&);
};

#endif
