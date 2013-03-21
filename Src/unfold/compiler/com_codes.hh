#ifndef COM_CODES_HH
#define COM_CODES_HH


namespace com_codes
{
    const byte_t block_end = 0;

    // Automatic variables:
    const byte_t auto_string_next = 1;
    const byte_t auto_int_next = 2;

    // Member variables:
    const byte_t member_offset = 20;    
    const byte_t member_string_next = 1 + member_offset;
    const byte_t member_int_next = 2 + member_offset;

    // Return types:
    const byte_t return_offset = 40; // '('
    const byte_t return_string = 1 + return_offset; // ')'
    const byte_t return_int = 2 + return_offset; // '*'

    const byte_t np_break = 'B'; // 66
    const byte_t call = 'C'; // 67
    const byte_t jump_forward = 'F'; //
    const byte_t np_if = 'I'; // 73
    const byte_t np_continue = 'O'; // Ok -- 79
    const byte_t print = 'P'; // 80
    const byte_t np_return = 'R'; // 82
    const byte_t np_while = 'W'; // 87

    const byte_t inc_auto_int = 'i'; //
    const byte_t dec_auto_int = 'd'; //
    const byte_t inc_member_int = 'j'; //
    const byte_t dec_member_int = 'e'; //

    const byte_t print_char = 'p'; //
}


#endif
