#include <cJSON/cJSON.h>

#include "core_debug.h"
#include "core_pkbuf.h"
#include "3gpp_types.h"
#include "smf_json_handler.h"

status_t smf_json_handler_create_session(pkbuf_t **pkbuf, creat_session_t *createSession)
{ 
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    // string = cJSON_Print(session);
    // d_info(string);
    d_assert(session, return CORE_ERROR, "Null param");

    JSONTRANSFORM_JsToSt_create_session_request(createSession, session);
    
    cJSON_Delete(session);
    return CORE_OK;
}