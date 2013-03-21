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

#ifndef LSTS_FILE_HH
#define LSTS_FILE_HH

#include "FileFormat/scFileFormat.hh"
#include "LSTS_File/Header.hh"


class LSTS_File : public scFileFormat
{
 public:
    LSTS_File( bool checks_on = true );
    virtual ~LSTS_File();

 protected:
    void LSTS_Format();
    void DoHeaderChecks( TokenStream* ) const;
    virtual void DoEndFileChecks( const std::vector<std::string>&
                                  passed_sections,
                                  TokenStream* tokenStream );
    Header header;

 private:
    // Preventing use of copy constructor and assignment operator:
    LSTS_File( const LSTS_File& );
    LSTS_File& operator=( const LSTS_File& );
};


#endif
