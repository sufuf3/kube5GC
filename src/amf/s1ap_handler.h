#ifndef __S1AP_HANDLER_H__
#define __S1AP_HANDLER_H__

#include "s1ap/s1ap_message.h"

#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) s1ap_handle_ue_radio_capability_match_response(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_e_rab_modification_indication(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_nas_delivery_indication(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_nas_non_delivery_indication(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_eNB_configuration_update(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_eNB_cp_relocation_indication(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_ue_context_suspend(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_CBC_write_replace_warning_message(
        amf4g_enb_t *enb);        
CORE_DECLARE(void) s1ap_handle_CBC_stop_warning_message(
        amf4g_enb_t *enb);
CORE_DECLARE(void) s1ap_handle_ERAB_release_response(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_mme_configuration_update_acknowledge(
    amf4g_enb_t * enb,s1ap_message_t * message);
CORE_DECLARE(void) s1ap_handle_mme_configuration_update_failure(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_enb_direct_information_transfer(
    amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_e_rab_release_indication(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_s1_setup_request(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_initial_ue_message(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_uplink_nas_transport(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_ue_capability_info_indication(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_initial_context_setup_response(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_initial_context_setup_failure(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_e_rab_setup_response(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_e_rab_modify_response(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_ue_context_release_request(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_ue_context_release_complete(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_ue_context_resume_request(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_ue_context_modification_response(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_ue_context_modification_failure(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_ue_context_modification_indication(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_paging(amf4g_ue_t *amf4g_ue);

CORE_DECLARE(void) s1ap_handle_path_switch_request(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_handover_required(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_handover_request_ack(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_handover_failure(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_handover_cancel(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_enb_status_transfer(
        amf4g_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_enb_configuration_transfer(
        amf4g_enb_t *enb, s1ap_message_t *message, pkbuf_t *pkbuf);
CORE_DECLARE(void) s1ap_handle_handover_notification(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_s1_reset(
        amf4g_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_retrieve_ue_information(
        amf4g_enb_t *enb, s1ap_message_t *message);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __S1AP_HANDLER_H__ */
