#pragma once

#include <stdio.h>
#include "global_macro.h"


typedef struct _parse_handler {
    char input_file_path[FILE_PATH_MAX];
    char output_file_path[FILE_PATH_MAX];
} parse_handler;