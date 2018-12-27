#ifndef __NGAP_HANDLER_H__
#define __NGAP_HANDLER_H__

#include "ngap/ngap_message.h"

/* TODP: need to modify the include head file to amf_context.h*/
#include "amf4g_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) ngap_handle_ng_setup_request(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_initial_context_setup_response(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_initial_context_setup_failure(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_pdu_session_resource_setup_response(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_pdu_session_resource_modify_response(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_pdu_session_resource_modify_indication(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_pdu_session_resource_release_response(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_pdu_session_resource_notify(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_ue_context_modification_response(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_ue_context_modification_failure(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_ue_context_release_request(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_ng_reset(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_handover_cancel(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_handover_required(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_handover_request_acknowledge(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_handover_failure(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_handover_notification(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_path_switch_request(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_amf_configuration_update_acknowledge(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_amf_configuration_update_failure(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_ue_radio_capability_check_response(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_nas_non_delivery_indication(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_uplink_nas_transport(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_uplink_ue_associated_nrppa_transport(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_uplink_non_ue_associated_nrppa_transport(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_location_reporting_failure_indication(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_location_report(amf_ran_t *ran, ngap_message_t *message);
CORE_DECLARE(void) ngap_handle_uplink_ran_configuration_transfer(amf_ran_t *ran, ngap_message_t *message);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NGAP_HANDLER_H__ */
