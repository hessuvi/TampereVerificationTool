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

// FILE_DES: lts_draw_reader.cc: Grafiikka/piirtoalusta
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: lts_draw_reader.cc 1.9 Fri, 10 Sep 2004 13:30:57 +0300 timoe $";
// 
// Luokkien ja funktioiden toteutus lts-komentojen lukemista varten
// tiedostosta.
//

// $Log:$

#include "lts_draw_reader.hh"

#ifndef MAKEDEPEND
#include <cstdlib>
#include <cctype>
#include <vector>
#endif

using namespace std;

// Kantaluokan toteutus
//===========================================================================

LTS_Item::LTS_Item(ItemType t): type(t) { }

LTS_Item::~LTS_Item() { }

static const string NullString("");

int LTS_Item::NumberOfCoords() { return 0; }
Point LTS_Item::GetCoords() { return Point(0,0); }
const string& LTS_Item::GetColor() { return NullString; }
const string& LTS_Item::GetDash() { return NullString; }
const string& LTS_Item::GetText() { return NullString; }
bool LTS_Item::isCutState() { return false; }
bool LTS_Item::isSticky() { return false; }

const list<string>& LTS_Item::getStringData()
{
    static list<string> dummy;
    return dummy;
}


// Periytetyt luokat
//==================

// Tilaluokka
//===========================================================================
class LTS_Tila:public LTS_Item
{
public:
// Kantaluokan rajapinnan toteutus
    Point GetCoords() { return p; }
    void setCoords(Point np) { p = np; }

    const string& GetColor()
    {   switch(whichcolor)
        { case 0: whichcolor=1; return bordercolor;
          case 1: whichcolor=0;
        }
        return fillcolor;
    }

    bool isCutState() { return cutBit; }
    bool isSticky() { return stickyBit; }

    const list<string>& getStringData() { return stateProps; }

// Toteutuskohtaiset funktiot
    LTS_Tila(double x, double y, const string& bc, const string& fc,
             bool isCut, bool sticky, list<string> props):
        LTS_Item(TILA), p(x,y),
        bordercolor(bc), fillcolor(fc), whichcolor(0),
        cutBit(isCut), stickyBit(sticky), stateProps(props)
    { }

    virtual ~LTS_Tila() { }

private:
    Point p;
    string bordercolor, fillcolor;
    int whichcolor;
    bool cutBit, stickyBit;
    list<string> stateProps;
};

// Tapahtumaluokka
//===========================================================================
class LTS_Tapahtuma:public LTS_Item
{
public:
// Kantaluokan rajapinnan toteutus
    int NumberOfCoords() { return points.size(); }

    Point GetCoords()
    {
        list<Point>::iterator i=ptr;
        ++ptr; if(ptr==points.end()) ptr=points.begin();
        return *i;
    }

    list<Point>& getCoordsList() { return points; }

    const string& GetColor() { return color; }
    const string& GetDash() { return dash; }

// Toteutuskohtaiset funktiot
    LTS_Tapahtuma(const string& c, const string& d):
        LTS_Item(TAPAHTUMA), color(c), dash(d)
    { }

    virtual ~LTS_Tapahtuma() { }

    void AddCoords(double x, double y)
    {
        points.insert(points.end(), Point(x,y));
        ptr=points.begin();
    }

private:
    list<Point> points;
    list<Point>::iterator ptr;
    string color, dash;
};

// Tekstiluokka
//===========================================================================
class LTS_Teksti:public LTS_Item
{
public:
// Kantaluokan rajapinnan toteutus
    Point GetCoords() { return coord; }
    void setCoords(Point np) { coord = np; }
    const string& GetText() { return teksti; }

// Toteutuskohtaiset funktiot
    LTS_Teksti(const string& s, double x, double y):
        LTS_Item(TEKSTI), teksti(s), coord(x,y)
    { }

private:
    string teksti;
    Point coord;
};

// Tilaseliteluokka
//===========================================================================
class LTS_Tilaselite:public LTS_Item
{
public:
// Kantaluokan rajapinnan toteutus
    const string& GetColor()
    {   switch(whichcolor)
        { case 0: whichcolor=1; return bordercolor;
          case 1: whichcolor=0;
        }
        return fillcolor;
    }
    const string& GetText() { return nimi; }

    bool isCutState() { return cutBit; }
    bool isSticky() { return stickyBit; }

// Toteutuskohtaiset funktiot
    LTS_Tilaselite(const string& n, const string& bc, const string& fc,
        bool isCut, bool sticky):
        LTS_Item(TILASELITE),
        nimi(n), bordercolor(bc), fillcolor(fc), whichcolor(0),
        cutBit(isCut), stickyBit(sticky)
    { }

private:
    string nimi, bordercolor, fillcolor;
    int whichcolor;
    bool cutBit, stickyBit;
};

// Tapahtumaseliteluokka
//===========================================================================
class LTS_Tapahtumaselite:public LTS_Item
{
public:
// Kantaluokan rajapinnan toteutus
    const string& GetColor() { return color; }
    const string& GetDash() { return dash; }
    const string& GetText() { return nimi; }

// Toteutuskohtaiset funktiot
    LTS_Tapahtumaselite(const string& n,const string& c,const string& d):
        LTS_Item(TAPAHTUMASELITE),
        nimi(n), color(c), dash(d)
    { }

private:
    string nimi, color, dash;
};

// Historyluokka
//===========================================================================
class LTS_History:public LTS_Item
{
public:
// Kantaluokan rajapinnan toteutus
    const string& GetText() { return text; }

// Toteutuskohtaiset funktiot
    LTS_History(const string& t):
        LTS_Item(HISTORY), text(t)
    { }

private:
    string text;
};





//===========================================================================
//===========================================================================
// LTS-komentojen lukufunktio
//===========================================================================

