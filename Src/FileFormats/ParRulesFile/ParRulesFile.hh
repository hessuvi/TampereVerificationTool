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

Contributor(s): Juha Nieminen, Timo Erkkilä, Terhi Kilamo, Heikki Virtanen.
*/

// FILE_DES: ParrulesFile.hh: ParrulesFile
// Nieminen Juha

// $Id: ParRulesFile.hh 1.1 Thu, 19 Dec 2002 02:14:34 +0200 timoe $
// 
//

// $Log: ParrulesFile.hh,v $
// Revision 1.1  1999/08/12 13:22:18  warp
// Initial revision
//

#ifdef CIRC_PARRULESFILE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_PARRULESFILE_HH_
#define ONCE_PARRULESFILE_HH_
#define CIRC_PARRULESFILE_HH_

#include "FileFormat/scFileFormat.hh"
#include "LSTS_File/Header.hh"


// Class predeclarations:
class iParComponentsAP;
class iRulesAP;
class iStatePropRulesAP;
class iActionNamesAP;

class oParComponentsAP;
class oRulesAP;
class oStatePropRulesAP;
class oActionNamesAP;


class ParrulesFile: public scFileFormat
{
 public:
    virtual ~ParrulesFile();

 protected:
    ParrulesFile();
    virtual void DoEndFileChecks( const std::vector<std::string>&
                                  passed_sections,
                                  TokenStream* tokenStream );
 private:
    void init();
    Header header;
};


class iParrulesFile: public ParrulesFile
{
 public:
    iParrulesFile( InStream& );
    virtual ~iParrulesFile(); 

    void AddParComponentsReader( iParComponentsAP& iap );
    void AddRulesReader( iRulesAP& iap );
    void AddStatePropRulesReader( iStatePropRulesAP& iap );
    void AddActionNamesReader( iActionNamesAP& iap );

 private:
    // Kopiointiesto
    iParrulesFile (const iParrulesFile&);
    iParrulesFile& operator=(const iParrulesFile&);

    void readHeader( InStream& is );
};


class oParrulesFile: public ParrulesFile
{
 public:
    oParrulesFile();
    virtual ~oParrulesFile();

    void AddParComponentsWriter( oParComponentsAP& oap );
    void AddRulesWriter( oRulesAP& oap );
    void AddStatePropRulesWriter( oStatePropRulesAP& oap );
    void AddActionNamesWriter( oActionNamesAP& oap );

    void WriteFile( OutStream& os );

 private:
    // Kopiointiesto
    oParrulesFile (const oParrulesFile&);
    oParrulesFile& operator=(const oParrulesFile&);
};


#undef CIRC_PARRULESFILE_HH_
#endif
