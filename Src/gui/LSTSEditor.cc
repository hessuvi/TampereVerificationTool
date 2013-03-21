#include "LSTSEditor.hh"
#include "ActionNameEditor.hh"
#include "LSTSEditorMainDrawingWindow.hh"
#include "ILangEditorTextWindow.hh"
#include "constants.hh"

#include <string>

GtkItemFactoryEntry LSTSEditor::menuItems[] =
{
    { "/_File", NULL, NULL, 0, "<Branch>", 0 },
    { "/File/_Save", "<ctrl>s",
      GtkItemFactoryCallback(saveMenuCallback), 0,
      "<StockItem>", GTK_STOCK_SAVE },
    { "/File/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/File/_Import LSTS...", NULL,
      GtkItemFactoryCallback(importMenuCallback), 0, NULL, NULL },
    { "/File/Import ilang source...", NULL,
      GtkItemFactoryCallback(importMenuCallback), 1, NULL, NULL },
    { "/File/sep2", NULL, NULL, 0, "<Separator>", NULL },
    { "/File/_Close", "<ctrl>w",
      GtkItemFactoryCallback(closeWindowMenuCallback), 0,
      "<StockItem>", GTK_STOCK_CLOSE },

    { "/_Edit", NULL, NULL, 0, "<Branch>", 0 },
    { "/Edit/_Calculate layout", NULL,
      GtkItemFactoryCallback(calculateLayoutMenuCallback), 0, NULL, NULL },
    { "/Edit/_Delete all", NULL,
      GtkItemFactoryCallback(deleteAllMenuCallback), 0, NULL, NULL }
};


// Constructor and destructor
// --------------------------
LSTSEditor::LSTSEditor(const char* lstsName, const char* filename,
                       bool editInputLanguage):
    lstsAbstractName(lstsName),
    lstsFilename(filename),
    readLSTSafterStart(false),
    quitGTKAfterClosing(false),
    thisIsAnILangEditor(editInputLanguage),
    mainWindowPosition(-1, -1),
    mainWindowSize(LSTS_EDITOR_DEFAULT_WINDOW_SIZE),
    mainEditorWindow(0), lstsEditorPanedWindow(0), iLangEditorPanedWindow(0),
    lstsEditorPanedWindowPosition(0), actionNameEditorPanePosition(0),
    iLangEditorPanedWindowPosition(0),
    actionNameWindow(0), mainDrawingWindow(0), iLangEditorWindow(0)
{
}

LSTSEditor::~LSTSEditor()
{
    if(actionNameWindow) delete actionNameWindow;
    if(mainDrawingWindow) delete mainDrawingWindow;
    if(iLangEditorWindow) delete iLangEditorWindow;
}

