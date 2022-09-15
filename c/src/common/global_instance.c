#include "common/global_instance.h"
#include "kits/vector.h"

int get_parse_handler_instance(parse_handler **ph)
{
    *ph = (parse_handler *)malloc(sizeof(parse_handler));
    if (!*ph) {
        return CMM_GET_INSTANCE_FAILED;
    }

    memset(*ph, 0, sizeof(parse_handler));
    return CMM_SUCCESS;
}

int get_token_list_instance(vector *vc)
{
    *vc = vector_create(sizeof(token));
    if (!*vc) {
        return CMM_GET_INSTANCE_FAILED;
    }

    return CMM_SUCCESS;
}
