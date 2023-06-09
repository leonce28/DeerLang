#include <stdlib.h>
#include "frontend/syntax.h"
#include "common/funcs.h"

static DeerListCell *cell = nullptr;
static char *curr_scope = nullptr;

DeerNode *make_expr_stmt();
DeerNode *make_stmt();
DeerNode *make_stmt_list();

void match_token(TokenType type)
{
    DeerToken *token = dcell_data(DeerToken, cell);
    if (token->type == type) {
        cell = dcell_next(cell);
    } else {
        invalid_token(token);
    }
}

VarType scan_type()
{
    /*
        EBNF:
            type ::= int
                | void
    */
    
    VarType vt;
    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_INT:
            vt = VT_INT;
            break;
        case TOKEN_VOID:
            vt = VT_VOID;
            break;
        default:
            invalid_token(CURRENT_TOKEN());
            break;
    }

    match_token(CURRENT_TOKEN_TYPE());
    return vt;
}

DeerNode *make_param()
{
    /*
        EBNF:
            param ::= type id [ '[' ']' ]
    */
    DeerNode *param = NEW_AST_NODE("Param", TOKEN_PARAM);
    dtree_append(param, scan_type());

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        dtree_append(param, dtree_create(CURRENT_TOKEN()));
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    if (TOKEN_IS_MATCH(TOKEN_LEFT_SQUARE_BRACKET)) {
        match_token(TOKEN_LEFT_SQUARE_BRACKET);
        match_token(TOKEN_RIGHT_SQUARE_BRACKET);
    }

    return param;
}

DeerLinkedList *make_param_list()
{
    /*
        EBNF:
            param_list ::= [ param { ',' param } ]
    */
    DeerLinkedList *param_list = nullptr; 
    
    if (!TOKEN_IS_MATCH(TOKEN_INT) &&
        !TOKEN_IS_MATCH(TOKEN_VOID)) {
        return param_list;
    }

    param_list = dlist_push_back(param_list, make_param());

    while (TOKEN_IS_MATCH(TOKEN_COMMA)) {
        match_token(TOKEN_COMMA);
        dlist_push_back(param_list, make_param());
    }

    return param_list;
}

DeerNode *make_params()
{
    /*
        EBNF:
            params ::= [ param_list ]
    */

    if (TOKEN_IS_MATCH(TOKEN_INT) || 
        TOKEN_IS_MATCH(TOKEN_VOID)) {
        return make_param_list();
    }

    return nullptr;
}

DeerNode *make_var_declared()
{
    /*
        EBNF:
            var_declared ::= type id [ '[' number ']' ] ';'
    */

    DeerVarDecl *var_decl = CREATE_NODE(VarDecl);

    var_decl->vt = scan_type();

    if (curr_scope) {
        var_decl->is_local = true;
        var_decl->scope = curr_scope;
    } else {
        var_decl->is_local = false;
    }

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        var_decl->id = CURRENT_TOKEN_STR();
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    if (TOKEN_IS_MATCH(TOKEN_LEFT_SQUARE_BRACKET)) {
        var_decl->is_array = true;
        match_token(TOKEN_LEFT_SQUARE_BRACKET);
        var_decl->arr_size = atoi(CURRENT_TOKEN_STR());
        match_token(TOKEN_NUMBER);
        match_token(TOKEN_RIGHT_SQUARE_BRACKET);
    }

    match_token(TOKEN_SEMICOLON);

    return var_decl;
}


DeerLinkedList *make_var_decl_list()
{
    /*
        EBNF:
            var_decl_list ::= [ var_declared { var_declared }]
    */

    DeerLinkedList *var_decl_list = nullptr;

    while (TOKEN_IS_MATCH(TOKEN_INT) || TOKEN_IS_MATCH(TOKEN_VOID)) {
        var_decl_list = dlist_push_back(var_decl_list, make_var_declared());
    }

    return var_decl_list;
}


DeerLinkedList *make_arg_list()
{
    /*
        EBNF:
            arg_list ::= expr { ',' expr }
    */
    
    DeerLinkedList *arg_list = nullptr;

    arg_list = dlist_push_back(arg_list, make_expr_stmt());

    while (TOKEN_IS_MATCH(TOKEN_COMMA)) {
        match_token(TOKEN_COMMA);
        dlist_push_back(arg_list, make_expr_stmt());
    }

    return arg_list;
}

