#include "Compiler.hh"

#include "FoldedLsts.hh"
#include "Alphabet.hh"
#include "VarTable.hh"
#include "ps.hh"
#include "oBC.hh"
#include "ExpressionParser.hh"

#include "folded/fdState.hh"

#include "syntax/ufTypeDefs.hh"
#include "syntax/ufStateProps.hh"
#include "syntax/Variables.hh"
#include "syntax/AlphabetDef.hh"
#include "syntax/itx.hh"

#include <string>
#include <vector>


// Global input token stream 'it':
ff::itstream it;


// Internal:
namespace { FoldedLsts* fl = 0; unsigned number_of_vars = 0; }


static void
ParseEventParameters( fdState& st, fdTransition& tr )
{
    for ( unsigned par_i = 0; true; ++par_i )
    {
        // Output expression?
        if ( it.opt_get( ff::punct( "!" ) ) )
        {
            type_index_t e_type = ExpressionParser::ParseOutput( st );
            byte_t* bc = oBC::GiveCopy(); oBC::Reset();
            tr.AddOutput( par_i, e_type, bc );
            continue;
        }
        
        // Input expression?
        if ( it.opt_get( ff::punct( "?" ) ) )
        {
            std::string name;
            if ( !itx::opt_getName( name ) )
            { it.error( "a variable name expected" ); }
            ff::opt_punct( "'" );
            unsigned var_i = ps::checkName( name, ps::var_names );
            tr.AddInput( par_i, var_i ); st.CreateVar( var_i );
            continue;
        }

        break;
    }
    
}

static void
ParsePostcondition( fdState& st, fdTransition& tr, bool real_postc )
{
    std::cerr << "postcond!#" << std::endl;

    tr.is_postcond_real = real_postc;
    if ( tr.is_postcond_real )
    {
        oBC::ReserveUsedVarTable( number_of_vars );
        ExpressionParser::ParsePostcondition( st );
        it >> ff::opt_punct( "]" );
    }

    if ( tr.inputs.size() )
    {
        oBC::ReserveUsedVarTable( number_of_vars );
        for ( unsigned i = 0; i < tr.inputs.size(); ++i )
        { oBC::MarkVarUsed( tr.inputs[i].var_i ); }
    }
        
    if ( !oBC::GiveByteCodeSize() ) { return; }
    tr.bc_postcond = oBC::GiveCopy();
    oBC::Reset();
}

struct VarData
{
    VarData( const std::string& var_name ) : name( var_name ) { }
    std::string name;
    unsigned var_i;
};

static void
ParseEndState( fdState& st, fdTransition& tr )
{
    it >> ff::opt_punct( "->" );

    std::vector<VarData> var_data;

    if ( it >> ff::opt_rword( "delete" ) )
    {
        std::string name;
        while ( itx::opt_getName( name ) )
        {
            var_data.push_back( VarData( name ) );
            VarData& data = var_data.back();
            data.var_i = ps::checkName( data.name, ps::var_names );
        }
    }

    it >> ff::opt_punct( "->" );

    std::string end_st_name;
    if ( !itx::opt_getName( end_st_name ) )
    { it.error( "end state name expected" ); }

    std::cerr << "END_STATE=" << end_st_name << std::endl;

    ps::addNameIfNeeded( end_st_name, ps::state_names );
    tr.dest_state_n = ps::state_names.last_i();
    st.AddTr( tr );

    fdState& end_st = fl->GetState( tr.dest_state_n );

    for ( unsigned i = 0; i < var_data.size(); ++i )
    {
        unsigned var_i = var_data[i].var_i;
        if ( end_st.isUsedVar( var_i ) )
        { it.error( end_st_name, std::string( "cannot delete variable " ) +
                    var_data[i].name +
                    " here since it is used in the end state" ); }
        if ( end_st.varExists( var_i ) )
        { it.error( end_st_name, std::string( "cannot delete variable " ) +
                    var_data[i].name +
                    " here since it already exists in the end state" ); }
        end_st.DeleteVar( var_i );
    }
    
    end_st.CheckVars( end_st_name );
}
    
static void
ParseInitClauses( fdState& st )
{
    std::cerr << "init 0" << std::endl;
    while ( it >> ff::opt_punct( "[ ->" ) )
    {
        std::cerr << "init 1" << std::endl;
        fdTransition tr;
        st.Reset();
        it >> ff::opt_punct( "->" );
        ParsePostcondition( st, tr, true );
        ParseEndState( st, tr );
    }
}

static void
ParseAction( const std::string& gate, fdState& st, fdTransition& tr )
{
    ParseEventParameters( st, tr );

    std::string a_subset_name( gate );
    for ( unsigned i = 0; i < tr.par_types.size(); ++i )
    { a_subset_name += "<"; a_subset_name += tr.par_types[i]->GiveName();
      a_subset_name += ">"; }        
    
    //    std::cerr << "a_subset_name == " << a_subset_name << std::endl;
    
    const Alphabet& alpha = fl->GiveAlphabet();
    tr.action_subset = alpha.GiveActionSubset( a_subset_name );
    if ( !tr.action_subset )
    { it.error( a_subset_name, "undefined action subset" ); }
    it >> ff::opt_punct( "->" );
}

