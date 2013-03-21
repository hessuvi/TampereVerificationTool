/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Juha Nieminen.
*/

// FILE_DES: gtklib.cc: Grafiikka/piirtoalusta/gtk
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: gtklib.cc 1.8 Fri, 27 Feb 2004 16:36:58 +0200 warp $";
// 
// GraphicLibrary:n GTK-toteutus.
//

// $Log:$

#include "gtklib.hh"
#include "constants.hh"

#ifndef MAKEDEPEND
#include <map>
using namespace std;
#endif

#define DOUBLEWIDTH_LINE_TRIG 80

// Gtk:n tarpeellisia ikkunank‰sittelyrutiineja
//=============================================

static GTKLibrary* GTK_Instance=0;
static int GTK_Instances=0;

struct DAreaInfo
{
    GdkPixmap* pm;
    int width, height;
};

typedef map<long,DAreaInfo> PixList;
static PixList pixmap;

static void redrawPixmaps()
{
    for(PixList::iterator iter = pixmap.begin(); iter != pixmap.end(); ++iter)
    {
        GtkWidget* widget = reinterpret_cast<GtkWidget*>(iter->first);
        // Clear the pixmap with a white rectangle
        gdk_draw_rectangle (iter->second.pm,
                            widget->style->white_gc,
                            TRUE,
                            0, 0,
                            widget->allocation.width,
                            widget->allocation.height);

        // Call the user's drawing funktion
        GTK_Instance->Redraw((GTKLibrary::DWindow)widget);

        // Redraw pixmap
        gdk_draw_pixmap(widget->window,
                        widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                        iter->second.pm,
                        widget->allocation.x,
                        widget->allocation.y,
                        widget->allocation.x,
                        widget->allocation.y,
                        widget->allocation.width,
                        widget->allocation.height);
    }
}

// Create a new backing pixmap of the appropriate size
static gint configure_event (GtkWidget *widget, GdkEventConfigure *)
{
    DAreaInfo dai;

    // If the pixmap exists, delete it.
    PixList::iterator i=pixmap.find((long)widget);
    if (i!=pixmap.end())
    {
        gdk_pixmap_unref(pixmap[(long)widget].pm);
        pixmap.erase((long)widget);
    }

    // Create the pixmap
    dai.pm = gdk_pixmap_new(widget->window,
                            widget->allocation.width,
                            widget->allocation.height,
                            -1);
    dai.width = widget->allocation.width;
    dai.height = widget->allocation.height;

    pixmap.insert(PixList::value_type((long)widget, dai));

    // Clear the pixmap with a white rectangle
    gdk_draw_rectangle (pixmap[(long)widget].pm,
                        widget->style->white_gc,
                        TRUE,
                        0, 0,
                        widget->allocation.width,
                        widget->allocation.height);

    // Call the user's drawing funktion
    GTK_Instance->Redraw((GTKLibrary::DWindow)widget);

    return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gint expose_event (GtkWidget *widget, GdkEventExpose *event)
{
    gdk_draw_pixmap(widget->window,
                    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                    pixmap[(long)widget].pm,
                    event->area.x, event->area.y,
                    event->area.x, event->area.y,
                    event->area.width, event->area.height);

    return FALSE;
}

static gint Button_Press_Event(GtkWidget *widget, GdkEventButton *event)
{
    GTK_Instance->MouseClick((GTKLibrary::DWindow)widget,
                             (unsigned)event->x,
                             (unsigned)event->y,
                             (unsigned)event->button);
    return FALSE;
}


//============================================================================

// Konstruktori
//=============
GTKLibrary::GTKLibrary(int& argc, char** &argv):
    GraphicLibrary(argc, argv)
{
    if(GTK_Instances==0)
    {
        gtk_init (&argc, &argv);
        GTK_Instance=this;
    }
    GTK_Instances++;
}

// Destruktori
//============
GTKLibrary::~GTKLibrary()
{
    GTK_Instances--;
    if(GTK_Instances==0)
        GTK_Instance=0;
}


// N‰pp‰inpanallusten k‰sittely:
static void handleKey(GtkWidget*, GdkEvent* event, gpointer)
{
    if ( !event || !(event->key.string) ) { return; }

    //if(event->key.string[0] == 'q') gtk_main_quit();
    GTK_Instance->KeyPress(event->key.string[0]);
}

// Napinpainalluksen k‰sittely:
static void handleButtonClick(GtkWidget*, gpointer data)
{
    GTK_Instance->ButtonClick(((char*)data)[0]-'A');
}

// Grafiikan initialisointi
//=========================
void GTKLibrary::Init(Callback& cb, char* title, int gridx, int gridy)
{
    CB=&cb;

    // the main window
    mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(mainwindow), title);
    gtk_window_set_policy(GTK_WINDOW(mainwindow), TRUE, TRUE, FALSE);
    gtk_signal_connect(GTK_OBJECT(mainwindow), "destroy",
                       GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(mainwindow), 5);
    gtk_widget_set_usize(GTK_WIDGET(mainwindow),
                         DEFAULT_MAINWINDOW_WIDTH, DEFAULT_MAINWINDOW_HEIGHT);

    gtk_signal_connect(GTK_OBJECT(mainwindow), "key_press_event",
                       GTK_SIGNAL_FUNC(handleKey), NULL);


    // table
    table = gtk_table_new(gridx, gridy, TRUE);
    gtk_container_add(GTK_CONTAINER(mainwindow), table);

    // font
    DefaultFont=gdk_font_load ("variable");//"-misc-fixed-medium-r-*-*-*-120-*-*-*-*-*-*");

    gtk_widget_show(table);
    gtk_widget_show(mainwindow);

    // Graphic context
    DrawingGC=gdk_gc_new(mainwindow->window);
    gdk_gc_copy(DrawingGC, mainwindow->style->fg_gc[0]);

    // Colormap
    ColorMap=gdk_colormap_get_system();
}

