#ifndef LINE_DRAG_HH
#define LINE_DRAG_HH

#include "DrawingTools.hh"

class LineDrag
{
 public:
    inline LineDrag(GtkWidget* drawingArea, GdkPixmap* pixmap,
                    int startX, int startY,
                    bool useDashedLine = false);
    inline ~LineDrag();

    inline void lineTo(int endX, int endY);


//--------------------------------------------------------------------------
 private:
    GtkWidget* dArea;
    GdkPixmap* shownPixmap;
    GdkPixmap* originalPixmap;
    GdkGC* lineGC;
    GdkGC* mappedGC;
    const int sx, sy;
    int ex, ey;

    LineDrag(const LineDrag&);
    LineDrag& operator=(const LineDrag&);
};


inline LineDrag::LineDrag(GtkWidget* drawingArea,
                          GdkPixmap* pixmap,
                          int startX, int startY,
                          bool useDashedLine):
    dArea(drawingArea), shownPixmap(pixmap),
    sx(startX), sy(startY), ex(startX), ey(startY)
{
    gint pixmapWidth, pixmapHeight;
    gdk_drawable_get_size(pixmap, &pixmapWidth, &pixmapHeight);
    originalPixmap = gdk_pixmap_new(dArea->window,
                                    pixmapWidth,
                                    pixmapHeight, -1);
    gdk_draw_drawable(originalPixmap,
                      dArea->style->fg_gc[GTK_WIDGET_STATE(dArea)],
                      pixmap, 0, 0, 0, 0, -1, -1);

    lineGC = gdk_gc_new(pixmap);

    mappedGC = gdk_gc_new(pixmap);
    gdk_gc_set_fill(mappedGC, GDK_TILED);
    gdk_gc_set_tile(mappedGC, originalPixmap);

    if(useDashedLine)
    {
        gint8 dashPattern[] = { 4, 4 };
        gdk_gc_set_dashes(lineGC, 0, dashPattern, 2);
        gdk_gc_set_line_attributes(lineGC, 1,
                                   GDK_LINE_ON_OFF_DASH,
                                   GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
        gdk_gc_set_dashes(mappedGC, 0, dashPattern, 2);
        gdk_gc_set_line_attributes(mappedGC, 1,
                                   GDK_LINE_ON_OFF_DASH,
                                   GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
    }
}

inline LineDrag::~LineDrag()
{
    g_object_unref(originalPixmap);
    g_object_unref(lineGC);
    g_object_unref(mappedGC);
}

inline void LineDrag::lineTo(int newEndX, int newEndY)
{
    int minx = tools::min3(sx, ex, newEndX),
        miny = tools::min3(sy, ey, newEndY);
    int width = tools::max3(sx, ex, newEndX) - minx + 1;
    int height = tools::max3(sy, ey, newEndY) - miny + 1;

    gdk_draw_line(shownPixmap, mappedGC, sx, sy, ex, ey);
    gdk_draw_line(shownPixmap, lineGC, sx, sy, newEndX, newEndY);

    gdk_draw_pixmap(dArea->window,
                    dArea->style->fg_gc[GTK_WIDGET_STATE(dArea)],
                    shownPixmap,
                    minx, miny, minx, miny, width, height);

    ex = newEndX;
    ey = newEndY;
}


#endif
