#ifndef __NGAP_PATH_H__
#define __NGAP_PATH_H__

#include "core_pkbuf.h"

#include "amf4g_context.h"
#include "ngap/ngap_message.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NGAP_NON_UE_SIGNALLING   0

CORE_DECLARE(status_t) ngap_init(int sctp_streams, c_uint16_t port);
CORE_DECLARE(status_t) ngap_final();

CORE_DECLARE(status_t) ngap_open();
CORE_DECLARE(status_t) ngap_close();

CORE_DECLARE(status_t) ngap_server(sock_node_t *snode, int type);
CORE_DECLARE(status_t) ngap_delete(sock_id sock);

CORE_DECLARE(int) ngap_recv_handler(sock_id sock, void *data);

CORE_DECLARE(status_t) ngap_send(sock_id sock,
        pkbuf_t *pkbuf, c_sockaddr_t *addr, c_uint16_t stream_no);
CORE_DECLARE(status_t) ngap_recv(sock_id id, pkbuf_t *pkbuf);

CORE_DECLARE(status_t) ngap_send_to_ran(
        amf_ran_t *ran, pkbuf_t *pkb, c_uint16_t stream_no);
CORE_DECLARE(status_t) ngap_send_to_ran_ue(ran_ue_t *ran_ue, pkbuf_t *pkbuf);
CORE_DECLARE(status_t) ngap_delayed_send_to_ran_ue(ran_ue_t *ran_ue,
        pkbuf_t *pkbuf, c_uint32_t duration);

CORE_DECLARE(status_t) ngap_send_handover_cancel_acknowledge(ran_ue_t *source_ue);
CORE_DECLARE(status_t) ngap_send_handover_preparation_failure(ran_ue_t *source_ue, NGAP_Cause_t *cause);
CORE_DECLARE(status_t)  ngap_send_handover_command(ran_ue_t *source_ue);
CORE_DECLARE(status_t) ngap_send_handover_request(
        amf_ue_t *amf_ue,
        amf_ran_t *target_ran,
        NGAP_RAN_UE_NGAP_ID_t *ran_ue_ngap_id,
        NGAP_AMF_UE_NGAP_ID_t *amf_ue_ngap_id,
        NGAP_HandoverType_t *handovertype,
        NGAP_Cause_t *cause,
        NGAP_SourceToTarget_TransparentContainer_t *SourceToTarget_TransparentContainer);

CORE_DECLARE(status_t) ngap_send_ue_context_release_command(ran_ue_t *ran_ue, NGAP_Cause_PR group, long cause, c_uint8_t action, c_uint32_t delay);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__S1_PATH_H__ */
