#ifndef POPUPMENU_HH
#define POPUPMENU_HH

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#include <vector>
#endif

class PopupMenu
{
 public:
    typedef std::vector<GtkItemFactoryEntry> MenuItems;

    inline PopupMenu();
    inline ~PopupMenu();

    inline void createMenu(const char* name, MenuItems& items, gpointer data);
    inline void popup(guint x, guint y, guint mouseButton, guint32 eventTime)
        const;
    inline void setSensitivity(const char* menuItem, bool enabled) const;
    inline void setToggle(const char* menuItem, bool checked) const;

    inline bool hasBeenCreated() const;

//------------------------------------------------------------------------
 private:
//------------------------------------------------------------------------
    GtkItemFactory* menuItems;

    PopupMenu(const PopupMenu&);
    PopupMenu& operator=(const PopupMenu&);
};


inline PopupMenu::PopupMenu():
    menuItems(0)
{}

inline PopupMenu::~PopupMenu()
{}

inline void PopupMenu::createMenu(const char* name, MenuItems& items,
                                  gpointer data)
{
    menuItems = gtk_item_factory_new(GTK_TYPE_MENU, name, NULL);
    gtk_item_factory_create_items(menuItems, items.size(), &items[0], data);
}

inline void PopupMenu::popup(guint x, guint y, guint mouseButton,
                             guint32 eventTime) const
{
    if(menuItems)
        gtk_item_factory_popup(menuItems, x, y, mouseButton, eventTime);
}

inline void PopupMenu::setSensitivity(const char* menuItem, bool enabled) const
{
    if(menuItems)
        gtk_widget_set_sensitive
            (gtk_item_factory_get_widget(menuItems, menuItem), enabled);
}

inline void PopupMenu::setToggle(const char* menuItem, bool checked) const
{
    gtk_check_menu_item_set_active
        (GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(menuItems, menuItem)),
         checked);
}

inline bool PopupMenu::hasBeenCreated() const
{
    return menuItems != 0;
}

#endif
