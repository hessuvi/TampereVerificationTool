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

#ifndef OLSTS_FILE_HH
#define OLSTS_FILE_HH

#include "LSTS_File/LstsFile.hh"
#include "LSTS_File/LSTSoAPpredecs.hh"


class oLSTS_File : public LSTS_File
{
 public:
    oLSTS_File( bool checks_on = true );
    virtual ~oLSTS_File();

    void AddStateNamesWriter( const oStateNamesAP& oap );
    void AddDivBitsWriter( const oDivBitsAP& oap );
    void AddAccSetsWriter( const oAccSetsAP& oap );
    void AddStatePropsWriter( const oStatePropsAP& oap );
    void AddActionNamesWriter( const oActionNamesAP& oap );
    void AddTransitionsWriter( const oTransitionsAP& oap );
    void AddLayoutWriter( const oLayoutAP& oap );

    void WriteFile( OutStream& os );

    void SetHeader( Header& hd ) { header = hd; }

 private:
    // Preventing the use of copy constructor and assignment operator:
    oLSTS_File( const oLSTS_File& );
    oLSTS_File& operator=( const oLSTS_File& );
};


#endif
