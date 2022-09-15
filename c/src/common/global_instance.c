#include "common/global_instance.h"

int get_parse_handler_instance(parse_handler **ph)
{
    *ph = (parse_handler *)malloc(sizeof(parse_handler));
    if (!*ph) {
        return CMM_GET_INSTANCE_FAILED;
    }

    memset(*ph, 0, sizeof(parse_handler));
    return CMM_GET_INSTANCE_SUCCESS;
}