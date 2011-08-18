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

// FILE_DES: graflib.hh: Grafiikka/piirtoalusta/rajapinta
// Juha Nieminen

// $Id: graflib.hh 1.6 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// Kaikkien piirtoalustojen yhteisen rajapinnan kuvaus
// GraphicLibrary:
//      GraphicLibrary : Kirjaston alustus komentoriparametreista
//      Init : Kirjaston fyysinen alustus ja konfigurointi
//      CreateDrawingWindow : Luo piirtoikkunan
//      CreateTextWindow : Luo teksti-ikkunan
//      DWindowXSize : Palauttaa piirtoikkunan x-koon loogisina pikselein‰
//      DWindowYSize : Palauttaa piirtoikkunan y-koon loogisina pikselein‰
//      DrawLine : Piirt‰‰ viivan annetuun piirtoikkunaan
//      DrawLines : Piirt‰‰ viivajoukon annettuun piirtoikkunaan
//      DrawFilledPolygon : Piirt‰‰ t‰ytetyn polygonin annettuun piirtoikkunaan
//      DrawArrowHead : Piirt‰‰ nuolenp‰‰n (ei vartta) annetuun piirtoikkunaan
//      DrawFilledCircle : Piirt‰‰ t‰ytetyn ympyr‰n annetuun piirtoikkunaan
//      DrawText : Piirt‰‰ teksti‰ annetuun piirtoikkunaan
//      PrintText : Tulostaa teksti‰ annettuun teksti-ikkunaan
//      Done : Kutsutaan, kun kaikki on valmista

// $Log:$

#ifdef CIRC_GRAFLIB_HH_
#error "Include recursion"
#endif

#ifndef ONCE_GRAFLIB_HH_
#define ONCE_GRAFLIB_HH_
#define CIRC_GRAFLIB_HH_

#ifndef MAKEDEPEND
#include <string>
#endif

class GraphicLibrary
{
public:

    // Section 1. Laitealustan alustus
    // -------------------------------

    GraphicLibrary( int& argc, char** &argv );
    /* Konstruktorille annetaan komentoriviparametrit, joista se poimii ne,
       jotka sit‰ kiinnostavat (ja poistaa ne sielt‰).
     */

    // Section 2. Piirtokomennot
    // -------------------------

    typedef void* DWindow;
    typedef void* TWindow;
    /* Piirtoikkuna- ja teksti-ikkuna-alkiot. Ikkunanluontimetodit palauttavat
       t‰m‰n tyyppisi‰ alkioita, joita sitten annetaan parametrina piirto-
       metodeille.
       (void* on kompromissi; ehk‰ muutettava Class jotain*:ksi)
     */

    class Callback
    { public:
      virtual ~Callback() {};
        virtual void DrawingFunction(DWindow) = 0;
        virtual void HandleKeyPress(char key) = 0;
        virtual void HandleButtonClick(unsigned buttonNumber) = 0;
        virtual void HandleMouseClick(DWindow,
                                      unsigned xCoord, unsigned yCoord,
                                      unsigned button) = 0;
    };
    /* Luokka, jonka metodeja GraphicLibrary kutsuu eventtien tapahtuessa.
       GraphicLibrary:n k‰ytt‰j‰luokka tulisi olla periytetty t‰st‰.
         DrawingFunktion() on se metodi, joka sis‰lt‰‰ kaikki kutsut
       GraphicLibrary:n piirtokomentoihin.
     */

    virtual void Init (
        Callback&,
        char* documentTitle,
        int gridx, int gridy
        ) = 0;
    /* Grafiikkakirjaston alustus. Se luo p‰‰ikkunan (joko loogisen tai
       fyysisen), johon voidaan luoda piirto- ja teksti-ikkunoita. Ikkunan
       todellinen fyysinen koko on toteutuksen p‰‰tett‰viss‰.
         Ensimm‰inen parametri on viite siihen callback-olioon, jonka
       metodeja grafiikkakirjasto kutsuu.
         documentTitle on t‰m‰n ikkunan nimi (laitetaan esimerkiksi
       ikkunan otsikkopalkkiin).
         Ikkuna jaetaan gridx:‰‰n loogiseen sarakkeeseen ja gridy:hyn
       loogiseen riviin. Ikkunanluontifunktioille annetaan luotavan ikkunan
       koko n‰iden sarakkeiden ja rivien mukaan. gridx:n ja gridy:n arvo on
       oltava v‰hint‰‰n 1. Jokaisen solun koko on sama.
     */

    virtual void CreateButton(const char* label,
                              int leftColumn, int rightColumn,
                              int upRow, int downRow)=0;

