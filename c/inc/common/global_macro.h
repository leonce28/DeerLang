#pragma once

#define TOKEN_SIZE_DEF  256
#define FILE_PATH_MAX   512
#define TOKEN_STR_MAX   32
#define TOKEN_LIST_MAX  512
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))

enum {
    CMM_FAILED = -1,            // failed
    CMM_SUCCESS = 0,            // success
};


enum _token_type{
    // Word
    TOKEN_ID = 1000,            // Id
    TOKEN_NUMBER,               // Number

    // Keyword
    TOKEN_VOID,                 // void
    TOKEN_INT,                  // int
    TOKEN_IF,                   // if
    TOKEN_ELSE,                 // else
    TOKEN_WHILE,                // while
    TOKEN_RETURN,               // return

    // Operator
    TOKEN_PLUS,                 // +
    TOKEN_MINUS,                // -
    TOKEN_MULTIPLY,             // *
    TOKEN_DIVIDE,               // /
    TOKEN_LESS,                 // <
    TOKEN_LESS_EQUAL,           // <=
    TOKEN_GREATER,              // >
    TOKEN_GREATER_EQUAL,        // >=
    TOKEN_EQUAL,                // ==
    TOKEN_NOT_EQUAL,            // !=
    TOKEN_ASSIGN,               // =
    TOKEN_SEMICOLON,            // ;
    TOKEN_COMMA,                // ,
    TOKEN_LEFT_ROUND_BRACKET,   // (
    TOKEN_RIGHT_ROUND_BRACKET,  // )
    TOKEN_LEFT_SQUARE_BRACKET,  // [
    TOKEN_RIGHT_SQUARE_BRACKET, // ]
    TOKEN_LEFT_CURLY_BRACKET,   // {
    TOKEN_RIGHT_CURLY_BRACKET,  // }

    // End
    TOKEN_END,                  // END

    // Ast
    TOKEN_DECL_LIST,            // AST: DeclList
    TOKEN_VAR_DECL,             // AST: VarDecl
    TOKEN_FUNC_DECL,            // AST: FuncDecl
    TOKEN_PARAM_LIST,           // AST: ParamList
    TOKEN_PARAM,                // AST: Param
    TOKEN_COMPOUND_STMT,        // AST: CompoundStmt
    TOKEN_LOCAL_DECL,           // AST: LocalDecl
    TOKEN_STMT_LIST,            // AST: StmtList
    TOKEN_IF_STMT,              // AST: IfStmt
    TOKEN_WHILE_STMT,           // AST: WhileStmt
    TOKEN_RETURN_STMT,          // AST: ReturnStmt
    TOKEN_EXPR,                 // AST: Expr
    TOKEN_VAR,                  // AST: Var
    TOKEN_SIMPLE_EXPR,          // AST: SimpleExpr
    TOKEN_ADD_EXPR,             // AST: AddExpr
    TOKEN_TERM,                 // AST: Term
    TOKEN_CALL,                 // AST: Call
    TOKEN_ARG_LIST,             // AST: ArgList
};

typedef enum _lexer_stage {
    STAGE_START = 2000,
    STAGE_IN_ID,
    STAGE_IN_NUMBER,
    STAGE_IN_DIVIDE,
    STAGE_IN_COMMENT,
    STAGE_IN_LESS,
    STAGE_IN_GREATER,
    STAGE_IN_ASSIGN,
    STAGE_IN_NOT,
    STAGE_END_COMMENT,
    STAGE_DONE
} lexer_stage; 
