#include "MainTextWindow.hh"

#include <iostream>
#include <cstdlib>

MainTextWindow::MainTextWindow(): textWindow(0)
{}

GtkWidget* MainTextWindow::initialize()
{
    if(textWindow != 0)
    {
        std::cerr <<
            "Fatal error: MainTextWindow initialized more than once.\n";
        std::abort();
    }

    textWindow = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textWindow), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textWindow), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textWindow), FALSE);

    GtkWidget* scrolledTextWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledTextWindow),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type
        (GTK_SCROLLED_WINDOW(scrolledTextWindow), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(scrolledTextWindow), textWindow);

    return scrolledTextWindow;
}

void MainTextWindow::addText(const char* text) const
{
    GtkTextBuffer* buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(textWindow));
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, text, -1);

    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textWindow),
                                 gtk_text_buffer_get_insert(buffer),
                                 0, FALSE, 0, 0);
}

void MainTextWindow::clear() const
{
    GtkTextBuffer* buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(textWindow));
    GtkTextIter begin, end;
    gtk_text_buffer_get_start_iter(buffer, &begin);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_delete(buffer, &begin, &end);
}
