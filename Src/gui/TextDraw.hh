// Class for creating text drawable on a pixmap
// --------------------------------------------

#ifndef TEXT_DRAW_HH
#define TEXT_DRAW_HH

#include "StringQueryDialog.hh"
#include "constants.hh"

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#endif


class TextDraw
{
 public:
    inline TextDraw(GtkWindow* parentWindow, const char* text = "",
                    int textWidth = 0, bool alignCenter = true);
    inline ~TextDraw();

    inline TextDraw(const TextDraw&);
    inline const TextDraw& operator=(const TextDraw&);

    inline void setText(const char* text);
    inline const char* getText() const;
    inline bool askString(const char* dialogName);

    inline void setUpwardsAlignment(bool flag=true);

    inline void draw(GdkPixmap* dest, GdkGC* drawingGC,
                     int x, int y) const;

    inline int getTextWidth() const;
    inline int getTextHeight() const;


//----------------------------------------------------------------------
 private:
//----------------------------------------------------------------------
    PangoLayout* layout;
    GtkWindow* parent;
    bool upwardsAlignment;

    inline int verticalOffset() const;
};






inline TextDraw::TextDraw(GtkWindow* parentWindow, const char* text,
                          int textWidth, bool alignCenter):
    parent(parentWindow),
    upwardsAlignment(false)
{
    layout = gtk_widget_create_pango_layout(GTK_WIDGET(parent), text);
    if(textWidth >= 0)
    {
        pango_layout_set_width(layout,
                               textWidth ? textWidth*PANGO_SCALE :
                               COLUMN_WIDTH*PANGO_SCALE);
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
        if(alignCenter)
            pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
    }

    PangoFontDescription* fontDesc =
        pango_context_get_font_description(pango_layout_get_context(layout));
    pango_font_description_set_size(fontDesc,
                                    DRAWING_WINDOW_FONT_SIZE*PANGO_SCALE);
}

inline TextDraw::~TextDraw()
{
    // Should 'layout' be deferenced here? The GTK doc doesn't say...
    g_object_unref(layout);
}

inline TextDraw::TextDraw(const TextDraw& rhs):
    layout(pango_layout_copy(rhs.layout)),
    parent(rhs.parent), upwardsAlignment(rhs.upwardsAlignment)
{
}

inline const TextDraw& TextDraw::operator=(const TextDraw& rhs)
{
    g_object_unref(layout);
    layout = pango_layout_copy(rhs.layout);
    parent = rhs.parent;
    upwardsAlignment = rhs.upwardsAlignment;
    return *this;
}

inline void TextDraw::setText(const char* text)
{
    pango_layout_set_text(layout, text, -1);
}

inline const char* TextDraw::getText() const
{
    return pango_layout_get_text(layout);
}

inline bool TextDraw::askString(const char* dialogName)
{
    StringQueryDialog query;
    const char* newText = query.run(parent, dialogName, getText());

    if(newText)
    {
        setText(newText);
        return true;
    }
    return false;
}

inline void TextDraw::setUpwardsAlignment(bool flag)
{
    upwardsAlignment = flag;
}

inline int TextDraw::getTextWidth() const
{
    int textWidth;
    pango_layout_get_size(layout, &textWidth, NULL);
    return textWidth/PANGO_SCALE;
}

inline int TextDraw::getTextHeight() const
{
    int textHeight;
    pango_layout_get_size(layout, NULL, &textHeight);
    return textHeight/PANGO_SCALE;
}

inline int TextDraw::verticalOffset() const
{
    if(upwardsAlignment)
    {
        return getTextHeight();
    }
    return 0;
}

inline void TextDraw::draw(GdkPixmap* dest, GdkGC* drawingGC,
                           int x, int y) const
{
    gdk_draw_layout(dest, drawingGC, x, y-verticalOffset(), layout);
}

#endif
