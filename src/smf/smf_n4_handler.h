#ifndef __SMF_N4_HANDLER_H__
#define __SMF_N4_HANDLER_H__

#include "pfcp/pfcp_message.h"
#include "pfcp/pfcp_xact.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) smf_n4_handle_heartbeat_request(
        pfcp_xact_t *xact, pfcp_heartbeat_request_t *req);
CORE_DECLARE(void) smf_n4_handle_heartbeat_response(
        pfcp_xact_t *xact, pfcp_heartbeat_response_t *rsp);

CORE_DECLARE(void) smf_n4_handle_association_setup_response(
        pfcp_xact_t *xact, pfcp_association_setup_response_t *rsp);
CORE_DECLARE(void) smf_n4_handle_association_update_request(
        pfcp_xact_t *xact, pfcp_association_update_request_t *rsp);
CORE_DECLARE(void) smf_n4_handle_association_update_response(
        pfcp_xact_t *xact, pfcp_association_update_response_t *rsp);
CORE_DECLARE(void) smf_n4_handle_association_release_request(
        pfcp_xact_t *xact, pfcp_association_release_request_t *rsp);
CORE_DECLARE(void) smf_n4_handle_association_release_response(
        pfcp_xact_t *xact, pfcp_association_release_response_t *rsp);
CORE_DECLARE(void) smf_n4_handle_node_report_request(
        pfcp_xact_t* xact, pfcp_node_report_request_t *req);

CORE_DECLARE(void) smf_n4_handle_session_establishment_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_establishment_response_t *rsp);
CORE_DECLARE(void) smf_n4_handle_session_modification_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_modification_response_t *rsp);
CORE_DECLARE(void) smf_n4_handle_session_deletion_response(
        pfcp_xact_t *xact, smf_sess_t *sess, pfcp_session_deletion_response_t *rsp);
CORE_DECLARE(void) smf_n4_handle_session_report_request(
        pfcp_xact_t* xact, smf_sess_t *sess, pfcp_session_report_request_t *req);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_N4_HANDLER_H__ */
