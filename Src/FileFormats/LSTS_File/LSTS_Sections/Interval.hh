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

// This class provides services for reading or writing numbers or
// subranges. Note: zero is excluded! (It has a special meaning in the
// code below.)


#ifdef CIRC_INTERVAL_HH_
#error "Include recursion"
#endif

#ifndef ONCE_INTERVAL_HH_
#define ONCE_INTERVAL_HH_
#define CIRC_INTERVAL_HH_

#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "FileFormat/Checker.hh"


class Interval
{
    
 public:

    Interval() : i_start( 0 ), i_end( 0 ), out_inter_disabled( false ) { }

    static inline
    const char* GiveSubrangePunctuator() { return ".."; }
    
    static inline
    bool Read( ITokenStream& its, lsts_index_t& state,
               lsts_index_t& inter_end );
    inline
    void Write( OTokenStream& ots, lsts_index_t number );

    inline
    void EnableOutputIntervals( bool en ) { out_inter_disabled = !en; }

    static inline
    void WriteSubrange( OTokenStream& ots,
                        lsts_index_t subrange_start,
                        lsts_index_t subrange_end );
    inline
    void WriteLast( OTokenStream& ots );
    
 private:
    
    bool AddNumber( lsts_index_t new_number,
                    lsts_index_t& start_of_interval,
                    lsts_index_t& end_of_interval )
    {
        if ( !i_start )
        {
            i_start = new_number;
            i_end = new_number;
        }
        else if ( new_number == i_end + 1 )
        {
            ++i_end;
        }
        else if ( new_number < i_start || i_end < new_number )
        {
            start_of_interval = i_start;
            if ( i_end != i_start )
            {
                end_of_interval = i_end;
            }
            else
            {
                end_of_interval = 0;
            }
            i_start = new_number;
            i_end = new_number;
            return false;
        }

        start_of_interval = 0;
        end_of_interval = 0;
        return true;
    }

    bool CheckOut( lsts_index_t& start_of_interval,
                   lsts_index_t& end_of_interval )
    { return !AddNumber( 0, start_of_interval, end_of_interval ); }

    static void WriteInProperWay( OTokenStream& ots,
                                  lsts_index_t st_i,
                                  lsts_index_t end_i )
    { 
        if ( end_i )
        {
            if ( st_i + 1 == end_i )
            {
                ots.PutNumber( st_i );
                ots.PutNumber( end_i );
            }
            else
            {
                WriteSubrange( ots, st_i, end_i );
            }
            
        }
        else
        {
            ots.PutNumber( st_i );
        }
    }


    lsts_index_t i_start;
    lsts_index_t i_end;
    bool out_inter_disabled;
};

inline
bool Interval::Read( ITokenStream& its, lsts_index_t& state,
                     lsts_index_t& inter_end )
{
    if ( its.GetNumber( state ) )
    {
        if ( its.Peek().isPunctuation( GiveSubrangePunctuator() ) )
        {
                its.Get();
                
                check_claim( its.GetNumber( inter_end ),
                             its.errorMsg( its.Peek(),
                                           "the end of the interval "
                                           "is missing" ) );
        }
        else { inter_end = 0; }
        
        return true;
    }
    
    check_claim( !its.Peek().isPunctuation( GiveSubrangePunctuator() ),
                 its.errorMsg( its.Peek(),
                               "the beginning of the interval "
                               "is missing" ) );
    return false;
}

inline
void Interval::Write( OTokenStream& ots, lsts_index_t number )
{
    if ( out_inter_disabled )
    {
        ots.PutNumber( number );
        return;
    }

    lsts_index_t st_i;
    lsts_index_t end_i;
    
    if ( AddNumber( number, st_i, end_i ) )
    {
        return;
    }
    
    WriteInProperWay( ots, st_i, end_i );
}

inline
void Interval::WriteSubrange( OTokenStream& ots,
                              lsts_index_t subrange_start,
                              lsts_index_t subrange_end )
{
    ots.PutNumber( subrange_start );
    ots.spaceOff();
    ots.PutPunctuation( GiveSubrangePunctuator() );
    ots.PutNumber( subrange_end );
    ots.spaceOn();
}

inline
void Interval::WriteLast( OTokenStream& ots )
{
    lsts_index_t st_i;
    lsts_index_t end_i;
    
    if ( CheckOut( st_i, end_i ) )
    {
        WriteInProperWay( ots, st_i, end_i );
    }
    
}


#undef CIRC_INTERVAL_HH_
#endif
