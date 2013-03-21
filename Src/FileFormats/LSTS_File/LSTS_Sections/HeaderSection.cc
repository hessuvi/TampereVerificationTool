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

#include "HeaderSection.hh"
#include "LSTS_File/Header.hh"
#include "FileFormat/Lexical/ITokenStream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"
#include "FileFormat/Lexical/charconv.hh"

#include "otfvi.hh"
#include "error_handling.hh"

using std::string;
using std::vector;


const char INTERRUPTED[] = "INTERRUPTED";

const char STATE_CNT[] = "STATE_CNT";
const char ACTION_CNT[] = "ACTION_CNT";
const char ELUSIVE_ACTION_CNT[] = "ELUSIVE_ACTION_CNT";
const char TRANSITION_CNT[] = "TRANSITION_CNT";
const char STATE_PROP_CNT[] = "STATE_PROP_CNT";

const char INITIAL_STATE[] = "INITIAL_STATE";
const char INITIAL_STATES[] = "INITIAL_STATES";

const char INITIALLY_UNSTABLE[] = "INITIALLY_UNSTABLE";
const char INITIALLY_STABLE[] = "INITIALLY_STABLE";

const char SEMANTICS[] = "SEMANTICS";

const char NO_LONG_TAU_LOOPS[] = "NO_LONG_TAU_LOOPS";
const char SHALLOW_TAUS[] = "SHALLOW_TAUS";
const char NO_TAUS[] = "NO_TAUS";
const char DETERMINISTIC[] = "DETERMINISTIC";
const char NORMAL[] = "NORMAL";

const char TRUE[] = "TRUE";
const char FALSE[] = "FALSE";

const char IS[] = "=";


// header_attributes[] connects the names of attributes with attribute parser
//   methods:
const iHeaderSection::Attribute iHeaderSection::header_attributes[] =
{ { INTERRUPTED, &iHeaderSection::ReadInterrupted },

  { STATE_CNT, &iHeaderSection::ReadStateCnt },
  { ACTION_CNT, &iHeaderSection::ReadActionCnt },
  { ELUSIVE_ACTION_CNT, &iHeaderSection::ReadElusiveActionCnt },

  { TRANSITION_CNT, &iHeaderSection::ReadTransitionCnt },
  { STATE_PROP_CNT, &iHeaderSection::ReadStatePropCnt },

  { INITIAL_STATE, &iHeaderSection::ReadInitialState },
  { INITIAL_STATES, &iHeaderSection::ReadInitialState },

  { INITIALLY_UNSTABLE, &iHeaderSection::ReadInitiallyUnstable },
  { INITIALLY_STABLE, &iHeaderSection::ReadInitiallyStable },

  { SEMANTICS, &iHeaderSection::ReadSemantics },

  // Promises:
  { NO_LONG_TAU_LOOPS, &iHeaderSection::ReadNoLongTauLoops },
  { SHALLOW_TAUS, &iHeaderSection::ReadShallowTaus },
  { NO_TAUS, &iHeaderSection::ReadNoTaus },
  { DETERMINISTIC, &iHeaderSection::ReadDeterministic },
  { NORMAL, &iHeaderSection::ReadNormal },

  { 0, 0 } };


// PRIVATE:




// *** The reading methods ***


// Header parser identifies all the attributes in the header section and
//   calls a proper attribute parser methods (private methods defined in the
//   end of this file).
//===========================================================================
// This method is called at the start of reading this section.
//===========================================================================
//
void
iHeaderSection::ReadSection( Header& )
{
    ITokenStream& token_source = its();

    token_source.SetPunctuation( string( IS ) + "#;#:#," );

    unsigned int i;
    
    // While reads attributes until it sees the end section token:
    while ( token_source.Peek().isIdentifier() &&
            token_source.Peek().stringData() != "END" )
    {
        
        // Attribute name expected first:
	const TT::Token& read_token =
            token_source.Get( "attribute name expected",
                              TT::IDENTIFIER );
	
	// We compare the identifier with our list of legal names of
	// attributes:
	for ( i = 0; header_attributes[i].name; i++ ) {
	    
	    // Does the name match?
	    if ( read_token.stringData() == header_attributes[i].name ) {
		
		// Here we call the function that parses the attribute:
		(this->*header_attributes[i].parser)();
		break;
	    }

        }
	
	// If the name of the attribute was not legal, we're going to CRASH:
	check_claim( header_attributes[i].name,
		     token_source.errorMsg( read_token,
                                            "unidentified attribute name" ) );
	
    } // End of while.    

}


