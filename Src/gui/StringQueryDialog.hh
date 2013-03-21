#ifndef STRING_QUERY_DIALOG_HH
#define STRING_QUERY_DIALOG_HH

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#endif


// Asks a string of text from the user
// ===================================

class StringQueryDialog
{
 public:
    inline StringQueryDialog();
    inline ~StringQueryDialog();

    // Returns 0 if the user cancelled, else the entered text.
    inline const char* run(GtkWindow* parentWindow,
                           const char* dialogTitle,
                           const char* text,
                           const char* label = 0);


//--------------------------------------------------------------------------
 private:
//--------------------------------------------------------------------------
    GtkWidget* dialog;

    StringQueryDialog(const StringQueryDialog&);
    StringQueryDialog& operator=(const StringQueryDialog&);
};

inline StringQueryDialog::StringQueryDialog():
    dialog(0)
{}

inline StringQueryDialog::~StringQueryDialog()
{
    if(dialog)
        gtk_widget_destroy(dialog);
}

inline const char* StringQueryDialog::run(GtkWindow* parentWindow,
                                          const char* dialogTitle,
                                          const char* text,
                                          const char* label)
{
    GtkWidget* textEntry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(textEntry), text);
    gtk_editable_select_region(GTK_EDITABLE(textEntry), 0, -1);
    gtk_entry_set_activates_default(GTK_ENTRY(textEntry), TRUE);

    dialog =
        gtk_dialog_new_with_buttons(dialogTitle, parentWindow,
                                    GtkDialogFlags
                                    (GTK_DIALOG_MODAL |
                                     GTK_DIALOG_DESTROY_WITH_PARENT),
                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                    GTK_STOCK_OK, GTK_RESPONSE_OK,
                                    NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

    if(label)
    {
        GtkWidget* dialogLabel = gtk_label_new(label);
        gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox),
                          dialogLabel);
    }

    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), textEntry);

    gtk_widget_show_all(dialog);

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    const char* retval = 0;
    if(result == GTK_RESPONSE_OK)
        retval = gtk_entry_get_text(GTK_ENTRY(textEntry));

    return retval;
}

#endif
