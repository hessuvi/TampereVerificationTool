#include "icstream.hh"

std::string ff::input_error::rep_str;

ff::input_error::input_error( const std::string& filename, Cursor& cursor,
                              const char* msg )
{
    std::string& rep_str = input_error::rep_str;
    if ( filename.size() ) { rep_str = filename; }
    else { rep_str = "<no filename>"; }
    rep_str += ": ";
    
    //    if ( cursor )
    //{
        rep_str += "L"; rep_str << cursor.line; rep_str += ", C";
        rep_str << cursor.column; rep_str += ": ";
        //}

    rep_str += msg;
}
