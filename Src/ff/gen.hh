namespace ff
{
    inline unsigned is_str_in_list( const std::string& str,
                                    const std::string& List )
    {
        unsigned found_n = 0;
        const char* list_p = List.c_str();
        while ( *list_p )
        {
            if ( *list_p == ' ' ) { ++list_p; continue; }
            const char* b = list_p; unsigned len = 1;
            while ( *++list_p && *list_p != ' ' ) { ++len; }
            ++found_n;
            if ( !str.compare( 0, str.size(), b, len ) ) { return found_n; }
        }
        return 0;
    }

    inline void convert_to_lower( std::string& str )
    { for ( unsigned i = 0; i < str.size(); ++i )
      { str[i] = tolower( str[i] ); } }

    namespace { std::string tmptmpstr; }
    inline unsigned is_tolower_str_in_list( const std::string& str,
                                            const std::string& List )
    { tmptmpstr = str; convert_to_lower( tmptmpstr );
      return is_str_in_list( tmptmpstr, List ); }
}
