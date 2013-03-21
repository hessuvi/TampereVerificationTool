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

// FILE_DES: lts_draw_reader.hh: Grafiikka/piirtoalusta
// Juha Nieminen

// $Id: lts_draw_reader.hh 1.7 Mon, 31 May 2004 17:20:54 +0300 warp $
// 
// Luokkien ja funktioiden esittely lts-komentojen lukemista varten
// tiedostosta.
// LTS_Item:
//     ItemType: Kertoo luokan tyypin
//     NumberOfCoords: Koordinaattipisteiden lkm.
//     GetCoords: Palauttaa seuraavan koordinaattiparin.
//     GetColor: Palauttaa alkion värin.
//     GetDash: Palauttaa alkion dash-patternin.
//     GetText: Palauttaa alkioon liittyvän tekstin (esim nimen).
//

// $Log:$

#ifdef CIRC_LTS_DRAW_READER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_LTS_DRAW_READER_HH_
#define ONCE_LTS_DRAW_READER_HH_
#define CIRC_LTS_DRAW_READER_HH_

#include "point.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <string>
#include <list>
#endif

class LTS_Item
{
public:
    enum ItemType { TILA, TAPAHTUMA, TEKSTI, TILASELITE, TAPAHTUMASELITE,
                    HISTORY };

    inline ItemType Type() { return type; }

    virtual int NumberOfCoords();
    virtual Point GetCoords();
    /* NumberOfCoords() kertoo, kuinka monta koordinaattiparia on tässä
       alkiossa. Koordinaattiparit voi lukea GetCoords():illa. Koordinaatteja
       on useampia lähinnä silloin, kun tyyppi on TAPAHTUMA.
     */

    virtual const std::string& GetColor();
    /* Palauttaa alkion värin. Jos alkiolla on monta väriä (reuna, sisus),
       niin peräkkäiset luvut palauttavat kyseiset värit.
     */

    virtual const std::string& GetDash();

    virtual const std::string& GetText();

    virtual bool isCutState();
    virtual bool isSticky();

    virtual const std::list<std::string>& getStringData();

    virtual ~LTS_Item();

protected:
    LTS_Item(ItemType t);

private:
    ItemType type;
};

typedef std::list<LTS_Item*> ItemList;

class LTSData
{
 public:
    ~LTSData();

    void readData(std::istream& InFile);
    inline const ItemList& getData() const { return data; }

    void rotate();

 private:
    ItemList data;
};

#undef CIRC_LTS_DRAW_READER_HH_
#endif
