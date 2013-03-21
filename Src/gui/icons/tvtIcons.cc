#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#endif

namespace
{
#include "icondata.cci"

    struct TVTIconData
    {
        const unsigned char* data;
        const char* name;
    };

    TVTIconData tvtIcons[] =
    {
        { SetNameIconData16x16, "tvt-set-name" },
        { ConnectOutputIconData16x16, "tvt-connect-output" },
        { DisconnectOutputIconData16x16, "tvt-disconnect-output" },
        { PushNextColumnIconData16x16, "tvt-push-to-next-column" },
        { PullPrevColumnIconData16x16, "tvt-pull-to-prev-column" },
        { MovePrevColumnIconData16x16, "tvt-move-to-prev-column" },
        { RemoveIconData16x16, "tvt-remove" },
        { RenameIconData16x16, "tvt-rename" },
        { ViewIconData16x16, "tvt-view" }
    };
}

void registerTVTIcons()
{
    GtkIconFactory* iconFactory = gtk_icon_factory_new();

    for(unsigned i = 0; i < sizeof(tvtIcons)/sizeof(tvtIcons[0]); ++i)
    {
        GdkPixbuf* pixbuf =
            gdk_pixbuf_new_from_data(tvtIcons[i].data, GDK_COLORSPACE_RGB,
                                     TRUE, 8, 16, 16, 16*4, NULL, NULL);
        GtkIconSet* iconSet =
            gtk_icon_set_new_from_pixbuf(pixbuf);
        gtk_icon_factory_add(iconFactory, tvtIcons[i].name, iconSet);

        g_object_unref(pixbuf);
    }

    gtk_icon_factory_add_default(iconFactory);
    g_object_unref(iconFactory);
}
