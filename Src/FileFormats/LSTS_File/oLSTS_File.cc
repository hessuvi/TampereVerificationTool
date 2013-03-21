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

// FILE_DES: oLSTS_File.cc: FileFormats/LSTS_File
// Timo Erkkilä

// $Id: oLSTS_File.cc 1.8 Tue, 28 Sep 2004 14:02:47 +0300 timoe $
// 
// LSTS_File defines the LSTS file format and provides means for
//   reading and writing components of LSTSs.

// $Log: oLSTS_File.cc,v $
// Revision 1.1  1999/08/09 13:25:01  timoe
// Initial revision
//

#include "oLSTS_File.hh"

#include "LSTS_Sections/HeaderSection.hh"
#include "LSTS_Sections/HistorySection.hh"
#include "LSTS_Sections/LSTS_Sections.hh"
#include "LSTS_Sections/LSTS_Checkers.hh"


oLSTS_File::oLSTS_File( bool checks_on ) : LSTS_File( checks_on ) { }
oLSTS_File::~oLSTS_File() { }

void
oLSTS_File::AddStateNamesWriter( const oStateNamesAP& oap )
{ AddOutputAP( oap ); }

void
oLSTS_File::AddDivBitsWriter( const oDivBitsAP& oap ) { AddOutputAP( oap ); }

void
oLSTS_File::AddAccSetsWriter( const oAccSetsAP& oap ) { AddOutputAP( oap ); }

void
oLSTS_File::AddStatePropsWriter( const oStatePropsAP& oap )
{ AddOutputAP( oap ); }

void
oLSTS_File::AddActionNamesWriter( const oActionNamesAP& oap ) 
{ AddOutputAP( oap ); }

void
oLSTS_File::AddTransitionsWriter( const oTransitionsAP& oap )
{ AddOutputAP( oap ); }

void
oLSTS_File::AddLayoutWriter( const oLayoutAP& oap ) { AddOutputAP( oap ); }

void
oLSTS_File::WriteFile( OutStream& os )
{
    OpenInputOTS( os );
    DoHeaderChecks( &ots() );
    oHistorySection ohistorySec;
    oHeaderSection oheaderSec( GiveHeader() );
    AddWriter( ohistorySec );
    AddWriter( oheaderSec );
    FileFormat::WriteFile( os );
}
