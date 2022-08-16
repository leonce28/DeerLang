/*
    LexicalAnalyzer.h
    =================
        Class LexicalAnalyzer header.
*/

#pragma once

#include <string>
#include <vector>
#include "Token.h"
#include "TokenType.hpp"
#include "LexerStage.hpp"

namespace CMM {

using std::string;
using std::vector;


class LexicalAnalyzer {
public:

    explicit LexicalAnalyzer(const string& inputFilePath = "");
    vector<Token> lexicalAnalysis() const;

private:
    string __inputFilePath;

    static void __invalidChar(char curChar, int lineNo);
    static void __nextTokenStartStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int& lineNo);
    static void __nextTokenInIDStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int&);
    static void __nextTokenInNumberStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int&);
    static void __nextTokenInDivideStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int&);
    static void __nextTokenInCommentStage(const char*& codePtr, LexerStage& lexerStage, TokenType&, string&, int&);
    static void __nextTokenEndCommentStage(const char*& codePtr, LexerStage& lexerStage, TokenType&, string&, int&);
    static void __nextTokenInLessStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int&);
    static void __nextTokenInGreaterStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int&);
    static void __nextTokenInAssignStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int&);
    static void __nextTokenInNotStage(const char*& codePtr, LexerStage& lexerStage, TokenType& tokenType, string& tokenStr, int& lineNo);

    static Token __nextToken(const char*& codePtr, int& lineNo);
    vector<Token> __lexicalAnalysis() const;
};


}  // End namespace CMM