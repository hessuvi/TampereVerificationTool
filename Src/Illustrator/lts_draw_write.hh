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

Contributor(s): Heikki Virtanen.
*/

// FILE_DES: lts_draw_write.hh: Grafiikka/piirtoalusta
// Juha Nieminen

// $Id: lts_draw_write.hh 1.6 Mon, 31 May 2004 17:20:54 +0300 warp $
// 
// LTS-piirtokomentojen tulostusluokka.
//
// [Luokan (tai vastaavan) nimi]:
//     [Metodin nimi]: [Metodin lyhyt kuvaus]
//     [Metodin nimi]: [Metodin lyhyt kuvaus]
//
// [Mahdollisen funktion nimi]: [Lyhyt kuvaus]
//

// $Log:$

#ifdef CIRC_LTS_DRAW_WRITE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_LTS_DRAW_WRITE_HH_
#define ONCE_LTS_DRAW_WRITE_HH_
#define CIRC_LTS_DRAW_WRITE_HH_

#include "point.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <string>
#include <list>
#endif

// Luokan esittely:

class LTS_Draw_Write
{
public:

    enum SymbolFlag { CUTFLAG=1, STICKYFLAG=2 };

    LTS_Draw_Write(ostream& output);
    /* Rakentajalle annetaan se stream, johon komennot tulostetaan.
     */

    void TulostaTila (
        Point coords,
        const string& name,
        const string& bordercolor,
        const string& fillcolor,
        int symbolFlags,
        const list<string>& propNames
        );

    void TulostaTapahtuma (
        list<Point>& viiva,
        const string& name,
        const string& color,
        const string& dash
        );

    void TulostaTeksti (
        Point coords,
        const string& teksti
        );

    void TulostaTilaselite (
        const string& name,
        const string& bordercolor,
        const string& fillcolor,
        int symbolFlags = 0
        );

    void TulostaTapahtumaselite (
        const string& name,
        const string& color,
        const string& dash
        );

    void TulostaHistoria (
        const string& teksti
        );

    ~LTS_Draw_Write();

private:
    ostream& out;

    // Kopiointiesto
    //LTS_Draw_Write (const LTS_Draw_Write&);
    //LTS_Draw_Write& operator=(const LTS_Draw_Write&);
};


#undef CIRC_LTS_DRAW_WRITE_HH_
#endif

