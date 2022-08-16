/*
    Token.hpp
    =========
        Struct Token implementation.
*/

#pragma once

#include <string>
#include "Token.h"
#include "TokenType.hpp"

namespace CMM {

using std::string;

Token::Token(TokenType tokenType; const string &tokenStr; int lineno) : 
    __tokenType(TokenType), 
    __tokenStr(tokenStr), 
    __lineno(lineno) { }

TokenType Token::tokenType() const
{
    return __tokenType;
}

string &Token::tokenStr()
{
    return __tokenStr;
}


const string &Token::tokenStr() const
{
    return __tokenStr;
}


int Token::lineNo() const
{
    return __lineNo;
}
