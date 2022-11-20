#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common/struct.h"
#include "common/funcs.h"
#include "frontend/lexical.h"
#include "frontend/syntax.h"
#include "backend/semantic.h"
#include "backend/generator.h"
#include "backend/execute.h"

static compiler_handle *handle;

void compiler_init(const int argc, char **argv);
void compiler_run();
