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

// FILE_DES: main.cc: Grafiikka/piirtoalusta
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: main.cc 1.14 Mon, 31 May 2004 17:20:54 +0300 warp $";
// 
// LTS-Viewerin p‰‰ohjelma.
//

// $Log:$


#include "lts_viewer.hh"
#include "gtklib.hh"
#include "lts_draw_reader.hh"

#ifndef MAKEDEPEND
#include <fstream>
#include <iostream>
#include <cstdio>
#include <set>
#include <vector>
#include <string>
using namespace std;
#endif

// Drawing class
//==============
class Draw:public LTS_Viewer::LTS_Draw
{
public:
    Draw(LTSData& d): data(d)
    {
        const ItemList& items = data.getData();

        set<string> propnames;
        for(ItemList::const_iterator i=items.begin(); i!=items.end(); i++)
        {
            const list<string>& nameslist = (*i)->getStringData();
            if((*i)->Type() == LTS_Item::TILA)
            {
                for(list<string>::const_iterator iter = ++nameslist.begin();
                    iter != nameslist.end(); ++iter)
                {
                    propnames.insert(*iter);
                }
            }
        }

        statePropNames.reserve(propnames.size());
        for(set<string>::iterator iter = propnames.begin();
            iter != propnames.end(); ++iter)
        {
            statePropNames.push_back(*iter);
        }
    }

    unsigned statePropsAmnt() { return statePropNames.size(); }

    string findAction(const string& color, const string& dash)
    {
        const ItemList& items = data.getData();

        for(ItemList::const_iterator i=items.begin(); i!=items.end(); i++)
        {
            if((*i)->Type() == LTS_Item::TAPAHTUMASELITE &&
               (*i)->GetColor() == color &&
               (*i)->GetDash() == dash)
            {
                return (*i)->GetText();
            }
        }
        return "";
    }

    void PiirraLTSIkkuna(LTS_Viewer* window)
    {
        const ItemList& items = data.getData();

        double minx=0, miny=0, maxx=0, maxy=0;
        bool first=true;
        Point p;
        // Laske kuvan koko
        for(ItemList::const_iterator i=items.begin(); i!=items.end(); i++)
        {
            if((*i)->Type() == LTS_Item::TILA)
            {
                p=(*i)->GetCoords();
                if(first)
                {
                    minx = maxx = p.xCoord();
                    miny = maxy = p.yCoord();
                    first = false;
                }
                if(p.xCoord()<minx) minx=p.xCoord();
                if(p.xCoord()>maxx) maxx=p.xCoord();
                if(p.yCoord()<miny) miny=p.yCoord();
                if(p.yCoord()>maxy) maxy=p.yCoord();
            }
        }
        window->KuvanKoko(minx, miny, maxx-minx, maxy-miny);

        window->resetStatePropNames();

        bool arrowHeadNamesIsEmpty = arrowHeadNames.empty();
        arrowHeadCoords.clear();

        for(ItemList::const_iterator i=items.begin(); i!=items.end(); i++)
        {
            // Tulosta tapahtumat
            if((*i)->Type() == LTS_Item::TAPAHTUMA)
            {
                list<Point> viiva;
                for(int n=(*i)->NumberOfCoords(); n>0; n--)
                    viiva.insert(viiva.end(), (*i)->GetCoords());
                window->PiirraTapahtuma(viiva,
                                        (*i)->GetColor(), (*i)->GetDash());

                arrowHeadCoords.push_back
                    (window->getArrowHeadCoord(viiva));
                if(arrowHeadNamesIsEmpty)
                    arrowHeadNames.push_back
                        (findAction((*i)->GetColor(), (*i)->GetDash()));
            }
            // Tulosta tilat
            else if((*i)->Type() == LTS_Item::TILA)
            {
                const string& bc=(*i)->GetColor();
                const string& fc=(*i)->GetColor();
                window->PiirraTila((*i)->GetCoords(), bc, fc,
                                   (*i)->isCutState(),
                                   (*i)->isSticky(),
                                   (*i)->getStringData());
            }
            // Tulosta tekstit
            else if((*i)->Type() == LTS_Item::TEKSTI)
            {
                window->PiirraTeksti((*i)->GetText(), (*i)->GetCoords());
            }
        }

        /*
        if(statePropNames.size())
        {
            Point ll = window->LowerLeft();
            window->DrawTextToAbsoluteCoords
                ("l-click = on state: list props; on prop name: highlight",
                 ll);
            ll.yCoord(ll.yCoord()-12);
            window->DrawTextToAbsoluteCoords
                ("q=quit, r-click=zoom, mid-click=scroll, 1=reset zoom", ll);
        }
        else
        {
            window->DrawTextToAbsoluteCoords
                ("q=quit, r-click=zoom, mid-click=scroll, 1=reset zoom",
                 window->LowerLeft());
        }
        */
        window->DrawTextToAbsoluteCoords
            ("Press q to quit, h for help", window->LowerLeft());
    }

