#pragma once

#include "common/macro.h"
#include "common/list.h"

typedef enum TreeType {
    TT_DeclList = 100,  // AST: DeclList
    TT_VarDecl,         // AST: VarDecl
    TT_FuncDecl,        // AST: FuncDecl
    TT_ParamList,       // AST: ParamList
    TT_Param,           // AST: Param
    TT_LocalDecl,       // AST: LocalDecl
    TT_IfStmt,          // AST: IfStmt
    TT_WhileStmt,       // AST: WhileStmt
    TT_ReturnStmt,      // AST: ReturnStmt
    TT_Var,             // AST: Var
    TT_AssignExpr,      // AST: AssignExpr
    TT_SimpleExpr,      // AST: SimpleExpr
    TT_FuncCall,        // AST: FuncCall
    TT_Number,          // AST: Number
    TT_ArgList,         // AST: ArgList
} TreeType;

typedef enum VarType {
    VT_UNDEFINE,
    VT_INT,
    VT_VOID,
} VarType;

typedef enum OperateType {
    OT_UNDEFINE,             // undefine
    OT_PLUS,                 // +
    OT_MINUS,                // -
    OT_MULTIPLY,             // *
    OT_DIVIDE,               // /
    OT_LESS,                 // <
    OT_LESS_EQ,              // <=
    OT_GREATER,              // >
    OT_GREATER_EQ,           // >=
    OT_EQUAL,                // ==
    OT_NOT_EQ,               // !=
    OT_ASSIGN,               // =
    OT_SEMICOLON,            // ;
    OT_COMMA,                // ,
    OT_LR_BRACKET,           // (
    OT_RR_BRACKET,           // )
    OT_LS_BRACKET,           // [
    OT_RS_BRACKET,           // ]
    OT_LC_BRACKET,           // {
    OT_RC_BRACKET,           // }
} OperateType;

typedef struct DeerNode {
    TreeType type;
} DeerNode;

typedef struct DeerNumber {
    TreeType type;
    int val;
} DeerNumber;

typedef struct DeerParam {
    TreeType type;
    VarType vt;                     // only integer
    char *id;                       // only id
} DeerParam;

typedef struct DeerVarDecl {
    TreeType type;
    VarType vt;                     // only integer
    char *id;                       // only id
    char *scope;                    // var scope, global or function internal
    bool is_local;                  // is local var, or global var
    bool is_array;                  // is array
    int arr_size;                   // array size while is_array is true
} DeerVarDecl;

typedef struct DeerSimpleExpr {
    TreeType type;
    OperateType op;
    DeerNode *lchild;
    DeerNode *rchild;
} DeerSimpleExpr;

typedef struct DeerVar {
    TreeType type;
    char *id;                   // only id
    DeerSimpleExpr *index;
} DeerVar;

typedef struct DeerAssignExpr {
    TreeType type;
    DeerVar *var;
    DeerSimpleExpr *expr;
} DeerAssignExpr;

typedef struct DeerIfStmt {
    TreeType type;
    DeerSimpleExpr *condition;
    DeerLinkedList *if_block;
    DeerLinkedList *else_block;
} DeerIfStmt; 

typedef struct DeerWhileStmt {
    TreeType type;
    DeerSimpleExpr *condition;
    DeerLinkedList *block;
} DeerWhileStmt;

typedef struct DeerReturnStmt {
    TreeType type;
    DeerSimpleExpr *expr;
} DeerReturnStmt;

typedef struct DeerFuncArgs {
    TreeType type;
    VarType vt;         // only integer
    char *id;           // only id
} DeerFuncArgs;

typedef struct DeerFuncCall {
    TreeType type;
    char *fname;                    // function name
    DeerLinkedList *args;           // function intput parameters, 
                                    // use linkedlist include DeerFuncArgs;
} DeerFuncCall;

typedef struct DeerFuncDecl {
    TreeType type;
    VarType vt;                     // return value type, only int / void
    char *fname;                    // function name
    DeerLinkedList *params;         // function intput parameters, 
                                    // use linkedlist include DeerFuncArgs;
    DeerLinkedList *var_list;       // var define list, detail: DeerVarDecl
    DeerLinkedList *stmt_list;      // statement list, detail: 
                                    // DeerExprStmt, DeerIfStmt, DeerWhileStmt, DeerReturnStmt
} DeerFuncDecl;

typedef struct DeerDeclList {
    TreeType type;
    DeerLinkedList *decls;          // DeerVarDecl / DeerFuncDecl
} DeerDeclList;

#define CREATE_NODE(_type_) ((Deer##_type_ *) dtree_new_node(sizeof(Deer##_type_), TT_##_type_))

extern DeerNode *dtree_new_node(size_t s, TreeType t);

// extern void dtree_distory(DeerMultiTree *tree);
// extern DeerMultiTree *dtree_create(void *data);
// extern DeerMultiTree *dtree_append(DeerMultiTree *tree, DeerMultiTree *child);
// extern DeerMultiTree *dtree_clean_clilds(DeerMultiTree *tree);