// Ikkunan uudelleenpiirto
//========================
void GTKLibrary::Redraw(DWindow darea)
{
    CB->DrawingFunction(darea);
}

// Napin painallus
//================
void GTKLibrary::KeyPress(char key)
{
    CB->HandleKeyPress(key);
}
void GTKLibrary::ButtonClick(unsigned buttonNumber)
{
    CB->HandleButtonClick(buttonNumber);
}
void GTKLibrary::MouseClick(DWindow darea, unsigned x, unsigned y, unsigned b)
{
    CB->HandleMouseClick(darea, x, y, b);
}

// Luo uusi painonappi
//====================
void GTKLibrary::CreateButton(const char* label,
                              int leftColumn, int rightColumn,
                              int upRow, int downRow)
{
    static char ButtonID[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    static unsigned ButtonIDIndex = 0; // Joo, hack

    GtkWidget* button = gtk_button_new_with_label(label);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
                       GTK_SIGNAL_FUNC(handleButtonClick),
                       (gpointer)(&ButtonID[ButtonIDIndex]));
    gtk_widget_show(button);

    gtk_table_attach (GTK_TABLE (table), button,
                      leftColumn, rightColumn, upRow, downRow,
                      GtkAttachOptions(GTK_FILL),
                      GtkAttachOptions(GTK_FILL),
                      0, 0);
    /*
    gtk_table_attach_defaults(GTK_TABLE(table), button,
                              leftColumn, rightColumn, upRow, downRow);
    */

    ButtonIDIndex++;
}

// Luo uusi piirtoikkuna
//======================
GTKLibrary::DWindow GTKLibrary::CreateDrawingWindow (
    int leftColumn, int rightColumn, int upRow, int downRow,
    int minXPixels, int minYPixels)
{
// Scrolled window
    GtkWidget *subwindow=gtk_scrolled_window_new(NULL,NULL);
// Drawing area
    GtkWidget *darea=gtk_drawing_area_new();

//if(leftColumn==0) minXPixels = 200;
// Setup scrolled window and drawing area
    gtk_drawing_area_size (GTK_DRAWING_AREA (darea), minXPixels, minYPixels);
    gtk_container_set_border_width (GTK_CONTAINER (subwindow), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (subwindow),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);

// Add drawing area to scrolled window
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(subwindow),
                                          darea);