DeerNode *make_func_call()
{
    /*
        EBNF:
            call ::= id '(' [ arg_list ] ')'
    */

    DeerFuncCall *func_call = CREATE_NODE(FuncCall);

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        func_call->fname = CURRENT_TOKEN_STR();
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN()); 
    }

    match_token(TOKEN_LEFT_ROUND_BRACKET);

    if (TOKEN_IS_MATCH(TOKEN_ID) || TOKEN_IS_MATCH(TOKEN_NUMBER) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET)) {
        func_call->args = make_arg_list();
    }

    match_token(TOKEN_RIGHT_ROUND_BRACKET);

    return func_call;
}

DeerNode *make_var()
{
    /*
        EBNF:
            var ::= id [ '[' expr ']' ]
    */

    DeerVar *var = CREATE_NODE(Var);

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        var->id = CURRENT_TOKEN_STR();
        match_token(TOKEN_ID);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    if (TOKEN_IS_MATCH(TOKEN_LEFT_SQUARE_BRACKET)) {
        match_token(TOKEN_LEFT_SQUARE_BRACKET);
        var->index = make_simple_expr();
        match_token(TOKEN_RIGHT_SQUARE_BRACKET);
    }

    return (DeerNode *)var;
}

DeerNode *make_number()
{

    DeerNumber *num = CREATE_NODE(Number);

    if (TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        num->val = atoi(CURRENT_TOKEN_STR());
        match_token(TOKEN_NUMBER);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return (DeerNode *)num;
}

DeerNode *scan_mul_op()
{
    /*
        EBNF:
            mul_op ::= '*' 
                    | '/'
    */
    OperateType op = nullptr;

    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_MULTIPLY:
            op = OT_MULTIPLY;
            break;
        case TOKEN_DIVIDE:
            op = OT_DIVIDE;
            break;
        default:
            invalid_token(CURRENT_TOKEN());
            break;
    } 

    return op;
}

DeerNode *make_factor()
{
    /*
        EBNF:
            factor ::= '(' expr ')'
                     | var
                     | call
                     | num
    */
   DeerNode *factor = nullptr;
   const DeerListCell *next = nullptr;
   switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_LEFT_ROUND_BRACKET:
            match_token(TOKEN_LEFT_ROUND_BRACKET);
            factor = make_expr_stmt();
            match_token(TOKEN_RIGHT_ROUND_BRACKET);
            break;
        case TOKEN_NUMBER:
            factor = make_number();
            break;
        case TOKEN_ID:
            next = dcell_next(cell);
            if (next && TOKEN_IS_MATCH2(dcell_data(DeerToken, next), 
                                        TOKEN_LEFT_ROUND_BRACKET)) {
                factor = make_func_call();
            } else {
                factor = make_var();
            }
            break;
        default:
            invalid_token(CURRENT_TOKEN());
            break;
   }

   return factor;
}

OperateType scan_add_op()
{
    /*
        EBNF:
            add_op ::= +
                    | -
    */
    OperateType op = nullptr;

    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_PLUS:
            op = OT_PLUS;
            break;
        case TOKEN_MINUS:
            op = OT_MINUS;
            break;
        default:
            invalid_token(CURRENT_TOKEN());
            break;
    } 

    return op;
}

DeerSimpleExpr *make_term()
{
    /*
        EBNF:
            term ::= factor { mul_op factor }
    */
    DeerSimpleExpr *term = CREATE_NODE(SimpleExpr);

    term->lchild = make_factor();

    while (TOKEN_IS_MATCH(TOKEN_MULTIPLY) || 
           TOKEN_IS_MATCH(TOKEN_DIVIDE)) {
        term->op = scan_mul_op();
        term->rchild = make_factor();
    }

    return term;
}

DeerSimpleExpr *make_add_expr()
{
    /*
        EBNF:
            add_expr ::= term { add_op term }
    */
    DeerSimpleExpr *add_expr = CREATE_NODE(SimpleExpr);

    add_expr->lchild = make_term();

    while (TOKEN_IS_MATCH(TOKEN_PLUS) || 
           TOKEN_IS_MATCH(TOKEN_MINUS)) {
        add_expr->op = scan_add_op();
        add_expr->rchild = make_term();
    }

    return add_expr;
}

