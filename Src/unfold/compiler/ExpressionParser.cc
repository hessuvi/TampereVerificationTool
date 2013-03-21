#include "ExpressionParser.hh"

#include "FoldedLsts.hh"
#include "sp_codes.hh"
#include "ps.hh"
#include "uf_types.hh"

#include "syntax/itx.hh"


namespace
{
    type_index_t TypeCheck( type_index_t left, const std::string& op,
                            type_index_t right, const char* oper = "operands" )
    {
        if ( uf_types::areTypesCompatible( left, right ) )
        {
            if ( left >= right ) { return left; }
            return right;
        }
        std::string msg( "incompatible " ); msg += oper;
        it.error( uf_types::GenOpTypeString( left, op, right ), msg );
        return 0;
    }

    void LogicCheck( type_index_t left, const std::string& op,
                     type_index_t right )
    {
        if ( left == logic_exp && right == logic_exp ) { return; }
        it.error( uf_types::GenOpTypeString( left, op, right ),
                  "both operands must be logical expressions" );
    }

    bool is_precond = false;
    bool is_output = false;
    fdState* state = 0;
}


type_index_t
ExpressionParser::ParsePrecondition( fdState& st )
{
    state = &st;
    is_precond = true; type_index_t type_i = Parse(); is_precond = false;
    return type_i;
}

type_index_t
ExpressionParser::ParsePostcondition( fdState& st )
{ state = &st; return Parse(); }

type_index_t
ExpressionParser::ParseOutput( fdState& st )
{
    state = &st;
    is_output = true;

    unsigned first_i = oBC::ReserveLink();
    type_index_t res_type = compPrint();
    oBC::putByte( 'Q' );
    oBC::SetLink( first_i, res_type );

    is_output = false;
    return res_type;
}

// <exp> ( ; <exp> )*
type_index_t
ExpressionParser::compCondition()
{
    type_index_t type = compPrint(); std::vector<unsigned> links;
    while ( it.opt_get( ff::punct=";" ) )
    {
        // Extra semi-colons are just ignored:
        while ( it.opt_get( ff::punct=";" ) );
        if ( it.opt_get( ff::punct="]" ) ) { break; }
        if ( type != logic_exp ) { oBC::putByte( 'k' ); }
        else { oBC::putByte( '&' ); links.push_back( oBC::ReserveLink() ); }
        type = compPrint();
    }

    if ( type != logic_exp )
    { oBC::putByte( 'k' ); oBC::PutBoolLiteral( true ); }
    for ( unsigned i = 0; i < links.size(); ++i ) { oBC::SetLink( links[i] ); }
    return type;
}

// pr is print.
type_index_t
ExpressionParser::compPrint()
{
    if ( it.opt_get( ff::rword( "prc" ) ) )
    {
        type_index_t printed_type = compAssignment();
        oBC::putByte( 'C' );
        return printed_type;
    }

    bool pr_found = it.opt_get( ff::rword( "pr" ) );
    std::string str;
    if ( pr_found ) { it.opt_get( str ); }

    // Reading the operand:
    type_index_t ext = compAssignment();
     
    if ( pr_found )
    {
        oBC::putByte( 'P' );
        oBC::PutStringLiteral( str );
        it >> ff::opt_str;
        str = *ff::opt_str;
        oBC::PutStringLiteral( str );
    }

    return ext;
}

// assign_to is assignment.
type_index_t
ExpressionParser::compAssignment()
{
    type_index_t left_type = compLogicEq();

    while ( it.opt_get( ff::rword( "assign_to" ) ) )
    {
        std::string op_name( "assign_to" );
        std::string var;
        if ( !itx::opt_getName( var ) )
        { it.error( "a variable name expected" ); }
        unsigned i = ps::checkName( var, ps::var_names );
        it >> ff::opt_punct( "'" );
        type_index_t right_type = ps::GiveVarTypeI( i );
        TypeCheck( left_type, op_name, right_type );
        oBC::putByte( ':' ); oBC::putByte( i );
    }
    it >> ff::default_msg;

    return left_type;
}

