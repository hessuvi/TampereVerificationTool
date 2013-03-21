#include "DrawingTools.hh"
#include "constants.hh"

#include <cmath>

void tools::drawArrow(GdkPixmap* pixmap, GdkGC* gc,
                      GdkPoint* points, unsigned amount, int trim,
                      bool filledArrowHead)
{
    double vx, vy;

    if(trim)
    {
        // Trim beginning:
        vx = points[1].x - points[0].x;
        vy = points[1].y - points[0].y;
        double vlen = std::sqrt(vx*vx+vy*vy);
        vx /= vlen; vy /= vlen;
        const int tx = int(vx*trim);
        const int ty = int(vy*trim);
        points[0].x += tx;
        points[0].y += ty;

        // Trim end:
        if(amount == 2)
        {
            points[1].x -= tx;
            points[1].y -= ty;
        }
        else
        {
            vx = points[amount-1].x - points[amount-2].x;
            vy = points[amount-1].y - points[amount-2].y;
            vlen = std::sqrt(vx*vx+vy*vy);
            vx /= vlen; vy /= vlen;
            points[amount-1].x -= int(vx*trim);
            points[amount-1].y -= int(vy*trim);
        }
    }
    else
    {
        vx = points[amount-1].x - points[amount-2].x;
        vy = points[amount-1].y - points[amount-2].y;
        double vlen = std::sqrt(vx*vx+vy*vy);
        vx /= vlen; vy /= vlen;
    }

    // Create arrow head:
    GdkPoint arrow[4] = { { points[amount-1].x, points[amount-1].y } };
    arrow[1].x = double2int(arrow[0].x - vx*LSTS_VIEW_ARROW_HEAD_LENGTH -
                            vy*LSTS_VIEW_ARROW_HEAD_LENGTH/3);
    arrow[1].y = double2int(arrow[0].y - vy*LSTS_VIEW_ARROW_HEAD_LENGTH +
                            vx*LSTS_VIEW_ARROW_HEAD_LENGTH/3);
    arrow[2].x = double2int(arrow[0].x - vx*LSTS_VIEW_ARROW_HEAD_LENGTH*.75);
    arrow[2].y = double2int(arrow[0].y - vy*LSTS_VIEW_ARROW_HEAD_LENGTH*.75);
    arrow[3].x = double2int(arrow[0].x - vx*LSTS_VIEW_ARROW_HEAD_LENGTH +
                            vy*LSTS_VIEW_ARROW_HEAD_LENGTH/3);
    arrow[3].y = double2int(arrow[0].y - vy*LSTS_VIEW_ARROW_HEAD_LENGTH -
                            vx*LSTS_VIEW_ARROW_HEAD_LENGTH/3);

    // Draw:
    gdk_draw_lines(pixmap, gc, points, amount);
    gdk_draw_polygon(pixmap, gc, filledArrowHead, arrow, 4);
}
