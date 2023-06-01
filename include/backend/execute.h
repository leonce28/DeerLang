#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "common/struct.h"
#include "common/funcs.h"

int load_code_segment(char *data, code_segment **cs_p);
int vm_execute(const code_segment *cs);