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

#ifndef FF_LAYOUT_CHECKER_HH
#define FF_LAYOUT_CHECKER_HH

#include "FileFormat/Checker.hh"
#include "LSTS_File/LayoutAP.hh"


class LayoutChecker : public iLayoutAP, public Checker
{
 public:
    LayoutChecker( iLayoutAP& iap, class FileFormat& ff ) :
        Checker( ff ), ap( iap ) { }
    virtual ~LayoutChecker();
    
 private:
    virtual void lsts_StartLayout( class Header& );
    virtual void lsts_StateLayout( lsts_index_t state_number,
                                   int x, int y );
    virtual void lsts_EndLayout();
    
    lsts_index_t state_cnt;
    iLayoutAP& ap;
};


#endif