static bool Search(const string& l, int& i, char c, char stop=0)
{
    while(l[i] && l[i]!=c)
    {
        if(l[i]==stop) return false;
        i++;
    }
    if(!l[i]) return true;
    i++;
    return false;
}

static bool SearchString(const string& line, int& i, string& dest,
                         char delim = '"')
{
    if(Search(line,i,delim)) return true;
    int j=i;
    if(Search(line,j,delim)) return true;
    dest=line.substr(i,j-i-1);
    i=j;
    return false;
}

// LTSData class
// -------------
void LTSData::readData(std::istream& InFile)
{
    string line;
    int i, linenumber=1;
    bool Ok;

    while(InFile)
    {
        getline(InFile,line);
        line+='\0';
        i=0;
        while(isspace(line[i])) i++;

        Ok=false;
        switch(line[i])
        {
          case 'S': // Tila
          {
              int flags = line[i+1]-'0';
              if(flags < 0 || flags > 3) flags = 0;
              bool cut = (flags&1) != 0, sticky = (flags&2) != 0;
              if(Search(line,i,'(')) break;
              double x=atof(line.substr(i).c_str());
              if(Search(line,i,',')) break;
              double y=atof(line.substr(i).c_str());
              string name;
              if(SearchString(line,i,name)) break;
              string bc;
              if(SearchString(line,i,bc)) break;
              string fc;
              if(SearchString(line,i,fc)) break;

              list<string> props;
              props.push_back(name);
              if(!Search(line,i,'{'))
              {
                  while(line[i]!='}')
                  {
                      props.resize(props.size()+1);
                      if(SearchString(line,i,props.back(),1)) break;
                  }
              }

              data.push_back(new LTS_Tila(x,y,bc,fc,cut,sticky,props));
              Ok=true;
              break;
          }

          case 'T': // Tapahtuma
          {
              vector<double> points;
              bool pointsOk=false;
              while(1)
              {
                  if(Search(line,i,'(','}')) { pointsOk=false; break; }
                  if(line[i]=='}') break;
                  points.push_back(atof(line.substr(i).c_str()));
                  if(Search(line,i,',')) { pointsOk=false; break; }
                  points.push_back(atof(line.substr(i).c_str()));
                  pointsOk=true;
              }
              if(!pointsOk) break;
              string name;
              if(SearchString(line,i,name)) break;
              string color;
              if(SearchString(line,i,color)) break;
              string dash;
              if(SearchString(line,i,dash)) break;
              LTS_Tapahtuma* t=new LTS_Tapahtuma(color,dash);
              for(unsigned i=0;i<points.size();i+=2)
                  t->AddCoords(points[i], points[i+1]);
              data.push_back(t);
              Ok=true;
              break;
          }

          case 'N': // Teksti
          {
              if(Search(line,i,'(')) break;
              double x=atof(line.substr(i).c_str());
              if(Search(line,i,',')) break;
              double y=atof(line.substr(i).c_str());
              string name;
              if(SearchString(line,i,name)) break;
              data.push_back(new LTS_Teksti(name,x,y));
              Ok=true;
              break;
          }

          case 'C': // Tilaselite
          {
              int flags = line[i+1]-'0';
              if(flags < 0 || flags > 3) flags = 0;
              bool cut = (flags&1) != 0, sticky = (flags&2) != 0;
              string name;
              if(SearchString(line,i,name)) break;
              string bc;
              if(SearchString(line,i,bc)) break;
              string fc;
              if(SearchString(line,i,fc)) break;
              data.push_back(new LTS_Tilaselite(name,bc,fc,cut,sticky));
              Ok=true;
              break;
          }

          case 'A': // Tapahtumaselite
          {
              string name;
              if(SearchString(line,i,name)) break;
              string color;
              if(SearchString(line,i,color)) break;
              string dash;
              if(SearchString(line,i,dash)) break;
              data.push_back(new LTS_Tapahtumaselite(name,color,dash));
              Ok=true;
              break;
          }

          case 'H': // History
          {
              string name;
              if(SearchString(line,i,name)) break;
              data.push_back(new LTS_History(name));
              Ok=true;
              break;
          }

          default:
              Ok=true;
        }

        if(!Ok) cerr << "Error at line " << linenumber << endl;

        linenumber++;
    }
}

LTSData::~LTSData()
{
    for(ItemList::iterator i = data.begin(); i != data.end(); ++i)
        delete (*i);
}

void LTSData::rotate()
{
    const double s45 = .70710678118654752440;
    for(ItemList::const_iterator i=data.begin(); i!=data.end(); i++)
    {
        if((*i)->Type() == LTS_Item::TAPAHTUMA)
        {
            LTS_Tapahtuma* tap = static_cast<LTS_Tapahtuma*>(*i);
            for(list<Point>::iterator i = tap->getCoordsList().begin();
                i != tap->getCoordsList().end(); ++i)
            {
                Point p = *i;
                *i = Point(p.xCoord()*s45+p.yCoord()*s45,
                           p.yCoord()*s45-p.xCoord()*s45);
            }
        }
        else if((*i)->Type() == LTS_Item::TILA)
        {
            LTS_Tila* tila = static_cast<LTS_Tila*>(*i);
            Point p = tila->GetCoords();
            tila->setCoords(Point(p.xCoord()*s45+p.yCoord()*s45,
                                  p.yCoord()*s45-p.xCoord()*s45));
        }
        else if((*i)->Type() == LTS_Item::TEKSTI)
        {
            LTS_Teksti* text = static_cast<LTS_Teksti*>(*i);
            Point p = text->GetCoords();
            text->setCoords(Point(p.xCoord()*s45+p.yCoord()*s45,
                                  p.yCoord()*s45-p.xCoord()*s45));
        }
    }
}
