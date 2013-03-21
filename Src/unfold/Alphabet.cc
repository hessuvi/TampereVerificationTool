#include "Alphabet.hh"


namespace { unsigned action_n = 0; }


void
ActionSubset::Print( iActionNamesAP& pipe )
{
    if ( par_types.empty() )
    { pipe.lsts_ActionName( ++action_n, gate ); return; }
    
    VarTable::SetUsedTypes( par_types );
    do { pipe.lsts_ActionName( ++action_n, VarTable::GenAction( gate ) ); }
    while ( VarTable::calcNextCombination() );
}


// LSTS-library methods:
lsts_index_t
Alphabet::lsts_numberOfActionNamesToWrite() { return action_cnt; }

void
Alphabet::lsts_WriteActionNames( iActionNamesAP& pipe )
{
    action_n = 0;
    for ( unsigned i = 1; i < subsets.size(); ++i )
    { subsets[i].Print( pipe ); }
}
