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

#include "TvtCLP.hh"
#include "LogWrite.hh"

#include "LSTS_File/ActionNamesStore.hh"
#include "LSTS_File/StateNamesStore.hh"

#include "LSTS_File/DivBitsStore.hh"
#include "RO_AccSets.hh"

#include "StateProps.hh"
#include "TransitionsContainer.hh"

#include "LSTS_File/iLSTS_File.hh"

#include "LSTS_File/LSTS_Sections/specialStateProps.hh"
#include "FileFormat/Lexical/charconv.hh"

#include <iostream>
#include <vector>
#include <cstdlib>

using std::string;


class SimulateCLP: public TvtCLP
{

 public:
    
    SimulateCLP(): TvtCLP(description)
    {
        /*        setOptionHandler( "-tvt", &SimulateCLP::setTVT, true,
                          "Writes the output in TVT file format (default)." );
        */
    }

 private:
    static const char* const description;
};

const char* const SimulateCLP::description=
LogWrite::neatString( "   tvt.simulate is an LSTS simulator with a simple "
                      "ascii user interface. Please, type \"help\" while "
                      "running the simulator to get the list of the commands "
                      "available."
                      ).c_str();


int simulate( SimulateCLP& clp );


int
main( int argc, char* argv[] )
{
#ifndef NO_EXCEPTIONS
  try {
#endif
    // Putting up the command line parser:

    SimulateCLP clp;

    if( !clp.parseCommandLine( argc, argv ) )
    {
        return 1;
    }

    return simulate( clp );
#ifndef NO_EXCEPTIONS
  }
  catch( int exp ) { return exp; }
  catch( ... ) { return 1; }
#endif
}

struct LSTS
{
    LSTS() :
        action_names( true ), transitions( true ), exitValue( EXIT_SUCCESS )
    { }

    void init( const Header& hd )
    {
        statesVisited.resize( hd.GiveStateCnt() + 1 );
         if ( !acc_sets.isInitialized() )
         { acc_sets.setMaxVal( hd.GiveActionCnt() ); }
        moveTo( hd.GiveInitialState(), "" );
    }

    ActionNamesStore action_names;
    StateNamesStore state_names;

    RO_AccSetsContainer acc_sets;
    DivBitsStore div_bits;
    
    StatePropsContainer state_props;
    TransitionsContainer transitions;
    
    lsts_index_t cState;
    unsigned cTrs;

    int exitValue;

    iTokenStream its;
    oTokenStream ots;

    void moveTo( lsts_index_t state, lsts_index_t action )
    {
        if ( !action ) { moveTo( state, "" ); }
        else { moveTo( state, action_names.GiveActionName( action ) ); }
    }

    void moveTo( lsts_index_t state, const string& action )
    {
        cState = state;
        cTrs = transitions.numberOfTransitions( cState );
        steps.push_back( cState );
        actionSequence.push_back( action );
        if ( !statesVisited[cState] ) { statesVisited[cState] = 1; }
        statesVisited[cState] += 1;
    }

    bool moveBack()
    {
        if ( steps.size() == 1 ) { return false; }
        statesVisited[cState] -= 1;
        steps.pop_back();
        actionSequence.pop_back();
        cState = steps.back();
        cTrs = transitions.numberOfTransitions( cState );
        return true;
    }

    void path( bool infoOn = false )
    {
        if ( infoOn ) { PrintCurrentState( steps[0], 1 ); }

        ots.spaceOff();
        ots.PutNumber( steps[0] );
        for ( unsigned i = 1; i < steps.size(); ++i )
        {
            ots.PutWord( "-" );
            if ( actionSequence[i].empty() )
            { ots.PutIdentifier( "tau" ); }
            else
            { ots.PutString( actionSequence[i] ); }
            ots.PutWord( "->" );
            ots.PutNumber( steps[i] );

            if ( infoOn )
            {
                cout << endl;
                PrintCurrentState( steps[i], i+1 );
                ots.spaceOff();
            }
        }

        ots.spaceOn();
        if ( steps.size() == 1 || !infoOn ) { ots.Endl(); }
    }

    bool visited() const { return statesVisited[cState]; }
    bool visited( lsts_index_t  state ) const { return statesVisited[state]; }
    lsts_index_t timesVisited() const { return timesVisited( cState ); }
    lsts_index_t timesVisited( lsts_index_t state ) const
    {
        if ( !statesVisited[state] ) { return 0; }
        return statesVisited[state] - 1;
    }

