#pragma once
#include "common/global_struct.h"

int token_list_init(token_list *tl);
int token_list_push(void *arg1, void *arg2);
int token_list_print(void *arg1);

token *create_token(int no, char *str, int type);
