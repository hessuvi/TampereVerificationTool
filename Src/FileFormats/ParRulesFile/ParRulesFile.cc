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

// FILE_DES: ParrulesFile.cc: ParrulesFile
// Nieminen Juha

static const char * const ModuleVersion=
"Module version: $Id: ParRulesFile.cc 1.5 Wed, 15 Sep 2004 21:48:51 +0300 timoe $";
// 
// Sääntötiedostolukijan toteutus
//

// $Log: ParrulesFile.cc,v $
// Revision 1.2  1999/09/08 10:53:16  warp
// *** empty log message ***
//
// Revision 1.1  1999/08/12 13:22:18  warp
// Initial revision
//


#include "ParRulesFile.hh"

#include "LSTS_File/LSTS_Sections/HeaderSection.hh"
#include "ParRulesSections/ParComponentsSection.hh"
#include "ParRulesSections/RulesSection.hh"
#include "ParRulesSections/StatePropRulesSection.hh"
#include "LSTS_File/LSTS_Sections/ActionNamesSection.hh"
#include "LSTS_File/LSTS_Sections/ActionNamesChecker.hh"
#include "FileFormat/Lexical/charconv.hh"

// ############ ParrulesFile ###########

// Rakentajat ja purkaja
//===========================================================================

ParrulesFile::ParrulesFile() :
    scFileFormat( "PARALLEL_COMPOSITION_RULES_FILE",
                  "END_PARALLEL_COMPOSITION_RULES", false )
{ 
    SetHeader( header );
    init();
}

ParrulesFile::~ParrulesFile() { }

void
ParrulesFile::init()
{
    //               Name,    Priority, Optional?
    AddSection( "HEADER",                   -1, false );
    AddSection( "ACTION_NAMES",              1, true );
    AddSection( "PARCOMPONENTS",             2, false );
    AddSection( "STATE_PROP_RULES",          3, true );
    AddSection( "RULES",                     3, false );
}

void
ParrulesFile::DoEndFileChecks( const std::vector<std::string>& passed_secs,
                               TokenStream* tokenStream )
{
    if ( !areChecksOn() ) { return; }

    if ( header.GiveActionCnt() )
    {
        check_claim( doesInclude( passed_secs, "ACTION_NAMES" ),
                     tokenStream->errorMsg( "non-zero Action_cnt while "
                                            "no Action_names section" ) );
    }
}


// ############ iParrulesFile ###########

iParrulesFile::iParrulesFile( InStream& is ) : ParrulesFile()
{ readHeader( is ); }

iParrulesFile::~iParrulesFile() { }

void
iParrulesFile::AddParComponentsReader( iParComponentsAP& iap )
{ AddInputSection<iParComponentsSection>( iap ); }
void
iParrulesFile::AddRulesReader( iRulesAP& iap )
{ AddInputSection<iRulesSection>( iap ); }
void
iParrulesFile::AddStatePropRulesReader( iStatePropRulesAP& iap )
{ AddInputSection<iStatePropRulesSection>( iap ); }
void
iParrulesFile::AddActionNamesReader( iActionNamesAP& iap )
{ SetChecks( true ); AddInputAP( iap ); SetChecks( false ); }

void
iParrulesFile::readHeader( InStream& is )
{
    ReadBeginFile( is );
 
    std::string sectionName;
    ReadBeginSection( sectionName );
    
    check_claim( sectionName == "HEADER",
                 its().errorMsg( "Header section expected" ) );

    iHeaderSection iheader_section( GiveHeader() );
    ReadSection( iheader_section );
}


// ############ oParrulesFile ###########

oParrulesFile::oParrulesFile() : ParrulesFile() { }
oParrulesFile::~oParrulesFile() { }

void
oParrulesFile::AddParComponentsWriter( oParComponentsAP& oap )
{ AddOutputSection<oParComponentsSection>( oap ); }
void
oParrulesFile::AddRulesWriter( oRulesAP& oap )
{ AddOutputSection<oRulesSection>( oap ); }
void
oParrulesFile::AddStatePropRulesWriter( oStatePropRulesAP& oap )
{ AddOutputSection<oStatePropRulesSection>( oap ); }
void
oParrulesFile::AddActionNamesWriter( oActionNamesAP& oap )
{ AddOutputSection<oActionNamesSection>( oap ); }

void
oParrulesFile::WriteFile( OutStream& os )
{
    oHeaderSection oheaderSec( GiveHeader() );
    AddWriter( oheaderSec );
    FileFormat::WriteFile( os );
}
