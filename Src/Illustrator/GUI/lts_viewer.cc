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

// FILE_DES: lts_viewer.cc: Grafiikka/piirtoalusta
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: lts_viewer.cc 1.19 Mon, 31 May 2004 17:20:54 +0300 warp $";
// 
// LTS_Viewer-luokan toteutus.
//

// $Log:$


#include "lts_viewer.hh"
#include "constants.hh"

#include <cmath>

const string LTS_Viewer::DefaultDash("");

// Konstruktori luo ikkunan
//=========================
LTS_Viewer::LTS_Viewer(GraphicLibrary* gl, LTS_Draw& draw,
                       unsigned actionAmnt, unsigned statePropsAmnt):
    GL(gl), tilasade(LTS_DEFAULT_RADIUS),
    tilaseliteYCoord(-1), tapahtumaseliteYCoord(-1),
    tilapropositiot(0), tilapropositioInd(~0U),
    lts_midx(0), lts_midy(0), lts_scale(1),
    zoomingFactor(1), zoomX(0), zoomY(0),
    redrawFunctions(draw),
    showStateNames(false), stateNamesClicked(0)
{
    GL->Init(*this, LTSVIEWER_MAINTITLE, 16, 16);

// Setup windows
    tilaseliteikkuna=GL->CreateDrawingWindow(0, 4, 1, 5, 0, 100);
    if(statePropsAmnt)
    {
        tapahtumaseliteikkuna =
            GL->CreateDrawingWindow(0, 4, 5, 12, 0,
                                    actionAmnt*TAPAHTUMANIMIENVALI+2);
        tilapropositioikkuna =
            GL->CreateDrawingWindow(0, 4, 12, 16, 0,
                                    statePropsAmnt*TILAPROPOSITIONIMIENVALI+2);
    }
    else
    {
        tapahtumaseliteikkuna =
            GL->CreateDrawingWindow(0, 4, 5, 16, 0,
                                    actionAmnt*TAPAHTUMANIMIENVALI+2);
        tilapropositioikkuna=0;
    }
    automaatti_ikkuna=GL->CreateDrawingWindow(4, 16, 0, 14);
    teksti_ikkuna=GL->CreateTextWindow(4, 16, 14, 16);

    GL->CreateButton("Rotate", 0, 4, 0, 1);
}

// Destruktori
//============
LTS_Viewer::~LTS_Viewer() { }


// Uudelleenpiirtofunktio
//=======================
void LTS_Viewer::DrawingFunction(GraphicLibrary::DWindow dw)
{
    if(dw==automaatti_ikkuna)
    {
        redrawFunctions.PiirraLTSIkkuna(this);
        //GL->DrawText(automaatti_ikkuna, "Press 'q' to quit",
        //             4, GL->DWindowYSize(automaatti_ikkuna)-4, "black");
    }
    else if(dw==tilaseliteikkuna)
    {
        tilaseliteYCoord=-1;
        redrawFunctions.PiirraTilaseliteIkkuna(this);
    }
    else if(dw==tapahtumaseliteikkuna)
    {
        tapahtumaseliteYCoord=-1;
        redrawFunctions.PiirraTapahtumaseliteIkkuna(this);
    }
    else if(dw==tilapropositioikkuna)
    {
        redrawFunctions.PiirraTilapropositioIkkuna(this);
    }
}

void LTS_Viewer::HandleKeyPress(char key)
{
    switch(key)
    {
      case 'q': GL->Quit(); break;
      case 'x': GL->Exit(); break;
      case '1':
          zoomingFactor = 1;
          zoomX = 0; zoomY = 0;
          GL->Redraw();
          break;
      case 'n':
          showStateNames = !showStateNames;
          stateNamesClicked = 0;
          GL->Redraw();
          break;

      case 'h':
          GL->ClearTextWindow(teksti_ikkuna);
          GL->PrintText(teksti_ikkuna,
"* Left-click on a state to list the state propositions of that state.\n"
"* Click on a state proposition name at the bottom left to highlight the "
"states where the proposition is on.\n"
"* Click on a transition arrow head (or close to it) to highlight its action "
"name on the left.\n"
"* Right-click on the graph window to zoom. When zoomed, middle-click to "
"scroll the view. Press '1' on the keyboard to reset the zoom.\n"
"* Press 'n' on the keyboard to turn on/off state numbers. When on, clicking "
"on states will print their number on this text window.");
          break;
    }
}

void LTS_Viewer::HandleButtonClick(unsigned buttonNumber)
{
    if(redrawFunctions.HandleButtonClick(buttonNumber))
    {
        GL->Redraw();
    }
}

