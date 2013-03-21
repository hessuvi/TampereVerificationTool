/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#ifndef FF_LAYOUT_HH
#define FF_LAYOUT_HH

#include "SectionAP.hh"

// Predeclaration:
class Header;


class iLayoutAP : public iSectionAP
{
 public:
    virtual ~iLayoutAP();
    
    virtual void lsts_StartLayout( Header& ) = 0;
    
    virtual void lsts_StateLayout( lsts_index_t state_number,
                                   int x, int y ) = 0;
    
    virtual void lsts_EndLayout() = 0;
    
    typedef class iLayoutSection section_t;
    typedef class LayoutChecker checker_t;
};


class oLayoutAP : public oSectionAP
{
 public:
    virtual ~oLayoutAP();

    virtual bool lsts_doWeWriteLayout() = 0;
    virtual void lsts_WriteLayout( iLayoutAP& writer ) = 0;

    typedef class oLayoutSection section_t;
    typedef class LayoutChecker checker_t;
};


#endif