static void
ParseTransitions( fdState& st )
{
    std::string gate;
    while ( itx::opt_getGate( gate ) || it >> ff::opt_punct( "[" ) )
    {
        fdTransition tr; // tr.line_n = tin.GiveLineNumber(); !!!!!!!!!
        st.Reset();

        // Precondition?
        if ( gate.empty() )
        {
            std::cerr << "PREcond!!!!" << std::endl;
                    
            ExpressionParser::ParsePrecondition( st );
            tr.bc_precond = oBC::GiveCopy(); oBC::Reset();
            it >> ff::punct( "]" );
            std::cerr << "PREcond ENDS!!!!" << std::endl;
        }

        // Skipping some optional puncuators:
        it >> ff::opt_punct( "- -- ->" );
        if ( gate.empty() && !itx::opt_getGate( gate ) )
        { it.error( "gate name expected" ); }

        ps::checkName( gate, ps::gate_names );

        ParseAction( gate, st, tr );
        ParsePostcondition( st, tr, it.opt_get( ff::punct="[" ) );
        ParseEndState( st, tr );
    }
}

static void
ParseStates()
{
    bool init_state = false;
    while ( ( init_state = it.opt_get( ff::rword="init" ) ) ||
            it.opt_get( ff::punct = "*" ) )
    {
        if ( init_state ) { ps::checkName( "init", ps::state_names ); }
        else
        {
            std::string name;
            if ( !itx::opt_getName( name ) )
            { it.error( "state name expected" ); }
            ps::checkName( name, ps::state_names );
            std::cerr << "STATE=" << name << std::endl;
        }

        fdState& st = fl->GetState( ps::state_names.last_i() );
        it >> ff::opt_punct( ":" );

        if ( init_state ) { ParseInitClauses( st ); }
        else { ParseTransitions( st ); }
    }

}


// PUBLIC:

Compiler::Compiler()
{
    const char* puncts[] =
    { "=", "!=", "+", "-", "*", "/", "(", ")", ">", "<", ">=", "<=",
      "\\/", "/\\", "<=>", "{", "}", "[", "]", ",", ":", "..", ";", "'",
      "--", "->", "!", "?", ":=" };
    it.set_puncts( puncts, sizeof( puncts ) );
    const char* rwords[] =
    { "process", "is", "typedef", "type_defs", "state_props", "variables",
      "alphabet", "endproc", "end_proc",

      "boolean", "bool", "true", "false", "int",

      "or", "and", "not", "neq",
      
      "pr", "prc", "assign_to", "wrap", "inc", "dec", "inc_no_wrap",
      "dec_no_wrap",

      "init", "tau", "delete" };
    it.set_rwords( rwords, sizeof( rwords ) );
    it.set_rword_case_sens( false );

    oBC::Reset();

    // Adding default Boolean and integer types:
    uf_types::AddType( (new uf_types::Bool), "boolean" );
    uf_types::AddType( (new uf_types::Int) );
}

Compiler::~Compiler()
{
    // Destructing created type objects:
    for ( unsigned i = 0; i < uf_types::types.size(); ++i )
    { delete uf_types::types[i]; uf_types::types[i] = 0; }
}

namespace bl
{
    const char* blocks[] = { "type_defs", "state_props", "variables",
                             "alphabet" };
    const unsigned bl_size = sizeof( blocks ) / sizeof( char* );
    //std::vector<std::string> recs( bl_size );
    std::vector<bool> bbs( bl_size, false );

    void CompileBlocks( ff::file_source&, fdState& st )
    {
        while ( true )
        {
            // Optional header tokens:
            if ( it >> ff::opt_rword( "process hei" ) )
            { std::string tmp; itx::opt_getName( tmp ); }
            it >> ff::opt_rword( "is" );

            if ( !( it >> ff::opt_rword( "type_defs state_props variables "
                                         "alphabet typedef" ) ) ) { return; }
            unsigned found_i = ff::opt_rword; --found_i;
            found_i %= 4; // Changes `typedef' to `type_defs' :)
            //std::string& rec = recs[found_i];
            if ( bbs[found_i] )
            { it.error( *ff::opt_rword, "the same block appears twice" ); }
            //fs.record( rec ); ja //fs.stop_recording();
            bbs[found_i] = true;
            
            switch ( found_i )
            {
              case 0: ufTypeDefs(); break;
              case 1: if ( bbs[2] ) { it.error( *ff::opt_rword,
                              "this block must appear before variables" ); }
                      ufStateProps::comp( st ); break;
              case 2: ( Variables( st ) ); break;
              case 3: AlphabetDef(); break;
              default: break; // We never come here.
            }
        }
        
    }

    void PrintBlocks()
    {
        /*        for ( unsigned i = 0; i < bl_size; ++i )
        {
            if ( recs[i].size() )
            {
                std::cerr << blocks[i] << "::::\n" << recs[i] << std::endl;
            }
            }*/
    }

}

void
Compiler::CompileFile( ff::file_source& fs, FoldedLsts& folded_lsts )
{
    it.open( fs );
    fl = &folded_lsts;

    // Adding the init state:
    ps::addNewName( "init", ps::state_names );
    fdState& st = fl->GetState( ps::state_names.last_i() );
    ps::addNewName( "tau", ps::gate_names );

    bl::CompileBlocks( fs, st );
    //bl::PrintBlocks();

    number_of_vars = ps::var_names.numberOfUniques();

    // Parsing the graph part:
    VarTable::Init();
    ParseStates();

    std::cerr << "5#" << std::endl;

    // End of input:
    it >> ff::opt_rword( "endproc" ) >> ff::opt_rword( "end_proc" );

    std::cerr << "6#" << std::endl;

    it >> ff::error_msg( "syntax error" ) >> ff::eoi >> ff::default_msg;

    std::cerr << "#" << std::endl;
}



/*

1 Tilojen lkm

2 Tila & teksti & koordinaatit yksikerrallaan , samaten transitiot (data + tekstimuodossa, alkuehto, aktionnimi, loppuehto kaikki pätkittynä)


Muuta: mihin tilojen koordinaatit laitetaan

erillinen sektio

Kirjoituspuoli?





 */
