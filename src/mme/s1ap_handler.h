#ifndef __S1AP_HANDLER_H__
#define __S1AP_HANDLER_H__

#include "s1ap/s1ap_message.h"

#include "mme_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//add by YEE
CORE_DECLARE(void) s1ap_handle_nas_delivery_indication(
        mme_enb_t *enb, s1ap_message_t *message);
/////////////////////////////////////////////////pan
CORE_DECLARE(void) s1ap_handle_eNB_configuration_update(
        mme_enb_t *enb, s1ap_message_t *message);
/////////////////////////////////////////////////
/*******************Qiu********************/
CORE_DECLARE(void) s1ap_handle_CBC_write_replace_warning_message(
		mme_enb_t *enb);		
CORE_DECLARE(void) s1ap_handle_CBC_stop_warning_message(
		mme_enb_t *enb);
/******************************************/
/*****Steven****/
CORE_DECLARE(void) s1ap_handle_mme_configuration_update_acknowledge(
	mme_enb_t * enb,s1ap_message_t * message);
CORE_DECLARE(void) s1ap_handle_mme_configuration_update_failure(
		mme_enb_t *enb, s1ap_message_t *message);
/***************/
CORE_DECLARE(void) s1ap_handle_nas_non_delivery_indication(
        mme_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_s1_setup_request(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_initial_ue_message(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_uplink_nas_transport(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_ue_capability_info_indication(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_initial_context_setup_response(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_initial_context_setup_failure(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_e_rab_setup_response(
        mme_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_ue_context_release_request(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_ue_context_release_complete(
        mme_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_paging(mme_ue_t *mme_ue);

CORE_DECLARE(void) s1ap_handle_path_switch_request(
        mme_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_handover_required(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_handover_request_ack(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_handover_failure(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_handover_cancel(
        mme_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_enb_status_transfer(
        mme_enb_t *enb, s1ap_message_t *message);
CORE_DECLARE(void) s1ap_handle_enb_configuration_transfer(
        mme_enb_t *enb, s1ap_message_t *message, pkbuf_t *pkbuf);
CORE_DECLARE(void) s1ap_handle_handover_notification(
        mme_enb_t *enb, s1ap_message_t *message);

CORE_DECLARE(void) s1ap_handle_s1_reset(
        mme_enb_t *enb, s1ap_message_t *message);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __S1AP_HANDLER_H__ */
