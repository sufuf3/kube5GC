#ifndef __NGAP_BUILD_H__
#define __NGAP_BUILD_H__

#include "ngap/ngap_message.h"
#include "mme_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) ngap_build_setup_rsp(pkbuf_t **pkbuf);
CORE_DECLARE(status_t) ngap_build_setup_failure(pkbuf_t **pkbuf, NGAP_Cause_PR group, long cause, long time_to_wait);

CORE_DECLARE(status_t) ngap_build_initial_context_setup_request(
            pkbuf_t **ngapbuf, amf_ue_t *amf_ue, pkbuf_t *naspdubuf);

CORE_DECLARE(status_t) ngap_build_pdu_session_resource_setup_request(pkbuf_t **ngapbuf);
CORE_DECLARE(status_t) ngap_build_pdu_session_resource_modify_request(pkbuf_t **ngapbuf, amf_ue_t *amf_ue, ngap_message_t *message);
CORE_DECLARE(status_t) ngap_build_pdu_session_resource_modify_confirm(pkbuf_t **ngapbuf, ran_ue_t *source_ue);
CORE_DECLARE(status_t) ngap_build_pdu_session_resource_release_command(pkbuf_t **ngapbuf, ran_ue_t *source_ue, NGAP_Cause_PR group, long cause);

CORE_DECLARE(status_t) ngap_build_ue_context_modification_request(pkbuf_t **ngapbuf, ran_ue_t *ran_ue);

CORE_DECLARE(status_t) ngap_build_ue_context_release_command(pkbuf_t **ngapbuf, ran_ue_t *ran_ue, NGAP_Cause_PR group, long cause);

CORE_DECLARE(status_t) ngap_build_ng_reset(pkbuf_t **ngapbuf, NGAP_Cause_PR group, long cause, NGAP_UE_associatedLogicalNG_ConnectionListRes_t *partOfNG_Interface);

CORE_DECLARE(status_t) ngap_build_handover_cancel_acknowledge(pkbuf_t **ngapbuf, ran_ue_t *source_ue);
CORE_DECLARE(status_t) ngap_build_handover_preparation_failure(pkbuf_t **ngapbuf, ran_ue_t *source_ue, NGAP_Cause_t *cause);
CORE_DECLARE(status_t) ngap_build_handover_command(pkbuf_t **ngapbuf, ran_ue_t *source_ue);
CORE_DECLARE(status_t) ngap_build_handover_request(
            pkbuf_t **ngapbuf, amf_ue_t *amf_ue, ran_ue_t *target_ue,
            NGAP_RAN_UE_NGAP_ID_t *ran_ue_ngap_id, NGAP_AMF_UE_NGAP_ID_t *amf_ue_ngap_id,
            NGAP_HandoverType_t *handovertype, NGAP_Cause_t *cause,
            NGAP_SourceToTarget_TransparentContainer_t *sourceToTarget_TransparentContainer);

CORE_DECLARE(status_t) ngap_build_path_switch_ack(pkbuf_t **ngapbuf, amf_ue_t *amf_ue);
CORE_DECLARE(status_t) ngap_build_path_switch_failure(pkbuf_t **ngapbuf,
    c_uint32_t ran_ue_ngap_id, c_uint32_t amf_ue_ngap_id,
    NGAP_Cause_PR group, long cause);

CORE_DECLARE(status_t) ngap_build_amf_status_indication(pkbuf_t **ngapbuf, ran_ue_t *ran_ue);
CORE_DECLARE(status_t) ngap_build_amf_configuration_update(pkbuf_t **pkbuf);
CORE_DECLARE(status_t) ngap_build_ran_configuration_update_acknowledge(pkbuf_t **pkbuf);
CORE_DECLARE(status_t) ngap_build_ran_configuration_update_failure(pkbuf_t **pkbuf, NGAP_Cause_PR group, long cause, long time_to_wait);

CORE_DECLARE(status_t) ngap_build_ue_radio_capability_check_request(pkbuf_t **ngapbuf, ran_ue_t *source_ue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NGAP_BUILD_H__ */
