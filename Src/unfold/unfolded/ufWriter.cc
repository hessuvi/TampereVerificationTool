#include "ufWriter.hh"

#include "ufSProps.hh"
#include "compiler/ps.hh"


ufWriter::ufWriter( oLSTS_File& oFile, const std::string& trs_file,
                    bool force ) : o_lsts_file( oFile ), tr_cont( false )
{
    oFile.AddTransitionsWriter( tr_cont );
    oFile.AddStatePropsWriter( state_props );

    if ( trs_file.size() ) { tr_cont.directFileWrite( trs_file, force); }
}
