#ifndef DRAWING_TOOLS_HH
#define DRAWING_TOOLS_HH

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#include <cstdlib>
#endif

namespace tools
{
    inline int min3(int i1, int i2, int i3)
    {
        return i1 < i2 ? (i1 < i3 ? i1 : i3) : (i2 < i3 ? i2 : i3);
    }

    inline int max3(int i1, int i2, int i3)
    {
        return i1 > i2 ? (i1 > i3 ? i1 : i3) : (i2 > i3 ? i2 : i3);
    }

    inline int double2int(double d)
    {
        return d < 0 ? -int(-d+.5) : int(d+.5);
    }

    void drawArrow(GdkPixmap* pixmap, GdkGC* gc,
                   GdkPoint* points, unsigned amount, int trim,
                   bool filledArrowHead = true);

    inline void generateRandomColor(GdkColor& color,
                                    int min1, int max1, int min2, int max2)
    {
        int r = int(min1+(double(max1-min1)*rand())/RAND_MAX);
        int g = int(min1+(double(max1-min1)*rand())/RAND_MAX);
        int b = int(min2+(double(max2-min2)*rand())/RAND_MAX);
        if(rand()%2 == 0) { int tmp = r; r = g; g = tmp; }
        if(rand()%2 == 0) { int tmp = r; r = b; b = tmp; }
        if(rand()%2 == 0) { int tmp = g; g = b; b = tmp; }
        color.red = r;
        color.green = g;
        color.blue = b;
    }
}

#endif