OperateType *scan_op()
{ 
    /*
        EBNF:
            rel_op ::= <
                    | <=
                    | >
                    | >=
                    | ==
                    | !=
    */
    OperateType op;
    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_LESS:
            op = OT_LESS;
            break;
        case TOKEN_LESS_EQUAL:
            op = OT_LESS_EQ;
            break;
        case TOKEN_GREATER:
            op = OT_GREATER;
            break;
        case TOKEN_GREATER_EQUAL:
            op = OT_GREATER_EQ;
            break;
        case TOKEN_EQUAL:
            op = OT_EQUAL;
            break;
        case TOKEN_NOT_EQUAL:
            op = OT_NOT_EQ;
            break;
        default:
            invalid_token(CURRENT_TOKEN());
            break;
    }

    match_token(CURRENT_TOKEN_TYPE());
    return op;
}

DeerSimpleExpr *make_simple_expr()
{
    /*
        EBNF:
            simple_expr ::= add_expr [ rel_op add_expr ]

    */
    DeerSimpleExpr *simple_expr = CREATE_NODE(SimpleExpr);
    
    simple_expr->lchild = make_add_expr();

    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
            simple_expr->op = scan_op();
            simple_expr->rchild = make_add_expr();
            break;
        default:
            break;
    }

    return simple_expr;
}

DeerNode *make_expr_stmt()
{
    /*
        EBNF:
            expr_stmt ::= var '=' expr
                        | simple_expr
    */
    DeerExprStmt *expr_stmt = CREATE_NODE(ExprStmt);

    if (TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET) ||
        TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        expr_stmt->simple = make_simple_expr();
    } else if (TOKEN_IS_MATCH(TOKEN_ID)) {
        DeerListCell *next = dcell_next(cell);
        if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_ROUND_BRACKET)) {
            expr_stmt->simple = make_simple_expr();
        } else {
            DeerListCell *back = cell;
            expr->var = make_var();

            if (TOKEN_IS_MATCH(TOKEN_ASSIGN)) {
                match_token(TOKEN_ASSIGN);
                expr->is_assign = true;
                expr->sub = make_expr_stmt();
            } else {
                expr->var = nullptr;
                cell = back;
                expr->simple = make_simple_expr();
            }
        }
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return (DeerNode *)expr;
}

DeerNode *make_expr_stmt_stmt()
{
    /*
        EBNF:
            expr_stmt ::= [ expr ] ';'
    */
    DeerNode *expr_stmt = nullptr;

    if (TOKEN_IS_MATCH(TOKEN_ID) || TOKEN_IS_MATCH(TOKEN_NUMBER) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET)) { 
        expr_stmt = make_expr_stmt();
    }

    match_token(TOKEN_SEMICOLON);
    return expr_stmt;
}

DeerNode *make_if_stmt()
{
    /*
        EBNF:
            if_stmt ::= if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]
    */
    DeerIfStmt *if_stmt = CREATE_NODE(IfStmt);

    match_token(TOKEN_IF);
    match_token(TOKEN_LEFT_ROUND_BRACKET);

    if_stmt->condition = make_expr_stmt();

    match_token(TOKEN_RIGHT_ROUND_BRACKET);

    match_token(TOKEN_LEFT_CURLY_BRACKET);

    if_stmt->if_block = make_stmt_list();

    match_token(TOKEN_RIGHT_CURLY_BRACKET);

    if (TOKEN_IS_MATCH(TOKEN_ELSE)) {
        match_token(TOKEN_ELSE);
        match_token(TOKEN_LEFT_CURLY_BRACKET);

        if_stmt->else_block = make_stmt_list();

        match_token(TOKEN_RIGHT_CURLY_BRACKET);
    }

    return if_stmt;
}

DeerNode *make_while_stmt()
{
    /*
        EBNF:
            while_stmt ::= while '(' expr ')' stmt_list
    */
    DeerNode *while_stmt = NEW_AST_NODE("WhileStmt", TOKEN_WHILE_STMT);

    match_token(TOKEN_WHILE);
    match_token(TOKEN_LEFT_ROUND_BRACKET);

    dtree_append(while_stmt, make_expr_stmt());

    match_token(TOKEN_RIGHT_ROUND_BRACKET);
    match_token(TOKEN_LEFT_CURLY_BRACKET);

    dtree_append(while_stmt, make_stmt_list());

    match_token(TOKEN_RIGHT_CURLY_BRACKET);

    return while_stmt;
}

