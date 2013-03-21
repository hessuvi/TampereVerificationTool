// Graphical LSTS editor
// ---------------------

#ifndef LSTS_EDITOR_HH
#define LSTS_EDITOR_HH

#include "StatusBar.hh"

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#include <string>
#endif

class LSTSEditor
{
 public:
    LSTSEditor(const char* lstsName, const char* filename = "test.lsts",
               bool editInputLanguage = false);
    ~LSTSEditor();

    void run(bool callGtkMain = false);

    void changeLSTSName(const char* newName);
    void changeFilename(const char* newName);

    // Return = true if LSTS is too big
    bool readLSTSandRun(bool overrideCheck = false);

//----------------------------------------------------------------------
 private:
//----------------------------------------------------------------------
    struct Coords
    {
        int x, y;
        Coords(int ix=0, int iy=0): x(ix), y(iy) {}
    };

    std::string lstsAbstractName;
    std::string lstsFilename;
    bool readLSTSafterStart, overrideReadingCheck, readingLSTSFailed;
    bool quitGTKAfterClosing, thisIsAnILangEditor;

    Coords mainWindowPosition, mainWindowSize;
    GtkWidget* mainEditorWindow;
    GtkWidget* lstsEditorPanedWindow;
    GtkWidget* iLangEditorPanedWindow;
    int lstsEditorPanedWindowPosition, actionNameEditorPanePosition;
    int iLangEditorPanedWindowPosition;

    StatusBar editorWindowStatusBar;
    class ActionNameEditor* actionNameWindow;
    class LSTSEditorMainDrawingWindow* mainDrawingWindow;
    class ILangEditorTextWindow* iLangEditorWindow;

    void saveLSTS();
    void setMainWindowTitle();
    void closeMainWindow();

    static gboolean configureEvent(GtkWidget*, GdkEventConfigure*,
                                   LSTSEditor* instance);
    static gboolean closeMainWindowEvent(GtkWidget*, GdkEvent*,
                                         LSTSEditor* instance);

    static GtkItemFactoryEntry menuItems[];
    static void saveMenuCallback(gpointer, guint, GtkWidget*);
    static void importMenuCallback(gpointer, guint, GtkWidget*);
    static void closeWindowMenuCallback(gpointer, guint, GtkWidget*);
    static void calculateLayoutMenuCallback(gpointer, guint, GtkWidget*);
    static void deleteAllMenuCallback(gpointer, guint, GtkWidget*);

    LSTSEditor(const LSTSEditor&);
    LSTSEditor& operator=(const LSTSEditor&);
};

#endif