// Main creation function
// ----------------------
void LSTSEditor::run(bool callGtkMain)
{
    // If already exists, bring it to the top:
    // --------------------------------------
    if(mainEditorWindow)
    {
        gtk_window_present(GTK_WINDOW(mainEditorWindow));
        return;
    }

    // Create main window:
    // ------------------
    mainEditorWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    setMainWindowTitle();

    // Connect signals:
    // ---------------
    g_signal_connect(GTK_OBJECT(mainEditorWindow), "configure-event",
                     G_CALLBACK(configureEvent), this);
    g_signal_connect(G_OBJECT(mainEditorWindow), "delete-event",
                     G_CALLBACK(closeMainWindowEvent), this);

    // Create main menu:
    // ----------------
    GtkAccelGroup* accelGroup = gtk_accel_group_new();
    GtkItemFactory* mainMenuItemFactory =
        gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<LSTSEditorMainMenu>",
                             accelGroup);
    gtk_item_factory_create_items(mainMenuItemFactory,
                                  sizeof(menuItems)/sizeof(menuItems[0]),
                                  menuItems, this);
    gtk_window_add_accel_group(GTK_WINDOW(mainEditorWindow), accelGroup);
    GtkWidget* mainMenuBar =
        gtk_item_factory_get_widget(mainMenuItemFactory,
                                    "<LSTSEditorMainMenu>");

    gtk_widget_set_sensitive
        (gtk_item_factory_get_widget(mainMenuItemFactory,
                                     "/File/Import ilang source..."), FALSE);

    // Create status bar:
    // -----------------
    GtkWidget* statusBarWidget = editorWindowStatusBar.create();

    // Create editing layout:
    // ---------------------
    actionNameWindow = new ActionNameEditor;
    GtkWidget* actionNameWindowWidget =
        actionNameWindow->initialize(GTK_WINDOW(mainEditorWindow),
                                     &editorWindowStatusBar, true);

    GtkWidget* iLangEditorWindowWidget = 0;
    if(thisIsAnILangEditor)
    {
        iLangEditorWindow = new ILangEditorTextWindow;
        iLangEditorWindowWidget =
            iLangEditorWindow->initialize(GTK_WINDOW(mainEditorWindow));
    }

    mainDrawingWindow = new LSTSEditorMainDrawingWindow(thisIsAnILangEditor);
    if(thisIsAnILangEditor)
        mainDrawingWindow->registerEventListener(iLangEditorWindow);
    GtkWidget* drawingWindowWidget =
        mainDrawingWindow->initialize(GTK_WINDOW(mainEditorWindow),
                                      actionNameWindow,
                                      &editorWindowStatusBar);

    lstsEditorPanedWindow = gtk_hpaned_new();
    gtk_paned_add1(GTK_PANED(lstsEditorPanedWindow), actionNameWindowWidget);
    gtk_paned_add2(GTK_PANED(lstsEditorPanedWindow), drawingWindowWidget);
    if(lstsEditorPanedWindowPosition == 0)
        lstsEditorPanedWindowPosition = mainWindowSize.x/6;
    gtk_paned_set_position(GTK_PANED(lstsEditorPanedWindow),
                           lstsEditorPanedWindowPosition);

    if(thisIsAnILangEditor)
    {
        iLangEditorPanedWindow = gtk_hpaned_new();
        gtk_paned_add1(GTK_PANED(iLangEditorPanedWindow),
                       lstsEditorPanedWindow);
        gtk_paned_add2(GTK_PANED(iLangEditorPanedWindow),
                       iLangEditorWindowWidget);
        mainWindowSize.x += mainWindowSize.x/3;
        if(iLangEditorPanedWindowPosition == 0)
            iLangEditorPanedWindowPosition = mainWindowSize.x/2;
        gtk_paned_set_position(GTK_PANED(iLangEditorPanedWindow),
                               iLangEditorPanedWindowPosition);
    }

    // Create main window layout:
    // -------------------------
    GtkWidget* mainVBox = gtk_vbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(mainVBox), mainMenuBar, FALSE, TRUE, 0);
    if(thisIsAnILangEditor)
        gtk_box_pack_start(GTK_BOX(mainVBox), iLangEditorPanedWindow,
                           TRUE, TRUE, 0);
    else
        gtk_box_pack_start(GTK_BOX(mainVBox), lstsEditorPanedWindow,
                           TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(mainVBox), statusBarWidget, FALSE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(mainEditorWindow), mainVBox);

    gtk_window_set_default_size(GTK_WINDOW(mainEditorWindow),
                                mainWindowSize.x, mainWindowSize.y);

    if(mainWindowPosition.x != -1)
        gtk_window_move(GTK_WINDOW(mainEditorWindow),
                        mainWindowPosition.x, mainWindowPosition.y);

    gtk_widget_show_all(mainEditorWindow);

    if(mainWindowPosition.x != -1)
        gtk_window_move(GTK_WINDOW(mainEditorWindow),
                        mainWindowPosition.x, mainWindowPosition.y);

    if(actionNameEditorPanePosition == 0)
        actionNameEditorPanePosition = 60;
    actionNameWindow->setGutterPosition(actionNameEditorPanePosition);

    if(callGtkMain)
    {
        quitGTKAfterClosing = true;
        gtk_main();
    }
}

