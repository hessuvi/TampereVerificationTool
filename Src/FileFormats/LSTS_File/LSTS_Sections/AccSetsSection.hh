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

#ifndef ACCSETSSECTION_HH
#define ACCSETSSECTION_HH

#include "FileFormat/Section.hh"
#include "LSTS_File/AccSetsAP.hh"
#include "Interval.hh"


// Class predeclaration:
class Header;


class iAccSetsSection : public iSection
{
 public:
    iAccSetsSection( iAccSetsAP& ap ) : iap( ap ) { } 
    virtual ~iAccSetsSection() { }
    virtual const char* GiveName() const { return "ACC_SETS"; }
    virtual void ReadSection( Header& );
 private:
    iAccSetsAP& iap;
};


class oAccSetsSection : public oSection, public iAccSetsAP
{
 public:
    oAccSetsSection( oAccSetsAP& ap ) : oap( &ap ) { }
    oAccSetsSection( OTokenStream& ot ) : oSection( ot ), oap( 0 ) { }
    virtual ~oAccSetsSection() { }
    virtual const char* GiveName() const { return "ACC_SETS"; }

    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );

    virtual void lsts_StartAccSets( Header& );
    // Writes the beginning of the section.
    
    virtual void lsts_StartAccSetsOfState( lsts_index_t state );
    virtual void lsts_StartSingleAccSet( lsts_index_t state );

    virtual void lsts_AccSetAction( lsts_index_t state, lsts_index_t action );
    virtual void lsts_AccSetActions( lsts_index_t state,
                                     lsts_index_t subrange_start,
                                     lsts_index_t subrange_end );

    virtual void lsts_EndSingleAccSet( lsts_index_t state );
    virtual void lsts_EndAccSetsOfState( lsts_index_t state );

    virtual void lsts_EndAccSets();
    // Writes the end of the section.

 private:
    Interval interv;
    oAccSetsAP* oap;
};


#endif
