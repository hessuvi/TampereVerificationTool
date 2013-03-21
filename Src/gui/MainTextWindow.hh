#ifndef MAIN_TEXT_WINDOW_HH
#define MAIN_TEXT_WINDOW_HH

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#endif

class MainTextWindow
{
 public:
    MainTextWindow();

    GtkWidget* initialize();

    void addText(const char* text) const;
    void clear() const;


 private:
    GtkWidget* textWindow;

    MainTextWindow(const MainTextWindow&);
    MainTextWindow& operator=(const MainTextWindow&);
};


#endif
