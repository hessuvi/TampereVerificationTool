/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is TVT-tools.

Copyright © 2003 Nokia and others. All Rights Reserved.

Contributor(s): Timo Erkkilä.
*/

// FILE_DES: SectionAP.hh: Src/FileFormats/LSTS_File/LSTS_Sections
// Timo Erkkilä

// $Id: SectionAP.hh 1.6 Sat, 24 Jan 2004 01:44:26 +0200 timoe $
// 
// SectionAP gives services for ...
//

// $Log:$

#ifndef SECTIONAP_HH
#define SECTIONAP_HH

#include "InOutStream.hh"


class iSectionAP
{
 public:
    virtual ~iSectionAP();
};


class oSectionAP
{
 public:
    oSectionAP() : include_out_stream( 0 ), include_filename( "" ) { }
    virtual ~oSectionAP();

    void SetIncludeFile( OutStream& includeFile )
    { include_out_stream = &includeFile; }

    OutStream* GiveIncludeFile() { return include_out_stream; }
    
    void SetIncludeFilename( const std::string& incFilename )
    { include_filename = incFilename; }

    const std::string& GiveIncludeFilename() const
    { return include_filename; }
    
 private:
    OutStream* include_out_stream;
    std::string include_filename;
};


#endif
