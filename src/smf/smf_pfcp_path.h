#ifndef __SMF_PFCP_PATH_H__
#define __SMF_PFCP_PATH_H__

#include "pfcp/pfcp_xact.h"
#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) smf_pfcp_open();
CORE_DECLARE(status_t) smf_pfcp_close();

CORE_DECLARE(status_t) smf_pfcp_send_heartbeat_request(pfcp_node_t *pnode);
CORE_DECLARE(status_t) smf_pfcp_send_association_setup_request(pfcp_node_t *pnode);
CORE_DECLARE(status_t) smf_pfcp_send_association_update_request(pfcp_node_t *pnode);
CORE_DECLARE(status_t) smf_pfcp_send_association_release_request(pfcp_node_t *pnode);

CORE_DECLARE(status_t) smf_pfcp_send_session_establishment_request(smf_sess_t *sess);
CORE_DECLARE(status_t) smf_pfcp_send_an_release_session_modification_request(smf_sess_t *sess);
CORE_DECLARE(status_t) smf_pfcp_send_setup_donwlink_session_modification_request(smf_sess_t *sess);
CORE_DECLARE(status_t) smf_pfcp_send_session_deletion_request(smf_sess_t *sess);
CORE_DECLARE(status_t) smf_pfcp_send_session_report_response(smf_sess_t *sess);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_PFCP_PATH_H__ */
