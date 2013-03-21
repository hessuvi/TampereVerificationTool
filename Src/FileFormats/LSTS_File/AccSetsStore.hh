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

// FILE_DES: AccSetsStore.hh: Src/FileFormats/LSTS_File
// Timo Erkkilä

// $Id: AccSetsStore.hh 1.9 Fri, 08 Feb 2002 14:14:18 +0200 timoe $
// 
// This class is a store for acc_sets section. It can be used to store
//   acceptance sets and to later write them out.
//

// $Log:$

#ifdef CIRC_ACCSETSSTORE_HH_
#error "Include recursion"
#endif

#ifndef ONCE_ACCSETSSTORE_HH_
#define ONCE_ACCSETSSTORE_HH_
#define CIRC_ACCSETSSTORE_HH_


#include "AccSetsAP.hh"
#include "renumber.hh"

#include <vector>


// Predeclarations:

//class Renumber<lsts_index_t>;


class AccSetsStore : public iAccSetsAP, public oAccSetsAP
{
    
 public:
    AccSetsStore();

    virtual bool lsts_doWeWriteAccSets();
    virtual void lsts_WriteAccSets( iAccSetsAP& writer );
    
    virtual void lsts_StartAccSets( Header& );

    virtual void lsts_StartAccSetsOfState( lsts_index_t state );
    virtual void lsts_StartSingleAccSet( lsts_index_t state );

    virtual void lsts_AccSetAction( lsts_index_t state, lsts_index_t action );

    virtual void lsts_EndSingleAccSet( lsts_index_t state );
    virtual void lsts_EndAccSetsOfState( lsts_index_t state );

    virtual void lsts_EndAccSets();

    lsts_index_t RenumberAccSets( Renumber<lsts_index_t>& renum,
                                  Renumber<lsts_index_t>& action_names_renum );

 private:

    // No copy constructor nor assigment operation in use:
    AccSetsStore( const AccSetsStore& );
    AccSetsStore& operator=( const AccSetsStore& );

    std::vector<lsts_index_t> sets;
};


#undef CIRC_ACCSETSSTORE_HH_
#endif
