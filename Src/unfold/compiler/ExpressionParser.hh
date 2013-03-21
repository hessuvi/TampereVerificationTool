#ifndef NEW_EXPRESSION_PARSER_HH
#define NEW_EXPRESSION_PARSER_HH

#include "oBC.hh"


class ExpressionParser
{
 public:
    static type_index_t ParsePrecondition( class fdState& );
    static type_index_t ParsePostcondition( class fdState& );
    static type_index_t ParseOutput( class fdState& );
    static type_index_t Parse()
    {
        unsigned first_i = oBC::ReserveLink();
        type_index_t res_type = compCondition();
        oBC::putByte( 'Q' );
        oBC::SetLink( first_i, res_type );
        return res_type;
    }

 private:
    static type_index_t compCondition();
    static type_index_t compPrint();
    static type_index_t compAssignment();
    static type_index_t compLogicEq();
    static type_index_t compOr();
    static type_index_t compAnd();
    static type_index_t compEq();
    static type_index_t compNot();
    static type_index_t compComp();
    static type_index_t compWrap();
    static type_index_t compIncDec();
    static type_index_t compAri();
    static type_index_t compTerm();
    static type_index_t compFactor();
};


#endif
