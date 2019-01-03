#ifndef __S1AP_BUILD_H__
#define __S1AP_BUILD_H__

#include "s1ap/s1ap_message.h"
#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) s1ap_build_overload_stop(pkbuf_t **pkbuf);
CORE_DECLARE(status_t) s1ap_build_reroute_nas_request(
    pkbuf_t **s1apbuf, enb_ue_t *enb_ue);
CORE_DECLARE(status_t) s1ap_build_ue_radio_capability_match_request(
    pkbuf_t **s1apbuf, enb_ue_t *enb_ue);
CORE_DECLARE(status_t) s1ap_build_e_rab_modification_confirm(
            pkbuf_t **s1apbuf, enb_ue_t *enb_ue);

CORE_DECLARE(status_t) s1ap_build_setup_rsp(pkbuf_t **pkbuf);

CORE_DECLARE(status_t) s1ap_build_overload_start(pkbuf_t **pkbuf);

CORE_DECLARE(status_t) s1ap_build_enb_configuration_update_acknowledge(pkbuf_t **pkbuf);
CORE_DECLARE(status_t) s1ap_build_enb_configuration_update_failure(
    pkbuf_t **pkbuf, S1AP_Cause_PR group, long cause, long time_to_wait);

CORE_DECLARE(status_t) s1ap_build_write_replace_warning_request(
    pkbuf_t **pkbuf);
CORE_DECLARE(status_t) s1ap_build_kill_request(
    pkbuf_t **pkbuf);

CORE_DECLARE(status_t) s1ap_build_ue_context_suspend_response(
        pkbuf_t **pkbuf,
	S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id);
CORE_DECLARE(status_t) s1ap_build_ERAB_release_command(pkbuf_t **s1apbuf,
    c_uint32_t enb_ue_s1ap_id, c_uint32_t mme_ue_s1ap_id);

CORE_DECLARE(status_t) s1ap_build_write_replace_warning_request(pkbuf_t **pkbuf);
CORE_DECLARE(status_t) s1ap_build_kill_request(pkbuf_t **pkbuf);

CORE_DECLARE(status_t) s1ap_build_mme_configuration_update(pkbuf_t **pkbuf);
CORE_DECLARE(status_t) s1ap_build_mme_cp_relocation_indication(
        pkbuf_t **s1apbuf, enb_ue_t *source_ue);

CORE_DECLARE(status_t) s1ap_build_setup_failure(
    pkbuf_t **pkbuf, S1AP_Cause_PR group, long cause, long time_to_wait);
CORE_DECLARE(status_t) s1ap_build_downlink_nas_transport(
    pkbuf_t **s1apbuf, enb_ue_t *enb_ue, pkbuf_t *emmbuf);
CORE_DECLARE(status_t) s1ap_build_initial_context_setup_request(
    pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue, pkbuf_t *emmbuf);
CORE_DECLARE(status_t) s1ap_build_e_rab_setup_request(
    pkbuf_t **s1apbuf, amf4g_bearer_t *bearer, pkbuf_t *esmbuf);
CORE_DECLARE(status_t) s1ap_build_e_rab_modify_request(
    pkbuf_t **s1apbuf, amf4g_bearer_t *bearer, pkbuf_t *esmbuf);
CORE_DECLARE(status_t) s1ap_build_e_rab_release_command(pkbuf_t **s1apbuf,
    amf4g_bearer_t *bearer, pkbuf_t *esmbuf, S1AP_Cause_PR group, long cause);
CORE_DECLARE(status_t) s1ap_build_ue_context_release_command(
    pkbuf_t **s1apbuf, enb_ue_t *enb_ue, S1AP_Cause_PR group, long cause);
CORE_DECLARE(status_t) s1ap_build_ue_context_resume_response(
    pkbuf_t **s1apbuf, 
    S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
    S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id);
CORE_DECLARE(status_t) s1ap_build_ue_context_resume_failure(
    pkbuf_t **s1apbuf, 
    S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
    S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
    S1AP_Cause_t *cause);
CORE_DECLARE(status_t) s1ap_build_paging(pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue);

CORE_DECLARE(status_t) s1ap_build_mme_configuration_transfer(
        pkbuf_t **s1apbuf, 
        S1AP_SONConfigurationTransfer_t *son_configuration_transfer);

CORE_DECLARE(status_t) s1ap_build_mme_direct_information_transfer(
    pkbuf_t **s1apbuf);

CORE_DECLARE(status_t) s1ap_build_path_switch_ack(
    pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue);
CORE_DECLARE(status_t) s1ap_build_path_switch_failure(pkbuf_t **s1apbuf,
    c_uint32_t enb_ue_s1ap_id, c_uint32_t mme_ue_s1ap_id,
    S1AP_Cause_PR group, long cause);

CORE_DECLARE(status_t) s1ap_build_handover_command(
        pkbuf_t **s1apbuf, enb_ue_t *source_ue);
CORE_DECLARE(status_t) s1ap_build_handover_preparation_failure(
        pkbuf_t **s1apbuf, enb_ue_t *source_ue, S1AP_Cause_t *cause);

CORE_DECLARE(status_t) s1ap_build_handover_request(
        pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue, enb_ue_t *target_ue,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_HandoverType_t *handovertype,
        S1AP_Cause_t *cause,
        S1AP_Source_ToTarget_TransparentContainer_t
            *source_totarget_transparentContainer);

CORE_DECLARE(status_t) s1ap_build_handover_cancel_ack(
        pkbuf_t **s1apbuf, enb_ue_t *source_ue);

CORE_DECLARE(status_t) s1ap_build_mme_status_transfer(pkbuf_t **s1apbuf,
        enb_ue_t *target_ue,
        S1AP_ENB_StatusTransfer_TransparentContainer_t
            *enb_statustransfer_transparentContainer);

CORE_DECLARE(status_t) s1ap_build_error_indication(
        pkbuf_t **s1apbuf,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
        S1AP_Cause_PR group, long cause);

CORE_DECLARE(status_t) s1ap_build_s1_reset(
        pkbuf_t **s1apbuf,
        S1AP_Cause_PR group, long cause,
        S1AP_UE_associatedLogicalS1_ConnectionListRes_t *partOfS1_Interface);

CORE_DECLARE(status_t) s1ap_build_s1_reset_partial(
        pkbuf_t **s1apbuf,
        S1AP_Cause_PR group, long cause,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id);

CORE_DECLARE(status_t) s1ap_build_s1_reset_ack(
        pkbuf_t **s1apbuf,
        S1AP_UE_associatedLogicalS1_ConnectionListRes_t *partOfS1_Interface);

CORE_DECLARE(status_t) s1ap_build_connection_establishment_indication(
        pkbuf_t **s1apbuf,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
	amf4g_ue_t *amf4g_ue);

CORE_DECLARE(status_t) s1ap_build_ue_information_transfer(
	pkbuf_t **s1apbuf, 
	amf4g_ue_t *amf4g_ue, 
	S1AP_S_TMSI_t *S_TMSI);

CORE_DECLARE(status_t) s1ap_build_ue_context_modification_request(
    pkbuf_t **s1apbuf,
    enb_ue_t *enb_ue);

CORE_DECLARE(status_t) s1ap_build_ue_context_modification_confirm(
	pkbuf_t **s1apbuf,
    enb_ue_t *enb_ue);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __S1AP_BUILD_H__ */
