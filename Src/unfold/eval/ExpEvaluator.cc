#include "ExpEvaluator.hh"

#include "iBC.hh"
#include "uf_types.hh"

#include "error_handling.hh"
#include "LogWrite.hh"
#include "FileFormat/Lexical/OTokenStream.hh"


std::vector<int> ExpEvaluator::stack;
std::vector<int>* ExpEvaluator::old_variables = 0;
std::vector<int>* ExpEvaluator::new_variables = 0;


// Internal:
namespace
{
    std::string str, str2; int mem_val = -1;
    std::string bug_pre( "a bug caught in unfold/eval/ExpEvaluator.cc, " );
    void Wrap( int& val ) { val = uf_types::WrapValue( iBC::getByte(), val ); }
}


#define int_bin_op( OP ) stack[stack.size()-2] OP stack.back(); stack.pop_back()
#define int_bin_op_no_zero_div( OP ) right = stack.back(); stack.pop_back(); \
    check_claim( right, "division by zero in an arithmetic expression **" ); \
    stack.back() OP right
#define comp_bin_op( OP )  mem_val = stack.back(); stack.pop_back(); \
                           stack.back() = ( stack.back() OP mem_val );

ExpEvaluator::~ExpEvaluator() { check_claim( stack.empty(), bug_pre +
                                  "~ExpEvaluator(): stack is not empty **" ); }

int
ExpEvaluator::Eval()
{
    int left = 0; int right = 0;

    while ( true )
    {
        char byte = static_cast<char>( iBC::getByte() );
        
        switch ( byte )
        {
            // Literals or variables next?
          case 'l': stack.push_back( iBC::getByte() ); break;
          case 'o': stack.push_back( (*old_variables)[iBC::getByte()] ); break;
          case 'n': stack.push_back( (*new_variables)[iBC::getByte()] ); break;
              
              // Minus sign:
          case '~': stack.back() = -stack.back(); break;
              
              // Arithetic operations:
          case '+': int_bin_op( += ); break;
          case '-': int_bin_op( -= ); break;
          case '*': int_bin_op( *= ); break;
          case '/': int_bin_op_no_zero_div( /= ); break;
          case '%': int_bin_op_no_zero_div( %= ); break;

              // wrap, inc and dec:
          case 'w': Wrap( stack.back() ); break;
          case 'y': Wrap( ++stack.back() ); break;
          case 'a': Wrap( --stack.back() ); break;
          case 'Y': ++stack.back(); break;
          case 'A': --stack.back(); break;
              
              // Assignment to a variable:
          case ':': left = stack.back(); right = iBC::getByte();
              (*new_variables)[right] = left;
              (*old_variables)[right] = left; break;

              // Printing a character:
          case 'C': LogWrite::getOStream() << static_cast<char>(stack.back());
              break;
              // Printing a value:
          case 'P': str = iBC::GetStringLiteral();
              oTokenStream::number2string( stack.back(), str2 );
              PrintMessageLine( str + str2 + iBC::GetStringLiteral() ); break;
              // Picking up a memorized value:
          case 'm': stack.push_back( mem_val ); break;
              // Skipping a value in the stack:
          case 'k' : stack.pop_back(); break;
              // Comparisons:
          case '=': comp_bin_op( == ); break;
          case 'D': comp_bin_op( != ); break;
          case '<': comp_bin_op( < ); break;
          case '>': comp_bin_op( > ); break;
          case '[': comp_bin_op( <= ); break;
          case ']': comp_bin_op( >= ); break;
              // Logical not:
          case '!': stack.back() = !stack.back(); break;
              // Logical and:
          case '&': if ( !stack.back() ) { iBC::JumpForward( iBC::getByte() ); }
                    else { iBC::getByte(); stack.pop_back(); } break;
              // Logical or:
          case '|': if ( stack.back() ) { iBC::JumpForward( iBC::getByte() ); }
                    else { iBC::getByte(); stack.pop_back(); } break;
              // End of expression:
          case 'Q': left = stack.back(); stack.pop_back(); return left;

          default: // We should never come here:
              str = bug_pre + "EvalLogicExp(): unknown byte code ";
              oTokenStream::number2string( static_cast<unsigned>(byte), str2 );
              str += str2;
              if ( static_cast<unsigned>(byte) > 32 )
              { str += " = '"; str += byte; str += "'"; }
              str += " **"; AbortWithMessage( str ); break;
        }
        
    }

    return 0;        
}