    vector<unsigned> statesVisited;
    vector<lsts_index_t> steps;
    vector<string> actionSequence;

    void
    PrintAppearingMsg( unsigned times )
    {
        cout << "appears ";
        if ( times == 1 ) { cout << "once"; }
        else { cout << times << " times"; }
        cout << " in the path";
    }

    void
    PrintCurrentState( lsts_index_t state = 0, unsigned step = 0 )
    {
        if ( !state ) { state = cState; step = steps.size(); }

        cout << "State " << state << ", ";
        PrintAppearingMsg( timesVisited( state ) );
        cout << ".       [step " << step << "]" << endl;

        for ( unsigned i = 0; i < cTrs; ++i )
        {
            const TransitionsContainer::Transition tr =
                transitions.getTransition( state, i );
            const string& an =
                action_names.GiveActionName( tr.transitionNumber );
            
            cout << " " << i + 1 << ") ";
            if ( an.size() )
            { cout << "\"" << an << "\""; }
            else
            { cout << "tau"; }
            
            cout << " -> " << tr.destStateNumber;
            
            if ( visited( tr.destStateNumber ) )
            {
                if ( timesVisited( tr.destStateNumber ) )
                {
                    cout << " (the target state ";
                    PrintAppearingMsg( timesVisited( tr.destStateNumber ) );
                    cout << ")";
                }
                else { cout << " (visited)"; }
            }
            
            cout << endl;
        }
        
        if ( !cTrs ) { cout << " [deadlock]" << endl; }
        
        // ##### PROPOSITIONS #>>>#
        const StatePropsContainer::StatePropsPtr spp =
            state_props.getStateProps( state );
        const unsigned max = state_props.getMaxStatePropNameNumber();
        
        for ( unsigned i = 1; i <= max; ++i )
        {
            if ( spp.isPropSet( i ) )
            {
                const string& name = state_props.getStatePropName( i );
                const SP::StatePropType ptype = SP::GiveStatePropType( name );
                
                if ( ptype )
                {
                    cout << " + proposition " << SP::SpecialPropIds[ptype] <<
                        " is on.";
                }
                else
                {
                    cout << " + proposition \"" <<
                        state_props.getStatePropName( i ) << "\" is on.";
                }
                cout << endl;
                
            }
        }
        // #<<<# PROPOSITIONS #####
        
        // ##### ACCEPTANCE SETS #>>>#
        const RO_AccSets as = acc_sets.getRO_AccSets( state );
        
        RO_AccSets::AccSetPtr asp = as.firstAccSet();
        if ( asp != as.endAccSet() )
        {
            ots.PutWord( "- acceptance sets: " );
            ots.spaceOff();
            bool notFirst = false;
            do
            {
                if ( notFirst ) { ots.PutWord( "," ); }
                notFirst = true;
                ots.PutWord( "{" );
                RO_AccSets::action_const_iterator it = as.begin( asp );
                const RO_AccSets::action_const_iterator end = as.end( asp );
                while ( it != end )
                {
                    ots.PutString( action_names.GiveActionName( *it ) );
                    ++it;
                }
                ots.PutWord( "}" );
                
                ++asp;
            }
            while ( asp != as.endAccSet() );
        
            ots.Endl();
            ots.spaceOn();
        }
        // #<<<# ACCEPTANCE SETS #####

        if ( div_bits.divBitOn( state ) )
        { cout << " * has divergence." << endl; }
    }
    
};

lsts_index_t CheckAction( LSTS& lsts, const string& str )
{
    for ( unsigned i = 0; i < lsts.cTrs; ++i )
    {
        const TransitionsContainer::Transition tr =
            lsts.transitions.getTransition( lsts.cState, i );
        const string& an =
            lsts.action_names.GiveActionName( tr.transitionNumber );
        if ( areStringsEqual( str, an ) )
        { return tr.destStateNumber; }
    }

    return 0;
}

lsts_index_t CheckTau( LSTS& lsts )
{
    for ( unsigned i = 0; i < lsts.cTrs; ++i )
    {
        const TransitionsContainer::Transition tr =
            lsts.transitions.getTransition( lsts.cState, i );
        if ( !tr.transitionNumber ) { return tr.destStateNumber; }
    }

    return 0;
}

