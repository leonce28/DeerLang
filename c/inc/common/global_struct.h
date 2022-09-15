#pragma once

#include <stdio.h>
#include "global_macro.h"


typedef struct _parse_handler {
    char input_cmm_path[FILE_PATH_MAX];
    char input_asm_path[FILE_PATH_MAX];
    char output_asm_path[FILE_PATH_MAX];
} parse_handler;