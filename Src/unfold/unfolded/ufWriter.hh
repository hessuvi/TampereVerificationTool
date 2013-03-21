#ifndef UF_WRITER
#define UF_WRITER

#include "LSTS_File/oLSTS_File.hh"
#include "TransitionsContainer.hh"

#include "ufSProps.hh"


class ufWriter
{
 public:
    ufWriter( oLSTS_File& oFile, const std::string& trs_file, bool force );
        
    void AddInitialState( unsigned state_n )
    { o_lsts_file.GiveHeader().AddInitialState( state_n ); }

    void start_st_transitions( unsigned state_n )
    { if ( state_n ) { tr_cont.startAddingTransitionsToState( state_n ); } }

    void add_transition( unsigned dest_state_n, unsigned action_n )
    { tr_cont.addTransitionToState( action_n, dest_state_n ); }

    void end_st_transitions( unsigned state_n )
    { if ( state_n ) { tr_cont.doneAddingTransitionsToState(); } }

    void SetStateCnt( unsigned state_cnt )
    { o_lsts_file.GiveHeader().SetStateCnt( state_cnt ); }

    oLSTS_File& GiveoLsts() { return o_lsts_file; }

 private:
    oLSTS_File& o_lsts_file;
    TransitionsContainer tr_cont;
    ufSProps state_props;
    bool first_tr;
};


#endif