// PRIVATE:



// ### Private methods used in reading the section ###



// This is for the use of the five first Set -members (see below).
//===========================================================================
//
lsts_index_t
iHeaderSection::ReadAttributeNumber( const string& /*attribute_name*/ )
{
    its().Get( TT::PUNCTUATION, IS );
    return its().Get( TT::NUMBER ).numberData();
} 

bool
iHeaderSection::readBoolean()
{
    const TT::Token& token = its().Peek();

    if ( token.isPunctuation() && token.stringData() == IS )
    {
        its().Get();
        const string& status = its().Get( TT::IDENTIFIER ).stringData();
        if ( status == TRUE )
        {
            return true;
        }
        return false;
    }

    return true;
}


void
iHeaderSection::ReadInterrupted()
{
    iTokenStream& it = its();

    it.Get( TT::PUNCTUATOR, ":" );

    if ( it.Peek().isString() )
    {
        header.SetError( it.Get().stringData() );
        return;
    }

    it.Get( TT::IDENTIFIER, "on_the_fly_error" );
    it.Get( TT::PUNCTUATOR, ":" );

    string type = it.Get( TT::IDENTIFIER ).stringData();
    ToLowerCase( type );

    unsigned otf_error = 0;
    do
    {
        if ( type == OTFVI::KEY_WORDS[ otf_error ] )
        { break; }
        ++otf_error;
    }
    while ( otf_error < OTFVI::AMOUNT );
    check_claim( otf_error < OTFVI::AMOUNT,
                 it.errorMsg( type, "an unknown on the fly error" ) );
    ++otf_error;

    it.Get( TT::IDENTIFIER, "in_state" );
    lsts_index_t state = it.Get( TT::NUMBER ).numberData();

    header.SetOnTheFlyError( static_cast<Header::OTF_ERROR>( otf_error ),
                             state );
    
    if ( it.skip( TT::PUNCTUATOR, "," ) )
    {
        it.Get( TT::IDENTIFIER, "guard" );
        header.SetGuard( it.Get( TT::NAT_NUMBER ).numberData() );
    }

}

// These 5 Read methods read in the values of the named attributes of LSTS:
//===========================================================================
//
void
iHeaderSection::ReadStateCnt()
{
    header.SetStateCnt( ReadAttributeNumber( STATE_CNT ) );

    // check_claim( header.GiveStatCount(),
    //             its().errorMsg( "state_cnt is zero --- can't have "
    //                                    "an LSTS without states" ) );
}
void
iHeaderSection::ReadTransitionCnt()
{ header.SetTransitionCnt( ReadAttributeNumber( TRANSITION_CNT ) ); }
void
iHeaderSection::ReadActionCnt()
{ header.SetActionCnt( ReadAttributeNumber( ACTION_CNT ) ); }
void
iHeaderSection::ReadElusiveActionCnt()
{ header.SetElusiveActionCnt( ReadAttributeNumber( ELUSIVE_ACTION_CNT ) ); }
void
iHeaderSection::ReadStatePropCnt()
{ header.SetStatePropCnt( ReadAttributeNumber( STATE_PROP_CNT ) ); }
void
iHeaderSection::ReadInitialState()
{
    header.SetInitialState( ReadAttributeNumber( INITIAL_STATE ) );

    while ( its().Peek().isNumber() )
    {
        header.AddInitialState( its().Get().numberData() );
    }
    
    if ( its().Peek().isPunctuation( ";" ) )
    {
        its().Get();
    }

}


// ReadSemantics() reads a semantics string and gives it to header.
//===========================================================================
//
void
iHeaderSection::ReadSemantics()
{
    its().Get( TT::PUNCTUATION, IS );
    const string sem = its().Get( TT::IDENTIFIER ).stringData();
    header.AddSemantics( sem );
}


// These Read methods set the following attributes of LSTS:
//===========================================================================
//

// Boolean:

void iHeaderSection::ReadInitiallyStable()
{ header.SetInitiallyUnstable( !readBoolean() ); }
void iHeaderSection::ReadInitiallyUnstable()
{ header.SetInitiallyUnstable( readBoolean() ); }

// Promises:

#define ReadPromise( m_name, promise ) \
 void iHeaderSection::m_name() { header.AddPromise( promise ); }

ReadPromise( ReadNoLongTauLoops, NO_LONG_TAU_LOOPS )
ReadPromise( ReadShallowTaus, SHALLOW_TAUS )
ReadPromise( ReadNoTaus, NO_TAUS )
ReadPromise( ReadDeterministic, DETERMINISTIC )
ReadPromise( ReadNormal, NORMAL )






