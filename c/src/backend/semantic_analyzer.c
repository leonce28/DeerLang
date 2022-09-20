#include <stdlib.h>
#include "backend/semantic_analyzer.h"
#include "common/global_funcs.h"


int semantic_analysis(const syntax_tree *ast, symbol_table **st)
{
    int global_idx, space_idx, var_idx, var_size = 0;
    int dl_idx, p_idx, vd_idx;
    const char *space_name;
    const syntax_tree *declared, *declared_list, *func_declared, *params, *param, 
    *compound_stmt, *local_decl, *var_decl, *var_id;
    symbol_space *space;
    symbol_table *table = create_symbol_table();
    table->ss[table->ss_idx++] = create_symbol_space("__GLOBAL__");

    // declared_list ::= declared { declared }
    for (dl_idx = 0, declared_list = ast; dl_idx < declared_list->sub_idx; ++dl_idx) {

        // declared ::= var_declared | func_declared
        declared = declared_list->sub_list[dl_idx];
        printf("deep 1: %s\n", declared->data->token_str);
        if (declared->data->token_type == TOKEN_FUNC_DECL) {

            // func_declared ::= type id '(' params ')' compound_stmt
            func_declared = declared;
            space_name = func_declared->sub_list[1]->data->token_str;
            space = create_symbol_space(space_name);
            space_idx = table->ss_idx++;
            table->ss[space_idx] = space;
            var_idx = 0;

            // params ::= [ param_list ] 
            params = func_declared->sub_list[2];
            if (params != NULL) {
                for (p_idx = 0; p_idx < params->sub_idx; ++p_idx) {

                    // param ::= type id [ '[' ']' ]
                    param = params->sub_list[p_idx];
                    space->s[space->s_idx++] = create_symbol(param->sub_list[1]->data->token_str, var_idx++, 0);
                    printf("\tdeep 2 param: %s\n", param->sub_list[1]->data->token_str);
                }
            }

            // compound_stmt ::= '{' local_decl stmt_list '}'
            compound_stmt = func_declared->sub_list[3];

            // local_decl :: = { var_decl }
            for (vd_idx = 0, local_decl = compound_stmt->sub_list[0]; vd_idx < local_decl->sub_idx; ++vd_idx) {

                // var_decl ::= type id [ '[' number ']' ] ';'
                var_decl = local_decl->sub_list[vd_idx];

                var_id = var_decl->sub_list[1];
                var_size = var_decl->sub_list[2] != NULL ? atoi(var_decl->sub_list[2]->data->token_str) : 0;
                space->s[space->s_idx++] = create_symbol(var_id->data->token_str, var_idx, var_size);

                printf("\tdeep 2 var_id: %s\n", var_id->data->token_str);

                var_idx += var_size + 1;
            }

            // stmt_list todo
 
        } else {
            // var_decl ::= type id [ '[' number ']' ] ';'
            var_decl = declared;

            var_size = var_decl->sub_list[2] != NULL ? atoi(var_decl->sub_list[2]->data->token_str) : 0;
            table->ss[0]->s[table->ss[0]->s_idx++] = create_symbol(var_id->data->token_str, global_idx, var_size);

            global_idx += var_size + 1;
        }
    }

    *st = table;
    return CMM_SUCCESS;
}