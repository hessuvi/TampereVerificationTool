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

// FILE_DES: iLSTS_File.hh: FileFormats/LSTS_File
// Timo Erkkilä

// $Id: iLSTS_File.hh 1.7 Tue, 28 Sep 2004 13:38:59 +0300 warp $
// 
// iLSTS_File provides means for reading components of LSTSs.

// $Log: LSTS_File.hh,v $
// Revision 1.1  1999/08/09 13:25:01  timoe
// Initial revision
//

#ifndef ILSTS_FILE_HH
#define ILSTS_FILE_HH

#include "LSTS_File/LstsFile.hh"
#include "LSTS_File/LSTSiAPpredecs.hh"
#include "StringTree/StringMap.hh"


class iLSTS_File : public LSTS_File
{
 public:
    iLSTS_File( InStream&, bool checks_on = true,
                bool allow_interrupted_lsts = false );
    // Reads Header and possible History sections and stores their
    //   information into a Header instance.
    virtual ~iLSTS_File();

    void AddStateNamesReader( iStateNamesAP& iap );
    void AddDivBitsReader( iDivBitsAP& iap );
    void AddAccSetsReader( iAccSetsAP& iap );
    void AddStatePropsReader( iStatePropsAP& iap );
    void AddActionNamesReader( iActionNamesAP& iap );
    void AddTransitionsReader( iTransitionsAP& iap );
    void AddLayoutReader( iLayoutAP& iap );

    // For experimental use only:

    OTokenStream& ConnectOutput( OutStream& os );

    //StringMap<unsigned>& StoreActionNames( class oLSTS_File& );
    //void ForwardActionNames( class oLSTS_File& );

    //    void ForwardStates( oLSTS_File& );
    void SyncReadAndWrite();

 protected:
    iLSTS_File( bool checks_on = true );
    void readHeader( const std::string& beginFile );

 private:
    // Preventing the use of copy constructor and assignment operator:
    iLSTS_File( const iLSTS_File& );
    iLSTS_File& operator=( const iLSTS_File& );

    bool can_read_interrupted_lsts;
};


#endif
