#ifndef FD_STATE_HH
#define FD_STATE_HH

#include "folded/fdTransition.hh"

#include <string>
#include <vector>


// fdState, a class for storing folded states.
struct fdState
{
    enum state_of_var { deleted = -1, exists_not, exists, used };

    fdState();
    void AddStateOfVar( state_of_var st ) { vars.push_back( st ); }

    void AddTr( const fdTransition& tr ) { trs.push_back( tr ); }

    void Reset() const { suggested_vars = vars; }

    void CreateVar( unsigned var_i ) const { suggested_vars[var_i] = exists; }
    void UseVar( unsigned var_i ) const { suggested_vars[var_i] = used; }
    void DeleteVar( unsigned var_i ) const { suggested_vars[var_i] = deleted; }

    bool varExists( unsigned var_i ) const { return vars[var_i] >= exists; }
    bool isUsedVar( unsigned var_i ) const { return vars[var_i] == used; }
    bool isDeletedVar( unsigned var_i ) const { return vars[var_i] == deleted; }

    void CheckVars( const std::string& end_st_name );

    std::vector<fdTransition> trs;
    std::vector<state_of_var> vars;

 private:
    static std::vector<state_of_var> suggested_vars;
    bool vars_set;

};


#endif
