#ifndef __AMF_N11_HANDLER_H__
#define __AMF_N11_HANDLER_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void)  amf_n11_handle_create_session_response(
        amf4g_ue_t *amf4g_ue, create_session_t *pCreateSession);

CORE_DECLARE(void) amf_n11_handle_modify_bearer_response(amf4g_ue_t *amf4g_ue, modify_bearer_t *modifyBearer);
CORE_DECLARE(void) amf_n11_handle_delete_session_response(amf4g_ue_t *amf4g_ue, delete_session_t *deleteSession);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF_N11_HANDLER_H__ */