// <=> is equivalence.
type_index_t
ExpressionParser::compLogicEq() { return compOr(); }

type_index_t
ExpressionParser::compOr()
{
    type_index_t left_type = compAnd();
    std::vector<unsigned> links;

    while ( true )
    {
        std::string op_name;
        if ( it.opt_get( ff::rword( "or" ) ) ) { op_name = *ff::rword; }
        else if ( it.opt_get( ff::punct( "\\/" ) ) ) { op_name = *ff::punct; }
        else break;
        oBC::putByte( '|' );
        links.push_back( oBC::ReserveLink() );
        type_index_t right_type = compAnd();
        LogicCheck( left_type, op_name, right_type );
    }

    for ( unsigned i = 0; i < links.size(); ++i ) { oBC::SetLink( links[i] ); }
    return left_type;
}

type_index_t
ExpressionParser::compAnd()
{
    type_index_t left_type = compNot();
    std::vector<unsigned> links;

    while ( true )
    {
        std::string op_name;
        if ( it.opt_get( ff::rword( "and" ) ) ) { op_name = *ff::rword; }
        else if ( it.opt_get( ff::punct( "/\\" ) ) ) { op_name = *ff::punct; }
        else break;
        oBC::putByte( '&' );
        links.push_back( oBC::ReserveLink() );
        type_index_t right_type = compNot();
        LogicCheck( left_type, op_name, right_type );
    }

    for ( unsigned i = 0; i < links.size(); ++i ) { oBC::SetLink( links[i] ); }
    return left_type;
}

type_index_t
ExpressionParser::compNot()
{
    bool not_found = it.opt_get( ff::rword( "not" ) );
    type_index_t ext = compComp();
    if ( not_found )
    {
        oBC::putByte( '!' );
        if ( ext != logic_exp )
        { it.error( uf_types::GenOpTypeString( *ff::rword, ext ),
                    "the operand must be a logical expression" ); }
    }
    return ext;
}

type_index_t
ExpressionParser::compComp()
{
    type_index_t left_type = compWrap();
    type_index_t result_type = left_type;
    bool first = true; std::vector<unsigned> links;

    while ( it >> ff::opt_punct( "= != > >= < <=" ) ||
            it >> ff::opt_rword( "neq" ) )
    {
        std::string op_name;
        if ( ff::opt_rword ) { op_name = *ff::opt_rword; }
        else { op_name = *ff::opt_punct; }

        if ( !first )
        {
            oBC::putByte( '&' );
            links.push_back( oBC::ReserveLink() );
            oBC::putByte( 'm' );
        } first = false;

        type_index_t right_type = compWrap();
        TypeCheck( left_type, op_name, right_type );

        if ( op_name.size() == 1 ) { oBC::putByte( op_name[0] ); }
        else switch ( op_name[0] )
        {
          case 'n': case '!': oBC::putByte( 'D' ); break;
          case '<': oBC::putByte( '[' ); break;
          case '>': oBC::putByte( ']' ); break;
          default:
              it.error( op_name,
                        "a bug caught in ExpressionParser::compComp()" );
              break;
        };

        result_type = logic_exp;
    }

    for ( unsigned i = 0; i < links.size(); ++i ) { oBC::SetLink( links[i] ); }
    return result_type;
}

type_index_t
ExpressionParser::compWrap()
{
    if ( it >> ff::opt_rword( "wrap" ) )
    {
        type_index_t type_i = compIncDec();
        if ( type_i <= int_exp )
        { it.error ( uf_types::GenOpTypeString( "wrap", type_i ),
                     "operand must be a bounded, non-logical expression" ); }
        oBC::putByte( 'w' ); oBC::putByte( type_i );
        return type_i;
    }

    return compIncDec();
}

