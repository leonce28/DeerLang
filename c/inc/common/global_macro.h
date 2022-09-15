#pragma once

#define FILE_PATH_MAX   512

enum {
    CMM_INIT_ARGUMENT_SUCCESS,  // init argument success
    CMM_GENERATE_CODE_SUCCESS,  // generate code list success
    CMM_EXECUTE_CODE_SUCCESS,   // generate binary file success
    CMM_GET_INSTANCE_SUCCESS,   // get instance success
    CMM_GET_INSTANCE_FAILED,    // get instance failed
};