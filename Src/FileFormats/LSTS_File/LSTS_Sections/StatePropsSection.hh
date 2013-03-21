/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2004 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

#ifndef STATEPROPSSECTION_HH
#define STATEPROPSSECTION_HH

#include "FileFormat/Section.hh"
#include "LSTS_File/StatePropsAP.hh"

#include "specialStateProps.hh"
#include "Interval.hh"


class iStatePropsSection : public iSection
{
 public:
    iStatePropsSection( iStatePropsAP& ap ) : iap( ap ) { } 
    virtual ~iStatePropsSection() { }
    virtual const char* GiveName() const { return "STATE_PROPS"; }

    virtual void ReadSection( Header& );
 private:
    iStatePropsAP& iap;
};


class oStatePropsSection : public oSection, public iStatePropsAP
{
 public:
    oStatePropsSection( oStatePropsAP& ap ) :
        type( SP::COMMON ), oap( &ap ) { }
    oStatePropsSection( OTokenStream& ot ) :
        oSection( ot ), type( SP::COMMON ), oap( 0 ) { }
    virtual ~oStatePropsSection() { }
    virtual const char* GiveName() const { return "STATE_PROPS"; }

    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );

    virtual void lsts_StartStateProps( Header& );
    // Writes the beginning of the section.

    virtual void lsts_StartPropStates( const std::string& );
    
    virtual void lsts_PropState( lsts_index_t );
    virtual void lsts_PropStates( lsts_index_t subrange_start,
                                  lsts_index_t subrange_end );
    
    virtual void lsts_EndPropStates( const std::string& );

    virtual void lsts_EndStateProps();
    // Writes the end of the section.
    
    void WriteSpecialProps();

 private:
    SP::StatePropType type;
    Interval interv;
    oStatePropsAP* oap;
};


#endif