type_index_t
ExpressionParser::compIncDec()
{
    if ( it >> ff::opt_rword( "inc inc_no_wrap dec dec_no_wrap" ) )
    {
        ff::rword_type comm( ff::opt_rword );
        std::cerr << "COMM=" << *comm << std::endl;
        type_index_t type_i = compAri();
        if ( type_i <= int_exp )
        { it.error( uf_types::GenOpTypeString( *comm, type_i ),
                    "operand must be a bounded, non-logical expression" ); }
        if ( comm == "inc" ) { oBC::putByte( 'y' ); oBC::putByte( type_i ); }
        else if ( comm == "dec" )
        {
            std::cerr << "DEC FOUND" << std::endl;
 oBC::putByte( 'a' ); oBC::putByte( type_i ); }
        else if ( comm == "inc_no_wrap" ) { oBC::putByte( 'Y' ); }
        else if ( comm == "dec_no_wrap" ) { oBC::putByte( 'A' ); }
        return type_i;
    }

    return compAri();
}

type_index_t
ExpressionParser::compAri()
{
    type_index_t res_type = compTerm();

    while ( it >> ff::opt_punct( "+ -" ) )
    {
        std::string op_name( *ff::opt_punct ); char op = op_name[0];
        type_index_t right_type = compTerm();
        res_type = TypeCheck( res_type, op_name, right_type, "terms" );
        oBC::putByte( op );
    }

    return res_type;
}

type_index_t
ExpressionParser::compTerm()
{
    type_index_t res_type = compFactor();

    while ( it >> ff::opt_punct( "* / %" ) )
    {
        std::string op_name( *ff::opt_punct ); char op = op_name[0];
        type_index_t right_type = compFactor();
        res_type = TypeCheck( res_type, op_name, right_type, "factors" );
        oBC::putByte( op );
    }

    return res_type;
}

type_index_t
ExpressionParser::compFactor()
{
    // A literal integer?
    if ( it >> ff::opt_integ )
    { oBC::PutIntLiteral( *ff::opt_integ ); return int_exp; }

    type_index_t type_i = 99999; int sign = 1;
    if ( it >> ff::opt_punct( "-" ) ) { sign = -1; }

    // A variable or a constant?
    std::string name;
    if ( itx::opt_getName( name ) )
    {
        ps::NameStore* ns = ps::findName( name );
        if ( !ns ) { it.error( name, "undefined identifier" ); }
        unsigned i = ns->last_i();

        // A variable?
        if ( ns == &ps::var_names )
        {
            // An old value of the variable?
            if ( !it.opt_get( ff::punct( "'" ) ) )
            {
                if ( state->isDeletedVar( i ) )
                { it.error( name, "cannot use deleted variable" ); }
                if ( !state->varExists( i ) )
                { it.error( name, "cannot use uncreated variable" ); }
                oBC::MarkOldIntVar( i );
            }
            else // A new value of the variable.
            {
                if ( is_precond )
                { it.error( name + "'",
                            "cannot refer to future value of variable in "
                            "precondition" ); }
                if ( is_output )
                { it.error( name + "'",
                            "cannot refer to future value of variable in "
                            "output parameter expression" ); }
                state->CreateVar( i );
                oBC::MarkNewIntVar( i );
                // Mark the new variable used:
                oBC::MarkVarUsed( i );
            }

            type_i = ps::GiveVarTypeI( i );
        }
        // A constant?
        else if ( ns == &ps::constants )
        { oBC::PutIntLiteral( uf_types::GiveValueOfConst( i ) );
          type_i = uf_types::GiveTypeOfConst( i ); }
        else { ps::addNewName( name, ps::constants ); }

    }
    else if ( it >> ff::opt_punct( "(" ) )
    { type_i = compPrint(); it >> ff::punct( ")" ); }
    // True or false?
    else if ( it >> ff::opt_rword )
    {
        type_i = logic_exp;
        if ( ff::opt_rword == "false" ) { oBC::PutBoolLiteral( false ); }
        else if ( ff::opt_rword == "true" ) { oBC::PutBoolLiteral( true ); }
        else { it.error( *ff::opt_rword, "misplaced reserved word" ); }
    }
    else { it.error( "bad expression" ); }

    if ( sign == -1 )
    {
        if ( !uf_types::isNumberType( type_i ) )
        {
            it.error( uf_types::GenOpTypeString( "-", type_i ),
                      "invalid type of operand" );
        }
        oBC::putByte( '~' );
    }
    
    return type_i;
}
