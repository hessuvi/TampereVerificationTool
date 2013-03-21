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

// FILE_DES: lts_viewer.hh: Grafiikka/piirtoalusta
// Juha Nieminen

// $Id: lts_viewer.hh 1.13 Mon, 31 May 2004 17:20:54 +0300 warp $
// 
// Luokka, jonka avulla piirret‰‰n LTS-kuvia. K‰ytt‰‰ GraphicLibrarya ja
// toimii rajapintana sen ja p‰‰ohjelman v‰lill‰.
// LTS_Viewer:
//     LTS_Viewer: Luokan alustus jolle annetaan k‰ytett‰ grafiikkakirjasto
//                 ja piirtofunktio
//     PiirraTila: Piirt‰‰ tilan annettuun kohtaan annetuilla v‰reill‰
//     PiirraTapahtuma: Piirt‰‰ tapahtuman (nuolen) annetun viivan mukaan
//     PiirraTeksti: Piirt‰‰ teksti‰ LTS-ikkunaan
//     TulostaTeksti: Tulostaa teksti‰ teksti-ikkunaan
//     PiirraTilaselite: Piirt‰‰ tilaselitteen annetuilla v‰reill‰ ja nimell‰
//     PiirraTapahtumaselite: Piirt‰‰ tapahtumaselitteen annetuilla v‰reill‰
//                            ja nimell‰.
//     KuvanKoko: Asettaa LTS-kuvan koon skaalausta varten.
//     TilanSade: Asettaa tilan s‰teen.
//     Done: Kutsutaan yleens‰ heti instanssin luomisen j‰lkeen.

// $Log:$

#ifdef CIRC_LTS_VIEWER_HH_
#error "Include recursion"
#endif

#ifndef ONCE_LTS_VIEWER_HH_
#define ONCE_LTS_VIEWER_HH_
#define CIRC_LTS_VIEWER_HH_

#include "graflib.hh"
#include "point.hh"

#ifndef MAKEDEPEND
#include <string>
#include <list>
#include <vector>
using namespace std;
#endif

class LTS_Viewer:public GraphicLibrary::Callback
{
public:

    class LTS_Draw
    {
    public:
        virtual void PiirraLTSIkkuna(LTS_Viewer*)=0;
        virtual void PiirraTapahtumaseliteIkkuna(LTS_Viewer*)=0;
        virtual void PiirraTilapropositioIkkuna(LTS_Viewer*)=0;
        virtual void PiirraTilaseliteIkkuna(LTS_Viewer*)=0;
        virtual bool HandleButtonClick(unsigned buttonNumber)=0;
        virtual void HandleMouseClick(LTS_Viewer*,
                                      unsigned x, unsigned y, unsigned b)=0;

        virtual ~LTS_Draw() {}
    };
    /* Abstrakti luokka, josta periytet‰‰n piirtokomennot sis‰lt‰v‰
       toteutus. Sen metodeja kutsutaan aina tarvittaessa (aluksi aina kerran
       ja sen j‰lkeen esimerkiksi kun ikkunan kokoa muutetaan ja kaikki
       joudutaan piirt‰m‰‰n uudestaan).
     */

    LTS_Viewer(GraphicLibrary*, LTS_Draw&,
               unsigned actionAmnt, unsigned statePropsAmnt);
    /* Rakentaja. Ensimm‰inen parametri on k‰ytetty grafiikkakirjasto (joka
       on periytetty GraphicLibrary:sta). Toinen parametri on LTS_Draw:ista
       periytetty olio, joka suorittaa kaiken piirt‰misen.
         Yleens‰ instanssin luomisen j‰lkeen kutsutaan suoraan Done()-metodia.
     */

    virtual ~LTS_Viewer();

    void resetStatePropNames() { statePropCoords.clear(); }

    bool printStatePropsForCoords(unsigned x, unsigned y);

