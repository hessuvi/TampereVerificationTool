#ifndef IT_X_HH
#define IT_X_HH

namespace itx
{
    inline bool
    opt_getName( std::string& name )
    {
        // Reading a name:
        if ( it >> ff::opt_id ) { name = *ff::opt_id; return true; }
        if ( it >> ff::opt_str ) { name = *ff::opt_str; return true; }
        name.clear(); return false;
    }

    inline bool
    opt_getGate( std::string& name )
    { if ( it >> ff::opt_rword( "tau" ) ) { name = "tau"; return true; }
      return opt_getName( name ); }
    










    /*
    
    
    struct gate_type : public str_data_token
    { static const char* name() { return "gate"; } };
    gate_type gate;
    
    class itx : public itstream
    {
        itx( file_source& src ) : itstream( src ) { }

        unsigned opt_get( gate_type& g )
        {
            std::string& sc = const_cast<std::string&>( *g );
            if ( itstream::opt_get( ff::id ) ) { sc = *s_id; return 1; }
            if ( itstream::opt_get( sc ) ) { return 1; }
            if ( itstream::opt_get( ff::rword="tau" ) )
            { sc = *s_rword; return 1; }
            sc.clear(); return 0;
        }
    };
    */
}

#endif
