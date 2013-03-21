#ifndef SP_CODES_HH
#define SP_CODES_HH

#include "config.hh"


namespace sp_codes
{
    // Literals:
    //    static const byte_t string_lit_next = 's'; // 115;
    static const byte_t int_lit_next = 'i'; // 105;
    static const byte_t float_lit_next = 'f'; //

    // Variables:
    static const byte_t old_int_var_next = 'O';
    static const byte_t new_int_var_next = 'I';

    // Logical expression stack symbols:
    static const byte_t logical_or = '|'; // 124
    static const byte_t logical_and = '&'; // 38
    static const byte_t logical_eq = 'E'; // 69
    static const byte_t logical_not = '!'; // 33

    // Arithmetic expression stack symbols:
    static const byte_t production = '*'; // 42
    static const byte_t addition = '+'; // 43
    static const byte_t substraction = '-'; // 45
    static const byte_t division = '/'; // 47
    static const byte_t minus_sign = '~'; // 126

    // Arithmetic comparison operators:
    static const byte_t lesser = '<'; // 60
    static const byte_t arith_eq = '='; // 61
    static const byte_t greater = '>'; // 62

    static const byte_t arith_neq = 'N'; // 78

    static const byte_t lesser_or_eq = '['; // 91
    static const byte_t greater_or_eq = ']'; // 93

    // static const byte_t print_char = 'c'; // 99

    // static const byte_t string_expression_end = 255;
    // static const byte_t int_expression_next = 232; // 'è'

}


#endif



/*    static const byte_t string_set_next = 6;
    static const byte_t string_map_next = 7;
    
    static const byte_t auto_string_set_next = 10;
    static const byte_t auto_string_map_next = 11;
*/
