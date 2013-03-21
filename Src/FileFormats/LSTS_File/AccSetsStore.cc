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

// FILE_DES: AccSetsStore.cc: Src/FileFormats/LSTS_File
// Timo Erkkilä

static const char * const ModuleVersion=
  "Module version: $Id: AccSetsStore.cc 1.7.2.3 Fri, 08 Feb 2002 14:14:18 +0200 timoe $";
// 
// Implementation of AccSetsStore class.
//

// Acc sets are encoded into a vector in the following way:
//   state and action numbers are stored as they are, parenthesis are
//   encoded as zeros (semicolons are ignored).
// For example, acc sets
//
//   1 { 1 2 3 } { 4 5 };
//   2 { };
//   3;
//
// will look in their encoded form like this:
//
//   <1,0,1,2,3,0,0,4,5,0,2,0,0,3>
//

// $Log:$


#include "AccSetsStore.hh"
#include "error_handling.hh"
//#include "renumber.hh"

using namespace std;

#ifdef DEBUG
#include <iostream>
#endif


AccSetsStore::AccSetsStore() :
    iAccSetsAP(),
    oAccSetsAP()
{ }


bool
AccSetsStore::lsts_doWeWriteAccSets()
{
    return sets.size();
}

void
AccSetsStore::lsts_WriteAccSets( iAccSetsAP& ap )
{
    unsigned i = 0;

    while ( i < sets.size() )
    {

        lsts_index_t state = sets[ i ];
        ap.lsts_StartAccSetsOfState( state );

#ifdef DEBUG
    cout << state << "W[";
#endif

        // Non-zero here means that no more acc sets
        //   for this state is coming:
        while ( ++i < sets.size() && !sets[ i ] )
        {
            ap.lsts_StartSingleAccSet( state );

#ifdef DEBUG
    cout << "{";
#endif
            
            // Zero here means the end of the acc set:
            for ( ++i; sets[ i ]; ++i )
            {
                ap.lsts_AccSetAction( state, sets[ i ] );

#ifdef DEBUG
    cout << "a:" << sets[ i ];
#endif

            }

            ap.lsts_EndSingleAccSet( state );

#ifdef DEBUG
    cout << "}";
#endif

       }

        ap.lsts_EndAccSetsOfState( state );

#ifdef DEBUG
    cout << "]" << endl;
#endif

    } // End of while.

}

void
AccSetsStore::lsts_StartAccSets( Header& )
{
    sets.clear();
}

void
AccSetsStore::lsts_StartAccSetsOfState( lsts_index_t state )
{
    sets.push_back( state );

#ifdef DEBUG
    cout << state << "R[";
#endif

}

void
AccSetsStore::lsts_StartSingleAccSet( lsts_index_t /*state*/ )
{
    sets.push_back( 0 );

#ifdef DEBUG
    cout << "{";
#endif

}

void
AccSetsStore::lsts_AccSetAction( lsts_index_t /*state*/, lsts_index_t action )
{
    sets.push_back( action );

#ifdef DEBUG
    cout << "a:" << action;
#endif

}

void
AccSetsStore::lsts_EndSingleAccSet( lsts_index_t /*state*/ )
{
    sets.push_back( 0 );

#ifdef DEBUG
    cout << "}";
#endif

}

void
AccSetsStore::lsts_EndAccSetsOfState( lsts_index_t /*state*/ )
{

#ifdef DEBUG
    cout << "]" << endl;
#endif

}

void
AccSetsStore::lsts_EndAccSets() { }



//   1 { 1 2 3 } { 4 5 };
//   2 { };
//   3;
//
// will look in their encoded form like this:
//
//   <1,0,1,2,3,0,0,4,5,0,2,0,0,3>
//

static inline
void copyAccSetsOfState( unsigned& i, unsigned& f, vector<lsts_index_t>& se,
                         Renumber<lsts_index_t>& an_ren )
{
    while ( ++f < se.size() && !se[ f ] )
    {
        se[ ++i ] = 0;
        for ( ++f; se[ f ]; ++f )
        { 
            se[ ++i ] = an_ren.newNumber( se[ f ] );
        }
        se[ ++i ] = 0;
    }
    ++i;
}

static inline
void skip( unsigned& f, vector<lsts_index_t>& se )
{
    while ( ++f < se.size() && !se[ f ] )
    {    
        for ( ++f; se[ f ]; ++f )
        { 
            ;
        }
    }
}

lsts_index_t
AccSetsStore::RenumberAccSets( Renumber<lsts_index_t>& renum,
                               Renumber<lsts_index_t>& an_ren )
{
    // Renumbering acc sets' states:

    unsigned i = 0;
    unsigned f = 0;

    while ( f < sets.size() )
    {
        if ( renum.newNumber( sets[ f ] ) )
        {
            sets[ i ] = renum.newNumber( sets[ f ] );
            copyAccSetsOfState( i, f, sets, an_ren );
        }
        else
        {
            skip( f, sets );
        }
    }

    // Removing non-existing acc sets:

    lsts_index_t removed_amount = f - i;

    while ( i != f )
    {
        sets.pop_back();
        --f;
    }

    return removed_amount;
}
