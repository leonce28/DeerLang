#pragma once
#include "common/global_struct.h"
#include "kits/token_list.h"

int lexical_analysis(const char *cmm_file_path, token_list *tokens);