// *** The writing methods ***


bool
oHeaderSection::doWeWriteSectionWithInit( Header& /*header*/ )
{
    return true;
}

// This method is called at the start of writing this section.
//===========================================================================
//
void
oHeaderSection::WriteSection( Header& /*header*/ )
{
    WriteBeginSection();
    
    WriteHeader( ots() );

    WriteEndSection();
}

static void
WritePromise( const string& attr, OTokenStream& o )
{
    o.PutIdentifier( attr );
    o.Endl(); 
}

// After the user has given header attributes, we'll write them to an
//   input token stream with this method.
//===========================================================================
//
void
oHeaderSection::WriteHeader( OTokenStream& ot )
{

    if ( header.isInterrupted() )
    {
        ot.PutIdentifier( INTERRUPTED );
        ot.PutPunctuator( ":" );

        if ( !header.isOnTheFlyError() )
        { ot.PutString( header.GiveError() ); }
        else
        {
            ot.PutIdentifier( "on_the_fly_error" );
            ot.PutPunctuator( ":" );
            lsts_index_t state = 0;
            unsigned type = header.GiveOnTheFlyError( state ) - 1;
            ot.PutIdentifier( OTFVI::KEY_WORDS[ type ] );
            ot.PutIdentifier( "in_state" );
            ot.PutNumber( state );

            if ( header.GiveGuard() )
            {
                ot.PutPunctuator( "," );
                ot.PutIdentifier( "guard" );
                ot.PutNatNumber( header.GiveGuard() );
            }

        }

        ot.Endl();
    }

    if ( header.isStateCntGiven() )
    {
        WriteAssignment( STATE_CNT, header.GiveStateCnt() );
    }
    
    if ( header.isActionCntGiven() )
    {
        WriteAssignment( ACTION_CNT, header.GiveActionCnt() );
    }

    if ( header.GiveElusiveActionCnt() )
    {
	WriteAssignment( ELUSIVE_ACTION_CNT, header.GiveElusiveActionCnt() );
    }

    if ( header.isTransitionCntGiven() )
    {
        WriteAssignment( TRANSITION_CNT, header.GiveTransitionCnt() );
    }
    
    if ( header.hasStatePropCnt() )
    {
        WriteAssignment( STATE_PROP_CNT, header.GiveStatePropCnt() );
    }

    header.WriteInitialStates( ot );

    // Boolean:
    if ( header.isInitiallyUnstableGiven() )
    {
        WriteBoolean( INITIALLY_UNSTABLE, header.isInitiallyUnstable() );
    }


    if ( header.hasSemantics() )
    {
        vector<string> sems;
        sems = header.GiveSemantics();
        
        for ( unsigned i = 0; i < sems.size(); i++ )
        {
            ot.PutIdentifier( SEMANTICS );
            ot.PutPunctuation( IS );
            ot.PutIdentifier( sems[i] );
            ot.Endl();
        }
    }

    
    // Promises:

    if ( header.hasPromise( NO_LONG_TAU_LOOPS ) )
    {
        WritePromise( NO_LONG_TAU_LOOPS, ot );
    }
    
    if ( header.hasPromise( SHALLOW_TAUS ) )
    {
        WritePromise( SHALLOW_TAUS, ot );
    }
    
    if ( header.hasPromise( NO_TAUS  ) )
    {
        WritePromise( NO_TAUS, ot );
    }
    
    if ( header.hasPromise( DETERMINISTIC ) )
    {
        WritePromise( DETERMINISTIC, ot );
    }
    
    if ( header.hasPromise( NORMAL ) )
    {
        WritePromise( NORMAL, ot );
    }
    
}




// For writing a line in format:
// "<attribute_name> = <number>"
//===========================================================================
//
void
oHeaderSection::WriteAssignment( const string& attr, lsts_index_t num )
{
    ots().PutIdentifier( attr );
    ots().PutPunctuation( IS );
    ots().PutNumber( num );
    ots().Endl(); 
}

// For writing a line in format:
// "<boolean_attribute_name>"
//===========================================================================
//
void
oHeaderSection::WriteBoolean( const string& attr, bool status )
{
    ots().PutIdentifier( attr );
    ots().PutPunctuator( IS );

    if ( status ) { ots().PutIdentifier( TRUE ); }
    else { ots().PutIdentifier( FALSE ); }
    ots().Endl(); 
}