    void PiirraTila (
        Point coords,
        const string& bordercolor, const string& fillcolor,
        bool isCutState, bool isStickyState, const list<string>& stateProps
        );
    /* Piirt‰‰ tilan LTS-ikkunaan kohtaan x,y annetuilla v‰reill‰.
     */

    void PiirraTapahtuma (
        const list<Point>& viiva,
        const string& color, const string& dash=DefaultDash
        );
    /* Piirt‰‰ tapahtuman (nuolen) LTS-ikkunaan annetun pistelistan mukaan
       annetulla v‰rill‰ ja dash-kuviolla.
     */
    Point getArrowHeadCoord(const list<Point>& viiva);

    void PiirraTeksti (
        const string& s,
        Point coords
        );
    /* Piirt‰‰ teksti‰ LTS-ikkunaan annettuun kohtaan.
     */

    void TulostaTeksti(const string& s);
    /* Tulostaa teksti‰ teksti-ikkunaan.
     */

    void PiirraTilaselite(
        const string& nimi,
        const string& bordercolor, const string& fillcolor,
        bool isCutState, bool isStickyState
        );
    /* Piirt‰‰ tilaselitteen (tila+nimi) tilaseliteikkunaan.
     */

    void PiirraTapahtumaselite(
        const string& nimi,
        const string& color, const string& dash=DefaultDash
        );
    /* Piirt‰‰ tapahtumaselitteen (tapahtuma+nimi) tapahtumaseliteikkunaan.
     */

    void PiirraTilapropositionimet(const vector<string>& nimet);

    void KuvanKoko(double minx, double miny, double width, double height);
    /* Annetaan LTS-kaavion dimensiot, jotta luokka osaa skaalata sen oikean
       kokoiseksi LTS-ikkunaan.
     */

    void TilanSade(double radius);
    /* Asettaa tilaa esitt‰v‰n ympyr‰n s‰teen.
     */

    void Done(void);
    /* Kutsutaan yleens‰ instanssin luomisen j‰lkeen. J‰‰ odottamaan
       tapahtumia ikkunoiduissa j‰rjestelmiss‰.
     */


    void DrawTextToAbsoluteCoords(const string& s, Point coords);

    inline Point LowerLeft()
    { return Point(4, GL->DWindowYSize(automaatti_ikkuna)-4); }

    inline Point toScreenCoords(const Point& p) const
    {
        return Point(PositionX(p.xCoord()), PositionY(p.yCoord()));
    }

    void highlightActionName(const string& name);

protected:
    void DrawingFunction(GraphicLibrary::DWindow);
    void HandleKeyPress(char key);
    void HandleButtonClick(unsigned buttonNumber);
    void HandleMouseClick(GraphicLibrary::DWindow,
                          unsigned xCoord, unsigned yCoord,
                          unsigned button);


private:
    static const string DefaultDash;

    GraphicLibrary* GL;
    double tilasade;
    double tilaseliteYCoord, tapahtumaseliteYCoord;
    const vector<string>* tilapropositiot;
    unsigned tilapropositioInd;

    GraphicLibrary::DWindow tilaseliteikkuna;
    GraphicLibrary::DWindow tapahtumaseliteikkuna;
    GraphicLibrary::DWindow automaatti_ikkuna;
    GraphicLibrary::DWindow tilapropositioikkuna;
    GraphicLibrary::TWindow teksti_ikkuna;

    double lts_midx, lts_midy, lts_scale, zoomingFactor, zoomX, zoomY;

    LTS_Draw& redrawFunctions;

    struct StatePropNames
    {
        const list<string>* names;
        unsigned xCoord, yCoord;

        StatePropNames(const list<string>& propNames, unsigned x, unsigned y):
            names(&propNames), xCoord(x), yCoord(y)
        {}
    };

    list<StatePropNames> statePropCoords;

    string highlightedActionName;
    bool showStateNames;
    unsigned stateNamesClicked;

    int PositionX(double x) const;
    int PositionY(double y) const;
    int Skaalaa(double d) const;
    double TilaSade() const;
};

#undef CIRC_LTS_VIEWER_HH_
#endif
