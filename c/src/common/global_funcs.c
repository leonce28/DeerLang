#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common/global_funcs.h"

extern char *optarg;
extern int getopt();

void invalid_call(const char *state)
{
    assert(state != NULL);
    printf("%s failed\n", state);
    exit(0);
}

void invalid_token(const token *t)
{
    printf("invalid token: %s in line %d\n", t->token_str, t->line_no);
    exit(0);
}

void _arguments_usage()
{
    printf("usage: compiler [option]\n");
    printf("option: \n");
    printf("  -i <file> \tinput cmm file path, default a.cmm.\n");
    printf("  -o <file> \toutput asm file path, default a.out.\n");
    printf("  -r <file> \texecute asm file, default a.out.\n");
    printf("  -h \t\tshow this help message and exit.\n");

    exit(0);
}

static void _token_print(token *t)
{
    assert(t != NULL);

    printf("\t\t\tline_no: %d,\n\t\t\ttoken_str: \"%s\",\n\t\t\ttoken_len: %d,\n\t\t\ttoken_type: %d\n",
    t->line_no, t->token_str, t->token_len, t->token_type);
}

int arguments_init(int argc, char **argv, parse_handler *handler)
{
    int option;
    while ((option = getopt(argc, (char **)argv, "ho:i:r:")) != EOF) {
        switch (option) {
            case 'o':
                strncpy(handler->asm_file, optarg, FILE_PATH_MAX);
                break;
            case 'i':
                strncpy(handler->cmm_file, optarg, FILE_PATH_MAX);
                break;
            case 'r':
                strncpy(handler->asm_file, optarg, FILE_PATH_MAX);
                break;
            case 'h':
            default:
                _arguments_usage();
                break;
        }
    }

    memset(handler, 0, sizeof(parse_handler));

    if (strlen(handler->cmm_file) == 0) {
        strcpy(handler->cmm_file, "a.cmm");
    }
    if (strlen(handler->asm_file) == 0) {
        strcpy(handler->asm_file, "a.out");
    }

    return CMM_SUCCESS;
}

void token_print(const token_list *tokens, int token_idx)
{
    assert(tokens != NULL);

    token *t = tokens->data[token_idx];
    printf("%d: { line_no: %d, token_str: \"%s\", token_len: %d, token_type: %d }\n", 
        token_idx, t->line_no, t->token_str, 
        t->token_len, t->token_type);
}

token *create_token(const char *str, int type)
{
    token *t = (token *)malloc(sizeof(token));
    if (t == NULL) {
        return NULL;
    }

    memset(t, 0, sizeof(token));
    t->token_type = type;
    t->token_len = 0;
    t->line_no = 0;

    if (str != NULL) {
        t->token_len = MIN(TOKEN_STR_MAX, strlen(str));
        strncpy(t->token_str, str, t->token_len);
    }
    return t;
}

int token_list_init(token_list *tokens)
{
    assert(tokens != NULL);

    int idx;
    tokens->data = (token **)malloc(TOKEN_LIST_MAX * sizeof(token *));
    if (tokens->data == NULL) {
        printf("tokens->data is NULL.\n");
        return CMM_FAILED;
    }

    for (idx = 0; idx < tokens->capacity; ++idx) {
        tokens->data[idx] = (token *)malloc(sizeof(token));
        if (tokens->data[idx] == NULL) {
            printf("tokens->data[idx] is NULL.\n");
            return CMM_FAILED;
        }
    }

    tokens->capacity = TOKEN_LIST_MAX;
    tokens->size = 0;

    return CMM_SUCCESS;
}

int token_list_push(token_list *tl, token *t)
{
    assert(tl != NULL && t != NULL);

    tl->data[tl->size++] = t;
    if (tl->size >= tl->capacity) {
        printf("insufficient free space, please expand.\n");
        // todo
        return CMM_FAILED;
    }

    return CMM_SUCCESS;
}

void token_list_print(const token_list *tl)
{
    assert(tl != NULL);

    int idx;
    printf("token_list: [\n");
    for (idx = 0; idx < tl->size; ++idx) {
        printf("\t{\n\t\tidx: %d,\n\t\ttoken: {\n", idx);
        _token_print(tl->data[idx]);
        printf("\t\t}\n\t}");
        if (idx != tl->size - 1) {
            printf(",");
        }
        printf("\n");
    }

    printf("]\n");
}

void token_push_char(token *t, char ch)
{
    assert(t != NULL);

    if (t->token_len >= TOKEN_STR_MAX) {
        printf("token length is too long for token_str array, max length: %d.\n", TOKEN_STR_MAX);
        exit(0);
    }

    t->token_str[t->token_len++] = ch;
}

syntax_tree *create_ast_node(token *t)
{
    syntax_tree *node = (syntax_tree *)malloc(sizeof(syntax_tree));
    memset(node, 0, sizeof(syntax_tree));
    node->data = t;
    node->sub_list = (syntax_tree **)malloc(AST_LIST_MAX * sizeof(syntax_tree *));
    node->sub_idx = 0;

    return node;
}