bool AskAction( LSTS& lsts )
{
    lsts_index_t option;

    const TT::Token& tok = lsts.its.Get();

    if ( tok.isIdentifier( "tau" ) )
    {
        unsigned destState = CheckTau( lsts );
        if ( destState )
        {
            lsts.moveTo( destState, "" );
            lsts.PrintCurrentState();
            cout << "> ";
            return true;
        }

        cout << "No tau action available in the state " << lsts.cState << "."
             << endl;

        lsts.exitValue = EXIT_FAILURE;
        return false;
    }

    if ( tok.isString() || tok.isIdentifier( "tau" ) )
    {
        unsigned destState = CheckAction( lsts, tok.stringData() );
        if ( destState )
        {
            lsts.moveTo( destState, tok.stringData() );
            lsts.PrintCurrentState();
            cout << "> ";
            return true;
        }

        cout << "No action \"" << tok.stringData()
             << "\" available in the state " << lsts.cState << "."
             << endl;
        lsts.exitValue = EXIT_FAILURE;
        return false;
    }

    if ( tok.isEndOfFile() || tok.isIdentifier( "q" ) ||
         tok.isIdentifier( "quit" ) )
    {
        return false;
    }
    else if ( tok.isIdentifier( "b" ) || tok.isIdentifier( "back" ) )
    {
        if ( lsts.moveBack() ) { lsts.PrintCurrentState(); }
        else { cout << "The current state is the initial state." << endl; }
    }
    else if ( tok.isIdentifier( "v" ) || tok.isIdentifier( "view" ) )
    { lsts.PrintCurrentState(); }
    else if ( tok.isIdentifier( "p" ) || tok.isIdentifier( "path" ) )
    { lsts.path(); }
    else if ( tok.isIdentifier( "pi" ) || tok.isIdentifier( "pathinfo" ) )
    { lsts.path( true ); }
    else if ( tok.isNumber() )
    {
        option = tok.numberData();

        if ( !option || option > lsts.cTrs )
        {
            cout << "No such action available in the current state."
                 << endl;
        }
        else
        {
            const TransitionsContainer::Transition tr =
                lsts.transitions.getTransition( lsts.cState, option - 1 );
            lsts.moveTo( tr.destStateNumber, tr.transitionNumber );
            lsts.PrintCurrentState();
        }
    }
    else if ( tok.isIdentifier( "h" ) || tok.isIdentifier( "help" ) )
    {
        cout << "The commands available in tvt.simulate:" << endl
             << " h, help   - this text." << endl
             << " q, quit   - exits the simulator." << endl
             << " v, view   - views the current state." << endl
             << " 1,2,...,n - chooses the transition the option number "
            "represents." << endl
             << " b, back   - moves one step back in the current path." << endl
             << " p, path   - shows the path from the initial state to the "
            "current state." << endl
             << " pi, pathinfo - shows the path from the initial state\n"
            "                to the current state with full information."
             << endl
             << " \"<action name>\" - chooses the a transition labelled with "
             << "<action name>." << endl
             << " tau       - chooses a transition labelled with tau." << endl;
    }
    else { cout << "Unknown command." << endl; }

    cout << "> ";
    return true;
}

int
simulate( SimulateCLP& clp )
{
    iLSTS_File reader( clp.getInputStream(), true, true );

    InStream is;
    LSTS lsts;

    reader.AddActionNamesReader( lsts.action_names );
    reader.AddTransitionsReader( lsts.transitions );
    reader.AddAccSetsReader( lsts.acc_sets );
    reader.AddDivBitsReader( lsts.div_bits );
    reader.AddStatePropsReader( lsts.state_props );
    reader.AddStateNamesReader( lsts.state_names );

    reader.ReadFile();
    const Header& hd = reader.GiveHeader();
    lsts.init( hd );

    cout << ">>> Welcome to tvt.simulate." << endl
         << "Please type \"help\" to get the list of the commands available."
         << endl << endl;

    cout << "The LSTS loaded from \"" << clp.getInputFilename( 0 ) <<
        "\" has: " << endl;

    cout << " * " << hd.GiveStateCnt() << " states" << endl <<
        " * " << hd.GiveTransitionCnt() << " arcs" << endl <<
        " * " << hd.GiveActionCnt() << " action names" <<
        endl << endl;

    OutStream os;
    os.OpenStdout();
    lsts.ots.Open( os );
    lsts.ots.UseLowerCaseIdentifiers();

    lsts.PrintCurrentState();

    cout << "> ";

    is.OpenStdin();
    lsts.its.Open( is );

    while ( AskAction( lsts ) );

    if ( lsts.exitValue == EXIT_FAILURE )
    {
        cout << "Exiting tvt.simulate due to failure." << endl;
    }
    else
    {
        cout << "Exiting tvt.simulate." << endl;
    }

    return lsts.exitValue;
}
