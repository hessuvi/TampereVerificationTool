#ifndef PS_HH
#define PS_HH

#include "uf_types.hh"

//#include "uf_config.hh"
//#include "com_codes.hh"

#include "StringTree/StringMap.hh"

#include <string>
#include <vector>


class ps // == Process' Store, holds different data of the process.
{
 public:
    // Methods:
    static inline unsigned AddStringLiteral( const std::string& str )
    { string_literals.push_back( str ); return string_literals.size() - 1; }
    static inline const std::string& GiveStringLiteral( unsigned i )
    { return string_literals[i]; }

    class NameStore
    {
     public:
        NameStore( const char* name );
        const std::string& GiveName() const { return store_name; }
        unsigned Misplaced( const std::string& name );
        bool find( const std::string& name )
        { return name_map.find( name, i ); }
        bool add( const std::string& name, bool alias_of_prev = false );
        unsigned last_i() { return i; }
        unsigned numberOfUniques() const { return number_of_uniques; }

        // Map from names to variable numbers 0,1,...,name_map.size():
        StringMap<unsigned> name_map;

     private:
        const std::string store_name;
        unsigned i;
        unsigned number_of_uniques;
    };

    static NameStore type_names;
    static NameStore var_names;
    static NameStore constants;
    static NameStore state_names;
    static NameStore gate_names;
    static NameStore state_prop_names;

    static unsigned checkName( const std::string& name, NameStore& );
    static unsigned addNewName( const std::string& name, NameStore&,
                                bool alias_of_prev = false );
    // Returns: true if adding was done, false if the name existed.
    static bool addNameIfNeeded( const std::string& name, NameStore& );
    static NameStore* findName( const std::string& name );

    static void AddVar( const std::string& name, type_index_t type_i )
    { var_strings.push_back( name ); var_i2type_i.push_back( type_i ); }
    static const std::string& GiveVarName( unsigned var_i ) 
    { return var_strings[var_i]; }

    static uf_types::Type& GiveVarType( unsigned var_i )
    { return *uf_types::types[ var_i2type_i[var_i] ]; }

    static unsigned GiveVarTypeI( unsigned var_i )
    { return var_i2type_i[var_i]; }

 private:
    // Var number i -> index (not necessarily unique one) of
    // var_types vector:
    static std::vector<unsigned> var_i2type_i;

    static std::vector<std::string> string_literals;
    static std::vector<std::string> var_strings;
};


#endif
