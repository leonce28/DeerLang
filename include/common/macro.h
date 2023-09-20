#pragma once

#define nullptr                     NULL

#ifndef bool
#define bool                       short
#define true                         (1)
#define false                        (0)
#endif 

#define VAR_SIZE_MAX                  32
#define VAR_NAME_MAX                  32
#define TOKEN_STR_MAX                 32
#define MAP_NAME_MAX                  64
#define SPACE_NAME_MAX                64
#define VAR_OFFSET_MAX                64
#define TOKEN_SIZE_DEF               256
#define TOKEN_LIST_MAX               512
#define AST_LIST_MAX                1024
#define MAP_LIST_MAX                1024
#define CODE_LSIT_MAX               1024
#define MASP_CODE_MAX               1024
#define TABLE_SYMBOL_MAX            1024
#define TABLE_SPACE_MAX             1024
#define FILE_PATH_MAX               1024
#define NAMESPACE_GLOBAL            "__GLOBAL__"
#define NAMESPACE_ACCESS            "main"
#define DEFAULT_ASM_PATH            "./a.out"
#define MIN(a, b)                   (((a) < (b)) ? (a) : (b))
#define MAX(a, b)                   (((a) > (b)) ? (a) : (b))
#define AST_NODE_BACK(node)         (node->sub_list[node->sub_idx])
#define NEW_AST_NODE(_str, _type)   (dtree_create(create_token(_str, _type)))
#define CURRENT_TOKEN()             (dcell_data(DeerToken, cell))
#define CURRENT_TOKEN_STR()         (CURRENT_TOKEN()->token_str)
#define CURRENT_TOKEN_TYPE()        (CURRENT_TOKEN()->type)
#define TOKEN_IS_MATCH(_t)          (CURRENT_TOKEN_TYPE() == _t)
#define TOKEN_IS_MATCH2(_n, _t)     (_n->type == _t)
#define MOVE_NEXT_LINE(data)        do { if (*data == '\n') { data++; break; } } while (data++ < end)
#define PATH_IS_EMPTY(path)         (path != NULL && (strlen(path) == 0))
#define NEW(POINTER, TYPE)          do {                                        \
                                        POINTER = (TYPE *)malloc(sizeof(TYPE)); \
                                        assert(POINTER != NULL);                \
                                        memset(POINTER, 0, sizeof(TYPE));       \
                                    } while(0);

typedef enum _result {
    CMM_FAILED = -1,            // failed
    CMM_SUCCESS = 0,            // success
} result;

typedef enum TokenType {
    // Word
    TOKEN_ID,                   // Id
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
} TokenType;

typedef enum LexicalStage {
    STAGE_START,
    STAGE_IN_ID,
    STAGE_IN_NUMBER,
    STAGE_IN_DIVIDE,
    STAGE_IN_COMMENT,
    STAGE_IN_LESS,
    STAGE_IN_GREATER,
    STAGE_IN_ASSIGN,
    STAGE_IN_NOT,
    STAGE_END_COMMENT,
    STAGE_DONE,
} LexicalStage; 

typedef enum Instruction {
    // Load
    INS_LDC,
    INS_LD,
    INS_ALD,

    // Store
    INS_ST,
    INS_AST,

    // Push, Pop
    INS_PUSH,
    INS_POP,

    // Jump
    INS_JMP,
    INS_JZ,

    // Arithmetic
    INS_ADD,
    INS_SUB,
    INS_MUL,
    INS_DIV,

    // Relationship
    INS_LT,
    INS_LE,
    INS_GT,
    INS_GE,
    INS_EQ,
    INS_NE,

    // I/O
    INS_IN,
    INS_OUT,

    // Address
    INS_ADDR,

    // Functional
    INS_CALL,
    INS_RET,
} Instruction;
