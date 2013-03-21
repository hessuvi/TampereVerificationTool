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

// Defines the TokenStream class, which is inherited by either
// ITokenStream or OTokenStream classes. It methods to generate
// formatted error messages.

#ifndef TOKENSTREAM_HH
#define TOKENSTREAM_HH

#include "constants.hh"
#include "InOutStream.hh"
#include "Token.hh"

#include <string>


class TokenStream
{
 public: 
    // Methods for formatted error messages:
    
    std::string errorMsg( const TT::Token& error_token, std::string message );
    // Returns an error message with format:
    //   "<partA> `<error_token>', <message>"

    std::string errorMsg( std::string error_data, std::string message );
    // Format:
    //   "<partA> `<error_data>', <message>"

    std::string errorMsg( lsts_index_t error_data, std::string message );
    // Format:
    //   "<partA> `<error_data>', <message>"

    std::string errorMsg( std::string message );
    // Format:
    //  "<partA> <message>"    

    std::string errorMsg( std::string first, lsts_index_t value,
                          std::string last );
    // Format:
    //  "<partA> <first> <value> <last>"

    std::string errorMsg( lsts_index_t value, std::string message,
                          lsts_index_t rangeFirst, lsts_index_t rangeLast );
    // Format:
    //  "<partA> `<value>', <message> should be in range " +
    //  "[<rangeFirst>,<rangeLast>]"


    void SetSectionName( const std::string& name ) { sectionName = name; }
    // Setting the name of the section handled at the moment (the name
    // will be shown in possible error messages).
    const std::string& GiveSectionName() const { return sectionName; }

    // Other services:

    const std::string& GiveFilename() const { return filename; }
    // Quering the filename connected to this stream.
    void SetFilename( const std::string& name ) { filename = name; }

 protected:
    virtual ~TokenStream();
    
    virtual std::string errorMsgPartA() const = 0;
    // This pure virtual method returns the first part of the
    // formatted error message.

 private:
    std::string filename;
    std::string sectionName;
};


#endif
