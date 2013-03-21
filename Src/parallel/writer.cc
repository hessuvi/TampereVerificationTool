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

Contributor(s): Juha Nieminen.
*/

// FILE_DES: writer.cc: Parallel
// Nieminen Juha

static const char * const ModuleVersion=
  "Module version: $Id: writer.cc 1.14 Tue, 23 Mar 2004 17:01:05 +0200 warp $";
// 
// Rinnankytkijän lsts-kirjoittajan toteutus
//

// $Log:$

#include "writer.hh"
#include "LSTS_File/oLSTS_File.hh"

//#include "..."

#ifndef MAKEDEPEND
//#include <...>
using namespace std;
#endif

// Rakentaja ja purkaja
//===========================================================================
Writer::Writer(OutStream& os, OutputLSTS& ol, ParRules& pr,
               unsigned guardProcess)
{
    //cerr << "Writing result to '" << os.GiveFilename() << "'" << endl;

    oLSTS_File ofile;

    ofile.AddTransitionsWriter(ol.getTransitionsContainer());
    ofile.AddActionNamesWriter(pr.getActionNamesStore());
    if(ol.hasBeenInterrupted())
    {
        ofile.AddStateNamesWriter(ol);
    }

    StatePropsContainer& spcont =
        pr.getStatePropRules().getStatePropsContainer();
    ofile.AddStatePropsWriter(spcont);

    Header hd;
    hd.SetStateCnt(ol.StateCnt());
    hd.SetInitialState(ol.InitialState());
    hd.SetStatePropCnt(spcont.calculateStatePropCnt());

    if(ol.hasBeenInterrupted())
    {
        //hd.SetOnTheFlyError(ol.getInterruptedMsg());
        hd.SetOnTheFlyError(ol.getInterruptedErrorType(),
                            ol.getInterruptedState());
        if(ol.getInterruptedErrorType() == Header::LL_REJ)
            hd.SetGuard(guardProcess);
        ol.getTransitionsContainer().writeExtraDataToOutputFile();
    }

    ofile.GiveHeader() = hd;

    ofile.WriteFile(os);
}
