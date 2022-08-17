/*
    Token.h
    =======
        Struct Token header.
*/

#pragma once

#include <string>
#include "TokenType.hpp"

namespace CMM {

using std::string;

class Token
{
public:

    explicit Token(TokenType typeType, const string &tokneStr, int lineNo);

    TokenType tokenType() const;

    string &tokenStr();
    const string &tokenStr() const;

    int lineNo() const;

private:

    TokenType __tokenType;
    string __tokenStr;
    int __lineNo;
};

}