bool LSTSEditor::readLSTSandRun(bool overrideCheck)
{
    readLSTSafterStart = true;
    overrideReadingCheck = overrideCheck;
    run(true);
    return readingLSTSFailed;
}

void LSTSEditor::saveLSTS()
{
    mainDrawingWindow->saveLSTS(lstsFilename);
}

// Name changing:
// -------------
void LSTSEditor::setMainWindowTitle()
{
    if(mainEditorWindow)
    {
        std::string mainWindowTitle(lstsAbstractName);
        if(mainWindowTitle.empty())
        {
            mainWindowTitle = lstsFilename;
            if(thisIsAnILangEditor)
                mainWindowTitle += " - "ILANG_EDITOR_WINDOW_TITLE;
            else
                mainWindowTitle += " - "LSTS_EDITOR_WINDOW_TITLE;
        }
        else
        {
            mainWindowTitle += " (";
            mainWindowTitle += lstsFilename;
            if(thisIsAnILangEditor)
                mainWindowTitle += ") - "ILANG_EDITOR_WINDOW_TITLE;
            else
                mainWindowTitle += ") - "LSTS_EDITOR_WINDOW_TITLE;
        }
        gtk_window_set_title(GTK_WINDOW(mainEditorWindow),
                             mainWindowTitle.c_str());
    }
}

void LSTSEditor::changeLSTSName(const char* newName)
{
    lstsAbstractName = newName;
    setMainWindowTitle();
}

void LSTSEditor::changeFilename(const char* newName)
{
    lstsFilename = newName;
    setMainWindowTitle();
}

// Close main window:
// -----------------
namespace
{
    gint confirmClose(GtkWidget* parent)
    {
        GtkWidget* dialog =
            gtk_message_dialog_new(GTK_WINDOW(parent),
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_WARNING,
                                   GTK_BUTTONS_CANCEL,
                                   "The LSTS has been modified.\n"
                                   "Are you sure you want to exit?");
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Discard and close",
                              GTK_RESPONSE_CLOSE);
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Save and close",
                              GTK_RESPONSE_OK);
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return response;
    }
}

void LSTSEditor::closeMainWindow()
{
    if(mainEditorWindow)
    {
        // Check if LSTS has been saved:
        if(mainDrawingWindow->hasBeenModified())
        {
            gint response = confirmClose(mainEditorWindow);
            if(response == GTK_RESPONSE_CANCEL)
                return;
            if(response == GTK_RESPONSE_OK)
                saveLSTS();
        }

        // Get dimensions (so that they can be restored if the window is
        // opened again):
        gtk_window_get_position(GTK_WINDOW(mainEditorWindow),
                                &mainWindowPosition.x, &mainWindowPosition.y);
        mainWindowSize.x = mainEditorWindow->allocation.width;
        mainWindowSize.y = mainEditorWindow->allocation.height;
        lstsEditorPanedWindowPosition =
            gtk_paned_get_position(GTK_PANED(lstsEditorPanedWindow));
        actionNameEditorPanePosition = actionNameWindow->getGutterPosition();

        // Close the window:
        delete actionNameWindow; actionNameWindow = 0;
        delete mainDrawingWindow; mainDrawingWindow = 0;
        GtkWidget* window = mainEditorWindow;
        mainEditorWindow = 0;       // Just in case gtk_widget_destroy()
        gtk_widget_destroy(window); // calls closeMainWindowEvent() again.
    }
    if(quitGTKAfterClosing)
        gtk_main_quit();
}


// Event callbacks:
// ---------------
gboolean LSTSEditor::configureEvent(GtkWidget*, GdkEventConfigure*,
                                    LSTSEditor* instance)
{
    if(instance->readLSTSafterStart)
    {
        instance->readLSTSafterStart = false;
        instance->readingLSTSFailed =
            !instance->mainDrawingWindow->readLSTS
            (instance->lstsFilename, instance->overrideReadingCheck, false,
             true);
        if(instance->readingLSTSFailed)
            gtk_main_quit();
    }
    return FALSE;
}

