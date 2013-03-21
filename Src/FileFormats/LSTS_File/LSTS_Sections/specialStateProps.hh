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

#ifndef SPECIALSTATEPROPS_HH
#define SPECIALSTATEPROPS_HH

#include <string>


namespace SP
{
    static char special = '*';
    static unsigned prefix_length = 4;
    static unsigned postfix_length = prefix_length;

#define START  "* <["
#define END  "]> *"
    
    static char cut[] = START "cut state" END;
    static char rej[] = START "reject state" END;
    static char dl_rej[] = START "deadlock reject state" END;
    static char ll_rej[] = START "livelock reject state" END;
    static char inf_rej[] = START "infinity reject state" END;
    
#undef START
#undef END
    
    enum StatePropType
    {
        COMMON = 0,
        CUT,
        REJ,
        DL_REJ,
        LL_REJ,
        INF_REJ,
        SIZE
    };
    
    static const char* SpecialPropIds[] =
    {
        "COMMON",
        "CUT",
        "REJ",
        "DL_REJ",
        "LL_REJ",
        "INF_REJ",
        "index out of bounds!"
    };
    static char warningRemover = SpecialPropIds[0][ postfix_length ];
    
    
    static StatePropType
    GiveStatePropType( const std::string& name )
    {
        if ( name.empty() || name[ 0 ] != special )
        {
            return COMMON;
        }
        
        if ( name == cut )
        {
            return CUT;
        }
        
        if ( name == rej )
        {
            return REJ;
        }
        
        if ( name == dl_rej )
        {
            return DL_REJ;
        }
        
        if ( name == ll_rej )
        {
            return LL_REJ;
        }
        
        if ( name == inf_rej )
        {
            return INF_REJ;
        }
        
        return COMMON;
    }
    static StatePropType warningRemover2 = GiveStatePropType( "" );
    
}


#endif
