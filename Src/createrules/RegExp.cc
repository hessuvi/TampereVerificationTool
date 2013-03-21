#include "RegExp.hh"

#ifndef NO_REGEXP
#include <regex.h>
#include <cctype>
#include <iostream>
#endif


//-----------------------
#ifndef NO_REGEXP
//-----------------------

extern "C"
{
    struct RegExp::CompRE
    {
        static const int MAX_SUBEXPS = 10;
        std::string originalStr;
        regex_t regexp;
        mutable regmatch_t osat[MAX_SUBEXPS];
        unsigned refcnt;
    };


    void RegExp::incRefCnt() { ++(comp->refcnt); }

    void RegExp::decRefCnt()
    {
        if((--(comp->refcnt)) == 0)
        {
            regfree(&(comp->regexp));
            delete comp;
        }
    }


    RegExp::RegExp(const std::string& regular_expression)
    {
        comp = new CompRE;
        comp->refcnt = 1;

        if(USE_REGULAR_EXPRESSIONS)
        {
            std::string r = std::string("^")+regular_expression+"$";
            int res = regcomp(&(comp->regexp), r.c_str(), REG_EXTENDED);
            if(res)
            {
                std::cerr << "Error in regexp \""
                          << regular_expression << "\": ";
                printError(res);
                throw(res);
            }
        }
        else
            comp->originalStr = regular_expression;
    }

    RegExp::RegExp(const RegExp& cpy):
        comp(cpy.comp)
    {
        incRefCnt();
    }

    RegExp& RegExp::operator=(const RegExp& rhs)
    {
        if(comp != rhs.comp)
        {
            decRefCnt();
            comp = rhs.comp;
            incRefCnt();
        }
        return *this;
    }

    RegExp::~RegExp()
    {
        decRefCnt();
    }

    void RegExp::printError(int errcode) const
    {
        size_t errmsgSize = regerror(errcode, &(comp->regexp), NULL, 0);
        if(errmsgSize > 0)
        {
            char* errmsg = new char[errmsgSize];
            regerror(errcode, &(comp->regexp), errmsg, errmsgSize);
            std::cerr << errmsg << std::endl;
            delete[] errmsg;
        }
        else
        {
            std::cerr << "(Error messages not implemented for this regex.h)"
                      << std::endl;
        }
    }

    bool RegExp::match(const std::string& str) const
    {
        /*
        int res = regexec(&(comp->regexp), str.c_str(),
                          CompRE::MAX_SUBEXPS, comp->osat, 0);
        if(res!=0 && res!=REG_NOMATCH)
        {
            std::cerr << "Error in regexec(): ";
            printError(res);
        }
        return res == 0;
        */
        if(USE_REGULAR_EXPRESSIONS)
            return regexec(&(comp->regexp), str.c_str(),
                           CompRE::MAX_SUBEXPS, comp->osat, 0) == 0;
        else
            return str == comp->originalStr;
    }

    std::string RegExp::replace(const std::string& matchingStr,
                                const std::string& replacement) const
    {
        if(USE_REGULAR_EXPRESSIONS)
        {
            std::string res;
            for(unsigned i=0; i<replacement.size(); i++)
            {
                if(i<replacement.size()-1 &&
                   replacement[i]=='\\' && isdigit(replacement[i+1]))
                {
                    unsigned ind = replacement[++i]-'0';
                    if(comp->osat[ind].rm_so < 0) continue;
                    res.append(matchingStr, comp->osat[ind].rm_so,
                               comp->osat[ind].rm_eo - comp->osat[ind].rm_so);
                }
                else
                    res += replacement[i];
            }
            return res;
        }
        else return replacement;
    }

//-----------------------
#else
//-----------------------

    RegExp::RegExp(const std::string& regular_expression):
        regexp(regular_expression)
    {}
    RegExp::~RegExp() {}

    bool RegExp::match(const std::string& str) const
    {
        return regexp == str;
    }

    std::string RegExp::replace(const std::string&,
                                const std::string& replacement) const
    {
        return replacement;
    }

//-----------------------
#endif
//-----------------------
}
