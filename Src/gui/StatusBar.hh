#ifndef STATUSBAR_HH
#define STATUSBAR_HH

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#endif

class StatusBar
{
 public:
    inline StatusBar();
    inline ~StatusBar();

    inline GtkWidget* create();

    inline void setText(const char* text);
    inline void clear();


 private:
    GtkWidget* statusBar;
    guint contextID;
    bool hasText;
    const char* lastShownText;

    StatusBar(const StatusBar&);
    StatusBar& operator=(const StatusBar&);
};


inline StatusBar::StatusBar():
    statusBar(0),
    hasText(false),
    lastShownText(0)
{}

inline StatusBar::~StatusBar()
{
    //if(statusBar) gtk_widget_destroy(statusBar);
}

inline GtkWidget* StatusBar::create()
{
    statusBar = gtk_statusbar_new();
    contextID = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusBar),
                                             "Status");
    gtk_widget_show(statusBar);
    return statusBar;
}

inline void StatusBar::setText(const char* text)
{
    if(statusBar && text != lastShownText)
    {
        if(hasText) gtk_statusbar_pop(GTK_STATUSBAR(statusBar), contextID);
        gtk_statusbar_push(GTK_STATUSBAR(statusBar), contextID, text);
        hasText = true;
        lastShownText = text;
    }
}

inline void StatusBar::clear()
{
    if(hasText) gtk_statusbar_pop(GTK_STATUSBAR(statusBar), contextID);
    hasText = false;
    lastShownText = 0;
}

#endif
