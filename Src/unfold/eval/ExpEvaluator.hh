#ifndef NEW_EXP_EVALUATOR_HH
#define NEW_EXP_EVALUATOR_HH

#include <vector>


struct ExpEvaluator
{
    ~ExpEvaluator();

    static void SetVariables( std::vector<int>& old_vars,
                              std::vector<int>& new_vars )
    { old_variables = &old_vars; new_variables = &new_vars; }

    static int Eval();

 private:
    static std::vector<int> stack;

    static std::vector<int>* old_variables;
    static std::vector<int>* new_variables;
};


#endif
