/*
 * Copyright (c) 2017, NextEPC Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*******************************************************************************
 * This file had been created by gtpv2c_tlv.py script v0.1.0
 * Please do not modify this file but regenerate it via script.
 * Created on: 2018-09-20 17:24:08.856514 by waiting
 * from 24501-f00.docx
 ******************************************************************************/

#ifndef __NAS_IES_H__
#define __NAS_IES_H__

#include "core_pkbuf.h"
#include "nas_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(c_int16_t) nas_encode_optional_type(pkbuf_t *pkbuf, c_uint8_t type);

CORE_DECLARE(c_int16_t) nas_decode_additional_information(nas_additional_information_t *additional_information, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_eap_message(nas_eap_message_t *eap_message, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_gprs_timer(nas_gprs_timer_t *gprs_timer, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_gprs_timer_3(nas_gprs_timer_3_t *gprs_timer_3, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_s_nssai(nas_s_nssai_t *s_nssai, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_gprs_timer_2(nas_gprs_timer_2_t *gprs_timer_2, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gmm_capability(nas_5gmm_capability_t *5gmm_capability, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_access_type(nas_access_type_t *access_type, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_allowed_pdu_session_status(nas_allowed_pdu_session_status_t *allowed_pdu_session_status, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_authentication_failure_parameter(nas_authentication_failure_parameter_t *authentication_failure_parameter, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_authentication_parameter(nas_authentication_parameter_t *authentication_parameter, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_authentication_parameter_autn(nas_authentication_parameter_autn_t *authentication_parameter_autn, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_authentication_response_parameter(nas_authentication_response_parameter_t *authentication_response_parameter, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_configuration_update_indication(nas_configuration_update_indication_t *configuration_update_indication, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_daylight_saving_time(nas_daylight_saving_time_t *daylight_saving_time, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_de_registration_type(nas_de_registration_type_t *de_registration_type, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_dnn(nas_dnn_t *dnn, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gmm_cause(nas_5gmm_cause_t *5gmm_cause, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_drx_parameters(nas_drx_parameters_t *drx_parameters, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_emergency_number_list(nas_emergency_number_list_t *emergency_number_list, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_eps_nas_message_container(nas_eps_nas_message_container_t *eps_nas_message_container, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_eps_nas_security_algorithms(nas_eps_nas_security_algorithms_t *eps_nas_security_algorithms, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_extended_emergency_number_list(nas_extended_emergency_number_list_t *extended_emergency_number_list, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_hashamf(nas_hashamf_t *hashamf, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_imeisv_request(nas_imeisv_request_t *imeisv_request, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_ladn_information(nas_ladn_information_t *ladn_information, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_mico_indication(nas_mico_indication_t *mico_indication, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_key_set_identifier(nas_key_set_identifier_t *key_set_identifier, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gs_identity_type(nas_5gs_identity_type_t *5gs_identity_type, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_message_container(nas_message_container_t *message_container, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_security_algorithms(nas_security_algorithms_t *security_algorithms, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_network_name(nas_network_name_t *network_name, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_nssai(nas_nssai_t *nssai, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_payload_container(nas_payload_container_t *payload_container, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_payload_container_type(nas_payload_container_type_t *payload_container_type, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_pdu_session_identity_2(nas_pdu_session_identity_2_t *pdu_session_identity_2, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_pdu_session_reactivation_result(nas_pdu_session_reactivation_result_t *pdu_session_reactivation_result, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_pdu_session_reactivation_result_error_cause(nas_pdu_session_reactivation_result_error_cause_t *pdu_session_reactivation_result_error_cause, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gs_mobile_identity(nas_5gs_mobile_identity_t *5gs_mobile_identity, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5g_mobile_identity(nas_5g_mobile_identity_t *5g_mobile_identity, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_pdu_session_status(nas_pdu_session_status_t *pdu_session_status, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_plmn_list(nas_plmn_list_t *plmn_list, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_rejected_nssai(nas_rejected_nssai_t *rejected_nssai, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_request_type(nas_request_type_t *request_type, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_s1_ue_network_capability(nas_s1_ue_network_capability_t *s1_ue_network_capability, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_service_area_list(nas_service_area_list_t *service_area_list, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_time_zone(nas_time_zone_t *time_zone, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_time_zone_and_time(nas_time_zone_and_time_t *time_zone_and_time, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_transparent_container(nas_transparent_container_t *transparent_container, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_ue_security_capability(nas_ue_security_capability_t *ue_security_capability, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gs_network_feature_support(nas_5gs_network_feature_support_t *5gs_network_feature_support, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_ue_usage_setting(nas_ue_usage_setting_t *ue_usage_setting, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_ue_status(nas_ue_status_t *ue_status, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_uplink_data_status(nas_uplink_data_status_t *uplink_data_status, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gs_registration_result(nas_5gs_registration_result_t *5gs_registration_result, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gs_registration_type(nas_5gs_registration_type_t *5gs_registration_type, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gs_tracking_area_identity(nas_5gs_tracking_area_identity_t *5gs_tracking_area_identity, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gs_tracking_area_identity_list(nas_5gs_tracking_area_identity_list_t *5gs_tracking_area_identity_list, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gsm_capability(nas_5gsm_capability_t *5gsm_capability, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_session_ambr(nas_session_ambr_t *session_ambr, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_sm_pdu_dn_request_container(nas_sm_pdu_dn_request_container_t *sm_pdu_dn_request_container, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_ssc_mode(nas_ssc_mode_t *ssc_mode, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_5gsm_cause(nas_5gsm_cause_t *5gsm_cause, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_allowed_ssc_mode(nas_allowed_ssc_mode_t *allowed_ssc_mode, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_extended_protocol_configuration_options(nas_extended_protocol_configuration_options_t *extended_protocol_configuration_options, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_mapped_eps_bearer_contexts(nas_mapped_eps_bearer_contexts_t *mapped_eps_bearer_contexts, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_maximum_number_of_supported_packet_filters(nas_maximum_number_of_supported_packet_filters_t *maximum_number_of_supported_packet_filters, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_pdu_address(nas_pdu_address_t *pdu_address, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_pdu_session_type(nas_pdu_session_type_t *pdu_session_type, pkbuf_t *pkbuf);
CORE_DECLARE(c_int16_t) nas_decode_qos_rules(nas_qos_rules_t *qos_rules, pkbuf_t *pkbuf);

CORE_DECLARE(c_int16_t) nas_encode_additional_information(pkbuf_t *pkbuf, nas_additional_information_t *additional_information);
CORE_DECLARE(c_int16_t) nas_encode_eap_message(pkbuf_t *pkbuf, nas_eap_message_t *eap_message);
CORE_DECLARE(c_int16_t) nas_encode_gprs_timer(pkbuf_t *pkbuf, nas_gprs_timer_t *gprs_timer);
CORE_DECLARE(c_int16_t) nas_encode_gprs_timer_3(pkbuf_t *pkbuf, nas_gprs_timer_3_t *gprs_timer_3);
CORE_DECLARE(c_int16_t) nas_encode_s_nssai(pkbuf_t *pkbuf, nas_s_nssai_t *s_nssai);
CORE_DECLARE(c_int16_t) nas_encode_gprs_timer_2(pkbuf_t *pkbuf, nas_gprs_timer_2_t *gprs_timer_2);
CORE_DECLARE(c_int16_t) nas_encode_5gmm_capability(pkbuf_t *pkbuf, nas_5gmm_capability_t *5gmm_capability);
CORE_DECLARE(c_int16_t) nas_encode_access_type(pkbuf_t *pkbuf, nas_access_type_t *access_type);
CORE_DECLARE(c_int16_t) nas_encode_allowed_pdu_session_status(pkbuf_t *pkbuf, nas_allowed_pdu_session_status_t *allowed_pdu_session_status);
CORE_DECLARE(c_int16_t) nas_encode_authentication_failure_parameter(pkbuf_t *pkbuf, nas_authentication_failure_parameter_t *authentication_failure_parameter);
CORE_DECLARE(c_int16_t) nas_encode_authentication_parameter(pkbuf_t *pkbuf, nas_authentication_parameter_t *authentication_parameter);
CORE_DECLARE(c_int16_t) nas_encode_authentication_parameter_autn(pkbuf_t *pkbuf, nas_authentication_parameter_autn_t *authentication_parameter_autn);
CORE_DECLARE(c_int16_t) nas_encode_authentication_response_parameter(pkbuf_t *pkbuf, nas_authentication_response_parameter_t *authentication_response_parameter);
CORE_DECLARE(c_int16_t) nas_encode_configuration_update_indication(pkbuf_t *pkbuf, nas_configuration_update_indication_t *configuration_update_indication);
CORE_DECLARE(c_int16_t) nas_encode_daylight_saving_time(pkbuf_t *pkbuf, nas_daylight_saving_time_t *daylight_saving_time);
CORE_DECLARE(c_int16_t) nas_encode_de_registration_type(pkbuf_t *pkbuf, nas_de_registration_type_t *de_registration_type);
CORE_DECLARE(c_int16_t) nas_encode_dnn(pkbuf_t *pkbuf, nas_dnn_t *dnn);
CORE_DECLARE(c_int16_t) nas_encode_5gmm_cause(pkbuf_t *pkbuf, nas_5gmm_cause_t *5gmm_cause);
CORE_DECLARE(c_int16_t) nas_encode_drx_parameters(pkbuf_t *pkbuf, nas_drx_parameters_t *drx_parameters);
CORE_DECLARE(c_int16_t) nas_encode_emergency_number_list(pkbuf_t *pkbuf, nas_emergency_number_list_t *emergency_number_list);
CORE_DECLARE(c_int16_t) nas_encode_eps_nas_message_container(pkbuf_t *pkbuf, nas_eps_nas_message_container_t *eps_nas_message_container);
CORE_DECLARE(c_int16_t) nas_encode_eps_nas_security_algorithms(pkbuf_t *pkbuf, nas_eps_nas_security_algorithms_t *eps_nas_security_algorithms);
CORE_DECLARE(c_int16_t) nas_encode_extended_emergency_number_list(pkbuf_t *pkbuf, nas_extended_emergency_number_list_t *extended_emergency_number_list);
CORE_DECLARE(c_int16_t) nas_encode_hashamf(pkbuf_t *pkbuf, nas_hashamf_t *hashamf);
CORE_DECLARE(c_int16_t) nas_encode_imeisv_request(pkbuf_t *pkbuf, nas_imeisv_request_t *imeisv_request);
CORE_DECLARE(c_int16_t) nas_encode_ladn_information(pkbuf_t *pkbuf, nas_ladn_information_t *ladn_information);
CORE_DECLARE(c_int16_t) nas_encode_mico_indication(pkbuf_t *pkbuf, nas_mico_indication_t *mico_indication);
CORE_DECLARE(c_int16_t) nas_encode_key_set_identifier(pkbuf_t *pkbuf, nas_key_set_identifier_t *key_set_identifier);
CORE_DECLARE(c_int16_t) nas_encode_5gs_identity_type(pkbuf_t *pkbuf, nas_5gs_identity_type_t *5gs_identity_type);
CORE_DECLARE(c_int16_t) nas_encode_message_container(pkbuf_t *pkbuf, nas_message_container_t *message_container);
CORE_DECLARE(c_int16_t) nas_encode_security_algorithms(pkbuf_t *pkbuf, nas_security_algorithms_t *security_algorithms);
CORE_DECLARE(c_int16_t) nas_encode_network_name(pkbuf_t *pkbuf, nas_network_name_t *network_name);
CORE_DECLARE(c_int16_t) nas_encode_nssai(pkbuf_t *pkbuf, nas_nssai_t *nssai);
CORE_DECLARE(c_int16_t) nas_encode_payload_container(pkbuf_t *pkbuf, nas_payload_container_t *payload_container);
CORE_DECLARE(c_int16_t) nas_encode_payload_container_type(pkbuf_t *pkbuf, nas_payload_container_type_t *payload_container_type);
CORE_DECLARE(c_int16_t) nas_encode_pdu_session_identity_2(pkbuf_t *pkbuf, nas_pdu_session_identity_2_t *pdu_session_identity_2);
CORE_DECLARE(c_int16_t) nas_encode_pdu_session_reactivation_result(pkbuf_t *pkbuf, nas_pdu_session_reactivation_result_t *pdu_session_reactivation_result);
CORE_DECLARE(c_int16_t) nas_encode_pdu_session_reactivation_result_error_cause(pkbuf_t *pkbuf, nas_pdu_session_reactivation_result_error_cause_t *pdu_session_reactivation_result_error_cause);
CORE_DECLARE(c_int16_t) nas_encode_5gs_mobile_identity(pkbuf_t *pkbuf, nas_5gs_mobile_identity_t *5gs_mobile_identity);
CORE_DECLARE(c_int16_t) nas_encode_5g_mobile_identity(pkbuf_t *pkbuf, nas_5g_mobile_identity_t *5g_mobile_identity);
CORE_DECLARE(c_int16_t) nas_encode_pdu_session_status(pkbuf_t *pkbuf, nas_pdu_session_status_t *pdu_session_status);
CORE_DECLARE(c_int16_t) nas_encode_plmn_list(pkbuf_t *pkbuf, nas_plmn_list_t *plmn_list);
CORE_DECLARE(c_int16_t) nas_encode_rejected_nssai(pkbuf_t *pkbuf, nas_rejected_nssai_t *rejected_nssai);
CORE_DECLARE(c_int16_t) nas_encode_request_type(pkbuf_t *pkbuf, nas_request_type_t *request_type);
CORE_DECLARE(c_int16_t) nas_encode_s1_ue_network_capability(pkbuf_t *pkbuf, nas_s1_ue_network_capability_t *s1_ue_network_capability);
CORE_DECLARE(c_int16_t) nas_encode_service_area_list(pkbuf_t *pkbuf, nas_service_area_list_t *service_area_list);
CORE_DECLARE(c_int16_t) nas_encode_time_zone(pkbuf_t *pkbuf, nas_time_zone_t *time_zone);
CORE_DECLARE(c_int16_t) nas_encode_time_zone_and_time(pkbuf_t *pkbuf, nas_time_zone_and_time_t *time_zone_and_time);
CORE_DECLARE(c_int16_t) nas_encode_transparent_container(pkbuf_t *pkbuf, nas_transparent_container_t *transparent_container);
CORE_DECLARE(c_int16_t) nas_encode_ue_security_capability(pkbuf_t *pkbuf, nas_ue_security_capability_t *ue_security_capability);
CORE_DECLARE(c_int16_t) nas_encode_5gs_network_feature_support(pkbuf_t *pkbuf, nas_5gs_network_feature_support_t *5gs_network_feature_support);
CORE_DECLARE(c_int16_t) nas_encode_ue_usage_setting(pkbuf_t *pkbuf, nas_ue_usage_setting_t *ue_usage_setting);
CORE_DECLARE(c_int16_t) nas_encode_ue_status(pkbuf_t *pkbuf, nas_ue_status_t *ue_status);
CORE_DECLARE(c_int16_t) nas_encode_uplink_data_status(pkbuf_t *pkbuf, nas_uplink_data_status_t *uplink_data_status);
CORE_DECLARE(c_int16_t) nas_encode_5gs_registration_result(pkbuf_t *pkbuf, nas_5gs_registration_result_t *5gs_registration_result);
CORE_DECLARE(c_int16_t) nas_encode_5gs_registration_type(pkbuf_t *pkbuf, nas_5gs_registration_type_t *5gs_registration_type);
CORE_DECLARE(c_int16_t) nas_encode_5gs_tracking_area_identity(pkbuf_t *pkbuf, nas_5gs_tracking_area_identity_t *5gs_tracking_area_identity);
CORE_DECLARE(c_int16_t) nas_encode_5gs_tracking_area_identity_list(pkbuf_t *pkbuf, nas_5gs_tracking_area_identity_list_t *5gs_tracking_area_identity_list);
CORE_DECLARE(c_int16_t) nas_encode_5gsm_capability(pkbuf_t *pkbuf, nas_5gsm_capability_t *5gsm_capability);
CORE_DECLARE(c_int16_t) nas_encode_session_ambr(pkbuf_t *pkbuf, nas_session_ambr_t *session_ambr);
CORE_DECLARE(c_int16_t) nas_encode_sm_pdu_dn_request_container(pkbuf_t *pkbuf, nas_sm_pdu_dn_request_container_t *sm_pdu_dn_request_container);
CORE_DECLARE(c_int16_t) nas_encode_ssc_mode(pkbuf_t *pkbuf, nas_ssc_mode_t *ssc_mode);
CORE_DECLARE(c_int16_t) nas_encode_5gsm_cause(pkbuf_t *pkbuf, nas_5gsm_cause_t *5gsm_cause);
CORE_DECLARE(c_int16_t) nas_encode_allowed_ssc_mode(pkbuf_t *pkbuf, nas_allowed_ssc_mode_t *allowed_ssc_mode);
CORE_DECLARE(c_int16_t) nas_encode_extended_protocol_configuration_options(pkbuf_t *pkbuf, nas_extended_protocol_configuration_options_t *extended_protocol_configuration_options);
CORE_DECLARE(c_int16_t) nas_encode_mapped_eps_bearer_contexts(pkbuf_t *pkbuf, nas_mapped_eps_bearer_contexts_t *mapped_eps_bearer_contexts);
CORE_DECLARE(c_int16_t) nas_encode_maximum_number_of_supported_packet_filters(pkbuf_t *pkbuf, nas_maximum_number_of_supported_packet_filters_t *maximum_number_of_supported_packet_filters);
CORE_DECLARE(c_int16_t) nas_encode_pdu_address(pkbuf_t *pkbuf, nas_pdu_address_t *pdu_address);
CORE_DECLARE(c_int16_t) nas_encode_pdu_session_type(pkbuf_t *pkbuf, nas_pdu_session_type_t *pdu_session_type);
CORE_DECLARE(c_int16_t) nas_encode_qos_rules(pkbuf_t *pkbuf, nas_qos_rules_t *qos_rules);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NAS_IES_H__ */