static void _ast_print_node(const syntax_tree *node, int deep)
{
    assert(node != NULL);
    int tab_size = deep;
    while (tab_size-- > 0) {
        printf("   ");
    }
    printf("deep: %d, sub_size: %d, token: ", deep, node->sub_idx);
    printf("{ line_no: %d, token_str: \"%s\", token_len: %d, token_type: %d }\n",
        node->data->line_no, node->data->token_str,
        node->data->token_len, node->data->token_type);
}

static void _syntax_tree_print(const syntax_tree *node, int deep)
{
    int si;
    if (node != NULL) {
        _ast_print_node(node, deep);
        for (si = 0; si < node->sub_idx; ++si) {
            if (node->sub_list[si] != NULL) {
                _syntax_tree_print(node->sub_list[si], deep + 1);
            }
        }
    }
}

void syntax_tree_print(const syntax_tree *ast)
{
    int deep = 0;
    _syntax_tree_print(ast, deep);
}

symbol *create_symbol(const char *var_name, int var_idx, int var_size)
{
    assert(var_name != NULL);

    symbol *s = (symbol *)malloc(sizeof(symbol));
    memset(s, 0, sizeof(symbol));
    strncpy(s->var_name, var_name, MIN(VAR_NAME_MAX, strlen(var_name)));
    s->var_idx = var_idx;
    s->var_size = var_size;

    return s;
}

symbol_space *create_symbol_space(const char* space_name)
{
    assert(space_name != NULL);

    symbol_space *space = (symbol_space *)malloc(sizeof(symbol_space));
    memset(space, 0, sizeof(symbol_space));
    strncpy(space->space_name, space_name, MIN(FUNC_NAME_MAX, strlen(space_name)));
    space->s_idx = 0;
    space->s = (symbol **)malloc(TABLE_SYMBOL_MAX * sizeof(symbol *));

    return space;
}

symbol_space *get_global_space(const symbol_table *table)
{
    assert(table != NULL && (table->ss_idx > 0));
    return table->ss[0];
}

symbol_space *get_symbol_space(symbol_table **table, const char *space_name)
{
    assert(table != NULL && (*table) != NULL && space_name != NULL);
    int ss_idx;
    
    for (ss_idx = 0; ss_idx < (*table)->ss_idx; ++ss_idx) {
        if (strcmp(space_name, (*table)->ss[ss_idx]->space_name) == 0) {
            return (*table)->ss[ss_idx];
        }
    }

    (*table)->ss[(*table)->ss_idx] = create_symbol_space(space_name);

    return (*table)->ss[(*table)->ss_idx++];
}

symbol_table *create_symbol_table()
{
    symbol_table *table = (symbol_table *)malloc(sizeof(symbol_table));
    memset(table, 0, sizeof(symbol_table));
    table->ss_idx = 0;
    table->ss = (symbol_space **)malloc(TABLE_SPACE_MAX * sizeof(symbol_space *));
    table->ss[table->ss_idx++] = create_symbol_space(NAMESPACE_GLOBAL);
    return table;
}

void symbol_table_print(const symbol_table *table)
{
    assert(table != NULL);
    
    int table_idx, space_idx;
    for (table_idx = 0; table_idx < table->ss_idx; ++table_idx) {
        printf("space_name: %s, size: %d\n", table->ss[table_idx]->space_name, table->ss[table_idx]->s_idx);
        
        for (space_idx = 0; space_idx < table->ss[table_idx]->s_idx; ++space_idx) {
            printf("\tvar_idx: %d, var_size: %d, var_name: %s\n", 
                table->ss[table_idx]->s[space_idx]->var_idx,
                table->ss[table_idx]->s[space_idx]->var_size,
                table->ss[table_idx]->s[space_idx]->var_name);
        }
    }
}

code_map *create_code_map(const char *map_name)
{
    assert(map_name != NULL);

    code_map *maps = (code_map *)malloc(sizeof(code_map));
    memset(maps, 0, sizeof(code_map));

    strncpy(maps->map_name, map_name, MIN(MAP_NAME_MAX, strlen(map_name)));
    maps->c_idx = 0;
    maps->c = (code **)malloc(MASP_CODE_MAX * sizeof(code *));

    return maps;
}

unordered_code_map *create_unordered_code_map()
{
    unordered_code_map *ucm = (unordered_code_map *)malloc(sizeof(unordered_code_map));
    memset(ucm, 0, sizeof(unordered_code_map));
    ucm->m_idx = 0;
    ucm->maps = (code_map **)malloc(CODE_MAPS_MAX * sizeof(code_map *));

    ucm->maps[ucm->m_idx++] = create_code_map(NAMESPACE_GLOBAL);
    return ucm;
}