void LTS_Viewer::HandleMouseClick(GraphicLibrary::DWindow dw,
                                  unsigned x, unsigned y, unsigned b)
{
    if(dw==automaatti_ikkuna)
    {
        if(b==3)
        {
            if(zoomingFactor < 200)
            {
                zoomX = (x-zoomX)/zoomingFactor + zoomX;
                zoomY = (y-zoomY)/zoomingFactor + zoomY;
                zoomingFactor *= 1.25;
                GL->Redraw();
            }
        }
        else if(b==2)
        {
                zoomX = (x-zoomX)/zoomingFactor + zoomX;
                zoomY = (y-zoomY)/zoomingFactor + zoomY;
                GL->Redraw();
        }
        else
        {
            redrawFunctions.HandleMouseClick(this, x, y, b);
        }
    }
    else if(dw==tilapropositioikkuna)
    {
        tilapropositioInd = y/TILAPROPOSITIONIMIENVALI;
        GL->Redraw();
    }
}


// Piirt‰‰ tilan automaatti-ikkunaan
//==================================
void LTS_Viewer::PiirraTila(Point coords,
                            const string& bordercolor,
                            const string& fillcolor,
                            bool isCutState, bool isStickyState,
                            const list<string>& stateProps)
{
    if(tilapropositiot && tilapropositioInd < tilapropositiot->size())
    {
        for(list<string>::const_iterator iter = ++stateProps.begin();
            iter != stateProps.end(); ++iter)
        {
            if((*tilapropositiot)[tilapropositioInd] == *iter)
            {
                GL->DrawFilledCircle(automaatti_ikkuna,
                                     PositionX(coords.xCoord()),
                                     PositionY(coords.yCoord()),
                                     int(Skaalaa(TilaSade())*1.5),
                                     "#000000", "#FFFF00");
                break;
            }
        }
    }

    GL->DrawFilledCircle(automaatti_ikkuna,
                         PositionX(coords.xCoord()),
                         PositionY(coords.yCoord()),
                         int(Skaalaa(TilaSade())),
                         bordercolor, fillcolor);
    if(isCutState)
    {
        GL->DrawFilledCircle(automaatti_ikkuna,
                             PositionX(coords.xCoord()),
                             PositionY(coords.yCoord()),
                             int(Skaalaa(TilaSade())/3),
                             bordercolor, fillcolor);
    }

    if(isStickyState)
    {
        int offset = int(Skaalaa(TilaSade())/2);
        GL->DrawLine(automaatti_ikkuna,
                     PositionX(coords.xCoord()) - offset,
                     PositionY(coords.yCoord()) + offset,
                     PositionX(coords.xCoord()) + offset,
                     PositionY(coords.yCoord()) - offset,
                     bordercolor, fillcolor);
    }

    statePropCoords.push_back(StatePropNames(stateProps,
                                             PositionX(coords.xCoord()),
                                             PositionY(coords.yCoord())));

    if(showStateNames && !stateProps.front().empty())
    {
        GL->DrawText(automaatti_ikkuna, stateProps.front(),
                     PositionX(coords.xCoord())-5,
                     PositionY(coords.yCoord())+4, "#000000");
        GL->DrawText(automaatti_ikkuna, stateProps.front(),
                     PositionX(coords.xCoord())-3,
                     PositionY(coords.yCoord())+4, "#000000");
        GL->DrawText(automaatti_ikkuna, stateProps.front(),
                     PositionX(coords.xCoord())-4,
                     PositionY(coords.yCoord())+3, "#000000");
        GL->DrawText(automaatti_ikkuna, stateProps.front(),
                     PositionX(coords.xCoord())-4,
                     PositionY(coords.yCoord())+5, "#000000");
        GL->DrawText(automaatti_ikkuna, stateProps.front(),
                     PositionX(coords.xCoord())-4,
                     PositionY(coords.yCoord())+4, "#FFD020");
    }
}

// Apufunktio, joka lyhent‰‰ viivaa annetun m‰‰r‰n
//================================================
static void TrimLine(double x1, double y1, double& x2, double& y2,
                     double length, double zoomingFactor)
{
    x1 *= zoomingFactor; y1 *= zoomingFactor;
    x2 *= zoomingFactor; y2 *= zoomingFactor;
    double vx = x2-x1, vy = y2-y1;
    double vlen = sqrt(vx*vx+vy*vy);
    if(vlen==0) return;
    vx /= vlen; vy /= vlen;
    x2 -= vx*length; y2 -= vy*length;
    x2 /= zoomingFactor; y2 /= zoomingFactor;
}

