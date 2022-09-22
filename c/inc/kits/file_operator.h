#pragma once
#include "common/global_struct.h"

int read_code_string(const char *cmm_path, char **str);

int generate_asm(const char *cmm_file, char *asm_file);

int execute_code(const char *asm_file);