#include "TVTGUI.hh"
#include "constants.hh"

void registerTVTIcons();


// ========================================================================
// Main menu contents:
// ========================================================================
GtkItemFactoryEntry TVTGUI::menuItems[] =
{
    { "/_File", NULL, NULL, 0, "<Branch>", 0 },
    { "/File/_New project...", "<ctrl>n",
      GtkItemFactoryCallback(menuNewProjectCallback), 1,
      "<StockItem>", GTK_STOCK_NEW },
    { "/File/_Open project...", "<ctrl>o",
      GtkItemFactoryCallback(menuOpenProjectCallback), 1,
      "<StockItem>", GTK_STOCK_OPEN },
    { "/File/_Quit", "<ctrl>q", menuQuitProgram, 0, "<StockItem>",
      GTK_STOCK_QUIT },

    { "/_Edit", NULL, NULL, 0, "<Branch>", 0 },
    { "/Edit/_Undo", "<ctrl>z", GtkItemFactoryCallback(menuEditCallback), 1,
      "<StockItem>", GTK_STOCK_UNDO },
    { "/Edit/_Redo", "<ctrl>r", GtkItemFactoryCallback(menuEditCallback), 2,
      "<StockItem>", GTK_STOCK_REDO }
};



// ========================================================================
// Main window creation:
// ========================================================================
TVTGUI::TVTGUI(int& argc, char**& argv)
{
    gtk_init(&argc, &argv); // GTK initialization

    registerTVTIcons();

    // Create main window:
    // ------------------
    GtkWidget* mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(mainWindow), MAIN_WINDOW_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(mainWindow),
                                DEFAULT_APP_WINDOW_SIZE);

    // Connect delete event (to quit the program):
    // ------------------------------------------
    g_signal_connect(G_OBJECT(mainWindow), "delete_event",
                     G_CALLBACK(gtk_main_quit), NULL);

    // Create main menu:
    // ----------------
    GtkAccelGroup* accelGroup = gtk_accel_group_new();
    mainMenuItemFactory = gtk_item_factory_new(GTK_TYPE_MENU_BAR,
                                               "<TVTGUIMainMenu>",
                                               accelGroup);
    gtk_item_factory_create_items(mainMenuItemFactory,
                                  sizeof(menuItems)/sizeof(menuItems[0]),
                                  menuItems, this);
    gtk_window_add_accel_group(GTK_WINDOW(mainWindow), accelGroup);
    GtkWidget* mainMenuBar =
        gtk_item_factory_get_widget(mainMenuItemFactory, "<TVTGUIMainMenu>");

    enableUndo(FALSE);
    enableRedo(FALSE);

    // Create status bar:
    // -----------------
    GtkWidget* statusBarWidget = mainStatusBar.create();

    // Create scrolled drawing window:
    // ------------------------------
    GtkWidget* drawingWindow =
        mainDrawingWindow.initialize(GTK_WINDOW(mainWindow), mainStatusBar);

    // Create scrolled text window:
    // ---------------------------
    GtkWidget* textWindow = mainTextWindow.initialize();

    // Create main window contents (paned window):
    // ------------------------------------------
    mainPanedWindow = gtk_vpaned_new();
    gtk_paned_add1(GTK_PANED(mainPanedWindow), drawingWindow);
    gtk_paned_add2(GTK_PANED(mainPanedWindow), textWindow);

    g_signal_connect(G_OBJECT(mainPanedWindow), "size-allocate",
                     G_CALLBACK(mainPanedWindowResize), NULL);

    // Create main window layout (vertical container box):
    // --------------------------------------------------
    GtkWidget* mainVBox = gtk_vbox_new(FALSE, 1);
    gtk_box_pack_start(GTK_BOX(mainVBox), mainMenuBar, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(mainVBox), mainPanedWindow, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(mainVBox), statusBarWidget, FALSE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(mainWindow), mainVBox);

gtk_widget_show_all(mainPanedWindow);

    gtk_widget_show(mainMenuBar);
    gtk_widget_show(mainVBox);
    gtk_widget_show(mainWindow);
}

void TVTGUI::mainLoop()
{
    gtk_main();
}



// ========================================================================
// Main menu calls:
// ========================================================================
void TVTGUI::menuQuitProgram()
{
    gtk_main_quit();
}

#include <iostream>

void TVTGUI::menuNewProjectCallback(gpointer data, guint, GtkWidget*)
{
    std::cout << "New project called." << std::endl;

    TVTGUI* instance = reinterpret_cast<TVTGUI*>(data);
    gtk_widget_show_all(instance->mainPanedWindow);
}

void TVTGUI::menuOpenProjectCallback(gpointer, guint, GtkWidget*)
{
    std::cout << "Open project called." << std::endl;
}

void TVTGUI::menuEditCallback(gpointer, guint index, GtkWidget*)
{
    if(index == 1)
        std::cout << "Undo called." << std::endl;
    else
        std::cout << "Redo called." << std::endl;
}

void TVTGUI::enableUndo(gboolean enable)
{
    gtk_widget_set_sensitive(gtk_item_factory_get_widget(mainMenuItemFactory,
                                                         "/Edit/Undo"),
                             enable);
}

void TVTGUI::enableRedo(gboolean enable)
{
    gtk_widget_set_sensitive(gtk_item_factory_get_widget(mainMenuItemFactory,
                                                         "/Edit/Redo"),
                             enable);
}


// ========================================================================
// Main paned window resizing event:
// ========================================================================
void TVTGUI::mainPanedWindowResize(GtkWidget* panedWindow, GtkAllocation*,
                                   gpointer)
{
    static gint oldHeight = 0;
    static double panePositionFactor = .8;

    gint height = panedWindow->allocation.height;

    if(height != oldHeight)
    {
        oldHeight = height;
        gtk_paned_set_position(GTK_PANED(panedWindow),
                               gint(height*panePositionFactor));
    }
    else
    {
        if(height > 0)
            panePositionFactor =
                double(gtk_paned_get_position(GTK_PANED(panedWindow))) /
                double(height);
    }
}