// Piirt‰‰ tapahtuman automaatti-ikkunaan
//=======================================
void LTS_Viewer::PiirraTapahtuma(const list<Point>& viiva,
                                 const string& color, const string& dash)
{
    int* p = new int[viiva.size()*2];
    int Ind=0;
    list<Point>::const_iterator i=viiva.begin();
    double x1, y1, x2=i->xCoord(), y2=i->yCoord();
    bool first=true;

    for(i++, Ind++; i!=viiva.end(); i++, Ind++)
    {
        x1=x2; y1=y2;
        x2=i->xCoord(); y2=i->yCoord();
        if(first)
        {
            TrimLine(x2, y2, x1, y1, TilaSade(), zoomingFactor);
            first=false;
            p[0] = PositionX(x1);
            p[1] = PositionY(y1);
        }
        else if(i == --(viiva.end()))
            TrimLine(x1, y1, x2, y2, TilaSade(), zoomingFactor);

        p[Ind*2] = PositionX(x2);
        p[Ind*2+1] = PositionY(y2);
    }
    GL->DrawLines(automaatti_ikkuna, p, viiva.size(), color, dash);

    --i;
    double vx = i->xCoord(), vy = i->yCoord();
    --i;
    vx = x2+(i->xCoord()-vx); vy = y2+(i->yCoord()-vy);
    GL->DrawArrowHead(automaatti_ikkuna,
                      PositionX(vx), PositionY(vy),
                      PositionX(x2), PositionY(y2),
                      LTS_DEFAULT_ARROWHEAD_LENGTH,
                      color);

    delete[] p;
}

Point LTS_Viewer::getArrowHeadCoord(const list<Point>& viiva)
{
    list<Point>::const_iterator i = --viiva.end();
    double x2 = i->xCoord(), y2 = i->yCoord();
    --i;
    double x1 = i->xCoord(), y1 = i->yCoord();
    TrimLine(x1, y1, x2, y2, TilaSade(), zoomingFactor);
    return Point(PositionX(x2), PositionY(y2));
}


// Piirt‰‰ teksti‰ automaatti-ikkunaan
//====================================
void LTS_Viewer::PiirraTeksti(const string& s, Point coords)
{
    GL->DrawText(automaatti_ikkuna, s,
                 PositionX(coords.xCoord()),
                 PositionY(coords.yCoord()), "black");
}

void LTS_Viewer::DrawTextToAbsoluteCoords(const string& s, Point coords)
{
    GL->DrawText(automaatti_ikkuna, s,
                 int(coords.xCoord()), int(coords.yCoord()), "black");
}

// Tulostaa teksti‰ teksti-ikkunaan
//=================================
void LTS_Viewer::TulostaTeksti(const string& s)
{
    GL->PrintText(teksti_ikkuna, s);
}

bool LTS_Viewer::printStatePropsForCoords(unsigned x, unsigned y)
{
    double rad = Skaalaa(TilaSade());
    double rad2 = rad*rad;
    list<StatePropNames>::iterator iter;
    for(iter = statePropCoords.begin(); iter != statePropCoords.end(); ++iter)
    {
        /*
        if(x <= iter->xCoord+rad &&
           x >= iter->xCoord-rad &&
           y <= iter->yCoord+rad &&
           y >= iter->yCoord-rad)
        */
        double
            vx = double(iter->xCoord) - double(x),
            vy = double(iter->yCoord) - double(y);
        if(vx*vx + vy*vy <= rad2)
        {
            if(showStateNames)
            {
                if(stateNamesClicked == 0) GL->ClearTextWindow(teksti_ikkuna);
                else GL->PrintText(teksti_ikkuna, " ");
                GL->PrintText(teksti_ikkuna, iter->names->front());
                ++stateNamesClicked;
            }
            else
            {
                GL->ClearTextWindow(teksti_ikkuna);
                list<string>::const_iterator iter2;
                for(iter2 = ++(iter->names->begin());
                    iter2 != iter->names->end(); ++iter2)
                {
                    GL->PrintText(teksti_ikkuna, (*iter2)+"\n");
                }
            }
            return true;
        }
    }
    return false;
}


