#include "TvtCLP.hh"
#include "LSTS_File/oLSTS_File.hh"

#include "unfolded/ufWriter.hh"

#include "UnfoldedLsts.hh"

#include "compiler/ps.hh"
#include "compiler/Compiler.hh"


struct UnfoldClp : public TvtCLP
{
    UnfoldClp() : TvtCLP( "  Unfolds an LSTS with variables." )
    {
        setOptionHandler( "-write-trs-asap=", &UnfoldClp::trsAsapHandler,
                          false,
                          "Specifies an outputfile for transitions section.",
                          "<filename>", false );
    }
    
    bool trsAsapHandler( const std::string& param )
    {
        par_str = param.substr( 17 );
        if( par_str.empty() || par_str == "-" )
        {
            std::cerr << "Invalid filename for --write-trs-asap=" << std::endl;
            return false;
        }
        return true;
    }

    std::string par_str;
};

int
main( int argc, char* argv[] )
{
    try
    {
        UnfoldClp options;
        
        if ( !options.parseCommandLine( argc, argv ) ) { return 1; }
        
        // For output:
        OutStream& os = options.getOutputStream();
        oLSTS_File oFile;
        ufWriter writer( oFile, options.par_str, options.getForce() );

        // Compiling the TVT input file:
        Compiler comp;
        FoldedLsts folded_lsts( writer );

        InStream& is = options.getInputStream();
        ff::file_source fs( is.GiveInStream(), is.GiveFilename() );
        comp.CompileFile( fs, folded_lsts );

        // Unfolding and writing the result:
        UnfoldedLsts ufd_lsts( folded_lsts );
        ufd_lsts.Unfold( writer );
        oFile.WriteFile( os ); 
    }
    catch( std::exception& ex )
    { std::cerr << ex.what() << std::endl; return EXIT_FAILURE; }
    catch( ... )
    { std::cerr << "unknown exception" << std::endl; return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}