gboolean LSTSEditor::closeMainWindowEvent(GtkWidget*, GdkEvent*,
                                          LSTSEditor* instance)
{
    instance->closeMainWindow();
    return TRUE;
}


// Main menu callbacks:
// -------------------
void LSTSEditor::saveMenuCallback(gpointer ptr, guint, GtkWidget*)
{
    reinterpret_cast<LSTSEditor*>(ptr)->saveLSTS();
}

namespace
{
    std::string filenameToOpen;

    void openDialogGetFilename(GtkWidget*, GtkWidget* dialog)
    {
        filenameToOpen =
            gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog));
        gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    }
    void openDialogCancel(GtkWidget*, GtkWidget* dialog)
    {
        gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    }
}

void LSTSEditor::importMenuCallback(gpointer ptr, guint which, GtkWidget*)
{
    LSTSEditor* instance = reinterpret_cast<LSTSEditor*>(ptr);

    /*
    // Works only with gtk 2.4:
    GtkWidget* dialog =
        gtk_file_chooser_dialog_new ("Open File",
                                     GTK_WINDOW(instance->mainEditorWindow),
                                     GTK_FILE_CHOOSER_ACTION_OPEN,
                                     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                     NULL);

    std::string filename;
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char* f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        filename = f;
        g_free(f);
    }
    gtk_widget_destroy(dialog);
    */

    // In previous versions we have to do it the hard way:
    GtkWidget* dialog =
        gtk_file_selection_new(which ?
                               "Select ilang file" :
                               "Select lsts file");
    gtk_window_set_transient_for(GTK_WINDOW(dialog),
                                 GTK_WINDOW(instance->mainEditorWindow));
    g_signal_connect(GTK_FILE_SELECTION(dialog)->ok_button, "clicked",
                     G_CALLBACK(openDialogGetFilename), dialog);
    g_signal_connect(GTK_FILE_SELECTION(dialog)->cancel_button, "clicked",
                     G_CALLBACK(openDialogCancel), dialog);

    filenameToOpen.clear();
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if(response == GTK_RESPONSE_OK && !filenameToOpen.empty())
    {
        GtkWidget* dialog =
            gtk_message_dialog_new
            (GTK_WINDOW(instance->mainEditorWindow),
             GTK_DIALOG_DESTROY_WITH_PARENT,
             GTK_MESSAGE_WARNING,
             GTK_BUTTONS_YES_NO,
             "The currently edited LSTS data will be replaced by the\n"
             "LSTS being imported. Are you sure you want to continue?");
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        if(response != GTK_RESPONSE_YES)
            return;

        bool readOk = true;
        if(which == 0)
        {
            readOk = instance->mainDrawingWindow->readLSTS(filenameToOpen,
                                                           false, true);
        }

        if(!readOk)
        {
            GtkWidget* dialog =
                gtk_message_dialog_new(GTK_WINDOW(instance->mainEditorWindow),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_OK,
                                       "The lsts is too big to be\n"
                                       "edited in a feasible way.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }
    }
}

void LSTSEditor::closeWindowMenuCallback(gpointer ptr, guint, GtkWidget*)
{
    reinterpret_cast<LSTSEditor*>(ptr)->closeMainWindow();
}

void LSTSEditor::calculateLayoutMenuCallback(gpointer ptr, guint, GtkWidget*)
{
    LSTSEditor* instance = reinterpret_cast<LSTSEditor*>(ptr);

    instance->mainDrawingWindow->calculateLayout();
}

void LSTSEditor::deleteAllMenuCallback(gpointer ptr, guint, GtkWidget*)
{
    LSTSEditor* instance = reinterpret_cast<LSTSEditor*>(ptr);

    GtkWidget* dialog = gtk_message_dialog_new
        (GTK_WINDOW(instance->mainEditorWindow),
         GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
         "Are you sure you want to delete\n"
         "the current LSTS being edited?");
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm deletion");
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if(result == GTK_RESPONSE_YES)
    {
        instance->mainDrawingWindow->deleteEverything();
    }
}
