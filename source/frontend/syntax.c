#include <stdlib.h>
#include "frontend/syntax.h"
#include "common/funcs.h"

static DeerListCell *cell = nullptr;
static char *curr_scope = nullptr;

DeerNode *make_raw_stmt();
DeerNode *make_expr_stmt();
DeerLinkedList *make_stmt_list();
DeerAssignExpr *make_assign_expr();
DeerSimpleExpr *make_simple_expr();

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
            vt = VT_UNDEFINE;
            invalid_token(CURRENT_TOKEN());
            break;
    }

    match_token(CURRENT_TOKEN_TYPE());
    return vt;
}

DeerParam *make_param()
{
    /*
        EBNF:
            param ::= type id [ '[' ']' ]
    */
    DeerParam *param = CREATE_NODE(Param);
    param->vt = scan_type();

    if (TOKEN_IS_MATCH(TOKEN_ID)) {
        param->id = CURRENT_TOKEN_STR();
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

DeerVarDecl *make_var_declared()
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
            arg_list ::= simple_expr { ',' simple_expr }
    */

    DeerLinkedList *arg_list = nullptr;

    arg_list = dlist_push_back(arg_list, make_simple_expr());

    while (TOKEN_IS_MATCH(TOKEN_COMMA)) {
        match_token(TOKEN_COMMA);
        dlist_push_back(arg_list, make_simple_expr());
    }

    return arg_list;
}

DeerFuncCall *make_func_call()
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

    if (TOKEN_IS_MATCH(TOKEN_ID) || 
        TOKEN_IS_MATCH(TOKEN_NUMBER) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET)) {
        func_call->args = make_arg_list();
    }

    match_token(TOKEN_RIGHT_ROUND_BRACKET);

    return func_call;
}

DeerVar *make_var()
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

    return var;
}

DeerNumber *make_number()
{

    DeerNumber *num = CREATE_NODE(Number);

    if (TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        num->val = atoi(CURRENT_TOKEN_STR());
        match_token(TOKEN_NUMBER);
    } else {
        invalid_token(CURRENT_TOKEN());
    }

    return num;
}

OperateType scan_mul_op()
{
    /*
        EBNF:
            mul_op ::= '*' 
                    | '/'
    */
    OperateType op;

    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_MULTIPLY:
            op = OT_MULTIPLY;
            break;
        case TOKEN_DIVIDE:
            op = OT_DIVIDE;
            break;
        default:
            op = OT_UNDEFINE;
            invalid_token(CURRENT_TOKEN());
            break;
    } 

    match_token(CURRENT_TOKEN_TYPE());
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
            factor = (DeerNode *) make_simple_expr();
            match_token(TOKEN_RIGHT_ROUND_BRACKET);
            break;
        case TOKEN_NUMBER:
            factor = (DeerNode *) make_number();
            break;
        case TOKEN_ID:
            next = dcell_next(cell);
            if (next && TOKEN_IS_MATCH2(dcell_data(DeerToken, next), 
                                        TOKEN_LEFT_ROUND_BRACKET)) {
                factor = (DeerNode *) make_func_call();
            } else {
                factor = (DeerNode *) make_var();
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
    OperateType op;

    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_PLUS:
            op = OT_PLUS;
            break;
        case TOKEN_MINUS:
            op = OT_MINUS;
            break;
        default:
            op = OT_UNDEFINE;
            invalid_token(CURRENT_TOKEN());
            break;
    } 

    match_token(CURRENT_TOKEN_TYPE());
    return op;
}

DeerNode *make_term()
{
    /*
        EBNF:
            term ::= factor { mul_op factor }
    */
    const DeerListCell *next = nullptr;
    DeerSimpleExpr *term = nullptr, *terms = nullptr;

    if ((next = dcell_next(cell)) == nullptr || 
        (!TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_MULTIPLY) && 
         !TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_DIVIDE))) {
        return make_factor();
    }

    term = CREATE_NODE(SimpleExpr);
    term->lchild = (DeerNode *) make_factor();
    term->op = scan_mul_op();
    term->rchild = (DeerNode *) make_factor();

    while (TOKEN_IS_MATCH(TOKEN_MULTIPLY) || TOKEN_IS_MATCH(TOKEN_DIVIDE)) {
        terms = CREATE_NODE(SimpleExpr);
        terms->lchild = (DeerNode *)term;
        terms->op = scan_mul_op();
        terms->rchild = (DeerNode *) make_factor();
        term = terms;
    }

    return (DeerNode *)term;
}

DeerNode *make_add_expr()
{
    /*
        EBNF:
            add_expr ::= term { add_op term }
    */
    const DeerListCell *next = nullptr;
    DeerSimpleExpr *add_expr = nullptr, *add_exprs = nullptr;

    if ((next = dcell_next(cell)) == nullptr || 
        (!TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_PLUS) && 
         !TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_MINUS))) {
        return make_term();
    }

    add_expr = CREATE_NODE(SimpleExpr);
    add_expr->lchild = (DeerNode *) make_term();
    add_expr->op = scan_add_op();
    add_expr->rchild = (DeerNode *) make_term();

    while (TOKEN_IS_MATCH(TOKEN_PLUS) || TOKEN_IS_MATCH(TOKEN_MINUS)) {
        add_exprs = CREATE_NODE(SimpleExpr);
        add_exprs->lchild = (DeerNode *)add_expr;
        add_exprs->op = scan_add_op();
        add_exprs->rchild = (DeerNode *) make_term();
        add_expr = add_exprs;
    }

    return (DeerNode *) add_expr;
}

