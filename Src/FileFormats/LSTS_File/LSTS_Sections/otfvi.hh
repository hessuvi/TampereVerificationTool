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

// FILE_DES: constants.hh: FileFormats/Lexical
// Timo Erkkilä

// The namespace OTFVI (On The Fly Verification Information).
// WHO NEEDS: StatePropsSection and StatePropNamesSection.

#include <string>


namespace OTFVI
{
    static const unsigned AMOUNT = 5;
    
    enum OTFV_BIT
    {
        CUT_STATE = 1,
        REJECT_STATE = 2,
        DL_REJECT_STATE = 3,
        LL_REJECT_STATE = 4,
        INF_REJECT_STATE = 5
    };
    
    static const std::string KEY_WORDS[] =
    {
        "cut",
        "rej",
        "dl_rej",
        "ll_rej",
        "inf_rej"
    };

    static const std::string OTFVI_NAMES[] =
    { 
        "\\OTFVI: CUT_STATE",
        "\\OTFVI: REJECT_STATE",
        "\\OTFVI: DL_REJECT_STATE",
        "\\OTFVI: LL_REJECT_STATE",
        "\\OTFVI: INF_REJECT_STATE"
    };
            
}
