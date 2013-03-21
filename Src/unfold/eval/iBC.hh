#ifndef iBC_HH
#define iBC_HH

#include "compiler/uf_config.hh"
#include "compiler/ps.hh"

#include <string>


class iBC
{
 public:
    // One byte operations:
    static inline byte_t peekByte() { return byte_code[cursor_i]; }
    static inline byte_t getByte() { return byte_code[cursor_i++]; }
    static inline void skipByte() { ++cursor_i; }

    // Multiple byte operations:
    static inline void SkipBytes( unsigned i ) { cursor_i += i; }
    static inline const std::string& GetStringLiteral()
    { return ps::GiveStringLiteral( getByte() ); }

    // Miscellaneous:
    static inline void SetByteCode( const byte_t* bc, unsigned cursor = 0 )
    { byte_code = bc; cursor_i = cursor + 1; }
    static inline void SetCursor( unsigned cursor ) { cursor_i = cursor; }

    static inline const byte_t* const GiveByteCode() { return byte_code; }
    static inline unsigned GiveCursor() { return cursor_i; }

    static inline void JumpForward( unsigned steps ) { cursor_i += steps; }
    
 private:
    static const byte_t* byte_code;
    static unsigned cursor_i;
    static std::string str;
};


#endif
