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

// HeaderSection takes care of reading the header attributes from an
//   input token stream or quering them from an user application and
//   writing them to an output token stream. It stores the attribute
//   values to Header class which preserves them and provides access
//   to them.
//     

#ifndef HEADERSECTION_HH
#define HEADERSECTION_HH

#include <string>

#include "FileFormat/Section.hh"
#include "config.hh"


class iHeaderSection : public iSection
{
 public:
    iHeaderSection( Header& hd ) : iSection(), header( hd ) { }
    virtual ~iHeaderSection() { }
    virtual const char* GiveName() const { return "HEADER"; }

 private:
    virtual void ReadSection( Header& );
    
    typedef void (iHeaderSection::*AttributeFunction)();
    // Method pointer. Points to attribute parsing methods.
    
    struct Attribute
    {
        const char* name;
        AttributeFunction parser;
    };
    // Attribute carries the name of attribute and the pointer to the function
    //   that is used to deal with reading of that attribute.
    
    static const Attribute header_attributes[];
    // A vector containing all the header attributes.
    
    lsts_index_t ReadAttributeNumber( const std::string& attribute_name );
    // This is for the use of the five first Set() methods (see below).

    bool readBoolean();
    
    // The following Read methods read an attribute from the header section and
    //   store it to HeaderAttributes class instance.
    void ReadInterrupted();

    void ReadStateCnt();
    void ReadActionCnt();
    void ReadElusiveActionCnt();
    void ReadTransitionCnt();
    void ReadStatePropCnt();

    void ReadInitialState();
    void ReadInitiallyUnstable();
    void ReadInitiallyStable();
    
    void ReadSemantics();
    
    void ReadNoLongTauLoops();
    void ReadShallowTaus();
    void ReadNoTaus();
    void ReadDeterministic();
    void ReadNormal();

    Header& header;
};


class oHeaderSection : public oSection
{
 public:
    oHeaderSection( Header& hd ) : oSection(), header( hd ) { }
    oHeaderSection( Header& hd, OTokenStream& ot ) :
        oSection( ot ), header( hd ) { }
    virtual ~oHeaderSection() { }
    virtual const char* GiveName() const { return "HEADER"; }
    
    void WriteHeaderAttributes();
    virtual bool doWeWriteSectionWithInit( Header& );
    virtual void WriteSection( Header& );
    // All three above are called by Section.

    void WriteHeader( OTokenStream& to );
    
    void WriteAssignment( const std::string& attr, lsts_index_t num );
    // For writing a line in format:
    // attribute_name = number
    
    void WriteBoolean( const std::string& attr, bool status );
    // For writing a line in format:
    // boolean_attribute_name

    Header& header;
};


#endif
