#include <cJSON/cJSON.h>

#include "core_debug.h"
#include "core_pkbuf.h"
#include "3gpp_types.h"
#include "smf_json_handler.h"

status_t smf_json_handler_create_session(pkbuf_t **pkbuf, create_session_t *createSession)
{ 
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    // string = cJSON_Print(session);
    // d_info(string);
    d_assert(session, return CORE_ERROR, "Null param");

    JSONTRANSFORM_JsToSt_create_session_request(createSession, session);
    d_print_hex(createSession->ue_pco.buffer, createSession->ue_pco.length);
    
    cJSON_Delete(session);
    return CORE_OK;
}

status_t smf_json_handler_update_session(pkbuf_t **pkbuf, modify_bearer_t *modifyBearer)
{ 
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    // string = cJSON_Print(session);
    // d_info(string);
    d_assert(session, return CORE_ERROR, "Null param");

    JSONTRANSFORM_JsToSt_modify_bearer_request(modifyBearer, session);
    
    cJSON_Delete(session);
    return CORE_OK;
}
