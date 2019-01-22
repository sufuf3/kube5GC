#ifndef __SMF_N4_BUILD_H__
#define __SMF_M4_BUILD_H__

#include "pfcp/pfcp_message.h"

#include "smf_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) smf_n4_build_heartbeat_request(
        pkbuf_t **pkbuf);
CORE_DECLARE(status_t) smf_n4_build_heartbeat_response(
        pkbuf_t **pkbuf);
CORE_DECLARE(status_t) smf_n4_build_association_setup_request(
        pkbuf_t **pkbuf);
CORE_DECLARE(status_t) smf_n4_build_association_update_request(
        pkbuf_t **pkbuf);
CORE_DECLARE(status_t) smf_n4_build_association_update_response(
        pkbuf_t **pkbuf, c_uint8_t cause);
CORE_DECLARE(status_t) smf_n4_build_association_release_request(
        pkbuf_t **pkbuf);
CORE_DECLARE(status_t) smf_n4_build_association_release_response(
        pkbuf_t **pkbuf, c_uint8_t cause);

CORE_DECLARE(status_t) smf_n4_build_session_establishment_request(
        pkbuf_t **pkbuf, smf_sess_t *sess);
CORE_DECLARE(status_t) smf_n4_build_session_modification_request_for_setup_downlink(
        pkbuf_t **pkbuf, smf_sess_t *sess);
CORE_DECLARE(status_t) smf_n4_build_session_modification_request_for_an_release(
        pkbuf_t **pkbuf, smf_sess_t *sess);
CORE_DECLARE(status_t) smf_n4_build_session_deletion_request(
        pkbuf_t **pkbuf, smf_sess_t *sess);
CORE_DECLARE(status_t) smf_n4_build_session_report_response(
        pkbuf_t **pkbuf, smf_sess_t *sess);

CORE_DECLARE(status_t) smf_n4_build_create_pdr(
        tlv_create_pdr_t *create_pdr, smf_pdr_t *, smf_sess_t *sess);
CORE_DECLARE(status_t) smf_n4_free_create_pdr(
        tlv_create_pdr_t *create_pdr);
CORE_DECLARE(status_t) smf_n4_build_create_far(
        tlv_create_far_t *create_far, smf_far_t *far, smf_sess_t *sess);
CORE_DECLARE(status_t) smf_n4_free_create_far(
        tlv_create_far_t *create_far);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_N$_BUILD_H__ */
