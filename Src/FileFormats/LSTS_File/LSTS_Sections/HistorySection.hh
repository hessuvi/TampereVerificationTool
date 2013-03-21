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

#ifndef HISTORYSECTION_HH
#define HISTORYSECTION_HH

#include "FileFormat/Section.hh"


// Predeclarations:
class Header;


class iHistorySection : public iSection
{
 public:
    iHistorySection() : iSection() { }
    virtual ~iHistorySection() { }
    virtual const char* GiveName() const { return "HISTORY"; }
 private:
    virtual void ReadSection( Header& );
};


class oHistorySection : public oSection
{
 public:
    oHistorySection() : oSection() { }
    oHistorySection( OTokenStream& ot ) : oSection( ot ) { }
    virtual ~oHistorySection() { }
    virtual const char* GiveName() const { return "HISTORY"; }
 private:
    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );
};


#endif
