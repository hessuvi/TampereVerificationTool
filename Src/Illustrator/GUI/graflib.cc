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

// FILE_DES: graflib.cc: Grafiikka/piirtoalusta/rajapinta
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: graflib.cc 1.6 Fri, 27 Feb 2004 16:36:58 +0200 warp $";
// 
// GraphicLibrary:n ei-abstraktien metodien toteutus.
//

// $Log:$

#include "graflib.hh"

#include <cctype>
#include <cmath>

using namespace std;

// Destructori
//===========================================================================
GraphicLibrary::~GraphicLibrary() { }


// Rakentajat
//===========================================================================
GraphicLibrary::GraphicLibrary(int&, char** &) { }


// Nuolenp‰‰n piirto
//===========================================================================
void GraphicLibrary::DrawArrowHead (
    DWindow w,
    int x1, int y1,
    int x2, int y2,
    int arrowLength,
    const std::string& color
    )
/* Piirt‰‰ nuolenp‰‰n (ne kaksi vinoa viivaa nuolen p‰‰ss‰) annettuun
   piirtoikkunaan.
     Toimii vastaavasti kuin DrawLine(), eli ottaa alkupisteen ja
   loppupisteen koordinaatit, Se ei kuitenkaan piirr‰ viivaa t‰st‰
   alkupisteest‰ loppupisteeseen, vaan ainoastaan nuolen k‰rjen
   loppupisteen kohdalle (alkupiste on tarpeellinen k‰rjen suunnan
   selvitt‰miseksi). Jos metodeja DrawLine() ja DrawArrowHead()
   kutsutaan samoilla koordinaattipareilla, syntyy tuloksena nuoli.
     arrowLength kertoo nuolenp‰‰n pituuden pikselein‰. Pituus lasketaan
   annettujen koordinaattien kautta kulkevaa suoraa pitkin.
     T‰t‰ metodia ei tarvitse v‰ltt‰m‰tt‰ toteuttaa periytetyss‰
   luokassa. Kantaluokan toteutus kutsuu DrawLine()-metodia.
 */
{
    int p[3*2];
    double vx=x2-x1, vy=y2-y1, nx, ny;
    double len=sqrt(vx*vx+vy*vy);
    if(len==0) return;
    vx/=len; vy/=len;

    nx=vy; ny=-vx;

    p[0] = (int) (x2 - vx*arrowLength + nx*arrowLength/3);
    p[1] = (int) (y2 - vy*arrowLength + ny*arrowLength/3);
    p[2] = (int) (x2 - vx*arrowLength - nx*arrowLength/3);
    p[3] = (int) (y2 - vy*arrowLength - ny*arrowLength/3);
    p[4] = (int) (x2);
    p[5] = (int) (y2);

    DrawFilledPolygon(w, p, 3, color);
}

// Muuntaa stringin long-tyyppiseksi v‰riarvoksi
//===========================================================================
unsigned long GraphicLibrary::string2color(const std::string& s)
/* Muuttaa annetun stringin long-tyyppiseksi arvoksi, joka sis‰lt‰‰
   v‰riarvon. 8 alinta bitti‰ ovat punainen, 8 seuraavaa bitti‰ vihre‰
   ja 8 seuraavaa sininen.
 */
{
    unsigned long col=0, mul=1;
    int c;

    for(unsigned i=0; i<6 && i<s.size(); i++)
    {
        c=toupper(s[i]);
        if(c>'9') c=10+c-'A';
        else c-='0';
        col+=c*mul;
        mul*=16;
    }
    return col;
}