OperateType scan_op()
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
            op = OT_UNDEFINE;
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
    DeerNode *temp = nullptr;
    DeerSimpleExpr *simple = nullptr; 

    temp = make_add_expr();

    switch (CURRENT_TOKEN_TYPE()) {
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
            simple = CREATE_NODE(SimpleExpr);
            simple->op = scan_op();
            simple->rchild = make_add_expr();
            break;
        default:
            break;
    }

    if (simple) {
        simple->lchild = temp;
        return simple;
    } else if (temp->type != TT_SimpleExpr) {
        simple = CREATE_NODE(SimpleExpr);
        simple->lchild = temp;
        return simple;
    }

    return (DeerSimpleExpr *)temp;
}

DeerAssignExpr *make_assign_expr()
{
    /*
        EBNF:
            assign_expr ::= var '=' expr
    */
    DeerVar *var = nullptr;
    DeerListCell *back = cell;
    DeerListCell *next = dcell_next(cell);
    DeerAssignExpr *assign_expr = nullptr;

    // 如果是函数
    if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_ROUND_BRACKET)) {
        return nullptr;
    }

    var = make_var();

    if (!var || !TOKEN_IS_MATCH(TOKEN_ASSIGN)) {
        cell = back;
        return nullptr;
    } 

    match_token(TOKEN_ASSIGN);

    assign_expr = CREATE_NODE(AssignExpr);

    assign_expr->var = var;
    assign_expr->expr = make_simple_expr();

    return assign_expr;
}

DeerNode *make_expr_stmt()
{
    /*
        EBNF:
            expr_stmt ::= assign_expr
                        | simple_expr
    */
    DeerNode *expr = nullptr;

    if (!TOKEN_IS_MATCH(TOKEN_ID) && 
        !TOKEN_IS_MATCH(TOKEN_NUMBER) &&
        !TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET)) { 
        return nullptr;
    } 

    // 只能是简单表达式
    if (TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET) ||
        TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        expr = (DeerNode *) make_simple_expr();
        return expr;
    } 

    if (!TOKEN_IS_MATCH(TOKEN_ID)) {
        invalid_token(CURRENT_TOKEN());
    }
    
    // 判断是否是赋值操作
    if ((expr = (DeerNode *) make_assign_expr()) != nullptr) {
        match_token(TOKEN_SEMICOLON);
        return expr;
    }

    // 不是赋值操作，那自然是简单表达式
    expr = (DeerNode *) make_simple_expr();
    return expr;
}

DeerIfStmt *make_if_stmt()
{
    /*
        EBNF:
            if_stmt ::= if '(' expr ')' '{' stmt_list '}' [ else '{' stmt_list '}' ]
    */
    DeerIfStmt *if_stmt = CREATE_NODE(IfStmt);

    match_token(TOKEN_IF);
    match_token(TOKEN_LEFT_ROUND_BRACKET);

    if_stmt->condition = make_simple_expr();

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

DeerWhileStmt *make_while_stmt()
{
    /*
        EBNF:
            while_stmt ::= while '(' expr ')' stmt_list
    */
    DeerWhileStmt *while_stmt = CREATE_NODE(WhileStmt);

    match_token(TOKEN_WHILE);
    match_token(TOKEN_LEFT_ROUND_BRACKET);

    while_stmt->condition = make_simple_expr();

    match_token(TOKEN_RIGHT_ROUND_BRACKET);
    match_token(TOKEN_LEFT_CURLY_BRACKET);

    while_stmt->block = make_stmt_list();

    match_token(TOKEN_RIGHT_CURLY_BRACKET);

    return while_stmt;
}

DeerReturnStmt *make_return_stmt()
{
    /*
        EBNF:
            return_stmt ::= return [ expr ] ';'
    */
    DeerReturnStmt *return_stmt = CREATE_NODE(ReturnStmt);

    match_token(TOKEN_RETURN);

    if (TOKEN_IS_MATCH(TOKEN_ID) ||
        TOKEN_IS_MATCH(TOKEN_LEFT_ROUND_BRACKET) ||
        TOKEN_IS_MATCH(TOKEN_NUMBER)) {
        return_stmt->expr = make_simple_expr();
    }

    match_token(TOKEN_SEMICOLON);

    return return_stmt;
}

DeerNode *make_raw_stmt()
{
    /*
        EBNF:
            raw_stmt ::= expr_stmt
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
            stmt = (DeerNode *) make_expr_stmt();
            break;
        case TOKEN_IF:
            stmt = (DeerNode *) make_if_stmt();
            break;
        case TOKEN_WHILE:
            stmt = (DeerNode *) make_while_stmt();
            break;
        case TOKEN_RETURN:
            stmt = (DeerNode *) make_return_stmt();
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
            stmt_list ::= { raw_stmt }
    */
    DeerLinkedList *stmt_list = nullptr;

    for (bool flag = true; flag && cell; ) {
        switch (CURRENT_TOKEN_TYPE()) {
            case TOKEN_ID:
            case TOKEN_LEFT_ROUND_BRACKET:
            case TOKEN_NUMBER:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
                stmt_list = dlist_push_back(stmt_list, make_raw_stmt());
                break;
            case TOKEN_SEMICOLON:
                match_token(TOKEN_SEMICOLON);
                break;
            default:
                flag = false;
                break;
        }
    }

    return stmt_list;
}

DeerFuncDecl *make_func_declared()
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
        node = (DeerNode *) make_var_declared();
    } else if (TOKEN_IS_MATCH2(dcell_data(DeerToken, next), TOKEN_LEFT_ROUND_BRACKET)) {
        node = (DeerNode *) make_func_declared();
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
        decl_list->decls = dlist_push_back(decl_list->decls, make_declared());
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
