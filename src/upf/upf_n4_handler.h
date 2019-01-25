#ifndef __UPF_N4_HANDLER_H__
#define __UPF_N4_HANDLER_H__

#include "pfcp/pfcp_message.h"
#include "pfcp/pfcp_xact.h"

#include "upf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) upf_n4_handle_session_establishment_request(
        upf_sess_t *sess, pfcp_xact_t *p_xact, pfcp_session_establishment_request_t *req);
CORE_DECLARE(void) upf_n4_handle_session_modification_request(
        upf_sess_t *sess, pfcp_xact_t *xact, pfcp_session_modification_request_t *req);
CORE_DECLARE(void) upf_n4_handle_session_deletion_request(
        upf_sess_t *sess, pfcp_xact_t *xact, pfcp_session_deletion_request_t *req);
CORE_DECLARE(void) upf_n4_handle_session_report_response(
        upf_sess_t *sess, pfcp_xact_t *xact, pfcp_session_report_response_t *req);
CORE_DECLARE(void) upf_n4_handle_association_setup_request(
        pfcp_xact_t *xact, pfcp_association_setup_request_t *req);
CORE_DECLARE(void) upf_n4_handle_association_update_request(
        pfcp_xact_t *xact, pfcp_association_update_request_t *req);
CORE_DECLARE(void) upf_n4_handle_association_release_request(
        pfcp_xact_t *xact, pfcp_association_release_request_t *req); 
CORE_DECLARE(void) upf_n4_handle_heartbeat_request(
        pfcp_xact_t *xact, pfcp_heartbeat_request_t *req);      
CORE_DECLARE(void) upf_n4_handle_heartbeat_response(
        pfcp_xact_t *xact, pfcp_heartbeat_response_t *rsp);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UPF_N4_HANDLER_H__ */