// Piirt‰‰ tilan ja tekstin tilaseliteikkunaan
//============================================
void LTS_Viewer::PiirraTilaselite(
    const string& nimi,
    const string& bordercolor,
    const string& fillcolor,
    bool isCutState, bool isStickyState)
{
    double xsize=GL->DWindowXSize(tilaseliteikkuna);
    double rad=xsize/18.0;
    if(rad < TILASELITETILAN_MINIMISADE) rad = TILASELITETILAN_MINIMISADE;
    if(rad > TILASELITETILAN_MAKSIMISADE) rad = TILASELITETILAN_MAKSIMISADE;

    if(tilaseliteYCoord<0)
        tilaseliteYCoord=rad*1.5;

    GL->DrawFilledCircle(tilaseliteikkuna,
                         int(rad*1.5), int(tilaseliteYCoord), int(rad),
                         bordercolor, fillcolor);
    if(isCutState)
    {
        GL->DrawFilledCircle(tilaseliteikkuna,
                             int(rad*1.5), int(tilaseliteYCoord), int(rad/3),
                             bordercolor, fillcolor);
    }
    if(isStickyState)
    {
        GL->DrawLine(tilaseliteikkuna,
                     int(rad*1.5-rad/2), int(tilaseliteYCoord+rad/2),
                     int(rad*1.5+rad/2), int(tilaseliteYCoord-rad/2),
                     bordercolor, fillcolor);
    }

    GL->DrawText(tilaseliteikkuna, nimi, int(rad*3),
                 int(tilaseliteYCoord+rad/2), "black");

    tilaseliteYCoord+=rad*2.5;
}

// Piirt‰‰ tapahtuman ja tekstin tapahtumaseliteikkunaan
//======================================================
void LTS_Viewer::PiirraTapahtumaselite(
    const string& nimi,
    const string& color, const string& dash)
{
    if(tapahtumaseliteYCoord<0)
        tapahtumaseliteYCoord = TAPAHTUMANIMIENVALI;

    GL->DrawLine(tapahtumaseliteikkuna,
                 4, int(tapahtumaseliteYCoord),
                 GL->DWindowXSize(tapahtumaseliteikkuna)-4,
                 int(tapahtumaseliteYCoord),
                 color, dash);
    GL->DrawText(tapahtumaseliteikkuna, nimi,
                 4, int(tapahtumaseliteYCoord-2),
                 nimi == highlightedActionName ? "red" : "black");

    tapahtumaseliteYCoord += TAPAHTUMANIMIENVALI;
}


void LTS_Viewer::highlightActionName(const string& name)
{
    highlightedActionName = name;
    GL->Redraw();
}

void LTS_Viewer::PiirraTilapropositionimet(const vector<string>& nimet)
{
    tilapropositiot = &nimet;

    for(unsigned ind = 0; ind < nimet.size(); ++ind)
    {
        GL->DrawText(tilapropositioikkuna, nimet[ind],
                     4, TILAPROPOSITIONIMIENVALI*(ind+1),
                     ind==tilapropositioInd ? "red" : "black");
    }
}

// Asettaa automaattikuvan rajat (skaalausta varten)
//==================================================
void LTS_Viewer::KuvanKoko(double minx, double miny,
                           double width, double height)
{
    minx-=.5; miny-=.5; width+=1; height+=1;
    lts_midx=minx+width/2; lts_midy=miny+height/2;
    double xscale=GL->DWindowXSize(automaatti_ikkuna)/width;
    double yscale=GL->DWindowYSize(automaatti_ikkuna)/height;
    lts_scale = (xscale<yscale) ? xscale : yscale;
    if(lts_scale>LTS_MAXSCALE) lts_scale=LTS_MAXSCALE;
}

// Asettaa tilaa kuvaavan ympyr‰n s‰teen
//======================================
void LTS_Viewer::TilanSade(double radius)
{
    tilasade=radius;
}


void LTS_Viewer::Done(void)
{
    GL->Done();
}

// Koordinaattien skaalaus
//========================
int LTS_Viewer::PositionX(double x) const
{
    x = (x-lts_midx)*lts_scale + GL->DWindowXSize(automaatti_ikkuna)/2;
    return int((x-zoomX)*zoomingFactor+zoomX);
}

int LTS_Viewer::PositionY(double y) const
{
    y = (y-lts_midy)*lts_scale + GL->DWindowYSize(automaatti_ikkuna)/2;
    return int((y-zoomY)*zoomingFactor+zoomY);
}

int LTS_Viewer::Skaalaa(double d) const
{
    return int(d*lts_scale);
}

double LTS_Viewer::TilaSade() const
{
    if(tilasade*lts_scale < LTS_MIN_STATE_SIZE)
        return LTS_MIN_STATE_SIZE/lts_scale;
    return tilasade;
}
