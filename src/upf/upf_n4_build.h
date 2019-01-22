#ifndef __UPF_N4_BUILD_H__
#define __UPF_N4_BUILD_H__

#include "pfcp/pfcp_message.h"
//#include "fd/gx/gx_message.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


CORE_DECLARE(status_t) upf_n4_build_session_establishment_response(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess,
        pfcp_session_establishment_request_t *pfcp_req);
        
CORE_DECLARE(status_t) upf_n4_build_session_modification_response(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess,
        pfcp_session_modification_request_t *pfcp_req);
        
CORE_DECLARE(status_t) upf_n4_build_session_deletion_response(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess,
        pfcp_session_deletion_request_t *pfcp_req);

CORE_DECLARE(status_t) upf_n4_build_session_report_request_downlink_data_report(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess, c_int16_t pdr_id, 
        pfcp_session_report_request_t *pfcp_req);

CORE_DECLARE(status_t) upf_n4_build_association_setup_response(
        pkbuf_t **pkbuf, c_uint8_t type);

CORE_DECLARE(status_t) upf_n4_build_association_release_response(
        pkbuf_t **pkbuf, c_uint8_t type);

CORE_DECLARE(status_t) upf_n4_build_heartbeat_response(
        pkbuf_t **pkbuf, c_uint8_t type);
        
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UPF_N4_BUILD_H__ */
