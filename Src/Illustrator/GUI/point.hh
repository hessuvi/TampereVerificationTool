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

// FILE_DES: point.hh: Grafiikka/piirtoalusta
// Juha Nieminen

// $Id: point.hh 1.4 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// LTS-Viewerin käyttämä pisteluokka.
//

// $Log:$

#ifdef CIRC_POINT_HH_
#error "Include recursion"
#endif

#ifndef ONCE_POINT_HH_
#define ONCE_POINT_HH_
#define CIRC_POINT_HH_

class Point
{
public:
    inline Point(): x(0), y(0) { }
    inline Point(double d1, double d2): x(d1), y(d2) { }
    inline double xCoord() const { return x; }
    inline double yCoord() const { return y; }
    inline void xCoord(double d) { x=d; }
    inline void yCoord(double d) { y=d; }

private:
    double x,y;
};

#undef CIRC_POINT_HH_
#endif
