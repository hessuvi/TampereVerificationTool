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

#ifndef SCFILEFORMAT_HH
#define SCFILEFORMAT_HH

#include "FileFormat/FileFormat.hh"
#include "FileFormat/Forwarder.hh"

#include <string>
#include <vector>


class scFileFormat : public FileFormat
{
 public:
    scFileFormat( const std::string& begin_file, const std::string& end_file,
                  bool checks_on = true );
    virtual ~scFileFormat();

    void SetChecks( bool checks_on ) { checks = checks_on; }
    // Decides whether reasonability checks are done or not.
    bool areChecksOn() const { return checks; }
    // Returns true if checks are enabled.

    template<class iSection, class iAP>
    iSection& AddInputSection( iAP& iap )
    {
        iSection* isection = new iSection( iap );
        sections.push_back( isection );
        FileFormat::AddReader( *isection );
        return *isection;
    }
    
    template<class Checker, class iAP>
    Checker& AddInputChecker( iAP& iap )
    {
        Checker* checker = new Checker( iap, *this );
        checker->SetForInput();
        checkers.push_back( checker );
        return *checker;
    }
    
    template<class oSection, class oAP>
    oSection& AddOutputSection( oAP& oap )
    {
        oSection* osection = new oSection( oap );
	//osection->SetWriter( *osection );
        sections.push_back( osection );
        FileFormat::AddWriter( *osection );
        return *osection;
    }
    
    template<class Checker, class oSection>
    Checker& AddOutputChecker( oSection& osec )
    {
        Checker* checker = new Checker( osec, *this );
        checker->SetForOutput();
        checkers.push_back( checker );
        return *checker;
    }

    template<class iAP>
    typename iAP::section_t& AddInputAP( iAP& iap )
    { return AddInputAP3<iAP, typename iAP::section_t,
	typename iAP::checker_t>( iap ); }

    template<class iAP, class iSection>
    iSection& AddInputAP2( iAP& iap )
    { return AddInputAP3<iAP, iSection, typename iAP::checker_t>( iap ); }

    template<class iAP, class iSection, class Checker>
    iSection& AddInputAP3( iAP& iap )
    {
        // Check if reader is a forwarder:
        Forwarder* fw = dynamic_cast<Forwarder*>( &iap );
        if ( fw ) { AddForwarder( iSection( iap ).GiveName(), fw ); }
        if ( areChecksOn() )
        { return AddInputSection<iSection, iAP>
              ( AddInputChecker<Checker, iAP>( iap ) ); }
        
        return AddInputSection<iSection, iAP>( iap );
    }

    template<class iSection>
    void SkipInputSection() { FileFormat::SkipSection( iSection::NAME() ); }

    template<class oAP>
    oSection& AddOutputAP( const oAP& oap )
    { return AddOutputAP3<oAP, typename oAP::section_t,
          typename oAP::checker_t>( const_cast<oAP&>( oap ) ); }

    template<class oAP, class oSection>
    oSection& AddOutputAP2( oAP& oap )
    { return AddOutputAP3<oAP, oSection, typename oAP::checker_t>( oap ); }
    
    template<class oAP, class oSection, class Checker>
    oSection& AddOutputAP3( oAP& oap )
    {
        oSection& osection = AddOutputSection<oSection, oAP>( oap );

        Forwarder* fw = dynamic_cast<Forwarder*>( &oap );
        if ( !areChecksOn() || fw && fw->isChecked() ) { return osection; }
        // Contents of a checked Forwarder need not to be rechecked.

        //Checker& checker =
        AddOutputChecker<Checker, oSection>( osection );
        //osection.SetWriter( checker );
        return osection;
    }

    void Clear();

 private:
    std::vector<class Section*> sections;
    std::vector<class Checker*> checkers;
    bool checks;
};


#endif
