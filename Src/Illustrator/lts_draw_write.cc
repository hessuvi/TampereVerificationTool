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

// FILE_DES: lts_draw_write.cc: Grafiikka/piirtoalusta
// Juha Nieminen

static const char * const ModuleVersion=
  "Module version: $Id: lts_draw_write.cc 1.6 Mon, 31 May 2004 17:20:54 +0300 warp $";
// 
// LTS_Draw_Write-luokan toteutus.
//

// $Log:$

#include "lts_draw_write.hh"

// Rakentaja
//===========================================================================
LTS_Draw_Write::LTS_Draw_Write(ostream& output):
    out(output)
{
}

// Purkaja
//===========================================================================
LTS_Draw_Write::~LTS_Draw_Write() { }


// Tilan piirtokomennon tulostus
//===========================================================================
void LTS_Draw_Write::TulostaTila (
    Point coords,
    const string& name,
    const string& bordercolor,
    const string& fillcolor,
    int symbolFlags,
    const list<string>& propNames)
{
    if(symbolFlags) out << "S" << symbolFlags << " (";
    else out << "S (";
    out << coords.xCoord() << "," << coords.yCoord() << ") \""
        << name << "\" \"" << bordercolor << "\" \"" << fillcolor << "\"";
    if(propNames.size())
    {
        out << " {";
        for(list<string>::const_iterator iter = propNames.begin();
            iter != propNames.end(); ++iter)
        {
            out << (char)(1) << (*iter) << (char)(1);
        }
        out << "}";
    }
    out << endl;
}

// Tapahtuman piirtokomennon tulostus
//===========================================================================
void LTS_Draw_Write::TulostaTapahtuma (
    list<Point>& viiva,
    const string& name,
    const string& color,
    const string& dash)
{
    out << "T {";
    for(list<Point>::iterator i = viiva.begin(); i != viiva.end(); i++)
        out << "(" << i->xCoord() << "," << i->yCoord() << ")";
    out << "} \"" << name << "\" \"" << color << "\" \"" << dash << "\""
        << endl;
}

// Tekstin piirtokomennon tulostus
//===========================================================================
void LTS_Draw_Write::TulostaTeksti (
    Point coords,
    const string& teksti)
{
    out << "N (" << coords.xCoord() << "," << coords.yCoord() << ") \""
        << teksti << "\"" << endl;
}

// Tilaselitteen piirtokomennon tulostus
//===========================================================================
void LTS_Draw_Write::TulostaTilaselite (
    const string& name,
    const string& bordercolor,
    const string& fillcolor,
    int symbolFlags)
{
    if(symbolFlags) out << "C" << symbolFlags << " \"";
    else out << "C \"";
    out << name << "\" \"" << bordercolor << "\" \""
        << fillcolor << "\"" << endl;
}

// Tapahtumaselitteen piirtokomennon tulostus
//===========================================================================
void LTS_Draw_Write::TulostaTapahtumaselite (
    const string& name,
    const string& color,
    const string& dash)
{
    out << "A \"" << name << "\" \"" << color << "\" \""
        << dash << "\"" << endl;
}

// Historiatekstin tulostuskomennon tulostus
//===========================================================================
void LTS_Draw_Write::TulostaHistoria (
    const string& teksti)
{
    out << "H \"" << teksti << "\"" << endl;
}