    virtual DWindow CreateDrawingWindow (
        int leftColumn, int rightColumn, int upRow, int downRow,
        int minXPixels=0, int minYPixels=0
        ) = 0;
    /* Luo piirtoikkunan ja palauttaa sen tyyppisen alkion (joka annetaan
       sitten parametrina piirtometodeille jotta ne tiet‰v‰t, mihin ikkunaan
       piirret‰‰n).
         Ikkunan koko m‰‰ritell‰‰n Init():ille annettujen sarakkeiden ja
       rivien mukaan. leftColumn ja upRow m‰‰rittelev‰t ikkunan vasemman
       yl‰kulman ja rightColumn ja downRow oikean alakulman. Laillisia
       arvoja ovat arvot v‰lilt‰ 0 - gridx leftColumn:ille ja
       rightColumn:ille, sek‰ 0 - gridy upRow:lle ja downRow:lle.
         leftColumn:in ja rightColumn:in arvo 0 tarkoittaa ensimm‰isen
       sarakkeen vasenta reunaa, arvo 1 tarkoittaa ensimm‰isen sarakkeen
       oikeaa reunaa ja samalla toisen sarakkeen vasenta reunaa, arvo 2
       tarkoittaa toisen sarakkeen oikeaa reunaa ja samalla kolmannen
       sarakkeen vasenta reunaa, jne. upRow ja downRow toimivat vastaavasti
       mutta rivien kanssa.
         Jos siis haluamme luoda piirtoikkunan, joka on vasemman yl‰kulman
       solussa (ja on kooltaan vain sen yhden solun verran), olisi silloin
       leftColumn=0, upRow=0, rightColumn=1, downRow=1.
         minXPixels ja minYPixels m‰‰rittelev‰t piirtoalueen minimikoon
       pikseleiss‰ (loogisina tai fyysisin‰). DWindowXSize() ja DWindowYSize()
       palauttavat v‰hint‰‰n t‰m‰n suuruisia arvoja. N‰it‰ voi k‰ytt‰‰ mik‰li
       haluaa piirrett‰v‰n alueen olevan suurempi kuin varsinainen piirto-
       ikkuna (joka on tietenkin toteutusriippuvaista). Defaulttiarvo 0
       tarkoittaa k‰yt‰nnˆss‰ sit‰, ett‰ toteutus p‰‰tt‰‰ itse piirtoalueen
       pikselim‰‰r‰t.
     */

    virtual TWindow CreateTextWindow(int leftColumn, int rightColumn,
                                     int upRow, int downRow) = 0;
    /* Aivan vastaava kuin CreateDrawingWindow(), mutta luo teksti-ikkunan.
       Teksti-ikkunaan tulostettu teksti k‰ytt‰ytyy pitk‰lti samaan tapaan
       kuin esim. stdout:iin tulostettu teksti.
     */

    virtual int DWindowXSize (DWindow) = 0;
    virtual int DWindowYSize (DWindow) = 0;
    /* Palauttavat annetun piirtoikkunan koon pikselein‰ (loogisina tai
       fyysisin‰). N‰it‰ voi (ja tulisi) k‰ytt‰‰ piirtokomentojen
       skaalaamiseen oikeaan kokoon.
         Kun toteutus k‰ytt‰‰ loogisia pikseleit‰, tulisi n‰iden arvojen
       olla tarpeeksi suuria, jotta piirto onnistuu riitt‰v‰n tarkalla
       resoluutiolla.
     */

    virtual void DrawLine (
        DWindow,
        int x1, int y1,
        int x2, int y2,
        const std::string& color,
        const std::string& dash
        ) = 0;
    /* Piirt‰‰ viivan annettuun piirtoikkunaan.
       x1 ja y1 ovat alkupisteen koordinaatit pikselein‰ (loogisina tai
       fyysisin‰) ja x2 ja y2 vastaavasti loppupisteen.
         color on string, joka m‰‰rittelee viivan v‰rin.
         dash on string, joka m‰‰rittelee viivan kuvioinnin.
     */

    virtual void DrawLines (
        DWindow,
        int* points,
        int npoints,
        const std::string& color,
        const std::string& dash
        ) = 0;
    /* Piirt‰‰ viivajoukon annettuun piirtoikkunaan.
       points-taulukko sis‰lt‰‰ npoints*2 arvoa, jotka ovat koordinaatti-
       pareja.
     */

    virtual void DrawFilledPolygon (
        DWindow,
        int* points,
        int npoints,
        const std::string& color
        ) = 0;
    /* Piirt‰‰ t‰ytetyn polygonin annettuun piirtoikkunaan.
       points-taulukko sis‰lt‰‰ npoints*2 arvoa, jotka ovat koordinaatti-
       pareja.
     */

    virtual void DrawArrowHead (
        DWindow,
        int x1, int y1,
        int x2, int y2,
        int arrowLength,
        const std::string& color
        );
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
       luokassa. Kantaluokan toteutus kutsuu DrawFilledPolygon()-metodia.
     */
    
    virtual void DrawFilledCircle (
        DWindow,
        int x, int y,
        int Radius,
        const std::string& borderColor,
        const std::string& fillColor
        ) = 0;
    /* Piirt‰‰ t‰ytetyn ympyr‰n, jonka s‰de on Radius pikseli‰ kohtaan x,y
       annettuun piirtoikkunaan.
     */

    virtual void DrawText (
        DWindow,
        const std::string& s,
        int x, int y,
        const std::string& color
        ) = 0;
    /* Tulostaa teksti‰ annettuun piirtoikkunaan kohtaan x,y.
     */

    virtual void PrintText (
        TWindow,
        const std::string& s
        ) = 0;
    /* Tulostaa teksti‰ annettuun teksti-ikkunaan (pitk‰lti samaan tapaan
       kuin esim. stdout:iin tulostaessa).
     */

    virtual void ClearTextWindow(TWindow) = 0;

    virtual void Redraw() = 0;

    virtual void Done(void) = 0;
    /* Kutsutaan, kun kaikki muu on tehty. Graafisissa ymp‰ristˆiss‰ yleens‰
       j‰‰ odottamaan eventtej‰ ja lopettaa kun tulee jokin quit-event.
     */

    virtual void Quit() = 0;
    virtual void Exit() = 0;
  virtual int getStatus() = 0;
    /* Kun ohjelma halutaan lopettaa */

    virtual ~GraphicLibrary();

protected:
    virtual unsigned long string2color(const std::string&);
    /* Muuttaa annetun stringin long-tyyppiseksi arvoksi, joka sis‰lt‰‰
       v‰riarvon. 8 alinta bitti‰ ovat punainen, 8 seuraavaa bitti‰ vihre‰
       ja 8 seuraavaa sininen.
     */

private:
    GraphicLibrary (const GraphicLibrary&);
    GraphicLibrary& operator=(const GraphicLibrary&);
};


#undef CIRC_GRAFLIB_HH_
#endif

