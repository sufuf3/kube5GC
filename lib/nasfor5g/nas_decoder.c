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
 * Created on: 2018-09-20 17:24:08.876502 by waiting
 * from 24501-f00.docx
 ******************************************************************************/

#define TRACE_MODULE _nas_decoder

#include "core_debug.h"
#include "nas_message.h"

c_int32_t nas_decode_registration_request(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_registration_request_t *registration_request = &message->gmm.registration_request;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode REGISTRATION_REQUEST\n");

    size = nas_decode_5gs_registration_type(&registration_request->5gs_registration_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_key_set_identifier(&registration_request->ngksi, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_5gs_mobile_identity(&registration_request->5gs_mobile_identity, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_REGISTRATION_REQUEST_NON_CURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER_TYPE:
                 size = nas_decode_key_set_identifier(&registration_request->non_current_native_nas_key_set_identifier, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_NON_CURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_5GMM_CAPABILITY_TYPE:
                 size = nas_decode_5gmm_capability(&registration_request->5gmm_capability, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_5GMM_CAPABILITY_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_UE_SECURITY_CAPABILITY_TYPE:
                 size = nas_decode_ue_security_capability(&registration_request->ue_security_capability, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_UE_SECURITY_CAPABILITY_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_REQUESTED_NSSAI_TYPE:
                 size = nas_decode_nssai(&registration_request->requested_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_REQUESTED_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_LAST_VISITED_REGISTERED_TAI_TYPE:
                 size = nas_decode_5gs_tracking_area_identity(&registration_request->last_visited_registered_tai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_S1_UE_NETWORK_CAPABILITY_TYPE:
                 size = nas_decode_s1_ue_network_capability(&registration_request->s1_ue_network_capability, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_S1_UE_NETWORK_CAPABILITY_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_UPLINK_DATA_STATUS_TYPE:
                 size = nas_decode_uplink_data_status(&registration_request->uplink_data_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_UPLINK_DATA_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_pdu_session_status(&registration_request->pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_MICO_INDICATION_TYPE:
                 size = nas_decode_mico_indication(&registration_request->mico_indication, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_MICO_INDICATION_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_UE_STATUS_TYPE:
                 size = nas_decode_ue_status(&registration_request->ue_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_UE_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_ADDITIONAL_GUTI_TYPE:
                 size = nas_decode_5gs_mobile_identity(&registration_request->additional_guti, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_ADDITIONAL_GUTI_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_ALLOWED_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_allowed_pdu_session_status(&registration_request->allowed_pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_ALLOWED_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_UE_USAGE_SETTING_TYPE:
                 size = nas_decode_ue_usage_setting(&registration_request->ue_usage_setting, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_UE_USAGE_SETTING_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_REQUESTED_DRX_PARAMETERS_TYPE:
                 size = nas_decode_drx_parameters(&registration_request->requested_drx_parameters, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_REQUESTED_DRX_PARAMETERS_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_EPS_NAS_MESSAGE_CONTAINER_TYPE:
                 size = nas_decode_eps_nas_message_container(&registration_request->eps_nas_message_container, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_EPS_NAS_MESSAGE_CONTAINER_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REQUEST_PAYLOAD_CONTAINER_TYPE:
                 size = nas_decode_payload_container(&registration_request->payload_container, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_request->presencemask |= NAS_REGISTRATION_REQUEST_PAYLOAD_CONTAINER_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_registration_accept(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_registration_accept_t *registration_accept = &message->gmm.registration_accept;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode REGISTRATION_ACCEPT\n");

    size = nas_decode_5gs_registration_result(&registration_accept->5gs_registration_result, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_REGISTRATION_ACCEPT_5G_GUTI_TYPE:
                 size = nas_decode_5gs_mobile_identity(&registration_accept->5g_guti, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_5G_GUTI_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_EQUIVALENT_PLMNS_TYPE:
                 size = nas_decode_plmn_list(&registration_accept->equivalent_plmns, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_EQUIVALENT_PLMNS_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_TAI_LIST_TYPE:
                 size = nas_decode_5gs_tracking_area_identity_list(&registration_accept->tai_list, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_TAI_LIST_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_TYPE:
                 size = nas_decode_nssai(&registration_accept->allowed_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_REJECTED_NSSAI_TYPE:
                 size = nas_decode_rejected_nssai(&registration_accept->rejected_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_REJECTED_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_CONFIGURED_NSSAI_TYPE:
                 size = nas_decode_nssai(&registration_accept->configured_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_CONFIGURED_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_5GS_NETWORK_FEATURE_SUPPORT_TYPE:
                 size = nas_decode_5gs_network_feature_support(&registration_accept->5gs_network_feature_support, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_5GS_NETWORK_FEATURE_SUPPORT_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_pdu_session_status(&registration_accept->pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_TYPE:
                 size = nas_decode_pdu_session_reactivation_result(&registration_accept->pdu_session_reactivation_result, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_TYPE:
                 size = nas_decode_pdu_session_reactivation_result_error_cause(&registration_accept->pdu_session_reactivation_result_error_cause, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_LADN_INFORMATION_TYPE:
                 size = nas_decode_ladn_information(&registration_accept->ladn_information, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_LADN_INFORMATION_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_MICO_INDICATION_TYPE:
                 size = nas_decode_mico_indication(&registration_accept->mico_indication, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_MICO_INDICATION_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_SERVICE_AREA_LIST_TYPE:
                 size = nas_decode_service_area_list(&registration_accept->service_area_list, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_SERVICE_AREA_LIST_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_T3512_VALUE_TYPE:
                 size = nas_decode_gprs_timer_3(&registration_accept->t3512_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_T3512_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_NON__DE_REGISTRATION_TIMER_VALUE_TYPE:
                 size = nas_decode_gprs_timer_2(&registration_accept->non__de_registration_timer_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_NON__DE_REGISTRATION_TIMER_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_T3502_VALUE_TYPE:
                 size = nas_decode_gprs_timer_2(&registration_accept->t3502_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_T3502_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_EMERGENCY_NUMBER_LIST_TYPE:
                 size = nas_decode_emergency_number_list(&registration_accept->emergency_number_list, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_EMERGENCY_NUMBER_LIST_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_EXTENDED_EMERGENCY_NUMBER_LIST_TYPE:
                 size = nas_decode_extended_emergency_number_list(&registration_accept->extended_emergency_number_list, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_EXTENDED_EMERGENCY_NUMBER_LIST_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_TRANSPARENT_CONTAINER_TYPE:
                 size = nas_decode_transparent_container(&registration_accept->transparent_container, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_TRANSPARENT_CONTAINER_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_ACCEPT_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&registration_accept->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_accept->presencemask |= NAS_REGISTRATION_ACCEPT_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_registration_complete(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_registration_complete_t *registration_complete = &message->gmm.registration_complete;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode REGISTRATION_COMPLETE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_REGISTRATION_COMPLETE_TRANSPARENT_CONTAINER_TYPE:
                 size = nas_decode_transparent_container(&registration_complete->transparent_container, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_complete->presencemask |= NAS_REGISTRATION_COMPLETE_TRANSPARENT_CONTAINER_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_registration_reject(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_registration_reject_t *registration_reject = &message->gmm.registration_reject;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode REGISTRATION_REJECT\n");

    size = nas_decode_5gmm_cause(&registration_reject->5gmm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_REGISTRATION_REJECT_T3346_VALUE_TYPE:
                 size = nas_decode_gprs_timer_2(&registration_reject->t3346_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_reject->presencemask |= NAS_REGISTRATION_REJECT_T3346_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REJECT_T3502_VALUE_TYPE:
                 size = nas_decode_gprs_timer_2(&registration_reject->t3502_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_reject->presencemask |= NAS_REGISTRATION_REJECT_T3502_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_REGISTRATION_REJECT_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&registration_reject->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 registration_reject->presencemask |= NAS_REGISTRATION_REJECT_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_deregistration_request_from_ue(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_deregistration_request_from_ue_t *deregistration_request_from_ue = &message->gmm.deregistration_request_from_ue;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode DEREGISTRATION_REQUEST\n");

    size = nas_decode_de_registration_type(&deregistration_request_from_ue->de_registration_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_5gs_mobile_identity(&deregistration_request_from_ue->5gs_mobile_identity, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

c_int32_t nas_decode_deregistration_request_to_ue(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_deregistration_request_to_ue_t *deregistration_request_to_ue = &message->gmm.deregistration_request_to_ue;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode DEREGISTRATION_REQUEST\n");

    size = nas_decode_de_registration_type(&deregistration_request_to_ue->de_registration_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_DEREGISTRATION_REQUEST_5GMM_CAUSE_TYPE:
                 size = nas_decode_5gmm_cause(&deregistration_request_to_ue->5gmm_cause, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 deregistration_request_to_ue->presencemask |= NAS_DEREGISTRATION_REQUEST_5GMM_CAUSE_PRESENT;
                 decoded += size;
                 break;
             case NAS_DEREGISTRATION_REQUEST_T3346_VALUE_TYPE:
                 size = nas_decode_gprs_timer_2(&deregistration_request_to_ue->t3346_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 deregistration_request_to_ue->presencemask |= NAS_DEREGISTRATION_REQUEST_T3346_VALUE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_service_request(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_service_request_t *service_request = &message->gmm.service_request;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode SERVICE_REQUEST\n");

    size = nas_decode_key_set_identifier(&service_request->ngksi, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_5gs_mobile_identity(&service_request->5g_s_tmsi, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_SERVICE_REQUEST_UPLINK_DATA_STATUS_TYPE:
                 size = nas_decode_uplink_data_status(&service_request->uplink_data_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_request->presencemask |= NAS_SERVICE_REQUEST_UPLINK_DATA_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_SERVICE_REQUEST_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_pdu_session_status(&service_request->pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_request->presencemask |= NAS_SERVICE_REQUEST_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_SERVICE_REQUEST_ALLOWED_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_allowed_pdu_session_status(&service_request->allowed_pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_request->presencemask |= NAS_SERVICE_REQUEST_ALLOWED_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_service_reject(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_service_reject_t *service_reject = &message->gmm.service_reject;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode SERVICE_REJECT\n");

    size = nas_decode_5gmm_cause(&service_reject->5gmm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_SERVICE_REJECT_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_pdu_session_status(&service_reject->pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_reject->presencemask |= NAS_SERVICE_REJECT_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_SERVICE_REJECT_T3346_VALUE_TYPE:
                 size = nas_decode_gprs_timer_2(&service_reject->t3346_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_reject->presencemask |= NAS_SERVICE_REJECT_T3346_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_SERVICE_REJECT_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&service_reject->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_reject->presencemask |= NAS_SERVICE_REJECT_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_service_accept(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_service_accept_t *service_accept = &message->gmm.service_accept;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode SERVICE_ACCEPT\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_SERVICE_ACCEPT_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_pdu_session_status(&service_accept->pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_accept->presencemask |= NAS_SERVICE_ACCEPT_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             case NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_TYPE:
                 size = nas_decode_pdu_session_reactivation_result(&service_accept->pdu_session_reactivation_result, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_accept->presencemask |= NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_PRESENT;
                 decoded += size;
                 break;
             case NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_TYPE:
                 size = nas_decode_pdu_session_reactivation_result_error_cause(&service_accept->pdu_session_reactivation_result_error_cause, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_accept->presencemask |= NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_PRESENT;
                 decoded += size;
                 break;
             case NAS_SERVICE_ACCEPT_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&service_accept->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 service_accept->presencemask |= NAS_SERVICE_ACCEPT_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_configuration_update_command(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_configuration_update_command_t *configuration_update_command = &message->gmm.configuration_update_command;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode CONFIGURATION_UPDATE_COMMAND\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURATION_UPDATE_INDICATION_TYPE:
                 size = nas_decode_configuration_update_indication(&configuration_update_command->configuration_update_indication, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURATION_UPDATE_INDICATION_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_5G_GUTI_TYPE:
                 size = nas_decode_5gs_mobile_identity(&configuration_update_command->5g_guti, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_5G_GUTI_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_TAI_LIST_TYPE:
                 size = nas_decode_5gs_tracking_area_identity_list(&configuration_update_command->tai_list, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_TAI_LIST_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_ALLOWED_NSSAI_TYPE:
                 size = nas_decode_nssai(&configuration_update_command->allowed_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_ALLOWED_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_SERVICE_AREA_LIST_TYPE:
                 size = nas_decode_service_area_list(&configuration_update_command->service_area_list, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_SERVICE_AREA_LIST_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_FULL_NAME_FOR_NETWORK_TYPE:
                 size = nas_decode_network_name(&configuration_update_command->full_name_for_network, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_FULL_NAME_FOR_NETWORK_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_SHORT_NAME_FOR_NETWORK_TYPE:
                 size = nas_decode_network_name(&configuration_update_command->short_name_for_network, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_SHORT_NAME_FOR_NETWORK_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_LOCAL_TIME_ZONE_TYPE:
                 size = nas_decode_time_zone(&configuration_update_command->local_time_zone, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_LOCAL_TIME_ZONE_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_TYPE:
                 size = nas_decode_time_zone_and_time(&configuration_update_command->universal_time_and_local_time_zone, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_NETWORK_DAYLIGHT_SAVING_TIME_TYPE:
                 size = nas_decode_daylight_saving_time(&configuration_update_command->network_daylight_saving_time, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_NETWORK_DAYLIGHT_SAVING_TIME_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_LADN_INFORMATION_TYPE:
                 size = nas_decode_ladn_information(&configuration_update_command->ladn_information, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_LADN_INFORMATION_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_MICO_INDICATION_TYPE:
                 size = nas_decode_mico_indication(&configuration_update_command->mico_indication, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_MICO_INDICATION_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURED_NSSAI_TYPE:
                 size = nas_decode_nssai(&configuration_update_command->configured_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURED_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMMAND_REJECTED_NSSAI_TYPE:
                 size = nas_decode_rejected_nssai(&configuration_update_command->rejected_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_command->presencemask |= NAS_CONFIGURATION_UPDATE_COMMAND_REJECTED_NSSAI_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_configuration_update_complete(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_configuration_update_complete_t *configuration_update_complete = &message->gmm.configuration_update_complete;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode CONFIGURATION_UPDATE_COMPLETE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_CONFIGURATION_UPDATE_COMPLETE_PDU_SESSION_TYPE_TYPE:
                 size = nas_decode_pdu_session_type(&configuration_update_complete->pdu_session_type, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_complete->presencemask |= NAS_CONFIGURATION_UPDATE_COMPLETE_PDU_SESSION_TYPE_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMPLETE_SSC_MODE_TYPE:
                 size = nas_decode_ssc_mode(&configuration_update_complete->ssc_mode, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_complete->presencemask |= NAS_CONFIGURATION_UPDATE_COMPLETE_SSC_MODE_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMPLETE_5GSM_CAPABILITY_TYPE:
                 size = nas_decode_5gsm_capability(&configuration_update_complete->5gsm_capability, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_complete->presencemask |= NAS_CONFIGURATION_UPDATE_COMPLETE_5GSM_CAPABILITY_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMPLETE_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE:
                 size = nas_decode_maximum_number_of_supported_packet_filters(&configuration_update_complete->maximum_number_of_supported_packet_filters, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_complete->presencemask |= NAS_CONFIGURATION_UPDATE_COMPLETE_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMPLETE_SM_PDU_DN_REQUEST_CONTAINER_TYPE:
                 size = nas_decode_sm_pdu_dn_request_container(&configuration_update_complete->sm_pdu_dn_request_container, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_complete->presencemask |= NAS_CONFIGURATION_UPDATE_COMPLETE_SM_PDU_DN_REQUEST_CONTAINER_PRESENT;
                 decoded += size;
                 break;
             case NAS_CONFIGURATION_UPDATE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&configuration_update_complete->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 configuration_update_complete->presencemask |= NAS_CONFIGURATION_UPDATE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_authentication_request(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_authentication_request_t *authentication_request = &message->gmm.authentication_request;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode AUTHENTICATION_REQUEST\n");

    size = nas_decode_key_set_identifier(&authentication_request->ngksi, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_RAND_TYPE:
                 size = nas_decode_authentication_parameter(&authentication_request->authentication_parameter_rand, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 authentication_request->presencemask |= NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_RAND_PRESENT;
                 decoded += size;
                 break;
             case NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_AUTN_TYPE:
                 size = nas_decode_authentication_parameter_autn(&authentication_request->authentication_parameter_autn, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 authentication_request->presencemask |= NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_AUTN_PRESENT;
                 decoded += size;
                 break;
             case NAS_AUTHENTICATION_REQUEST_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&authentication_request->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 authentication_request->presencemask |= NAS_AUTHENTICATION_REQUEST_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_authentication_response(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_authentication_response_t *authentication_response = &message->gmm.authentication_response;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode AUTHENTICATION_RESPONSE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_AUTHENTICATION_RESPONSE_AUTHENTICATION_RESPONSE_PARAMETER_TYPE:
                 size = nas_decode_authentication_response_parameter(&authentication_response->authentication_response_parameter, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 authentication_response->presencemask |= NAS_AUTHENTICATION_RESPONSE_AUTHENTICATION_RESPONSE_PARAMETER_PRESENT;
                 decoded += size;
                 break;
             case NAS_AUTHENTICATION_RESPONSE_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&authentication_response->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 authentication_response->presencemask |= NAS_AUTHENTICATION_RESPONSE_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_authentication_failure(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_authentication_failure_t *authentication_failure = &message->gmm.authentication_failure;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode AUTHENTICATION_FAILURE\n");

    size = nas_decode_5gmm_cause(&authentication_failure->5gmm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_AUTHENTICATION_FAILURE_AUTHENTICATION_FAILURE_PARAMETER_TYPE:
                 size = nas_decode_authentication_failure_parameter(&authentication_failure->authentication_failure_parameter, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 authentication_failure->presencemask |= NAS_AUTHENTICATION_FAILURE_AUTHENTICATION_FAILURE_PARAMETER_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_authentication_result(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_authentication_result_t *authentication_result = &message->gmm.authentication_result;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode AUTHENTICATION_RESULT\n");

    size = nas_decode_key_set_identifier(&authentication_result->ngksi, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_eap_message(&authentication_result->eap_message, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

c_int32_t nas_decode_identity_request(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_identity_request_t *identity_request = &message->gmm.identity_request;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode IDENTITY_REQUEST\n");

    size = nas_decode_5gs_identity_type(&identity_request->identity_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

c_int32_t nas_decode_identity_response(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_identity_response_t *identity_response = &message->gmm.identity_response;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode IDENTITY_RESPONSE\n");

    size = nas_decode_5gs_mobile_identity(&identity_response->mobile_identity, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

c_int32_t nas_decode_security_mode_command(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_security_mode_command_t *security_mode_command = &message->gmm.security_mode_command;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode SECURITY_MODE_COMMAND\n");

    size = nas_decode_security_algorithms(&security_mode_command->selected_nas_security_algorithms, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_key_set_identifier(&security_mode_command->ngksi, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_ue_security_capability(&security_mode_command->replayed_ue_security_capabilities, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_SECURITY_MODE_COMMAND_IMEISV_REQUEST_TYPE:
                 size = nas_decode_imeisv_request(&security_mode_command->imeisv_request, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 security_mode_command->presencemask |= NAS_SECURITY_MODE_COMMAND_IMEISV_REQUEST_PRESENT;
                 decoded += size;
                 break;
             case NAS_SECURITY_MODE_COMMAND_HASHAMF_TYPE:
                 size = nas_decode_hashamf(&security_mode_command->hashamf, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 security_mode_command->presencemask |= NAS_SECURITY_MODE_COMMAND_HASHAMF_PRESENT;
                 decoded += size;
                 break;
             case NAS_SECURITY_MODE_COMMAND_SELECTED_EPS_NAS_SECURITY_ALGORITHMS_TYPE:
                 size = nas_decode_eps_nas_security_algorithms(&security_mode_command->selected_eps_nas_security_algorithms, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 security_mode_command->presencemask |= NAS_SECURITY_MODE_COMMAND_SELECTED_EPS_NAS_SECURITY_ALGORITHMS_PRESENT;
                 decoded += size;
                 break;
             case NAS_SECURITY_MODE_COMMAND_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&security_mode_command->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 security_mode_command->presencemask |= NAS_SECURITY_MODE_COMMAND_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_security_mode_complete(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_security_mode_complete_t *security_mode_complete = &message->gmm.security_mode_complete;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode SECURITY_MODE_COMPLETE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_SECURITY_MODE_COMPLETE_IMEISV_TYPE:
                 size = nas_decode_5g_mobile_identity(&security_mode_complete->imeisv, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 security_mode_complete->presencemask |= NAS_SECURITY_MODE_COMPLETE_IMEISV_PRESENT;
                 decoded += size;
                 break;
             case NAS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_TYPE:
                 size = nas_decode_message_container(&security_mode_complete->nas_message_container, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 security_mode_complete->presencemask |= NAS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_security_mode_reject(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_security_mode_reject_t *security_mode_reject = &message->gmm.security_mode_reject;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode SECURITY_MODE_REJECT\n");

    size = nas_decode_5gmm_cause(&security_mode_reject->5gmm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

c_int32_t nas_decode_gmm_status(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_gmm_status_t *gmm_status = &message->gmm.gmm_status;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode GMM_STATUS\n");

    size = nas_decode_5gmm_cause(&gmm_status->5gmm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

c_int32_t nas_decode_notification(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_notification_t *notification = &message->gmm.notification;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode NOTIFICATION\n");

    size = nas_decode_access_type(&notification->access_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

c_int32_t nas_decode_notification_response(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_notification_response_t *notification_response = &message->gmm.notification_response;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode NOTIFICATION_RESPONSE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_NOTIFICATION_RESPONSE_PDU_SESSION_STATUS_TYPE:
                 size = nas_decode_pdu_session_status(&notification_response->pdu_session_status, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 notification_response->presencemask |= NAS_NOTIFICATION_RESPONSE_PDU_SESSION_STATUS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_ul_nas_transport(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_ul_nas_transport_t *ul_nas_transport = &message->gmm.ul_nas_transport;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode UL_NAS_TRANSPORT\n");

    size = nas_decode_payload_container_type(&ul_nas_transport->payload_container_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_payload_container(&ul_nas_transport->payload_container, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_UL_NAS_TRANSPORT_OLD_PDU_SESSION_ID_TYPE:
                 size = nas_decode_pdu_session_identity_2(&ul_nas_transport->old_pdu_session_id, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 ul_nas_transport->presencemask |= NAS_UL_NAS_TRANSPORT_OLD_PDU_SESSION_ID_PRESENT;
                 decoded += size;
                 break;
             case NAS_UL_NAS_TRANSPORT_REQUEST_TYPE_TYPE:
                 size = nas_decode_request_type(&ul_nas_transport->request_type, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 ul_nas_transport->presencemask |= NAS_UL_NAS_TRANSPORT_REQUEST_TYPE_PRESENT;
                 decoded += size;
                 break;
             case NAS_UL_NAS_TRANSPORT_S_NSSAI_TYPE:
                 size = nas_decode_s_nssai(&ul_nas_transport->s_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 ul_nas_transport->presencemask |= NAS_UL_NAS_TRANSPORT_S_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_UL_NAS_TRANSPORT_DNN_TYPE:
                 size = nas_decode_dnn(&ul_nas_transport->dnn, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 ul_nas_transport->presencemask |= NAS_UL_NAS_TRANSPORT_DNN_PRESENT;
                 decoded += size;
                 break;
             case NAS_UL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_TYPE:
                 size = nas_decode_additional_information(&ul_nas_transport->additional_information, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 ul_nas_transport->presencemask |= NAS_UL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_dl_nas_transport(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_dl_nas_transport_t *dl_nas_transport = &message->gmm.dl_nas_transport;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode DL_NAS_TRANSPORT\n");

    size = nas_decode_payload_container_type(&dl_nas_transport->payload_container_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_payload_container(&dl_nas_transport->payload_container, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_DL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_TYPE:
                 size = nas_decode_additional_information(&dl_nas_transport->additional_information, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 dl_nas_transport->presencemask |= NAS_DL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_PRESENT;
                 decoded += size;
                 break;
             case NAS_DL_NAS_TRANSPORT_5GMM_CAUSE_TYPE:
                 size = nas_decode_5gmm_cause(&dl_nas_transport->5gmm_cause, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 dl_nas_transport->presencemask |= NAS_DL_NAS_TRANSPORT_5GMM_CAUSE_PRESENT;
                 decoded += size;
                 break;
             case NAS_DL_NAS_TRANSPORT_BACK_OFF_TIMER_VALUE_TYPE:
                 size = nas_decode_gprs_timer_3(&dl_nas_transport->back_off_timer_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 dl_nas_transport->presencemask |= NAS_DL_NAS_TRANSPORT_BACK_OFF_TIMER_VALUE_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_establishment_request(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_establishment_request_t *pdu_session_establishment_request = &message->gsm.pdu_session_establishment_request;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_ESTABLISHMENT_REQUEST\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_PDU_SESSION_TYPE_TYPE:
                 size = nas_decode_pdu_session_type(&pdu_session_establishment_request->pdu_session_type, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_request->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_PDU_SESSION_TYPE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SSC_MODE_TYPE:
                 size = nas_decode_ssc_mode(&pdu_session_establishment_request->ssc_mode, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_request->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SSC_MODE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_5GSM_CAPABILITY_TYPE:
                 size = nas_decode_5gsm_capability(&pdu_session_establishment_request->5gsm_capability, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_request->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_5GSM_CAPABILITY_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE:
                 size = nas_decode_maximum_number_of_supported_packet_filters(&pdu_session_establishment_request->maximum_number_of_supported_packet_filters, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_request->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SM_PDU_DN_REQUEST_CONTAINER_TYPE:
                 size = nas_decode_sm_pdu_dn_request_container(&pdu_session_establishment_request->sm_pdu_dn_request_container, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_request->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SM_PDU_DN_REQUEST_CONTAINER_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_establishment_request->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_request->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_establishment_accept(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_establishment_accept_t *pdu_session_establishment_accept = &message->gsm.pdu_session_establishment_accept;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_ESTABLISHMENT_ACCEPT\n");

    size = nas_decode_pdu_session_type(&pdu_session_establishment_accept->selected_pdu_session_type, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_dnn(&pdu_session_establishment_accept->dnn, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_qos_rules(&pdu_session_establishment_accept->authorized_qos_rules, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    size = nas_decode_session_ambr(&pdu_session_establishment_accept->session_ambr, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_5GSM_CAUSE_TYPE:
                 size = nas_decode_5gsm_cause(&pdu_session_establishment_accept->5gsm_cause, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_accept->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_5GSM_CAUSE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_PDU_ADDRESS_TYPE:
                 size = nas_decode_pdu_address(&pdu_session_establishment_accept->pdu_address, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_accept->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_PDU_ADDRESS_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_RQ_TIMER_VALUE_TYPE:
                 size = nas_decode_gprs_timer(&pdu_session_establishment_accept->rq_timer_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_accept->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_RQ_TIMER_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_S_NSSAI_TYPE:
                 size = nas_decode_s_nssai(&pdu_session_establishment_accept->s_nssai, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_accept->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_S_NSSAI_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_MAPPED_EPS_BEARER_CONTEXTS_TYPE:
                 size = nas_decode_mapped_eps_bearer_contexts(&pdu_session_establishment_accept->mapped_eps_bearer_contexts, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_accept->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_MAPPED_EPS_BEARER_CONTEXTS_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&pdu_session_establishment_accept->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_accept->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_establishment_accept->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_accept->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_establishment_reject(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_establishment_reject_t *pdu_session_establishment_reject = &message->gsm.pdu_session_establishment_reject;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_ESTABLISHMENT_REJECT\n");

    size = nas_decode_5gsm_cause(&pdu_session_establishment_reject->5gsm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_ESTABLISHMENT_REJECT_BACK_OFF_TIMER_VALUE_TYPE:
                 size = nas_decode_gprs_timer_3(&pdu_session_establishment_reject->back_off_timer_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_reject->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REJECT_BACK_OFF_TIMER_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REJECT_ALLOWED_SSC_MODE_TYPE:
                 size = nas_decode_allowed_ssc_mode(&pdu_session_establishment_reject->allowed_ssc_mode, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_reject->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REJECT_ALLOWED_SSC_MODE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&pdu_session_establishment_reject->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_reject->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_establishment_reject->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_establishment_reject->presencemask |= NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_authentication_command(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_authentication_command_t *pdu_session_authentication_command = &message->gsm.pdu_session_authentication_command;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_AUTHENTICATION_COMMAND\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&pdu_session_authentication_command->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_authentication_command->presencemask |= NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_authentication_command->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_authentication_command->presencemask |= NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_authentication_complete(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_authentication_complete_t *pdu_session_authentication_complete = &message->gsm.pdu_session_authentication_complete;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_AUTHENTICATION_COMPLETE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&pdu_session_authentication_complete->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_authentication_complete->presencemask |= NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_authentication_complete->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_authentication_complete->presencemask |= NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_authentication_result(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_authentication_result_t *pdu_session_authentication_result = &message->gsm.pdu_session_authentication_result;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_AUTHENTICATION_RESULT\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_AUTHENTICATION_RESULT_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&pdu_session_authentication_result->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_authentication_result->presencemask |= NAS_PDU_SESSION_AUTHENTICATION_RESULT_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_AUTHENTICATION_RESULT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_authentication_result->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_authentication_result->presencemask |= NAS_PDU_SESSION_AUTHENTICATION_RESULT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_modification_request(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_modification_request_t *pdu_session_modification_request = &message->gsm.pdu_session_modification_request;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_MODIFICATION_REQUEST\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_MODIFICATION_REQUEST_5GSM_CAPABILITY_TYPE:
                 size = nas_decode_5gsm_capability(&pdu_session_modification_request->5gsm_capability, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_request->presencemask |= NAS_PDU_SESSION_MODIFICATION_REQUEST_5GSM_CAPABILITY_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE:
                 size = nas_decode_maximum_number_of_supported_packet_filters(&pdu_session_modification_request->maximum_number_of_supported_packet_filters, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_request->presencemask |= NAS_PDU_SESSION_MODIFICATION_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_REQUEST_REQUESTED_QOS_RULES_TYPE:
                 size = nas_decode_qos_rules(&pdu_session_modification_request->requested_qos_rules, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_request->presencemask |= NAS_PDU_SESSION_MODIFICATION_REQUEST_REQUESTED_QOS_RULES_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_modification_request->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_request->presencemask |= NAS_PDU_SESSION_MODIFICATION_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_modification_reject(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_modification_reject_t *pdu_session_modification_reject = &message->gsm.pdu_session_modification_reject;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_MODIFICATION_REJECT\n");

    size = nas_decode_5gsm_cause(&pdu_session_modification_reject->5gsm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_MODIFICATION_REJECT_BACK_OFF_TIMER_VALUE_TYPE:
                 size = nas_decode_gprs_timer_3(&pdu_session_modification_reject->back_off_timer_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_reject->presencemask |= NAS_PDU_SESSION_MODIFICATION_REJECT_BACK_OFF_TIMER_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_modification_reject->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_reject->presencemask |= NAS_PDU_SESSION_MODIFICATION_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_modification_command(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_modification_command_t *pdu_session_modification_command = &message->gsm.pdu_session_modification_command;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_MODIFICATION_COMMAND\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_MODIFICATION_COMMAND_5GSM_CAUSE_TYPE:
                 size = nas_decode_5gsm_cause(&pdu_session_modification_command->5gsm_cause, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_command->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMMAND_5GSM_CAUSE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_COMMAND_SESSION_AMBR_TYPE:
                 size = nas_decode_session_ambr(&pdu_session_modification_command->session_ambr, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_command->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMMAND_SESSION_AMBR_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_COMMAND_RQ_TIMER_VALUE_TYPE:
                 size = nas_decode_gprs_timer(&pdu_session_modification_command->rq_timer_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_command->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMMAND_RQ_TIMER_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_COMMAND_AUTHORIZED_QOS_RULES_TYPE:
                 size = nas_decode_qos_rules(&pdu_session_modification_command->authorized_qos_rules, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_command->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMMAND_AUTHORIZED_QOS_RULES_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_COMMAND_MAPPED_EPS_BEARER_CONTEXTS_TYPE:
                 size = nas_decode_mapped_eps_bearer_contexts(&pdu_session_modification_command->mapped_eps_bearer_contexts, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_command->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMMAND_MAPPED_EPS_BEARER_CONTEXTS_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_MODIFICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_modification_command->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_command->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_modification_complete(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_modification_complete_t *pdu_session_modification_complete = &message->gsm.pdu_session_modification_complete;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_MODIFICATION_COMPLETE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_MODIFICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_modification_complete->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_complete->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_modification_command_reject(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_modification_command_reject_t *pdu_session_modification_command_reject = &message->gsm.pdu_session_modification_command_reject;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_MODIFICATION_COMMAND_REJECT\n");

    size = nas_decode_5gsm_cause(&pdu_session_modification_command_reject->5gsm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_modification_command_reject->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_modification_command_reject->presencemask |= NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_release_request(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_release_request_t *pdu_session_release_request = &message->gsm.pdu_session_release_request;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_RELEASE_REQUEST\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_RELEASE_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_release_request->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_release_request->presencemask |= NAS_PDU_SESSION_RELEASE_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_release_reject(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_release_reject_t *pdu_session_release_reject = &message->gsm.pdu_session_release_reject;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_RELEASE_REJECT\n");

    size = nas_decode_5gsm_cause(&pdu_session_release_reject->5gsm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_RELEASE_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_release_reject->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_release_reject->presencemask |= NAS_PDU_SESSION_RELEASE_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_release_command(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_release_command_t *pdu_session_release_command = &message->gsm.pdu_session_release_command;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_RELEASE_COMMAND\n");

    size = nas_decode_5gsm_cause(&pdu_session_release_command->5gsm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_RELEASE_COMMAND_BACK_OFF_TIMER_VALUE_TYPE:
                 size = nas_decode_gprs_timer_3(&pdu_session_release_command->back_off_timer_value, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_release_command->presencemask |= NAS_PDU_SESSION_RELEASE_COMMAND_BACK_OFF_TIMER_VALUE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_RELEASE_COMMAND_EAP_MESSAGE_TYPE:
                 size = nas_decode_eap_message(&pdu_session_release_command->eap_message, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_release_command->presencemask |= NAS_PDU_SESSION_RELEASE_COMMAND_EAP_MESSAGE_PRESENT;
                 decoded += size;
                 break;
             case NAS_PDU_SESSION_RELEASE_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_release_command->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_release_command->presencemask |= NAS_PDU_SESSION_RELEASE_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_pdu_session_release_complete(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_pdu_session_release_complete_t *pdu_session_release_complete = &message->gsm.pdu_session_release_complete;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode PDU_SESSION_RELEASE_COMPLETE\n");

    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
             case NAS_PDU_SESSION_RELEASE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE:
                 size = nas_decode_extended_protocol_configuration_options(&pdu_session_release_complete->extended_protocol_configuration_options, pkbuf);
                 d_assert(size >= 0, return -1, "decode failed");
                 pdu_session_release_complete->presencemask |= NAS_PDU_SESSION_RELEASE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT;
                 decoded += size;
                 break;
             default:
                d_warn("Unknown type(0x%x) or not implemented\n", type);
                break;
        }
    }

    return decoded;
}

c_int32_t nas_decode_gsm_status(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_gsm_status_t *gsm_status = &message->gsm.gsm_status;
    c_int32_t decoded = 0;
    c_int32_t size = 0;

    d_trace(25, "[NAS] Decode GSM_STATUS\n");

    size = nas_decode_5gsm_cause(&gsm_status->5gsm_cause, pkbuf);
    d_assert(size >= 0, return -1, "decode failed");
    decoded += size;

    return decoded;
}

status_t nas_gmm_decode(nas_message_t *message, pkbuf_t *pkbuf)
{
    status_t rv = CORE_ERROR;
    c_uint16_t size = 0;
    c_uint16_t decoded = 0;

    d_assert(pkbuf, return CORE_ERROR, "Null param");
    d_assert(pkbuf->payload, return CORE_ERROR, "Null param");

    memset(message, 0, sizeof(nas_message_t));

    size = sizeof(nas_gmm_header_t);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, 
            return CORE_ERROR, "pkbuf_header error");
    memcpy(&message->gmm.h, pkbuf->payload - size, size);
    decoded += size;

    if (message->gmm.h.security_header_type >=
            NAS_SECURITY_HEADER_FOR_SERVICE_REQUEST_MESSAGE)
    {
        d_assert(pkbuf_header(pkbuf, 1) == CORE_OK, 
                return CORE_ERROR, "pkbuf_header error");
        decoded -= 1;
        size = nas_decode_service_request(message, pkbuf);
        d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
        decoded += size;

        goto out;
    }

    switch(message->gmm.h.message_type)
    {
        case NAS_REGISTRATION_REQUEST:
            size = nas_decode_registration_request(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_REGISTRATION_ACCEPT:
            size = nas_decode_registration_accept(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_REGISTRATION_COMPLETE:
            size = nas_decode_registration_complete(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_REGISTRATION_REJECT:
            size = nas_decode_registration_reject(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_DEREGISTRATION_REQUEST:
            size = nas_decode_deregistration_request_from_ue(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_DEREGISTRATION_ACCEPT:
            break;
        case NAS_SERVICE_REJECT:
            size = nas_decode_service_reject(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_SERVICE_ACCEPT:
            size = nas_decode_service_accept(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_CONFIGURATION_UPDATE_COMMAND:
            size = nas_decode_configuration_update_command(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_CONFIGURATION_UPDATE_COMPLETE:
            size = nas_decode_configuration_update_complete(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_AUTHENTICATION_REQUEST:
            size = nas_decode_authentication_request(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_AUTHENTICATION_RESPONSE:
            size = nas_decode_authentication_response(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_AUTHENTICATION_REJECT:
            break;
        case NAS_AUTHENTICATION_FAILURE:
            size = nas_decode_authentication_failure(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_AUTHENTICATION_RESULT:
            size = nas_decode_authentication_result(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_IDENTITY_REQUEST:
            size = nas_decode_identity_request(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_IDENTITY_RESPONSE:
            size = nas_decode_identity_response(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_SECURITY_MODE_COMMAND:
            size = nas_decode_security_mode_command(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_SECURITY_MODE_COMPLETE:
            size = nas_decode_security_mode_complete(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_SECURITY_MODE_REJECT:
            size = nas_decode_security_mode_reject(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_GMM_STATUS:
            size = nas_decode_gmm_status(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_NOTIFICATION:
            size = nas_decode_notification(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_NOTIFICATION_RESPONSE:
            size = nas_decode_notification_response(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_UL_NAS_TRANSPORT:
            size = nas_decode_ul_nas_transport(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_DL_NAS_TRANSPORT:
            size = nas_decode_dl_nas_transport(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gmm.h.message_type);
            break;
    }

out:
    rv = pkbuf_header(pkbuf, decoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    return CORE_OK;
}
status_t nas_gsm_decode(nas_message_t *message, pkbuf_t *pkbuf)
{
    status_t rv = CORE_ERROR;
    c_uint16_t size = 0;
    c_uint16_t decoded = 0;

    d_assert(pkbuf, return CORE_ERROR, "Null param");
    d_assert(pkbuf->payload, return CORE_ERROR, "Null param");

    memset(message, 0, sizeof(nas_message_t));

    size = sizeof(nas_gsm_header_t);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, 
            return CORE_ERROR, "pkbuf_header error");
    memcpy(&message->gsm.h, pkbuf->payload - size, size);
    decoded += size;

    switch(message->gsm.h.message_type)
    {
        case NAS_PDU_SESSION_ESTABLISHMENT_REQUEST:
            size = nas_decode_pdu_session_establishment_request(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT:
            size = nas_decode_pdu_session_establishment_accept(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_ESTABLISHMENT_REJECT:
            size = nas_decode_pdu_session_establishment_reject(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_AUTHENTICATION_COMMAND:
            size = nas_decode_pdu_session_authentication_command(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_AUTHENTICATION_COMPLETE:
            size = nas_decode_pdu_session_authentication_complete(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_AUTHENTICATION_RESULT:
            size = nas_decode_pdu_session_authentication_result(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_REQUEST:
            size = nas_decode_pdu_session_modification_request(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_REJECT:
            size = nas_decode_pdu_session_modification_reject(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_COMMAND:
            size = nas_decode_pdu_session_modification_command(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_COMPLETE:
            size = nas_decode_pdu_session_modification_complete(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT:
            size = nas_decode_pdu_session_modification_command_reject(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_REQUEST:
            size = nas_decode_pdu_session_release_request(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_REJECT:
            size = nas_decode_pdu_session_release_reject(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_COMMAND:
            size = nas_decode_pdu_session_release_command(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_PDU_SESSION_RELEASE_COMPLETE:
            size = nas_decode_pdu_session_release_complete(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        case NAS_GSM_STATUS:
            size = nas_decode_gsm_status(message, pkbuf);
            d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
            decoded += size;
            break;
        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gsm.h.message_type);
            break;
    }

    rv = pkbuf_header(pkbuf, decoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    return CORE_OK;
}

#if 0 /* deprecated */
status_t nas_plain_decode(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_security_header_t *h = NULL;

    d_assert(pkbuf, return CORE_ERROR, "Null param");
    h = pkbuf->payload;
    d_assert(h, return CORE_ERROR, "Null param");

    if (h->extended_protocol_discriminator == NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GMM)
        return nas_gmm_decode(message, pkbuf);
    else if (h->extended_protocol_discriminator == NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GSM)
        return nas_gsm_decode(message, pkbuf);

    d_assert(0, return CORE_ERROR, 
            "Invalid Protocol : %d", h->extended_protocol_discriminator);
}
#endif
