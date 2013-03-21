#ifndef UF_TYPES_HH
#define UF_TYPES_HH

#include "compiler/uf_config.hh"

#include "itstream.hh"
#include "FileFormat/Lexical/OTokenStream.hh"

#include <string>
#include <vector>

class uf_types
{
 public:
    static bool isNumberType( type_index_t type_i )
    { return types[type_i]->is_number; }

    static unsigned GiveNumberOfValues( unsigned type_i )
    { return types[type_i]->number_of_values; }

    static unsigned GiveValueOfConst( unsigned const_i )
    { return const_i2const_val[const_i]; }

    static unsigned GiveTypeOfConst( unsigned const_i )
    { return const_i2type_i[const_i]; }

    static int WrapValue( unsigned type_i, int val )
    {
        Type& typ = *types[type_i];
        int low = typ.lower_bound;
        unsigned siz = typ.number_of_values;

        if ( val >= low ) { val = ( val - low ) % siz; }
        else { val = siz - 1 + ( low - val - 1 ) % siz; }
        return val + low;
    }

    struct Type
    {
        Type( int lb, int ub, bool is_num ) :
            lower_bound( lb ), upper_bound( ub ), number_of_values( 0 ),
            is_number( is_num ) { }
        virtual ~Type() { }
        virtual Type* CreateCopy() = 0;
        virtual std::string GiveName() = 0;
        virtual void ValueToString( int val, std::string& str ) = 0;

        virtual void CheckValue( const std::string& varName, int val ) = 0;

        const int lower_bound;
        int upper_bound;
        unsigned number_of_values;
        const bool is_number;
    };

    // All types created are pointed at here:
    static std::vector<Type*> types;

    static void AddType( Type* typ, std::string alias = "" );

    static bool areTypesCompatible( type_index_t first, type_index_t second )
    { return first == second ||
          types[first]->is_number && types[second]->is_number; }

    static std::string GenOpTypeString( std::string op, type_index_t e_type )
    { return op + " <" + types[e_type]->GiveName() + " expression>"; }

    static std::string GenOpTypeString( type_index_t left,
                                        const std::string& op,
                                        type_index_t right )
    {
        return std::string( "<" ) + types[left]->GiveName() +
            " expression> " + op + " <" + types[right]->GiveName() +
            " expression>";
    }

    struct Bool : public Type
    {
        Bool() : Type( 0, 1, false ) { number_of_values = 2; }
        virtual ~Bool() { }
        virtual Bool* CreateCopy() { return new Bool;}
        virtual std::string GiveName() { return "bool"; }
        virtual void ValueToString( int val, std::string& str )
        { if ( val ) { str = "true"; } else { str = "false"; } }
        virtual void CheckValue( const std::string&, int ) { }
    };

    struct Int : public Type
    {
        Int() : Type( 0, 0, true ) { }
        virtual ~Int() { }
        virtual Int* CreateCopy() { return new Int;}
        virtual std::string GiveName() { return "int"; }
        virtual void ValueToString( int val, std::string& str )
        { oTokenStream::number2string( val, str ); }
            
        virtual void CheckValue( const std::string&, int ) { }
    };

    struct BoundedInt : public Type
    {
        BoundedInt( int lBound, int uBound ) : Type( lBound, uBound, true )
        { number_of_values = 1 + uBound - lBound; }
        virtual ~BoundedInt() { }
        virtual BoundedInt* CreateCopy()
        { return new BoundedInt( lower_bound, upper_bound );}
        virtual std::string GiveName()
        {
            std::string lb_str, ub_str;
            oTokenStream::number2string( lower_bound, lb_str );
            oTokenStream::number2string( upper_bound, ub_str );
            return lb_str + ".." + ub_str;
        }
        virtual void ValueToString( int val, std::string& str )
        { oTokenStream::number2string( val, str ); }
            
        virtual void CheckValue( const std::string& varName, int val )
        {
            if ( lower_bound >= val && val <= upper_bound ) { return; }
            std::cerr << varName << ", variable out of bounds" << std::endl;
        }
        
    };

    struct Enum : public Type
    {
        Enum() : Type( 0, -1, false ) { }
        virtual ~Enum() { }
        virtual Enum* CreateCopy() { return new Enum;}
        virtual std::string GiveName()
        {
            std::string name( "[" ); name += constants[0];
            for ( unsigned i = 1; i < constants.size(); ++i )
            { name += " "; name += constants[i]; }
            name += "]";
            return name;
        }

        virtual void ValueToString( int val, std::string& str )
        { str = constants[ val ]; }

        virtual void CheckValue( const std::string& varName, int val )
        {
            if ( 0 >= val && val < static_cast<int>( constants.size() ) )
            { return; }
            std::cerr << varName << ", variable out of bounds" << std::endl;
        }
 
        void AddConstant( const std::string& con );
        
        std::vector<std::string> constants;
    };

 private:
    static std::vector<unsigned> const_i2type_i;
    static std::vector<unsigned> const_i2const_val;
};


#endif