// Add scrolled window to table
/*
if(leftColumn==0)
    gtk_table_attach (GTK_TABLE (table), subwindow,
                      leftColumn, rightColumn, upRow, downRow,
                      GtkAttachOptions(GTK_FILL),
                      GtkAttachOptions(GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                      0, 0);
else
*/
    gtk_table_attach_defaults (GTK_TABLE(table), subwindow,
                               leftColumn, rightColumn, upRow, downRow);

// Connect signals to drawing area
    gtk_signal_connect (GTK_OBJECT(darea),"expose_event",
                        (GtkSignalFunc) expose_event, NULL);
    gtk_signal_connect (GTK_OBJECT(darea),"configure_event",
                        (GtkSignalFunc) configure_event, NULL);
    gtk_signal_connect (GTK_OBJECT(darea), "button_press_event",
                        (GtkSignalFunc) Button_Press_Event, NULL);

    gtk_widget_set_events (darea, GDK_EXPOSURE_MASK
                           | GDK_BUTTON_PRESS_MASK);

    gtk_widget_show(darea);
    gtk_widget_show(subwindow);

    return darea;
}

// Luo uusi teksti-ikkuna
//=======================
GTKLibrary::TWindow GTKLibrary::CreateTextWindow (
    int leftColumn, int rightColumn, int upRow, int downRow)
{
    GtkWidget *t= gtk_table_new (2, 1, FALSE);
    gtk_widget_show (t);

    GtkWidget *text = gtk_text_new (NULL, NULL);
    gtk_text_set_editable (GTK_TEXT (text), FALSE);
    gtk_text_set_word_wrap (GTK_TEXT (text), TRUE );
    gtk_table_attach (GTK_TABLE (t), text, 0, 1, 0, 1,
                      GtkAttachOptions(GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                      GtkAttachOptions(GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                      0, 0);
    gtk_widget_show (text);

    GtkWidget *vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
    gtk_table_attach (GTK_TABLE (t), vscrollbar, 1, 2, 0, 1,
                      GtkAttachOptions(GTK_FILL),
                      GtkAttachOptions(GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                      0, 0);
    gtk_widget_show (vscrollbar);

    gtk_table_attach_defaults (GTK_TABLE(table), t,
                               leftColumn, rightColumn, upRow, downRow);

    return text;
}

// Kaiken uudelleenpiirto
//=======================
void GTKLibrary::Redraw()
{
    redrawPixmaps();
    /*
    gtk_widget_set_usize(GTK_WIDGET(mainwindow),
                         10, 10); // KLUDGE!!! Ei hajuukaan miten se teh‰‰n
    */

    /*
    GdkEvent event;
    gint dummy;
    event.configure.type = GDK_CONFIGURE;
    event.configure.window = gtk_widget_get_parent_window(mainwindow);
    event.configure.send_event = TRUE;
    gint x,y,width,height;
    gdk_window_get_geometry(event.configure.window,
                            &x, &y, &width, &height, &dummy);
    event.configure.x = x;
    event.configure.y = y;
    event.configure.width = width;
    event.configure.height = height;
    gtk_main_do_event(&event);
    */
}

// Aseta piirtov‰ri
//=================
void GTKLibrary::DrawingColor(const string& color)
{
    GdkColor col;
    gdk_color_parse(color.c_str(),&col);
    gdk_color_alloc(ColorMap,&col);
    gdk_gc_set_foreground(DrawingGC, &col);
}

// Aseta viivan paksuus ja dash-pattern
//=====================================
void GTKLibrary::LineWidthDash(int width, const string& s)
{
    bool dashed = false;
    for(unsigned i=0; i<s.size(); i++)
        if(s[i]==' ') { dashed=true; break; }

    gdk_gc_set_line_attributes(DrawingGC, width,
                               dashed ? GDK_LINE_ON_OFF_DASH : GDK_LINE_SOLID,
                               GDK_CAP_NOT_LAST, GDK_JOIN_MITER);

    if(dashed)
    {
        char* dash=new char[s.size()];
        char c=0;
        int count=0;
        bool currentisspace=(s[0]==' ');
        for(unsigned i=0; i<s.size(); i++)
        {
            if((s[i]==' ' && !currentisspace) ||
               (s[i]!=' ' && currentisspace))
            {
                dash[count++] = c*2;
                c = 0;
                currentisspace = !currentisspace;
            }
            c++;
        }
        dash[count++]=c*2;
        gdk_gc_set_dashes(DrawingGC, 0, (gint8*)(dash), count);
        delete[] dash;
    }
}

// Piirtoikkunan dimensiot
//========================
int GTKLibrary::DWindowXSize(DWindow darea)
{
    return pixmap[(long)darea].width;
}

int GTKLibrary::DWindowYSize(DWindow darea)
{
    return pixmap[(long)darea].height;
}


// Viivanpiirto
//=============
static inline int Abs(int x) { return x>=0 ? x : -x; }
static inline int ChooseLineWidth(int, int, int, int)
{ return 2;//(Abs(x2-x1)>DOUBLEWIDTH_LINE_TRIG ||
           //Abs(y2-y1)>DOUBLEWIDTH_LINE_TRIG) ? 2 : 1;
}

void GTKLibrary::DrawLine (
    DWindow darea,
    int x1, int y1,
    int x2, int y2,
    const string& color,
    const string& dash)
{
    DrawingColor(color);
    LineWidthDash(ChooseLineWidth(x1, y1, x2, y2), dash);
    gdk_draw_line(pixmap[(long)darea].pm, DrawingGC,
                  x1, y1, x2, y2);
}

// Viivajoukon piirto
//===================
void GTKLibrary::DrawLines (
    DWindow darea,
    int* points,
    int npoints,
    const string& color,
    const string& dash)
{
    GdkPoint* p = new GdkPoint[npoints];

    for(int i=0;i<npoints;i++)
    {
        p[i].x = points[i*2];
        p[i].y = points[i*2+1];
    }
    DrawingColor(color);
    LineWidthDash(
        ChooseLineWidth(p[0].x, p[0].y, p[npoints-1].x, p[npoints-1].y),
        dash);
    gdk_draw_lines(pixmap[(long)darea].pm, DrawingGC, p, npoints);

    delete[] p;
}

// T‰ytetyn polygonin piirto
//==========================
void GTKLibrary::DrawFilledPolygon (
    DWindow darea,
    int* points,
    int npoints,
    const string& color)
{
    GdkPoint* p = new GdkPoint[npoints];

    for(int i=0;i<npoints;i++)
    {
        p[i].x = points[i*2];
        p[i].y = points[i*2+1];
    }
    DrawingColor(color);
    gdk_draw_polygon(pixmap[(long)darea].pm, DrawingGC, TRUE, p, npoints);

    delete[] p;
}

// Ympyr‰
//=======
void GTKLibrary::DrawFilledCircle (
    DWindow darea,
    int x, int y,
    int Radius,
    const string& borderColor,
    const string& fillColor)
{
    LineWidthDash(2,string(""));
// Sisus
    DrawingColor(fillColor);
    gdk_draw_arc(pixmap[(long)darea].pm, DrawingGC,
                 TRUE, x-Radius, y-Radius, 2*Radius, 2*Radius, 0, 24000);
// Reuna
    DrawingColor(borderColor);
    gdk_draw_arc(pixmap[(long)darea].pm, DrawingGC,
                 FALSE, x-Radius, y-Radius, 2*Radius, 2*Radius, 0, 24000);
}

// Teksti piirtoikkunaan
//======================
void GTKLibrary::DrawText (
    DWindow darea,
    const string& s,
    int x, int y,
    const string& color)
{
    DrawingColor(color);
    gdk_draw_text(pixmap[(long)darea].pm, DefaultFont, DrawingGC,
                  x, y, s.c_str(), s.size());
}

// Teksti teksti-ikkunaan
//=======================
void GTKLibrary::PrintText (
    TWindow textw,
    const string& s)
{
    gtk_text_insert(GTK_TEXT (textw), DefaultFont, NULL, NULL, s.c_str(), -1);
}

void GTKLibrary::ClearTextWindow(TWindow textw)
{
    gtk_text_backward_delete(GTK_TEXT(textw),
                             gtk_text_get_length(GTK_TEXT(textw)));
}

void GTKLibrary::Done()
{
    gtk_main ();
}

void GTKLibrary::Quit()
{
    gtk_main_quit();
}
