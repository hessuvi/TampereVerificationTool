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

// Checker gives services for checking action names.

#ifndef CHECKER_HH
#define CHECKER_HH

#include "FileFormat/FileFormat.hh"
#include "Exceptions.hh"

// Predeclarations:
class TokenStream;


class Checker
{
 public:
    Checker( FileFormat& ff ) :
        file_format( ff ), for_input( true ) { }
    virtual ~Checker();
    
    void SetForInput() { for_input = true; }
    void SetForOutput() { for_input = false; }

    TokenStream& ts()
    {
        if ( for_input ) { return file_format.its(); }
        return file_format.ots();
    }

    /*std::string errorMsg( lsts_index_t value,
                          const char message[],
                          lsts_index_t rangeFirst,
                          lsts_index_t rangeLast )
                          { ts().errorMsg( value, message, rangeFirst, rangeLast ); }*/

    void check_isGiven( const char attrName[], bool isAttrGiven )
    { check_claim( isAttrGiven,
                   ts().errorMsg( std::string( attrName ) +
                                  " is not given in the header - "
                                  "can't proceed with this section" ) ); }

    void check_state( lsts_index_t state, lsts_index_t stateCnt,
                      const char message[] )
    { check_claim( state && state <= stateCnt,
                   ts().errorMsg( state, message, 1, stateCnt ) ); }

    static void check_state( lsts_index_t state, lsts_index_t stateCnt,
                             const char message[], TokenStream* ts )
    { check_claim( state && state <= stateCnt,
                   ts->errorMsg( state, message, 1, stateCnt ) ); }

    void check_action( lsts_index_t action, lsts_index_t actionCnt,
                      const char message[]  )
    { check_claim( action <= actionCnt,
                   ts().errorMsg( action, message, 0, actionCnt ) ); }

    static void check_action( lsts_index_t action, lsts_index_t actionCnt,
                              const char message[], TokenStream* ts )
    { check_claim( action <= actionCnt,
                   ts->errorMsg( action, message, 0, actionCnt ) ); }

    void check_visible_action( lsts_index_t action, lsts_index_t actionCnt,
                               const char message[] )
    { check_claim( action && action <= actionCnt,
                   ts().errorMsg( action, message, 1, actionCnt ) ); }

    void check_amount( lsts_index_t amount, lsts_index_t promise,
                       const char itemName[], const char attrName[] )
    { check_claim( amount == promise,
                   ts().errorMsg(
           Exs::catString( std::string( "the amount of " ) + itemName + " (",
                         amount, ") " ) +
           Exs::catString( std::string( "doesn't match with " ) +
                         attrName + " (",
                         promise, ")" ) ) ); }

    void check_interval( const std::string& /*elemName*/,
                         lsts_index_t elemCnt,
                         lsts_index_t interv_start,
                         lsts_index_t interv_end )
    {
        check_state( interv_start, elemCnt,
                     "starting number of the interval", &ts() );
        
        check_state( interv_end, elemCnt,
                     "ending number of the interval", &ts() );
        
        check_claim( interv_end >= interv_start,
                     ts().errorMsg( interv_end,
                                    "the end of the interval can't be "
                                    "smaller than the beginning" ) );
    }

 private:
    FileFormat& file_format;
    bool for_input;
};


#endif
