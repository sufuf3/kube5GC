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
 * Created on: 2018-09-20 17:24:08.886123 by waiting
 * from 24501-f00.docx
 ******************************************************************************/

#define TRACE_MODULE _nas_encoder

#include "core_debug.h"
#include "nas_message.h"

c_int32_t nas_encode_registration_request(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_registration_request_t *registration_request = &message->gmm.registration_request;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode REGISTRATION_REQUEST\n");

    size = nas_encode_5gs_registration_type(pkbuf, &registration_request->5gs_registration_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_key_set_identifier(pkbuf, &registration_request->ngksi);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_5gs_mobile_identity(pkbuf, &registration_request->5gs_mobile_identity);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_NON_CURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER_PRESENT)
    {
        registration_request->non_current_native_nas_key_set_identifier.type = (NAS_REGISTRATION_REQUEST_NON_CURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER_TYPE >> 4);

        size = nas_encode_key_set_identifier(pkbuf, &registration_request->non_current_native_nas_key_set_identifier);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_5GMM_CAPABILITY_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_5GMM_CAPABILITY_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gmm_capability(pkbuf, &registration_request->5gmm_capability);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_UE_SECURITY_CAPABILITY_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_UE_SECURITY_CAPABILITY_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_ue_security_capability(pkbuf, &registration_request->ue_security_capability);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_REQUESTED_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_REQUESTED_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_nssai(pkbuf, &registration_request->requested_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_LAST_VISITED_REGISTERED_TAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gs_tracking_area_identity(pkbuf, &registration_request->last_visited_registered_tai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_S1_UE_NETWORK_CAPABILITY_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_S1_UE_NETWORK_CAPABILITY_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_s1_ue_network_capability(pkbuf, &registration_request->s1_ue_network_capability);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_UPLINK_DATA_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_UPLINK_DATA_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_uplink_data_status(pkbuf, &registration_request->uplink_data_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_status(pkbuf, &registration_request->pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_MICO_INDICATION_PRESENT)
    {
        registration_request->mico_indication.type = (NAS_REGISTRATION_REQUEST_MICO_INDICATION_TYPE >> 4);

        size = nas_encode_mico_indication(pkbuf, &registration_request->mico_indication);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_UE_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_UE_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_ue_status(pkbuf, &registration_request->ue_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_ADDITIONAL_GUTI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_ADDITIONAL_GUTI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gs_mobile_identity(pkbuf, &registration_request->additional_guti);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_ALLOWED_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_ALLOWED_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_allowed_pdu_session_status(pkbuf, &registration_request->allowed_pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_UE_USAGE_SETTING_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_UE_USAGE_SETTING_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_ue_usage_setting(pkbuf, &registration_request->ue_usage_setting);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_REQUESTED_DRX_PARAMETERS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_REQUESTED_DRX_PARAMETERS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_drx_parameters(pkbuf, &registration_request->requested_drx_parameters);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_EPS_NAS_MESSAGE_CONTAINER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_EPS_NAS_MESSAGE_CONTAINER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eps_nas_message_container(pkbuf, &registration_request->eps_nas_message_container);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_request->presencemask & NAS_REGISTRATION_REQUEST_PAYLOAD_CONTAINER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REQUEST_PAYLOAD_CONTAINER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_payload_container(pkbuf, &registration_request->payload_container);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_registration_accept(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_registration_accept_t *registration_accept = &message->gmm.registration_accept;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode REGISTRATION_ACCEPT\n");

    size = nas_encode_5gs_registration_result(pkbuf, &registration_accept->5gs_registration_result);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_5G_GUTI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_5G_GUTI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gs_mobile_identity(pkbuf, &registration_accept->5g_guti);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_EQUIVALENT_PLMNS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_EQUIVALENT_PLMNS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_plmn_list(pkbuf, &registration_accept->equivalent_plmns);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_TAI_LIST_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_TAI_LIST_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gs_tracking_area_identity_list(pkbuf, &registration_accept->tai_list);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_nssai(pkbuf, &registration_accept->allowed_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_REJECTED_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_REJECTED_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_rejected_nssai(pkbuf, &registration_accept->rejected_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_CONFIGURED_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_CONFIGURED_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_nssai(pkbuf, &registration_accept->configured_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_5GS_NETWORK_FEATURE_SUPPORT_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_5GS_NETWORK_FEATURE_SUPPORT_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gs_network_feature_support(pkbuf, &registration_accept->5gs_network_feature_support);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_status(pkbuf, &registration_accept->pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_reactivation_result(pkbuf, &registration_accept->pdu_session_reactivation_result);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_reactivation_result_error_cause(pkbuf, &registration_accept->pdu_session_reactivation_result_error_cause);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_LADN_INFORMATION_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_LADN_INFORMATION_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_ladn_information(pkbuf, &registration_accept->ladn_information);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_MICO_INDICATION_PRESENT)
    {
        registration_accept->mico_indication.type = (NAS_REGISTRATION_ACCEPT_MICO_INDICATION_TYPE >> 4);

        size = nas_encode_mico_indication(pkbuf, &registration_accept->mico_indication);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_SERVICE_AREA_LIST_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_SERVICE_AREA_LIST_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_service_area_list(pkbuf, &registration_accept->service_area_list);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_T3512_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_T3512_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_3(pkbuf, &registration_accept->t3512_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_NON__DE_REGISTRATION_TIMER_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_NON__DE_REGISTRATION_TIMER_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_2(pkbuf, &registration_accept->non__de_registration_timer_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_T3502_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_T3502_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_2(pkbuf, &registration_accept->t3502_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_EMERGENCY_NUMBER_LIST_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_EMERGENCY_NUMBER_LIST_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_emergency_number_list(pkbuf, &registration_accept->emergency_number_list);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_EXTENDED_EMERGENCY_NUMBER_LIST_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_EXTENDED_EMERGENCY_NUMBER_LIST_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_emergency_number_list(pkbuf, &registration_accept->extended_emergency_number_list);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_TRANSPARENT_CONTAINER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_TRANSPARENT_CONTAINER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_transparent_container(pkbuf, &registration_accept->transparent_container);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_accept->presencemask & NAS_REGISTRATION_ACCEPT_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_ACCEPT_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &registration_accept->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_registration_complete(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_registration_complete_t *registration_complete = &message->gmm.registration_complete;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode REGISTRATION_COMPLETE\n");

    if (registration_complete->presencemask & NAS_REGISTRATION_COMPLETE_TRANSPARENT_CONTAINER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_COMPLETE_TRANSPARENT_CONTAINER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_transparent_container(pkbuf, &registration_complete->transparent_container);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_registration_reject(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_registration_reject_t *registration_reject = &message->gmm.registration_reject;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode REGISTRATION_REJECT\n");

    size = nas_encode_5gmm_cause(pkbuf, &registration_reject->5gmm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (registration_reject->presencemask & NAS_REGISTRATION_REJECT_T3346_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REJECT_T3346_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_2(pkbuf, &registration_reject->t3346_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_reject->presencemask & NAS_REGISTRATION_REJECT_T3502_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REJECT_T3502_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_2(pkbuf, &registration_reject->t3502_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (registration_reject->presencemask & NAS_REGISTRATION_REJECT_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_REGISTRATION_REJECT_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &registration_reject->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_deregistration_request_from_ue(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_deregistration_request_from_ue_t *deregistration_request_from_ue = &message->gmm.deregistration_request_from_ue;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode DEREGISTRATION_REQUEST\n");

    size = nas_encode_de_registration_type(pkbuf, &deregistration_request_from_ue->de_registration_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_5gs_mobile_identity(pkbuf, &deregistration_request_from_ue->5gs_mobile_identity);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

c_int32_t nas_encode_deregistration_request_to_ue(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_deregistration_request_to_ue_t *deregistration_request_to_ue = &message->gmm.deregistration_request_to_ue;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode DEREGISTRATION_REQUEST\n");

    size = nas_encode_de_registration_type(pkbuf, &deregistration_request_to_ue->de_registration_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (deregistration_request_to_ue->presencemask & NAS_DEREGISTRATION_REQUEST_5GMM_CAUSE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_DEREGISTRATION_REQUEST_5GMM_CAUSE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gmm_cause(pkbuf, &deregistration_request_to_ue->5gmm_cause);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (deregistration_request_to_ue->presencemask & NAS_DEREGISTRATION_REQUEST_T3346_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_DEREGISTRATION_REQUEST_T3346_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_2(pkbuf, &deregistration_request_to_ue->t3346_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_service_request(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_service_request_t *service_request = &message->gmm.service_request;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode SERVICE_REQUEST\n");

    size = nas_encode_key_set_identifier(pkbuf, &service_request->ngksi);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_5gs_mobile_identity(pkbuf, &service_request->5g_s_tmsi);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (service_request->presencemask & NAS_SERVICE_REQUEST_UPLINK_DATA_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_REQUEST_UPLINK_DATA_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_uplink_data_status(pkbuf, &service_request->uplink_data_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (service_request->presencemask & NAS_SERVICE_REQUEST_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_REQUEST_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_status(pkbuf, &service_request->pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (service_request->presencemask & NAS_SERVICE_REQUEST_ALLOWED_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_REQUEST_ALLOWED_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_allowed_pdu_session_status(pkbuf, &service_request->allowed_pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_service_reject(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_service_reject_t *service_reject = &message->gmm.service_reject;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode SERVICE_REJECT\n");

    size = nas_encode_5gmm_cause(pkbuf, &service_reject->5gmm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (service_reject->presencemask & NAS_SERVICE_REJECT_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_REJECT_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_status(pkbuf, &service_reject->pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (service_reject->presencemask & NAS_SERVICE_REJECT_T3346_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_REJECT_T3346_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_2(pkbuf, &service_reject->t3346_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (service_reject->presencemask & NAS_SERVICE_REJECT_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_REJECT_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &service_reject->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_service_accept(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_service_accept_t *service_accept = &message->gmm.service_accept;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode SERVICE_ACCEPT\n");

    if (service_accept->presencemask & NAS_SERVICE_ACCEPT_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_ACCEPT_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_status(pkbuf, &service_accept->pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (service_accept->presencemask & NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_reactivation_result(pkbuf, &service_accept->pdu_session_reactivation_result);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (service_accept->presencemask & NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_reactivation_result_error_cause(pkbuf, &service_accept->pdu_session_reactivation_result_error_cause);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (service_accept->presencemask & NAS_SERVICE_ACCEPT_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SERVICE_ACCEPT_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &service_accept->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_configuration_update_command(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_configuration_update_command_t *configuration_update_command = &message->gmm.configuration_update_command;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode CONFIGURATION_UPDATE_COMMAND\n");

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURATION_UPDATE_INDICATION_PRESENT)
    {
        configuration_update_command->configuration_update_indication.type = (NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURATION_UPDATE_INDICATION_TYPE >> 4);

        size = nas_encode_configuration_update_indication(pkbuf, &configuration_update_command->configuration_update_indication);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_5G_GUTI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_5G_GUTI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gs_mobile_identity(pkbuf, &configuration_update_command->5g_guti);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_TAI_LIST_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_TAI_LIST_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gs_tracking_area_identity_list(pkbuf, &configuration_update_command->tai_list);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_ALLOWED_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_ALLOWED_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_nssai(pkbuf, &configuration_update_command->allowed_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_SERVICE_AREA_LIST_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_SERVICE_AREA_LIST_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_service_area_list(pkbuf, &configuration_update_command->service_area_list);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_FULL_NAME_FOR_NETWORK_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_FULL_NAME_FOR_NETWORK_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_network_name(pkbuf, &configuration_update_command->full_name_for_network);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_SHORT_NAME_FOR_NETWORK_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_SHORT_NAME_FOR_NETWORK_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_network_name(pkbuf, &configuration_update_command->short_name_for_network);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_LOCAL_TIME_ZONE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_LOCAL_TIME_ZONE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_time_zone(pkbuf, &configuration_update_command->local_time_zone);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_time_zone_and_time(pkbuf, &configuration_update_command->universal_time_and_local_time_zone);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_NETWORK_DAYLIGHT_SAVING_TIME_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_NETWORK_DAYLIGHT_SAVING_TIME_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_daylight_saving_time(pkbuf, &configuration_update_command->network_daylight_saving_time);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_LADN_INFORMATION_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_LADN_INFORMATION_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_ladn_information(pkbuf, &configuration_update_command->ladn_information);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_MICO_INDICATION_PRESENT)
    {
        configuration_update_command->mico_indication.type = (NAS_CONFIGURATION_UPDATE_COMMAND_MICO_INDICATION_TYPE >> 4);

        size = nas_encode_mico_indication(pkbuf, &configuration_update_command->mico_indication);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURED_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURED_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_nssai(pkbuf, &configuration_update_command->configured_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_command->presencemask & NAS_CONFIGURATION_UPDATE_COMMAND_REJECTED_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMMAND_REJECTED_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_rejected_nssai(pkbuf, &configuration_update_command->rejected_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_configuration_update_complete(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_configuration_update_complete_t *configuration_update_complete = &message->gmm.configuration_update_complete;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode CONFIGURATION_UPDATE_COMPLETE\n");

    if (configuration_update_complete->presencemask & NAS_CONFIGURATION_UPDATE_COMPLETE_PDU_SESSION_TYPE_PRESENT)
    {
        configuration_update_complete->pdu_session_type.type = (NAS_CONFIGURATION_UPDATE_COMPLETE_PDU_SESSION_TYPE_TYPE >> 4);

        size = nas_encode_pdu_session_type(pkbuf, &configuration_update_complete->pdu_session_type);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_complete->presencemask & NAS_CONFIGURATION_UPDATE_COMPLETE_SSC_MODE_PRESENT)
    {
        configuration_update_complete->ssc_mode.type = (NAS_CONFIGURATION_UPDATE_COMPLETE_SSC_MODE_TYPE >> 4);

        size = nas_encode_ssc_mode(pkbuf, &configuration_update_complete->ssc_mode);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_complete->presencemask & NAS_CONFIGURATION_UPDATE_COMPLETE_5GSM_CAPABILITY_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMPLETE_5GSM_CAPABILITY_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gsm_capability(pkbuf, &configuration_update_complete->5gsm_capability);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_complete->presencemask & NAS_CONFIGURATION_UPDATE_COMPLETE_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMPLETE_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_maximum_number_of_supported_packet_filters(pkbuf, &configuration_update_complete->maximum_number_of_supported_packet_filters);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_complete->presencemask & NAS_CONFIGURATION_UPDATE_COMPLETE_SM_PDU_DN_REQUEST_CONTAINER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMPLETE_SM_PDU_DN_REQUEST_CONTAINER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_sm_pdu_dn_request_container(pkbuf, &configuration_update_complete->sm_pdu_dn_request_container);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (configuration_update_complete->presencemask & NAS_CONFIGURATION_UPDATE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_CONFIGURATION_UPDATE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &configuration_update_complete->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_authentication_request(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_authentication_request_t *authentication_request = &message->gmm.authentication_request;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode AUTHENTICATION_REQUEST\n");

    size = nas_encode_key_set_identifier(pkbuf, &authentication_request->ngksi);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (authentication_request->presencemask & NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_RAND_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_RAND_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_authentication_parameter(pkbuf, &authentication_request->authentication_parameter_rand);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (authentication_request->presencemask & NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_AUTN_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_AUTN_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_authentication_parameter_autn(pkbuf, &authentication_request->authentication_parameter_autn);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (authentication_request->presencemask & NAS_AUTHENTICATION_REQUEST_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_AUTHENTICATION_REQUEST_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &authentication_request->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_authentication_response(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_authentication_response_t *authentication_response = &message->gmm.authentication_response;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode AUTHENTICATION_RESPONSE\n");

    if (authentication_response->presencemask & NAS_AUTHENTICATION_RESPONSE_AUTHENTICATION_RESPONSE_PARAMETER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_AUTHENTICATION_RESPONSE_AUTHENTICATION_RESPONSE_PARAMETER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_authentication_response_parameter(pkbuf, &authentication_response->authentication_response_parameter);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (authentication_response->presencemask & NAS_AUTHENTICATION_RESPONSE_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_AUTHENTICATION_RESPONSE_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &authentication_response->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_authentication_failure(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_authentication_failure_t *authentication_failure = &message->gmm.authentication_failure;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode AUTHENTICATION_FAILURE\n");

    size = nas_encode_5gmm_cause(pkbuf, &authentication_failure->5gmm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (authentication_failure->presencemask & NAS_AUTHENTICATION_FAILURE_AUTHENTICATION_FAILURE_PARAMETER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_AUTHENTICATION_FAILURE_AUTHENTICATION_FAILURE_PARAMETER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_authentication_failure_parameter(pkbuf, &authentication_failure->authentication_failure_parameter);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_authentication_result(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_authentication_result_t *authentication_result = &message->gmm.authentication_result;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode AUTHENTICATION_RESULT\n");

    size = nas_encode_key_set_identifier(pkbuf, &authentication_result->ngksi);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_eap_message(pkbuf, &authentication_result->eap_message);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

c_int32_t nas_encode_identity_request(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_identity_request_t *identity_request = &message->gmm.identity_request;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode IDENTITY_REQUEST\n");

    size = nas_encode_5gs_identity_type(pkbuf, &identity_request->identity_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

c_int32_t nas_encode_identity_response(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_identity_response_t *identity_response = &message->gmm.identity_response;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode IDENTITY_RESPONSE\n");

    size = nas_encode_5gs_mobile_identity(pkbuf, &identity_response->mobile_identity);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

c_int32_t nas_encode_security_mode_command(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_security_mode_command_t *security_mode_command = &message->gmm.security_mode_command;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode SECURITY_MODE_COMMAND\n");

    size = nas_encode_security_algorithms(pkbuf, &security_mode_command->selected_nas_security_algorithms);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_key_set_identifier(pkbuf, &security_mode_command->ngksi);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_ue_security_capability(pkbuf, &security_mode_command->replayed_ue_security_capabilities);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (security_mode_command->presencemask & NAS_SECURITY_MODE_COMMAND_IMEISV_REQUEST_PRESENT)
    {
        security_mode_command->imeisv_request.type = (NAS_SECURITY_MODE_COMMAND_IMEISV_REQUEST_TYPE >> 4);

        size = nas_encode_imeisv_request(pkbuf, &security_mode_command->imeisv_request);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (security_mode_command->presencemask & NAS_SECURITY_MODE_COMMAND_HASHAMF_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SECURITY_MODE_COMMAND_HASHAMF_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_hashamf(pkbuf, &security_mode_command->hashamf);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (security_mode_command->presencemask & NAS_SECURITY_MODE_COMMAND_SELECTED_EPS_NAS_SECURITY_ALGORITHMS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SECURITY_MODE_COMMAND_SELECTED_EPS_NAS_SECURITY_ALGORITHMS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eps_nas_security_algorithms(pkbuf, &security_mode_command->selected_eps_nas_security_algorithms);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (security_mode_command->presencemask & NAS_SECURITY_MODE_COMMAND_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SECURITY_MODE_COMMAND_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &security_mode_command->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_security_mode_complete(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_security_mode_complete_t *security_mode_complete = &message->gmm.security_mode_complete;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode SECURITY_MODE_COMPLETE\n");

    if (security_mode_complete->presencemask & NAS_SECURITY_MODE_COMPLETE_IMEISV_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SECURITY_MODE_COMPLETE_IMEISV_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5g_mobile_identity(pkbuf, &security_mode_complete->imeisv);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (security_mode_complete->presencemask & NAS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_message_container(pkbuf, &security_mode_complete->nas_message_container);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_security_mode_reject(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_security_mode_reject_t *security_mode_reject = &message->gmm.security_mode_reject;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode SECURITY_MODE_REJECT\n");

    size = nas_encode_5gmm_cause(pkbuf, &security_mode_reject->5gmm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

c_int32_t nas_encode_gmm_status(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_gmm_status_t *gmm_status = &message->gmm.gmm_status;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode GMM_STATUS\n");

    size = nas_encode_5gmm_cause(pkbuf, &gmm_status->5gmm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

c_int32_t nas_encode_notification(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_notification_t *notification = &message->gmm.notification;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode NOTIFICATION\n");

    size = nas_encode_access_type(pkbuf, &notification->access_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

c_int32_t nas_encode_notification_response(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_notification_response_t *notification_response = &message->gmm.notification_response;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode NOTIFICATION_RESPONSE\n");

    if (notification_response->presencemask & NAS_NOTIFICATION_RESPONSE_PDU_SESSION_STATUS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_NOTIFICATION_RESPONSE_PDU_SESSION_STATUS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_status(pkbuf, &notification_response->pdu_session_status);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_ul_nas_transport(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_ul_nas_transport_t *ul_nas_transport = &message->gmm.ul_nas_transport;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode UL_NAS_TRANSPORT\n");

    size = nas_encode_payload_container_type(pkbuf, &ul_nas_transport->payload_container_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_payload_container(pkbuf, &ul_nas_transport->payload_container);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (ul_nas_transport->presencemask & NAS_UL_NAS_TRANSPORT_OLD_PDU_SESSION_ID_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_UL_NAS_TRANSPORT_OLD_PDU_SESSION_ID_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_session_identity_2(pkbuf, &ul_nas_transport->old_pdu_session_id);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (ul_nas_transport->presencemask & NAS_UL_NAS_TRANSPORT_REQUEST_TYPE_PRESENT)
    {
        ul_nas_transport->request_type.type = (NAS_UL_NAS_TRANSPORT_REQUEST_TYPE_TYPE >> 4);

        size = nas_encode_request_type(pkbuf, &ul_nas_transport->request_type);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (ul_nas_transport->presencemask & NAS_UL_NAS_TRANSPORT_S_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_UL_NAS_TRANSPORT_S_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_s_nssai(pkbuf, &ul_nas_transport->s_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (ul_nas_transport->presencemask & NAS_UL_NAS_TRANSPORT_DNN_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_UL_NAS_TRANSPORT_DNN_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_dnn(pkbuf, &ul_nas_transport->dnn);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (ul_nas_transport->presencemask & NAS_UL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_UL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_additional_information(pkbuf, &ul_nas_transport->additional_information);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_dl_nas_transport(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_dl_nas_transport_t *dl_nas_transport = &message->gmm.dl_nas_transport;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode DL_NAS_TRANSPORT\n");

    size = nas_encode_payload_container_type(pkbuf, &dl_nas_transport->payload_container_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_payload_container(pkbuf, &dl_nas_transport->payload_container);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (dl_nas_transport->presencemask & NAS_DL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_DL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_additional_information(pkbuf, &dl_nas_transport->additional_information);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (dl_nas_transport->presencemask & NAS_DL_NAS_TRANSPORT_5GMM_CAUSE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_DL_NAS_TRANSPORT_5GMM_CAUSE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gmm_cause(pkbuf, &dl_nas_transport->5gmm_cause);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (dl_nas_transport->presencemask & NAS_DL_NAS_TRANSPORT_BACK_OFF_TIMER_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_DL_NAS_TRANSPORT_BACK_OFF_TIMER_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_3(pkbuf, &dl_nas_transport->back_off_timer_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_establishment_request(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_establishment_request_t *pdu_session_establishment_request = &message->gsm.pdu_session_establishment_request;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_ESTABLISHMENT_REQUEST\n");

    if (pdu_session_establishment_request->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_PDU_SESSION_TYPE_PRESENT)
    {
        pdu_session_establishment_request->pdu_session_type.type = (NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_PDU_SESSION_TYPE_TYPE >> 4);

        size = nas_encode_pdu_session_type(pkbuf, &pdu_session_establishment_request->pdu_session_type);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_request->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SSC_MODE_PRESENT)
    {
        pdu_session_establishment_request->ssc_mode.type = (NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SSC_MODE_TYPE >> 4);

        size = nas_encode_ssc_mode(pkbuf, &pdu_session_establishment_request->ssc_mode);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_request->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_5GSM_CAPABILITY_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_5GSM_CAPABILITY_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gsm_capability(pkbuf, &pdu_session_establishment_request->5gsm_capability);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_request->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_maximum_number_of_supported_packet_filters(pkbuf, &pdu_session_establishment_request->maximum_number_of_supported_packet_filters);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_request->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SM_PDU_DN_REQUEST_CONTAINER_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SM_PDU_DN_REQUEST_CONTAINER_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_sm_pdu_dn_request_container(pkbuf, &pdu_session_establishment_request->sm_pdu_dn_request_container);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_request->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_establishment_request->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_establishment_accept(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_establishment_accept_t *pdu_session_establishment_accept = &message->gsm.pdu_session_establishment_accept;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_ESTABLISHMENT_ACCEPT\n");

    size = nas_encode_pdu_session_type(pkbuf, &pdu_session_establishment_accept->selected_pdu_session_type);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_dnn(pkbuf, &pdu_session_establishment_accept->dnn);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_qos_rules(pkbuf, &pdu_session_establishment_accept->authorized_qos_rules);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    size = nas_encode_session_ambr(pkbuf, &pdu_session_establishment_accept->session_ambr);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (pdu_session_establishment_accept->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_5GSM_CAUSE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_5GSM_CAUSE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gsm_cause(pkbuf, &pdu_session_establishment_accept->5gsm_cause);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_accept->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_PDU_ADDRESS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_PDU_ADDRESS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_pdu_address(pkbuf, &pdu_session_establishment_accept->pdu_address);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_accept->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_RQ_TIMER_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_RQ_TIMER_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer(pkbuf, &pdu_session_establishment_accept->rq_timer_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_accept->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_S_NSSAI_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_S_NSSAI_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_s_nssai(pkbuf, &pdu_session_establishment_accept->s_nssai);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_accept->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_MAPPED_EPS_BEARER_CONTEXTS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_MAPPED_EPS_BEARER_CONTEXTS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_mapped_eps_bearer_contexts(pkbuf, &pdu_session_establishment_accept->mapped_eps_bearer_contexts);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_accept->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &pdu_session_establishment_accept->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_accept->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_establishment_accept->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_establishment_reject(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_establishment_reject_t *pdu_session_establishment_reject = &message->gsm.pdu_session_establishment_reject;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_ESTABLISHMENT_REJECT\n");

    size = nas_encode_5gsm_cause(pkbuf, &pdu_session_establishment_reject->5gsm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (pdu_session_establishment_reject->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REJECT_BACK_OFF_TIMER_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_REJECT_BACK_OFF_TIMER_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_3(pkbuf, &pdu_session_establishment_reject->back_off_timer_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_reject->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REJECT_ALLOWED_SSC_MODE_PRESENT)
    {
        pdu_session_establishment_reject->allowed_ssc_mode.type = (NAS_PDU_SESSION_ESTABLISHMENT_REJECT_ALLOWED_SSC_MODE_TYPE >> 4);

        size = nas_encode_allowed_ssc_mode(pkbuf, &pdu_session_establishment_reject->allowed_ssc_mode);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_reject->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &pdu_session_establishment_reject->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_establishment_reject->presencemask & NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_establishment_reject->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_authentication_command(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_authentication_command_t *pdu_session_authentication_command = &message->gsm.pdu_session_authentication_command;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_AUTHENTICATION_COMMAND\n");

    if (pdu_session_authentication_command->presencemask & NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &pdu_session_authentication_command->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_authentication_command->presencemask & NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_authentication_command->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_authentication_complete(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_authentication_complete_t *pdu_session_authentication_complete = &message->gsm.pdu_session_authentication_complete;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_AUTHENTICATION_COMPLETE\n");

    if (pdu_session_authentication_complete->presencemask & NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &pdu_session_authentication_complete->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_authentication_complete->presencemask & NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_authentication_complete->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_authentication_result(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_authentication_result_t *pdu_session_authentication_result = &message->gsm.pdu_session_authentication_result;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_AUTHENTICATION_RESULT\n");

    if (pdu_session_authentication_result->presencemask & NAS_PDU_SESSION_AUTHENTICATION_RESULT_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_AUTHENTICATION_RESULT_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &pdu_session_authentication_result->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_authentication_result->presencemask & NAS_PDU_SESSION_AUTHENTICATION_RESULT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_AUTHENTICATION_RESULT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_authentication_result->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_modification_request(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_modification_request_t *pdu_session_modification_request = &message->gsm.pdu_session_modification_request;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_MODIFICATION_REQUEST\n");

    if (pdu_session_modification_request->presencemask & NAS_PDU_SESSION_MODIFICATION_REQUEST_5GSM_CAPABILITY_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_REQUEST_5GSM_CAPABILITY_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gsm_capability(pkbuf, &pdu_session_modification_request->5gsm_capability);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_request->presencemask & NAS_PDU_SESSION_MODIFICATION_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_maximum_number_of_supported_packet_filters(pkbuf, &pdu_session_modification_request->maximum_number_of_supported_packet_filters);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_request->presencemask & NAS_PDU_SESSION_MODIFICATION_REQUEST_REQUESTED_QOS_RULES_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_REQUEST_REQUESTED_QOS_RULES_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_qos_rules(pkbuf, &pdu_session_modification_request->requested_qos_rules);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_request->presencemask & NAS_PDU_SESSION_MODIFICATION_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_modification_request->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_modification_reject(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_modification_reject_t *pdu_session_modification_reject = &message->gsm.pdu_session_modification_reject;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_MODIFICATION_REJECT\n");

    size = nas_encode_5gsm_cause(pkbuf, &pdu_session_modification_reject->5gsm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (pdu_session_modification_reject->presencemask & NAS_PDU_SESSION_MODIFICATION_REJECT_BACK_OFF_TIMER_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_REJECT_BACK_OFF_TIMER_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_3(pkbuf, &pdu_session_modification_reject->back_off_timer_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_reject->presencemask & NAS_PDU_SESSION_MODIFICATION_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_modification_reject->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_modification_command(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_modification_command_t *pdu_session_modification_command = &message->gsm.pdu_session_modification_command;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_MODIFICATION_COMMAND\n");

    if (pdu_session_modification_command->presencemask & NAS_PDU_SESSION_MODIFICATION_COMMAND_5GSM_CAUSE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMMAND_5GSM_CAUSE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_5gsm_cause(pkbuf, &pdu_session_modification_command->5gsm_cause);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_command->presencemask & NAS_PDU_SESSION_MODIFICATION_COMMAND_SESSION_AMBR_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMMAND_SESSION_AMBR_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_session_ambr(pkbuf, &pdu_session_modification_command->session_ambr);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_command->presencemask & NAS_PDU_SESSION_MODIFICATION_COMMAND_RQ_TIMER_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMMAND_RQ_TIMER_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer(pkbuf, &pdu_session_modification_command->rq_timer_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_command->presencemask & NAS_PDU_SESSION_MODIFICATION_COMMAND_AUTHORIZED_QOS_RULES_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMMAND_AUTHORIZED_QOS_RULES_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_qos_rules(pkbuf, &pdu_session_modification_command->authorized_qos_rules);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_command->presencemask & NAS_PDU_SESSION_MODIFICATION_COMMAND_MAPPED_EPS_BEARER_CONTEXTS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMMAND_MAPPED_EPS_BEARER_CONTEXTS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_mapped_eps_bearer_contexts(pkbuf, &pdu_session_modification_command->mapped_eps_bearer_contexts);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_modification_command->presencemask & NAS_PDU_SESSION_MODIFICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_modification_command->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_modification_complete(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_modification_complete_t *pdu_session_modification_complete = &message->gsm.pdu_session_modification_complete;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_MODIFICATION_COMPLETE\n");

    if (pdu_session_modification_complete->presencemask & NAS_PDU_SESSION_MODIFICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_modification_complete->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_modification_command_reject(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_modification_command_reject_t *pdu_session_modification_command_reject = &message->gsm.pdu_session_modification_command_reject;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_MODIFICATION_COMMAND_REJECT\n");

    size = nas_encode_5gsm_cause(pkbuf, &pdu_session_modification_command_reject->5gsm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (pdu_session_modification_command_reject->presencemask & NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_modification_command_reject->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_release_request(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_release_request_t *pdu_session_release_request = &message->gsm.pdu_session_release_request;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_RELEASE_REQUEST\n");

    if (pdu_session_release_request->presencemask & NAS_PDU_SESSION_RELEASE_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_RELEASE_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_release_request->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_release_reject(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_release_reject_t *pdu_session_release_reject = &message->gsm.pdu_session_release_reject;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_RELEASE_REJECT\n");

    size = nas_encode_5gsm_cause(pkbuf, &pdu_session_release_reject->5gsm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (pdu_session_release_reject->presencemask & NAS_PDU_SESSION_RELEASE_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_RELEASE_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_release_reject->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_release_command(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_release_command_t *pdu_session_release_command = &message->gsm.pdu_session_release_command;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_RELEASE_COMMAND\n");

    size = nas_encode_5gsm_cause(pkbuf, &pdu_session_release_command->5gsm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    if (pdu_session_release_command->presencemask & NAS_PDU_SESSION_RELEASE_COMMAND_BACK_OFF_TIMER_VALUE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_RELEASE_COMMAND_BACK_OFF_TIMER_VALUE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_gprs_timer_3(pkbuf, &pdu_session_release_command->back_off_timer_value);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_release_command->presencemask & NAS_PDU_SESSION_RELEASE_COMMAND_EAP_MESSAGE_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_RELEASE_COMMAND_EAP_MESSAGE_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_eap_message(pkbuf, &pdu_session_release_command->eap_message);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    if (pdu_session_release_command->presencemask & NAS_PDU_SESSION_RELEASE_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_RELEASE_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_release_command->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_pdu_session_release_complete(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_pdu_session_release_complete_t *pdu_session_release_complete = &message->gsm.pdu_session_release_complete;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode PDU_SESSION_RELEASE_COMPLETE\n");

    if (pdu_session_release_complete->presencemask & NAS_PDU_SESSION_RELEASE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT)
    {
        size = nas_encode_optional_type(pkbuf, NAS_PDU_SESSION_RELEASE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;

        size = nas_encode_extended_protocol_configuration_options(pkbuf, &pdu_session_release_complete->extended_protocol_configuration_options);
        d_assert(size >= 0, return encoded, "decode failed");
        encoded += size;
    }

    return encoded;
}

c_int32_t nas_encode_gsm_status(pkbuf_t *pkbuf, nas_message_t *message)
{
    nas_gsm_status_t *gsm_status = &message->gsm.gsm_status;
    c_int32_t encoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Encode GSM_STATUS\n");

    size = nas_encode_5gsm_cause(pkbuf, &gsm_status->5gsm_cause);
    d_assert(size >= 0, return -1, "encode failed");
    encoded += size;

    return encoded;
}

status_t nas_gmm_encode(pkbuf_t **pkbuf, nas_message_t *message)
{
    status_t rv = CORE_ERROR;
    c_int32_t size = 0;
    c_int32_t encoded = 0;

    d_assert(message, return CORE_ERROR, "Null param");

    /* The Packet Buffer(pkbuf_t) for NAS message MUST make a HEADROOM. 
     * When calculating AES_CMAC, we need to use the headroom of the packet. */
    *pkbuf = pkbuf_alloc(NAS_HEADROOM, MAX_SDU_LEN);
    d_assert(*pkbuf, return -1, "Null Param");

    size = sizeof(nas_gmm_header_t);
    rv = pkbuf_header(*pkbuf, -size);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    memcpy((*pkbuf)->payload - size, &message->gmm.h, size);
    encoded += size;

    if (message->gmm.h.security_header_type >=
            NAS_SECURITY_HEADER_FOR_SERVICE_REQUEST_MESSAGE)
    {
        d_assert(pkbuf_header(*pkbuf, 1) == CORE_OK, 
                return CORE_ERROR, "pkbuf_header error");
        encoded -= 1;
        size = nas_encode_service_request(*pkbuf, message);
        d_assert(size >= 0, return CORE_ERROR, "decode error");
        encoded += size;

        goto out;
    }

    switch(message->gmm.h.message_type)
    {
        case NAS_REGISTRATION_REQUEST:
            size = nas_encode_registration_request(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_REGISTRATION_ACCEPT:
            size = nas_encode_registration_accept(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_REGISTRATION_COMPLETE:
            size = nas_encode_registration_complete(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_REGISTRATION_REJECT:
            size = nas_encode_registration_reject(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_DEREGISTRATION_REQUEST:
            size = nas_encode_deregistration_request_to_ue(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_DEREGISTRATION_ACCEPT:
            break;
        case NAS_SERVICE_REJECT:
            size = nas_encode_service_reject(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_SERVICE_ACCEPT:
            size = nas_encode_service_accept(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_CONFIGURATION_UPDATE_COMMAND:
            size = nas_encode_configuration_update_command(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_CONFIGURATION_UPDATE_COMPLETE:
            size = nas_encode_configuration_update_complete(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_AUTHENTICATION_REQUEST:
            size = nas_encode_authentication_request(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_AUTHENTICATION_RESPONSE:
            size = nas_encode_authentication_response(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_AUTHENTICATION_REJECT:
            break;
        case NAS_AUTHENTICATION_FAILURE:
            size = nas_encode_authentication_failure(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_AUTHENTICATION_RESULT:
            size = nas_encode_authentication_result(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_IDENTITY_REQUEST:
            size = nas_encode_identity_request(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_IDENTITY_RESPONSE:
            size = nas_encode_identity_response(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_SECURITY_MODE_COMMAND:
            size = nas_encode_security_mode_command(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_SECURITY_MODE_COMPLETE:
            size = nas_encode_security_mode_complete(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_SECURITY_MODE_REJECT:
            size = nas_encode_security_mode_reject(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_GMM_STATUS:
            size = nas_encode_gmm_status(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_NOTIFICATION:
            size = nas_encode_notification(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_NOTIFICATION_RESPONSE:
            size = nas_encode_notification_response(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_UL_NAS_TRANSPORT:
            size = nas_encode_ul_nas_transport(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_DL_NAS_TRANSPORT:
            size = nas_encode_dl_nas_transport(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gmm.h.message_type);
            pkbuf_free((*pkbuf));
            return CORE_ERROR;
    }

out:
    rv = pkbuf_header(*pkbuf, encoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    (*pkbuf)->len = encoded;

    return CORE_OK;
}

status_t nas_gsm_encode(pkbuf_t **pkbuf, nas_message_t *message)
{
    status_t rv = CORE_ERROR;
    c_int32_t size = 0;
    c_int32_t encoded = 0;

    d_assert(message, return CORE_ERROR, "Null param");

    /* The Packet Buffer(pkbuf_t) for NAS message MUST make a HEADROOM. 
     * When calculating AES_CMAC, we need to use the headroom of the packet. */
    *pkbuf = pkbuf_alloc(NAS_HEADROOM, MAX_SDU_LEN);
    d_assert(*pkbuf, return -1, "Null Param");

    size = sizeof(nas_gsm_header_t);
    rv = pkbuf_header(*pkbuf, -size);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    memcpy((*pkbuf)->payload - size, &message->gsm.h, size);
    encoded += size;

    switch(message->gsm.h.message_type)
    {
        case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST:
            size = nas_encode_pdu_session_establishment_request(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT:
            size = nas_encode_pdu_session_establishment_accept(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_ESTABLISHMENT_REJECT:
            size = nas_encode_pdu_session_establishment_reject(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_AUTHENTICATION_COMMAND:
            size = nas_encode_pdu_session_authentication_command(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_AUTHENTICATION_COMPLETE:
            size = nas_encode_pdu_session_authentication_complete(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_AUTHENTICATION_RESULT:
            size = nas_encode_pdu_session_authentication_result(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_REQUEST:
            size = nas_encode_pdu_session_modification_request(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_REJECT:
            size = nas_encode_pdu_session_modification_reject(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_COMMAND:
            size = nas_encode_pdu_session_modification_command(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_COMPLETE:
            size = nas_encode_pdu_session_modification_complete(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT:
            size = nas_encode_pdu_session_modification_command_reject(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_REQUEST:
            size = nas_encode_pdu_session_release_request(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_REJECT:
            size = nas_encode_pdu_session_release_reject(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_COMMAND:
            size = nas_encode_pdu_session_release_command(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_COMPLETE:
            size = nas_encode_pdu_session_release_complete(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        case NAS_GSM_STATUS:
            size = nas_encode_gsm_status(*pkbuf, message);
            d_assert(size >= 0, return CORE_ERROR, "decode error");
            encoded += size;
            break;
        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gsm.h.message_type);
            pkbuf_free((*pkbuf));
            return CORE_ERROR;
    }

    rv = pkbuf_header(*pkbuf, encoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    (*pkbuf)->len = encoded;

    return CORE_OK;
}

status_t nas_plain_encode(pkbuf_t **pkbuf, nas_message_t *message)
{
    d_assert(message, return CORE_ERROR, "Null param");

    d_assert(message->gmm.h.extended_protocol_discriminator ==
            message->gsm.h.extended_protocol_discriminator, 
            return CORE_ERROR, "check UNION for protocol");

    if (message->gmm.h.extended_protocol_discriminator == 
            NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GMM)
        return nas_emm_encode(pkbuf, message);
    else if (message->gmm.h.extended_protocol_discriminator == 
            NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GSM)
        return nas_esm_encode(pkbuf, message);

    d_assert(0, return CORE_ERROR, 
            "Invalid Protocol : %d", message->gmm.h.extended_protocol_discriminator);
}
