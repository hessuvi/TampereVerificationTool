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

// FILE_DES: gtklib.hh: Grafiikka/piirtoalusta/gtk
// Juha Nieminen

// $Id: gtklib.hh 1.6 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// GraphicLibrary:n GTK-toteutuksen esittely.
//

// $Log:$

#ifdef CIRC_GTKLIB_HH_
#error "Include recursion"
#endif

#ifndef ONCE_GTKLIB_HH_
#define ONCE_GTKLIB_HH_
#define CIRC_GTKLIB_HH_

#include "graflib.hh"

#ifndef MAKEDEPEND
#include <string>
#include <gtk/gtk.h>
#endif

class GTKLibrary:public GraphicLibrary
{
public:
    GTKLibrary(int& argc, char** &argv);
    virtual ~GTKLibrary();

    void Init(Callback&, char* documentTitle, int gridx, int gridy);

    void CreateButton(const char* label,
                      int leftColumn, int rightColumn, int upRow, int downRow);

    DWindow CreateDrawingWindow (
        int leftColumn, int rightColumn, int upRow, int downRow,
        int minXPixels=0, int minYPixels=0);

    TWindow CreateTextWindow(int leftColumn, int rightColumn,
                             int upRow, int downRow);

    int DWindowXSize (DWindow);
    int DWindowYSize (DWindow);

    void DrawLine (
        DWindow,
        int x1, int y1,
        int x2, int y2,
        const std::string& color,
        const std::string& dash);

    void DrawLines (
        DWindow,
        int* points,
        int npoints,
        const std::string& color,
        const std::string& dash);

    void DrawFilledPolygon (
        DWindow,
        int* points,
        int npoints,
        const std::string& color);

    void DrawFilledCircle (
        DWindow,
        int x, int y,
        int Radius,
        const std::string& borderColor,
        const std::string& fillColor);

    void DrawText (
        DWindow,
        const std::string& s,
        int x, int y,
        const std::string& color);

    void PrintText (
        TWindow,
        const std::string& s);

    void ClearTextWindow(TWindow);

    void Done(void);
    void Quit();

    void Redraw();
    void Redraw(DWindow);
    void KeyPress(char key);
    void ButtonClick(unsigned buttonNumber);
    void MouseClick(DWindow, unsigned,unsigned,unsigned);

private:
    GtkWidget *mainwindow, *table;
    GdkGC* DrawingGC;
    GdkFont* DefaultFont;
    GdkColormap* ColorMap;

    Callback* CB;

    void DrawingColor(const std::string&);
    void LineWidthDash(int width, const std::string&);

    GTKLibrary (const GTKLibrary&);
    GTKLibrary& operator=(const GTKLibrary&);
};

#undef CIRC_GTKLIB_HH_
#endif
