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

// $Id: Forwarder.hh 1.2 Tue, 21 Jan 2003 01:45:54 +0200 timoe $

// $Log:$

#ifndef FORWARDER_HH
#define FORWARDER_HH

#include <string>


class Forwarder
{
 public:
    virtual ~Forwarder();
    // Forwarder( const Forwarder& );
    // Forwarder& operator=( const Forwarder& );

    virtual void ShowHeader( const class Header& );

    void SetSectionFilename( const std::string& secFilename );
    const std::string& GiveSectionFilename() const;
    
    void SetReadAlways( bool b = true ) { read_always = b; }
    bool isReadAlways() const { return read_always; }

    void SetChecked( bool b = true ) { checked = b; }
    bool isChecked() const { return checked; }

 protected:
    Forwarder( bool readAlways = false );

 private:
    std::string section_filename;
    bool read_always;
    bool checked;
};


#endif