    void PiirraTapahtumaseliteIkkuna(LTS_Viewer* window)
    {
        const ItemList& items = data.getData();

        // Tulosta tapahtumaselitteet
        for(ItemList::const_iterator i=items.begin(); i!=items.end(); i++)
        {
            if((*i)->Type() == LTS_Item::TAPAHTUMASELITE)
            {
                window->PiirraTapahtumaselite(
                    (*i)->GetText(), (*i)->GetColor(), (*i)->GetDash());
            }
        }
    }

    void PiirraTilapropositioIkkuna(LTS_Viewer* window)
    {
        window->PiirraTilapropositionimet(statePropNames);
    }

    void PiirraTilaseliteIkkuna(LTS_Viewer* window)
    {
        const ItemList& items = data.getData();

        // Tulosta tilaselitteet
        for(ItemList::const_iterator i=items.begin(); i!=items.end(); i++)
        {
            if((*i)->Type() == LTS_Item::TILASELITE)
            {
                const string& bc=(*i)->GetColor();
                const string& fc=(*i)->GetColor();
                window->PiirraTilaselite(
                    (*i)->GetText(), bc, fc,
                    (*i)->isCutState(), (*i)->isSticky());
            }
        }
    }

    bool HandleButtonClick(unsigned)
    {
        data.rotate();
        return true;
    }

    void HandleMouseClick(LTS_Viewer* window,
                          unsigned x, unsigned y, unsigned)
    {
        bool res = false;
        if(statePropNames.size())
            res = window->printStatePropsForCoords(x, y);

        if(!res)
        {
            double minDist = 1e10;
            unsigned minIndex = 0;
            for(unsigned i=0; i<arrowHeadCoords.size(); ++i)
            {
                //Point p = window->toScreenCoords(arrowHeadCoords[i]);
                Point p = arrowHeadCoords[i];
                double vx = p.xCoord()-double(x), vy = p.yCoord()-double(y);
                double dist = vx*vx + vy*vy;
                if(dist < minDist) { minDist = dist; minIndex = i; }
            }
            if(minDist < 20*20)
                window->highlightActionName(arrowHeadNames[minIndex]);
            else
                window->highlightActionName("");
        }
    }


private:
    LTSData& data;
    vector<string> statePropNames;
    vector<Point> arrowHeadCoords;
    vector<string> arrowHeadNames;
};

// Main
//===========================================================================
int main(int argc, char* argv[])
{
// Read LTS commands
    LTSData items;
    if(argc>1)
    {
        ifstream ifile(argv[1]);
        if(!ifile)
        {
            cerr << "Can't open ";
            perror(argv[1]);
            return 1 ; // EXIT_FAILURE;
        }
        items.readData(ifile);
    }
    else
        items.readData(cin);

// Show LTS
    GraphicLibrary* GL=new GTKLibrary(argc, argv);
    Draw draw(items);

    bool hasStateProps = false;
    unsigned actionAmnt = 0;
    for(ItemList::const_iterator i=items.getData().begin();
        i!=items.getData().end(); i++)
    {
        if((*i)->Type() == LTS_Item::TAPAHTUMASELITE)
        {
            ++actionAmnt;
        }
        if((*i)->Type() == LTS_Item::TILA && (*i)->getStringData().size())
        {
            hasStateProps = true;
        }
    }

    LTS_Viewer window(GL, draw, actionAmnt, draw.statePropsAmnt());

// Tulosta history
    for(ItemList::const_iterator i=items.getData().begin();
        i!=items.getData().end(); i++)
    {
        if((*i)->Type() == LTS_Item::HISTORY)
            window.TulostaTeksti((*i)->GetText()+"\n");
    }

    window.Done();

// Clean
    int status= GL->getStatus();
    delete GL;
    return status;
}