DeerNode *make_return_stmt()
{
    /*
        EBNF:
            return_stmt ::= return [ expr ] ';'
    */
    DeerNode *return_stmt = NEW_AST_NODE("ReturnStmt", TOKEN_RETURN_STMT);

    match_token(TOKEN_RETURN);

    if (TOKEN_IS_MATCH(TOKEN_ID) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET) ||
        TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        dtree_append(return_stmt, make_expr_stmt());
    }

    match_token(TOKEN_SEMICOLON);

    return return_stmt;
}

DeerNode *make_stmt()
{
    /*
        EBNF:
            stmt ::= expr_stmt
                   | if_stmt
                   | while_stmt
                   | return_stmt
    */

    DeerNode *stmt = nullptr;
    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_SEMICOLON:
        case TOKEN_ID:
        case TOKEN_LEFT_ROUND_BRACKET:
        case TOKEN_NUMBER:
            stmt = make_expr_stmt();
            break;
        case TOKEN_IF:
            stmt = make_if_stmt();
            break;
        case TOKEN_WHILE:
            stmt = make_while_stmt();
            break;
        case TOKEN_RETURN:
            stmt = make_return_stmt();
            break;
        default:
            invalid_token(CURRENT_TOKEN());
            break;
   }

   return stmt;
}

DeerLinkedList *make_stmt_list()
{
    /*
        EBNF:
            stmt_list ::= { stmt }
    */
    DeerLinkedList *stmt_list = nullptr;

    for (bool flag = true; flag && cell; ) {
        switch (CURRENT_TOKEN_TYPE()) {
            case TOKEN_SEMICOLON:
            case TOKEN_ID:
            case TOKEN_LEFT_ROUND_BRACKET:
            case TOKEN_NUMBER:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
                stmt_list = dtree_append(stmt_list, make_stmt());
                break;
            default:
                flag = false;
                break;
        }
    }

    return stmt_list;
}

DeerNode *make_func_declared()
{
    /*
        EBNF:
            func_declared ::= type id '(' [ param_list ] ')' '{' [ local_del ] [ stmt_list ] '}'
    */
    DeerFuncDecl *func_decl = CREATE_NODE(FuncDecl);

    func_decl->vt = scan_type();

    if (!TOKEN_IS_MATCH(TOKEN_ID)) {
        invalid_token(CURRENT_TOKEN());
    }

    curr_scope = CURRENT_TOKEN_STR();
    func_decl->fname = CURRENT_TOKEN_STR();
    match_token(TOKEN_ID);

    match_token(TOKEN_LEFT_ROUND_BRACKET);

    func_decl->params = make_param_list();

    match_token(TOKEN_RIGHT_ROUND_BRACKET);

    match_token(TOKEN_LEFT_CURLY_BRACKET);

    func_decl->var_list = make_var_decl_list();

    func_decl->stmt_list = make_stmt_list();

    match_token(TOKEN_RIGHT_CURLY_BRACKET);

    curr_scope = nullptr;
    return func_decl;
}

DeerNode *make_declared()
{
    /*
        EBNF:
            declared ::= local_declared
                        | func_declared
    */
    DeerNode *node = nullptr;
    const DeerListCell *next = nullptr;
    if (!TOKEN_IS_MATCH(TOKEN_INT) && !TOKEN_IS_MATCH(TOKEN_VOID)) {
        invalid_token(CURRENT_TOKEN());
    }

    next = dcell_next(cell);
    if (!TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_ID)) {
        invalid_token(dcell_data(DeerToken, next));
    }

    next = dcell_next(next);
    if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_SQUARE_BRACKET) || 
        TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_SEMICOLON)) {
        node = make_var_declared();
    } else if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_ROUND_BRACKET)) {
        node = make_func_declared();
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return node;
}

DeerDeclList *make_declared_list()
{
    /*
        EBNF:
            declared_list ::= declared { declared }
    */
    DeerDeclList *decl_list = CREATE_NODE(DeclList);

    while (cell) {
        dtree_append(decl_list, make_declared());
    }

    return decl_list;
}

int syntax_analysis(DeerCompilerHandle *handle)
{
    /*
        EBNF:
            program ::= declared_list
    */
    cell = dlist_head(handle->tokens);

    handle->ast = make_declared_list();

    return CMM_SUCCESS;
}
