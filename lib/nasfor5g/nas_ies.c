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
 * Created on: 2018-09-20 17:24:08.859156 by waiting
 * from 24501-f00.docx
 ******************************************************************************/

#define TRACE_MODULE _nas_ies

#include "core_debug.h"
#include "core_lib.h"
#include "nas_ies.h"

c_int16_t nas_encode_optional_type(pkbuf_t *pkbuf, c_uint8_t type)
{
    c_uint16_t size = sizeof(c_uint8_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &type, size);

    return size;
}
/* 9.10.2.1 Additional information
 * O TLV 3-n */
c_int16_t nas_decode_additional_information(nas_additional_information_t *additional_information, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_additional_information_t *source = pkbuf->payload;

    additional_information->length = source->length;
    size = additional_information->length + sizeof(additional_information->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(additional_information, pkbuf->payload - size, size);

    d_trace(25, "  ADDITIONAL_INFORMATION - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_additional_information(pkbuf_t *pkbuf, nas_additional_information_t *additional_information)
{
    c_uint16_t size = additional_information->length + sizeof(additional_information->length);
    nas_additional_information_t target;

    memcpy(&target, additional_information, sizeof(nas_additional_information_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  ADDITIONAL_INFORMATION - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.2.2 EAP message
 * O TLV-E 7-1503 */
c_int16_t nas_decode_eap_message(nas_eap_message_t *eap_message, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_eap_message_t *source = pkbuf->payload;

    eap_message->length = ntohs(source->length);
    size = eap_message->length + sizeof(eap_message->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    eap_message->buffer = pkbuf->payload - size + sizeof(eap_message->length);

    d_trace(25, "  EAP_MESSAGE - ");
    d_trace_hex(25, eap_message->buffer, eap_message->length);

    return size;
}

c_int16_t nas_encode_eap_message(pkbuf_t *pkbuf, nas_eap_message_t *eap_message)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(eap_message, return -1, "Null param");
    d_assert(eap_message->buffer, return -1, "Null param");

    size = sizeof(eap_message->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(eap_message->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = eap_message->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, eap_message->buffer, size);

    d_trace(25, "  EAP_MESSAGE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return eap_message->length + sizeof(eap_message->length);
}

/* 9.10.2.3 GPRS timer
 * O TV 2 */
c_int16_t nas_decode_gprs_timer(nas_gprs_timer_t *gprs_timer, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_gprs_timer_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(gprs_timer, pkbuf->payload - size, size);

    d_trace(25, "  GPRS_TIMER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_gprs_timer(pkbuf_t *pkbuf, nas_gprs_timer_t *gprs_timer)
{
    c_uint16_t size = sizeof(nas_gprs_timer_t);
    nas_gprs_timer_t target;

    memcpy(&target, gprs_timer, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  GPRS_TIMER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.2.5 GPRS timer 3
 * O TLV 3 */
c_int16_t nas_decode_gprs_timer_3(nas_gprs_timer_3_t *gprs_timer_3, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_gprs_timer_3_t *source = pkbuf->payload;

    gprs_timer_3->length = source->length;
    size = gprs_timer_3->length + sizeof(gprs_timer_3->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(gprs_timer_3, pkbuf->payload - size, size);

    d_trace(25, "  GPRS_TIMER_3 - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_gprs_timer_3(pkbuf_t *pkbuf, nas_gprs_timer_3_t *gprs_timer_3)
{
    c_uint16_t size = gprs_timer_3->length + sizeof(gprs_timer_3->length);
    nas_gprs_timer_3_t target;

    memcpy(&target, gprs_timer_3, sizeof(nas_gprs_timer_3_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  GPRS_TIMER_3 - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.2.6 S-NSSAI
 * O TLV 3-10 */
c_int16_t nas_decode_s_nssai(nas_s_nssai_t *s_nssai, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_s_nssai_t *source = pkbuf->payload;

    s_nssai->length = source->length;
    size = s_nssai->length + sizeof(s_nssai->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(s_nssai, pkbuf->payload - size, size);

    d_trace(25, "  S_NSSAI - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_s_nssai(pkbuf_t *pkbuf, nas_s_nssai_t *s_nssai)
{
    c_uint16_t size = s_nssai->length + sizeof(s_nssai->length);
    nas_s_nssai_t target;

    memcpy(&target, s_nssai, sizeof(nas_s_nssai_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  S_NSSAI - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.24 GPRS timer 2
 * O TLV 3 */
c_int16_t nas_decode_gprs_timer_2(nas_gprs_timer_2_t *gprs_timer_2, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_gprs_timer_2_t *source = pkbuf->payload;

    gprs_timer_2->length = source->length;
    size = gprs_timer_2->length + sizeof(gprs_timer_2->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(gprs_timer_2, pkbuf->payload - size, size);

    d_trace(25, "  GPRS_TIMER_2 - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_gprs_timer_2(pkbuf_t *pkbuf, nas_gprs_timer_2_t *gprs_timer_2)
{
    c_uint16_t size = gprs_timer_2->length + sizeof(gprs_timer_2->length);
    nas_gprs_timer_2_t target;

    memcpy(&target, gprs_timer_2, sizeof(nas_gprs_timer_2_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  GPRS_TIMER_2 - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.1 5GMM capability
 * O TLV 3-15 */
c_int16_t nas_decode_5gmm_capability(nas_5gmm_capability_t *5gmm_capability, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5gmm_capability_t *source = pkbuf->payload;

    5gmm_capability->length = source->length;
    size = 5gmm_capability->length + sizeof(5gmm_capability->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gmm_capability, pkbuf->payload - size, size);

    d_trace(25, "  5GMM_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gmm_capability(pkbuf_t *pkbuf, nas_5gmm_capability_t *5gmm_capability)
{
    c_uint16_t size = 5gmm_capability->length + sizeof(5gmm_capability->length);
    nas_5gmm_capability_t target;

    memcpy(&target, 5gmm_capability, sizeof(nas_5gmm_capability_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GMM_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.10 Access type
 * M V 1/2 */
c_int16_t nas_decode_access_type(nas_access_type_t *access_type, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_access_type_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(access_type, pkbuf->payload - size, size);

    d_trace(25, "  ACCESS_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_access_type(pkbuf_t *pkbuf, nas_access_type_t *access_type)
{
    c_uint16_t size = sizeof(nas_access_type_t);
    nas_access_type_t target;

    memcpy(&target, access_type, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  ACCESS_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.11 Allowed PDU session status
 * O TLV 4-34 */
c_int16_t nas_decode_allowed_pdu_session_status(nas_allowed_pdu_session_status_t *allowed_pdu_session_status, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_allowed_pdu_session_status_t *source = pkbuf->payload;

    allowed_pdu_session_status->length = source->length;
    size = allowed_pdu_session_status->length + sizeof(allowed_pdu_session_status->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(allowed_pdu_session_status, pkbuf->payload - size, size);

    d_trace(25, "  ALLOWED_PDU_SESSION_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_allowed_pdu_session_status(pkbuf_t *pkbuf, nas_allowed_pdu_session_status_t *allowed_pdu_session_status)
{
    c_uint16_t size = allowed_pdu_session_status->length + sizeof(allowed_pdu_session_status->length);
    nas_allowed_pdu_session_status_t target;

    memcpy(&target, allowed_pdu_session_status, sizeof(nas_allowed_pdu_session_status_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  ALLOWED_PDU_SESSION_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.12 Authentication failure parameter
 * O TLV 16 */
c_int16_t nas_decode_authentication_failure_parameter(nas_authentication_failure_parameter_t *authentication_failure_parameter, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_authentication_failure_parameter_t *source = pkbuf->payload;

    authentication_failure_parameter->length = source->length;
    size = authentication_failure_parameter->length + sizeof(authentication_failure_parameter->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(authentication_failure_parameter, pkbuf->payload - size, size);

    d_trace(25, "  AUTHENTICATION_FAILURE_PARAMETER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_authentication_failure_parameter(pkbuf_t *pkbuf, nas_authentication_failure_parameter_t *authentication_failure_parameter)
{
    c_uint16_t size = authentication_failure_parameter->length + sizeof(authentication_failure_parameter->length);
    nas_authentication_failure_parameter_t target;

    memcpy(&target, authentication_failure_parameter, sizeof(nas_authentication_failure_parameter_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  AUTHENTICATION_FAILURE_PARAMETER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.13 Authentication parameter
 * O TV 17 */
c_int16_t nas_decode_authentication_parameter(nas_authentication_parameter_t *authentication_parameter, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_authentication_parameter_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(authentication_parameter, pkbuf->payload - size, size);

    d_trace(25, "  AUTHENTICATION_PARAMETER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_authentication_parameter(pkbuf_t *pkbuf, nas_authentication_parameter_t *authentication_parameter)
{
    c_uint16_t size = sizeof(nas_authentication_parameter_t);
    nas_authentication_parameter_t target;

    memcpy(&target, authentication_parameter, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  AUTHENTICATION_PARAMETER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.14 Authentication parameter AUTN
 * O TLV 18 */
c_int16_t nas_decode_authentication_parameter_autn(nas_authentication_parameter_autn_t *authentication_parameter_autn, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_authentication_parameter_autn_t *source = pkbuf->payload;

    authentication_parameter_autn->length = source->length;
    size = authentication_parameter_autn->length + sizeof(authentication_parameter_autn->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(authentication_parameter_autn, pkbuf->payload - size, size);

    d_trace(25, "  AUTHENTICATION_PARAMETER_AUTN - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_authentication_parameter_autn(pkbuf_t *pkbuf, nas_authentication_parameter_autn_t *authentication_parameter_autn)
{
    c_uint16_t size = authentication_parameter_autn->length + sizeof(authentication_parameter_autn->length);
    nas_authentication_parameter_autn_t target;

    memcpy(&target, authentication_parameter_autn, sizeof(nas_authentication_parameter_autn_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  AUTHENTICATION_PARAMETER_AUTN - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.15 Authentication response parameter
 * O TLV 6-18 */
c_int16_t nas_decode_authentication_response_parameter(nas_authentication_response_parameter_t *authentication_response_parameter, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_authentication_response_parameter_t *source = pkbuf->payload;

    authentication_response_parameter->length = source->length;
    size = authentication_response_parameter->length + sizeof(authentication_response_parameter->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(authentication_response_parameter, pkbuf->payload - size, size);

    d_trace(25, "  AUTHENTICATION_RESPONSE_PARAMETER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_authentication_response_parameter(pkbuf_t *pkbuf, nas_authentication_response_parameter_t *authentication_response_parameter)
{
    c_uint16_t size = authentication_response_parameter->length + sizeof(authentication_response_parameter->length);
    nas_authentication_response_parameter_t target;

    memcpy(&target, authentication_response_parameter, sizeof(nas_authentication_response_parameter_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  AUTHENTICATION_RESPONSE_PARAMETER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.16 Configuration update indication
 * O TV 1 */
c_int16_t nas_decode_configuration_update_indication(nas_configuration_update_indication_t *configuration_update_indication, pkbuf_t *pkbuf)
{
    memcpy(configuration_update_indication, pkbuf->payload - 1, 1);

    d_trace(25, "  CONFIGURATION_UPDATE_INDICATION - ");
    d_trace_hex(25, pkbuf->payload - 1, 1);

    return 0;
}

c_int16_t nas_encode_configuration_update_indication(pkbuf_t *pkbuf, nas_configuration_update_indication_t *configuration_update_indication)
{
    c_uint16_t size = sizeof(nas_configuration_update_indication_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, configuration_update_indication, size);

    d_trace(25, "  CONFIGURATION_UPDATE_INDICATION - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.17 Daylight saving time
 * O TLV 3 */
c_int16_t nas_decode_daylight_saving_time(nas_daylight_saving_time_t *daylight_saving_time, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_daylight_saving_time_t *source = pkbuf->payload;

    daylight_saving_time->length = source->length;
    size = daylight_saving_time->length + sizeof(daylight_saving_time->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(daylight_saving_time, pkbuf->payload - size, size);

    d_trace(25, "  DAYLIGHT_SAVING_TIME - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_daylight_saving_time(pkbuf_t *pkbuf, nas_daylight_saving_time_t *daylight_saving_time)
{
    c_uint16_t size = daylight_saving_time->length + sizeof(daylight_saving_time->length);
    nas_daylight_saving_time_t target;

    memcpy(&target, daylight_saving_time, sizeof(nas_daylight_saving_time_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  DAYLIGHT_SAVING_TIME - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.18 De-registration type
 * M V 1/2 */
c_int16_t nas_decode_de_registration_type(nas_de_registration_type_t *de_registration_type, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_de_registration_type_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(de_registration_type, pkbuf->payload - size, size);

    d_trace(25, "  DE_REGISTRATION_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_de_registration_type(pkbuf_t *pkbuf, nas_de_registration_type_t *de_registration_type)
{
    c_uint16_t size = sizeof(nas_de_registration_type_t);
    nas_de_registration_type_t target;

    memcpy(&target, de_registration_type, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  DE_REGISTRATION_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.19 DNN
 * O TLV 3-102 */
c_int16_t nas_decode_dnn(nas_dnn_t *dnn, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_dnn_t *source = pkbuf->payload;

    dnn->length = source->length;
    size = dnn->length + sizeof(dnn->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(dnn, pkbuf->payload - size, size);

    d_trace(25, "  DNN - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_dnn(pkbuf_t *pkbuf, nas_dnn_t *dnn)
{
    c_uint16_t size = dnn->length + sizeof(dnn->length);
    nas_dnn_t target;

    memcpy(&target, dnn, sizeof(nas_dnn_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  DNN - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.2 5GMM cause
 * M V 1 */
c_int16_t nas_decode_5gmm_cause(nas_5gmm_cause_t *5gmm_cause, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_5gmm_cause_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gmm_cause, pkbuf->payload - size, size);

    d_trace(25, "  5GMM_CAUSE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gmm_cause(pkbuf_t *pkbuf, nas_5gmm_cause_t *5gmm_cause)
{
    c_uint16_t size = sizeof(nas_5gmm_cause_t);
    nas_5gmm_cause_t target;

    memcpy(&target, 5gmm_cause, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GMM_CAUSE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.20 DRX parameters
 * O TBD TBD */
c_int16_t nas_decode_drx_parameters(nas_drx_parameters_t *drx_parameters, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_drx_parameters_t *source = pkbuf->payload;

    drx_parameters->length = source->length;
    size = drx_parameters->length + sizeof(drx_parameters->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(drx_parameters, pkbuf->payload - size, size);

    d_trace(25, "  DRX_PARAMETERS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_drx_parameters(pkbuf_t *pkbuf, nas_drx_parameters_t *drx_parameters)
{
    c_uint16_t size = drx_parameters->length + sizeof(drx_parameters->length);
    nas_drx_parameters_t target;

    memcpy(&target, drx_parameters, sizeof(nas_drx_parameters_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  DRX_PARAMETERS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.21 Emergency number list
 * O TLV 5-50 */
c_int16_t nas_decode_emergency_number_list(nas_emergency_number_list_t *emergency_number_list, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_emergency_number_list_t *source = pkbuf->payload;

    emergency_number_list->length = source->length;
    size = emergency_number_list->length + sizeof(emergency_number_list->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(emergency_number_list, pkbuf->payload - size, size);

    d_trace(25, "  EMERGENCY_NUMBER_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_emergency_number_list(pkbuf_t *pkbuf, nas_emergency_number_list_t *emergency_number_list)
{
    c_uint16_t size = emergency_number_list->length + sizeof(emergency_number_list->length);
    nas_emergency_number_list_t target;

    memcpy(&target, emergency_number_list, sizeof(nas_emergency_number_list_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  EMERGENCY_NUMBER_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.22 EPS NAS message container
 * O TLV-E TBD */
c_int16_t nas_decode_eps_nas_message_container(nas_eps_nas_message_container_t *eps_nas_message_container, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_eps_nas_message_container_t *source = pkbuf->payload;

    eps_nas_message_container->length = ntohs(source->length);
    size = eps_nas_message_container->length + sizeof(eps_nas_message_container->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    eps_nas_message_container->buffer = pkbuf->payload - size + sizeof(eps_nas_message_container->length);

    d_trace(25, "  EPS_NAS_MESSAGE_CONTAINER - ");
    d_trace_hex(25, eps_nas_message_container->buffer, eps_nas_message_container->length);

    return size;
}

c_int16_t nas_encode_eps_nas_message_container(pkbuf_t *pkbuf, nas_eps_nas_message_container_t *eps_nas_message_container)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(eps_nas_message_container, return -1, "Null param");
    d_assert(eps_nas_message_container->buffer, return -1, "Null param");

    size = sizeof(eps_nas_message_container->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(eps_nas_message_container->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = eps_nas_message_container->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, eps_nas_message_container->buffer, size);

    d_trace(25, "  EPS_NAS_MESSAGE_CONTAINER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return eps_nas_message_container->length + sizeof(eps_nas_message_container->length);
}

/* 9.10.3.23 EPS NAS security algorithms
 * O TV 2 */
c_int16_t nas_decode_eps_nas_security_algorithms(nas_eps_nas_security_algorithms_t *eps_nas_security_algorithms, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_eps_nas_security_algorithms_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(eps_nas_security_algorithms, pkbuf->payload - size, size);

    d_trace(25, "  EPS_NAS_SECURITY_ALGORITHMS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_eps_nas_security_algorithms(pkbuf_t *pkbuf, nas_eps_nas_security_algorithms_t *eps_nas_security_algorithms)
{
    c_uint16_t size = sizeof(nas_eps_nas_security_algorithms_t);
    nas_eps_nas_security_algorithms_t target;

    memcpy(&target, eps_nas_security_algorithms, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  EPS_NAS_SECURITY_ALGORITHMS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.24 Extended emergency number list
 * O TLV TBD */
c_int16_t nas_decode_extended_emergency_number_list(nas_extended_emergency_number_list_t *extended_emergency_number_list, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_extended_emergency_number_list_t *source = pkbuf->payload;

    extended_emergency_number_list->length = source->length;
    size = extended_emergency_number_list->length + sizeof(extended_emergency_number_list->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(extended_emergency_number_list, pkbuf->payload - size, size);

    d_trace(25, "  EXTENDED_EMERGENCY_NUMBER_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_extended_emergency_number_list(pkbuf_t *pkbuf, nas_extended_emergency_number_list_t *extended_emergency_number_list)
{
    c_uint16_t size = extended_emergency_number_list->length + sizeof(extended_emergency_number_list->length);
    nas_extended_emergency_number_list_t target;

    memcpy(&target, extended_emergency_number_list, sizeof(nas_extended_emergency_number_list_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  EXTENDED_EMERGENCY_NUMBER_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.25 HashAMF
 * O TV 9 */
c_int16_t nas_decode_hashamf(nas_hashamf_t *hashamf, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_hashamf_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(hashamf, pkbuf->payload - size, size);

    d_trace(25, "  HASHAMF - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_hashamf(pkbuf_t *pkbuf, nas_hashamf_t *hashamf)
{
    c_uint16_t size = sizeof(nas_hashamf_t);
    nas_hashamf_t target;

    memcpy(&target, hashamf, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  HASHAMF - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.26 IMEISV request
 * O TV 1 */
c_int16_t nas_decode_imeisv_request(nas_imeisv_request_t *imeisv_request, pkbuf_t *pkbuf)
{
    memcpy(imeisv_request, pkbuf->payload - 1, 1);

    d_trace(25, "  IMEISV_REQUEST - ");
    d_trace_hex(25, pkbuf->payload - 1, 1);

    return 0;
}

c_int16_t nas_encode_imeisv_request(pkbuf_t *pkbuf, nas_imeisv_request_t *imeisv_request)
{
    c_uint16_t size = sizeof(nas_imeisv_request_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, imeisv_request, size);

    d_trace(25, "  IMEISV_REQUEST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.27 LADN information
 * O TLV-E 12-1707 */
c_int16_t nas_decode_ladn_information(nas_ladn_information_t *ladn_information, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_ladn_information_t *source = pkbuf->payload;

    ladn_information->length = ntohs(source->length);
    size = ladn_information->length + sizeof(ladn_information->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    ladn_information->buffer = pkbuf->payload - size + sizeof(ladn_information->length);

    d_trace(25, "  LADN_INFORMATION - ");
    d_trace_hex(25, ladn_information->buffer, ladn_information->length);

    return size;
}

c_int16_t nas_encode_ladn_information(pkbuf_t *pkbuf, nas_ladn_information_t *ladn_information)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(ladn_information, return -1, "Null param");
    d_assert(ladn_information->buffer, return -1, "Null param");

    size = sizeof(ladn_information->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(ladn_information->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = ladn_information->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, ladn_information->buffer, size);

    d_trace(25, "  LADN_INFORMATION - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return ladn_information->length + sizeof(ladn_information->length);
}

/* 9.10.3.28 MICO indication
 * O TV 1 */
c_int16_t nas_decode_mico_indication(nas_mico_indication_t *mico_indication, pkbuf_t *pkbuf)
{
    memcpy(mico_indication, pkbuf->payload - 1, 1);

    d_trace(25, "  MICO_INDICATION - ");
    d_trace_hex(25, pkbuf->payload - 1, 1);

    return 0;
}

c_int16_t nas_encode_mico_indication(pkbuf_t *pkbuf, nas_mico_indication_t *mico_indication)
{
    c_uint16_t size = sizeof(nas_mico_indication_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, mico_indication, size);

    d_trace(25, "  MICO_INDICATION - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.29 key set identifier
 * M V 1/2 */
c_int16_t nas_decode_key_set_identifier(nas_key_set_identifier_t *key_set_identifier, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_key_set_identifier_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(key_set_identifier, pkbuf->payload - size, size);

    d_trace(25, "  KEY_SET_IDENTIFIER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_key_set_identifier(pkbuf_t *pkbuf, nas_key_set_identifier_t *key_set_identifier)
{
    c_uint16_t size = sizeof(nas_key_set_identifier_t);
    nas_key_set_identifier_t target;

    memcpy(&target, key_set_identifier, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  KEY_SET_IDENTIFIER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.3 5GS identity type
 * M V 1/2 */
c_int16_t nas_decode_5gs_identity_type(nas_5gs_identity_type_t *5gs_identity_type, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_5gs_identity_type_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gs_identity_type, pkbuf->payload - size, size);

    d_trace(25, "  5GS_IDENTITY_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gs_identity_type(pkbuf_t *pkbuf, nas_5gs_identity_type_t *5gs_identity_type)
{
    c_uint16_t size = sizeof(nas_5gs_identity_type_t);
    nas_5gs_identity_type_t target;

    memcpy(&target, 5gs_identity_type, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GS_IDENTITY_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.31 message container
 * O TLV-E 3-n */
c_int16_t nas_decode_message_container(nas_message_container_t *message_container, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_message_container_t *source = pkbuf->payload;

    message_container->length = ntohs(source->length);
    size = message_container->length + sizeof(message_container->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    message_container->buffer = pkbuf->payload - size + sizeof(message_container->length);

    d_trace(25, "  MESSAGE_CONTAINER - ");
    d_trace_hex(25, message_container->buffer, message_container->length);

    return size;
}

c_int16_t nas_encode_message_container(pkbuf_t *pkbuf, nas_message_container_t *message_container)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(message_container, return -1, "Null param");
    d_assert(message_container->buffer, return -1, "Null param");

    size = sizeof(message_container->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(message_container->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = message_container->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, message_container->buffer, size);

    d_trace(25, "  MESSAGE_CONTAINER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return message_container->length + sizeof(message_container->length);
}

/* 9.10.3.32 security algorithms
 * M V 1 */
c_int16_t nas_decode_security_algorithms(nas_security_algorithms_t *security_algorithms, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_security_algorithms_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(security_algorithms, pkbuf->payload - size, size);

    d_trace(25, "  SECURITY_ALGORITHMS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_security_algorithms(pkbuf_t *pkbuf, nas_security_algorithms_t *security_algorithms)
{
    c_uint16_t size = sizeof(nas_security_algorithms_t);
    nas_security_algorithms_t target;

    memcpy(&target, security_algorithms, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  SECURITY_ALGORITHMS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.33 Network name
 * O TLV 3-n */
c_int16_t nas_decode_network_name(nas_network_name_t *network_name, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_network_name_t *source = pkbuf->payload;

    network_name->length = source->length;
    size = network_name->length + sizeof(network_name->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(network_name, pkbuf->payload - size, size);

    d_trace(25, "  NETWORK_NAME - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_network_name(pkbuf_t *pkbuf, nas_network_name_t *network_name)
{
    c_uint16_t size = network_name->length + sizeof(network_name->length);
    nas_network_name_t target;

    memcpy(&target, network_name, sizeof(nas_network_name_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  NETWORK_NAME - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.34 NSSAI
 * O TLV 4-74 */
c_int16_t nas_decode_nssai(nas_nssai_t *nssai, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_nssai_t *source = pkbuf->payload;

    nssai->length = source->length;
    size = nssai->length + sizeof(nssai->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(nssai, pkbuf->payload - size, size);

    d_trace(25, "  NSSAI - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_nssai(pkbuf_t *pkbuf, nas_nssai_t *nssai)
{
    c_uint16_t size = nssai->length + sizeof(nssai->length);
    nas_nssai_t target;

    memcpy(&target, nssai, sizeof(nas_nssai_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  NSSAI - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.35 Payload container
 * O TLV-E 4-65538 */
c_int16_t nas_decode_payload_container(nas_payload_container_t *payload_container, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_payload_container_t *source = pkbuf->payload;

    payload_container->length = ntohs(source->length);
    size = payload_container->length + sizeof(payload_container->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    payload_container->buffer = pkbuf->payload - size + sizeof(payload_container->length);

    d_trace(25, "  PAYLOAD_CONTAINER - ");
    d_trace_hex(25, payload_container->buffer, payload_container->length);

    return size;
}

c_int16_t nas_encode_payload_container(pkbuf_t *pkbuf, nas_payload_container_t *payload_container)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(payload_container, return -1, "Null param");
    d_assert(payload_container->buffer, return -1, "Null param");

    size = sizeof(payload_container->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(payload_container->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = payload_container->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, payload_container->buffer, size);

    d_trace(25, "  PAYLOAD_CONTAINER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return payload_container->length + sizeof(payload_container->length);
}

/* 9.10.3.36 Payload container type
 * M V 1/2 */
c_int16_t nas_decode_payload_container_type(nas_payload_container_type_t *payload_container_type, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_payload_container_type_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(payload_container_type, pkbuf->payload - size, size);

    d_trace(25, "  PAYLOAD_CONTAINER_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_payload_container_type(pkbuf_t *pkbuf, nas_payload_container_type_t *payload_container_type)
{
    c_uint16_t size = sizeof(nas_payload_container_type_t);
    nas_payload_container_type_t target;

    memcpy(&target, payload_container_type, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  PAYLOAD_CONTAINER_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.37 PDU session identity 2
 * C TV 2 */
c_int16_t nas_decode_pdu_session_identity_2(nas_pdu_session_identity_2_t *pdu_session_identity_2, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_pdu_session_identity_2_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pdu_session_identity_2, pkbuf->payload - size, size);

    d_trace(25, "  PDU_SESSION_IDENTITY_2 - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_pdu_session_identity_2(pkbuf_t *pkbuf, nas_pdu_session_identity_2_t *pdu_session_identity_2)
{
    c_uint16_t size = sizeof(nas_pdu_session_identity_2_t);
    nas_pdu_session_identity_2_t target;

    memcpy(&target, pdu_session_identity_2, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  PDU_SESSION_IDENTITY_2 - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.38 PDU session reactivation result
 * O TLV 4-32 */
c_int16_t nas_decode_pdu_session_reactivation_result(nas_pdu_session_reactivation_result_t *pdu_session_reactivation_result, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_pdu_session_reactivation_result_t *source = pkbuf->payload;

    pdu_session_reactivation_result->length = source->length;
    size = pdu_session_reactivation_result->length + sizeof(pdu_session_reactivation_result->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pdu_session_reactivation_result, pkbuf->payload - size, size);

    d_trace(25, "  PDU_SESSION_REACTIVATION_RESULT - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_pdu_session_reactivation_result(pkbuf_t *pkbuf, nas_pdu_session_reactivation_result_t *pdu_session_reactivation_result)
{
    c_uint16_t size = pdu_session_reactivation_result->length + sizeof(pdu_session_reactivation_result->length);
    nas_pdu_session_reactivation_result_t target;

    memcpy(&target, pdu_session_reactivation_result, sizeof(nas_pdu_session_reactivation_result_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  PDU_SESSION_REACTIVATION_RESULT - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.39 PDU session reactivation result error cause
 * O TLV-E 5-515 */
c_int16_t nas_decode_pdu_session_reactivation_result_error_cause(nas_pdu_session_reactivation_result_error_cause_t *pdu_session_reactivation_result_error_cause, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_pdu_session_reactivation_result_error_cause_t *source = pkbuf->payload;

    pdu_session_reactivation_result_error_cause->length = ntohs(source->length);
    size = pdu_session_reactivation_result_error_cause->length + sizeof(pdu_session_reactivation_result_error_cause->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    pdu_session_reactivation_result_error_cause->buffer = pkbuf->payload - size + sizeof(pdu_session_reactivation_result_error_cause->length);

    d_trace(25, "  PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE - ");
    d_trace_hex(25, pdu_session_reactivation_result_error_cause->buffer, pdu_session_reactivation_result_error_cause->length);

    return size;
}

c_int16_t nas_encode_pdu_session_reactivation_result_error_cause(pkbuf_t *pkbuf, nas_pdu_session_reactivation_result_error_cause_t *pdu_session_reactivation_result_error_cause)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(pdu_session_reactivation_result_error_cause, return -1, "Null param");
    d_assert(pdu_session_reactivation_result_error_cause->buffer, return -1, "Null param");

    size = sizeof(pdu_session_reactivation_result_error_cause->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(pdu_session_reactivation_result_error_cause->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = pdu_session_reactivation_result_error_cause->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, pdu_session_reactivation_result_error_cause->buffer, size);

    d_trace(25, "  PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return pdu_session_reactivation_result_error_cause->length + sizeof(pdu_session_reactivation_result_error_cause->length);
}

/* 9.10.3.4 5GS mobile identity
 * M  5-TBD */
c_int16_t nas_decode_5gs_mobile_identity(nas_5gs_mobile_identity_t *5gs_mobile_identity, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5gs_mobile_identity_t *source = pkbuf->payload;

    5gs_mobile_identity->length = source->length;
    size = 5gs_mobile_identity->length + sizeof(5gs_mobile_identity->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gs_mobile_identity, pkbuf->payload - size, size);

    d_trace(25, "  5GS_MOBILE_IDENTITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gs_mobile_identity(pkbuf_t *pkbuf, nas_5gs_mobile_identity_t *5gs_mobile_identity)
{
    c_uint16_t size = 5gs_mobile_identity->length + sizeof(5gs_mobile_identity->length);
    nas_5gs_mobile_identity_t target;

    memcpy(&target, 5gs_mobile_identity, sizeof(nas_5gs_mobile_identity_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GS_MOBILE_IDENTITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.4 5G mobile identity
 * O TLV 11 */
c_int16_t nas_decode_5g_mobile_identity(nas_5g_mobile_identity_t *5g_mobile_identity, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5g_mobile_identity_t *source = pkbuf->payload;

    5g_mobile_identity->length = source->length;
    size = 5g_mobile_identity->length + sizeof(5g_mobile_identity->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5g_mobile_identity, pkbuf->payload - size, size);

    d_trace(25, "  5G_MOBILE_IDENTITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5g_mobile_identity(pkbuf_t *pkbuf, nas_5g_mobile_identity_t *5g_mobile_identity)
{
    c_uint16_t size = 5g_mobile_identity->length + sizeof(5g_mobile_identity->length);
    nas_5g_mobile_identity_t target;

    memcpy(&target, 5g_mobile_identity, sizeof(nas_5g_mobile_identity_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5G_MOBILE_IDENTITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.40 PDU session status
 * O TLV 4-34 */
c_int16_t nas_decode_pdu_session_status(nas_pdu_session_status_t *pdu_session_status, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_pdu_session_status_t *source = pkbuf->payload;

    pdu_session_status->length = source->length;
    size = pdu_session_status->length + sizeof(pdu_session_status->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pdu_session_status, pkbuf->payload - size, size);

    d_trace(25, "  PDU_SESSION_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_pdu_session_status(pkbuf_t *pkbuf, nas_pdu_session_status_t *pdu_session_status)
{
    c_uint16_t size = pdu_session_status->length + sizeof(pdu_session_status->length);
    nas_pdu_session_status_t target;

    memcpy(&target, pdu_session_status, sizeof(nas_pdu_session_status_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  PDU_SESSION_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.41 PLMN list
 * O TLV 5-47 */
c_int16_t nas_decode_plmn_list(nas_plmn_list_t *plmn_list, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_plmn_list_t *source = pkbuf->payload;

    plmn_list->length = source->length;
    size = plmn_list->length + sizeof(plmn_list->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(plmn_list, pkbuf->payload - size, size);

    d_trace(25, "  PLMN_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_plmn_list(pkbuf_t *pkbuf, nas_plmn_list_t *plmn_list)
{
    c_uint16_t size = plmn_list->length + sizeof(plmn_list->length);
    nas_plmn_list_t target;

    memcpy(&target, plmn_list, sizeof(nas_plmn_list_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  PLMN_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.42 Rejected NSSAI
 * O TLV 4-42 */
c_int16_t nas_decode_rejected_nssai(nas_rejected_nssai_t *rejected_nssai, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_rejected_nssai_t *source = pkbuf->payload;

    rejected_nssai->length = source->length;
    size = rejected_nssai->length + sizeof(rejected_nssai->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(rejected_nssai, pkbuf->payload - size, size);

    d_trace(25, "  REJECTED_NSSAI - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_rejected_nssai(pkbuf_t *pkbuf, nas_rejected_nssai_t *rejected_nssai)
{
    c_uint16_t size = rejected_nssai->length + sizeof(rejected_nssai->length);
    nas_rejected_nssai_t target;

    memcpy(&target, rejected_nssai, sizeof(nas_rejected_nssai_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  REJECTED_NSSAI - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.43 Request type
 * O TV 1 */
c_int16_t nas_decode_request_type(nas_request_type_t *request_type, pkbuf_t *pkbuf)
{
    memcpy(request_type, pkbuf->payload - 1, 1);

    d_trace(25, "  REQUEST_TYPE - ");
    d_trace_hex(25, pkbuf->payload - 1, 1);

    return 0;
}

c_int16_t nas_encode_request_type(pkbuf_t *pkbuf, nas_request_type_t *request_type)
{
    c_uint16_t size = sizeof(nas_request_type_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, request_type, size);

    d_trace(25, "  REQUEST_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.44 S1 UE network capability
 * O TLV 4-15 */
c_int16_t nas_decode_s1_ue_network_capability(nas_s1_ue_network_capability_t *s1_ue_network_capability, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_s1_ue_network_capability_t *source = pkbuf->payload;

    s1_ue_network_capability->length = source->length;
    size = s1_ue_network_capability->length + sizeof(s1_ue_network_capability->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(s1_ue_network_capability, pkbuf->payload - size, size);

    d_trace(25, "  S1_UE_NETWORK_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_s1_ue_network_capability(pkbuf_t *pkbuf, nas_s1_ue_network_capability_t *s1_ue_network_capability)
{
    c_uint16_t size = s1_ue_network_capability->length + sizeof(s1_ue_network_capability->length);
    nas_s1_ue_network_capability_t target;

    memcpy(&target, s1_ue_network_capability, sizeof(nas_s1_ue_network_capability_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  S1_UE_NETWORK_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.45 Service area list
 * O TLV 6-114 */
c_int16_t nas_decode_service_area_list(nas_service_area_list_t *service_area_list, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_service_area_list_t *source = pkbuf->payload;

    service_area_list->length = source->length;
    size = service_area_list->length + sizeof(service_area_list->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(service_area_list, pkbuf->payload - size, size);

    d_trace(25, "  SERVICE_AREA_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_service_area_list(pkbuf_t *pkbuf, nas_service_area_list_t *service_area_list)
{
    c_uint16_t size = service_area_list->length + sizeof(service_area_list->length);
    nas_service_area_list_t target;

    memcpy(&target, service_area_list, sizeof(nas_service_area_list_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  SERVICE_AREA_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.47 Time zone
 * O TV 2 */
c_int16_t nas_decode_time_zone(nas_time_zone_t *time_zone, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_time_zone_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(time_zone, pkbuf->payload - size, size);

    d_trace(25, "  TIME_ZONE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_time_zone(pkbuf_t *pkbuf, nas_time_zone_t *time_zone)
{
    c_uint16_t size = sizeof(nas_time_zone_t);
    nas_time_zone_t target;

    memcpy(&target, time_zone, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  TIME_ZONE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.48 Time zone and time
 * O TV 8 */
c_int16_t nas_decode_time_zone_and_time(nas_time_zone_and_time_t *time_zone_and_time, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_time_zone_and_time_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(time_zone_and_time, pkbuf->payload - size, size);

    d_trace(25, "  TIME_ZONE_AND_TIME - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_time_zone_and_time(pkbuf_t *pkbuf, nas_time_zone_and_time_t *time_zone_and_time)
{
    c_uint16_t size = sizeof(nas_time_zone_and_time_t);
    nas_time_zone_and_time_t target;

    memcpy(&target, time_zone_and_time, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  TIME_ZONE_AND_TIME - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.49 Transparent container
 * O TBD TBD */
c_int16_t nas_decode_transparent_container(nas_transparent_container_t *transparent_container, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_transparent_container_t *source = pkbuf->payload;

    transparent_container->length = source->length;
    size = transparent_container->length + sizeof(transparent_container->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(transparent_container, pkbuf->payload - size, size);

    d_trace(25, "  TRANSPARENT_CONTAINER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_transparent_container(pkbuf_t *pkbuf, nas_transparent_container_t *transparent_container)
{
    c_uint16_t size = transparent_container->length + sizeof(transparent_container->length);
    nas_transparent_container_t target;

    memcpy(&target, transparent_container, sizeof(nas_transparent_container_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  TRANSPARENT_CONTAINER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.49 UE security capability
 * O TLV 4-6 */
c_int16_t nas_decode_ue_security_capability(nas_ue_security_capability_t *ue_security_capability, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_ue_security_capability_t *source = pkbuf->payload;

    ue_security_capability->length = source->length;
    size = ue_security_capability->length + sizeof(ue_security_capability->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(ue_security_capability, pkbuf->payload - size, size);

    d_trace(25, "  UE_SECURITY_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_ue_security_capability(pkbuf_t *pkbuf, nas_ue_security_capability_t *ue_security_capability)
{
    c_uint16_t size = ue_security_capability->length + sizeof(ue_security_capability->length);
    nas_ue_security_capability_t target;

    memcpy(&target, ue_security_capability, sizeof(nas_ue_security_capability_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  UE_SECURITY_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.5 5GS network feature support
 * O TLV 3-5 */
c_int16_t nas_decode_5gs_network_feature_support(nas_5gs_network_feature_support_t *5gs_network_feature_support, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5gs_network_feature_support_t *source = pkbuf->payload;

    5gs_network_feature_support->length = source->length;
    size = 5gs_network_feature_support->length + sizeof(5gs_network_feature_support->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gs_network_feature_support, pkbuf->payload - size, size);

    d_trace(25, "  5GS_NETWORK_FEATURE_SUPPORT - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gs_network_feature_support(pkbuf_t *pkbuf, nas_5gs_network_feature_support_t *5gs_network_feature_support)
{
    c_uint16_t size = 5gs_network_feature_support->length + sizeof(5gs_network_feature_support->length);
    nas_5gs_network_feature_support_t target;

    memcpy(&target, 5gs_network_feature_support, sizeof(nas_5gs_network_feature_support_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GS_NETWORK_FEATURE_SUPPORT - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.51 UE usage setting
 * O TLV 3 */
c_int16_t nas_decode_ue_usage_setting(nas_ue_usage_setting_t *ue_usage_setting, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_ue_usage_setting_t *source = pkbuf->payload;

    ue_usage_setting->length = source->length;
    size = ue_usage_setting->length + sizeof(ue_usage_setting->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(ue_usage_setting, pkbuf->payload - size, size);

    d_trace(25, "  UE_USAGE_SETTING - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_ue_usage_setting(pkbuf_t *pkbuf, nas_ue_usage_setting_t *ue_usage_setting)
{
    c_uint16_t size = ue_usage_setting->length + sizeof(ue_usage_setting->length);
    nas_ue_usage_setting_t target;

    memcpy(&target, ue_usage_setting, sizeof(nas_ue_usage_setting_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  UE_USAGE_SETTING - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.52 UE status
 * O TLV 3 */
c_int16_t nas_decode_ue_status(nas_ue_status_t *ue_status, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_ue_status_t *source = pkbuf->payload;

    ue_status->length = source->length;
    size = ue_status->length + sizeof(ue_status->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(ue_status, pkbuf->payload - size, size);

    d_trace(25, "  UE_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_ue_status(pkbuf_t *pkbuf, nas_ue_status_t *ue_status)
{
    c_uint16_t size = ue_status->length + sizeof(ue_status->length);
    nas_ue_status_t target;

    memcpy(&target, ue_status, sizeof(nas_ue_status_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  UE_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.53 Uplink data status
 * O TLV 4-34 */
c_int16_t nas_decode_uplink_data_status(nas_uplink_data_status_t *uplink_data_status, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_uplink_data_status_t *source = pkbuf->payload;

    uplink_data_status->length = source->length;
    size = uplink_data_status->length + sizeof(uplink_data_status->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(uplink_data_status, pkbuf->payload - size, size);

    d_trace(25, "  UPLINK_DATA_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_uplink_data_status(pkbuf_t *pkbuf, nas_uplink_data_status_t *uplink_data_status)
{
    c_uint16_t size = uplink_data_status->length + sizeof(uplink_data_status->length);
    nas_uplink_data_status_t target;

    memcpy(&target, uplink_data_status, sizeof(nas_uplink_data_status_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  UPLINK_DATA_STATUS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.6 5GS registration result
 * M LV 2 */
c_int16_t nas_decode_5gs_registration_result(nas_5gs_registration_result_t *5gs_registration_result, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5gs_registration_result_t *source = pkbuf->payload;

    5gs_registration_result->length = source->length;
    size = 5gs_registration_result->length + sizeof(5gs_registration_result->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gs_registration_result, pkbuf->payload - size, size);

    d_trace(25, "  5GS_REGISTRATION_RESULT - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gs_registration_result(pkbuf_t *pkbuf, nas_5gs_registration_result_t *5gs_registration_result)
{
    c_uint16_t size = 5gs_registration_result->length + sizeof(5gs_registration_result->length);
    nas_5gs_registration_result_t target;

    memcpy(&target, 5gs_registration_result, sizeof(nas_5gs_registration_result_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GS_REGISTRATION_RESULT - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.7 5GS registration type
 * M LV 2 */
c_int16_t nas_decode_5gs_registration_type(nas_5gs_registration_type_t *5gs_registration_type, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5gs_registration_type_t *source = pkbuf->payload;

    5gs_registration_type->length = source->length;
    size = 5gs_registration_type->length + sizeof(5gs_registration_type->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gs_registration_type, pkbuf->payload - size, size);

    d_trace(25, "  5GS_REGISTRATION_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gs_registration_type(pkbuf_t *pkbuf, nas_5gs_registration_type_t *5gs_registration_type)
{
    c_uint16_t size = 5gs_registration_type->length + sizeof(5gs_registration_type->length);
    nas_5gs_registration_type_t target;

    memcpy(&target, 5gs_registration_type, sizeof(nas_5gs_registration_type_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GS_REGISTRATION_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.8 5GS tracking area identity
 * O TV 7 */
c_int16_t nas_decode_5gs_tracking_area_identity(nas_5gs_tracking_area_identity_t *5gs_tracking_area_identity, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_5gs_tracking_area_identity_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gs_tracking_area_identity, pkbuf->payload - size, size);

    d_trace(25, "  5GS_TRACKING_AREA_IDENTITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gs_tracking_area_identity(pkbuf_t *pkbuf, nas_5gs_tracking_area_identity_t *5gs_tracking_area_identity)
{
    c_uint16_t size = sizeof(nas_5gs_tracking_area_identity_t);
    nas_5gs_tracking_area_identity_t target;

    memcpy(&target, 5gs_tracking_area_identity, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GS_TRACKING_AREA_IDENTITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.3.9 5GS tracking area identity list
 * O TLV 9-114 */
c_int16_t nas_decode_5gs_tracking_area_identity_list(nas_5gs_tracking_area_identity_list_t *5gs_tracking_area_identity_list, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5gs_tracking_area_identity_list_t *source = pkbuf->payload;

    5gs_tracking_area_identity_list->length = source->length;
    size = 5gs_tracking_area_identity_list->length + sizeof(5gs_tracking_area_identity_list->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gs_tracking_area_identity_list, pkbuf->payload - size, size);

    d_trace(25, "  5GS_TRACKING_AREA_IDENTITY_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gs_tracking_area_identity_list(pkbuf_t *pkbuf, nas_5gs_tracking_area_identity_list_t *5gs_tracking_area_identity_list)
{
    c_uint16_t size = 5gs_tracking_area_identity_list->length + sizeof(5gs_tracking_area_identity_list->length);
    nas_5gs_tracking_area_identity_list_t target;

    memcpy(&target, 5gs_tracking_area_identity_list, sizeof(nas_5gs_tracking_area_identity_list_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GS_TRACKING_AREA_IDENTITY_LIST - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.1 5GSM capability
 * O TLV 3-15 */
c_int16_t nas_decode_5gsm_capability(nas_5gsm_capability_t *5gsm_capability, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_5gsm_capability_t *source = pkbuf->payload;

    5gsm_capability->length = source->length;
    size = 5gsm_capability->length + sizeof(5gsm_capability->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gsm_capability, pkbuf->payload - size, size);

    d_trace(25, "  5GSM_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gsm_capability(pkbuf_t *pkbuf, nas_5gsm_capability_t *5gsm_capability)
{
    c_uint16_t size = 5gsm_capability->length + sizeof(5gsm_capability->length);
    nas_5gsm_capability_t target;

    memcpy(&target, 5gsm_capability, sizeof(nas_5gsm_capability_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GSM_CAPABILITY - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.10 Session-AMBR
 * M LV TBD */
c_int16_t nas_decode_session_ambr(nas_session_ambr_t *session_ambr, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_session_ambr_t *source = pkbuf->payload;

    session_ambr->length = source->length;
    size = session_ambr->length + sizeof(session_ambr->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(session_ambr, pkbuf->payload - size, size);

    d_trace(25, "  SESSION_AMBR - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_session_ambr(pkbuf_t *pkbuf, nas_session_ambr_t *session_ambr)
{
    c_uint16_t size = session_ambr->length + sizeof(session_ambr->length);
    nas_session_ambr_t target;

    memcpy(&target, session_ambr, sizeof(nas_session_ambr_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  SESSION_AMBR - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.11 SM PDU DN request container
 * O TBD TBD */
c_int16_t nas_decode_sm_pdu_dn_request_container(nas_sm_pdu_dn_request_container_t *sm_pdu_dn_request_container, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_sm_pdu_dn_request_container_t *source = pkbuf->payload;

    sm_pdu_dn_request_container->length = source->length;
    size = sm_pdu_dn_request_container->length + sizeof(sm_pdu_dn_request_container->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(sm_pdu_dn_request_container, pkbuf->payload - size, size);

    d_trace(25, "  SM_PDU_DN_REQUEST_CONTAINER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_sm_pdu_dn_request_container(pkbuf_t *pkbuf, nas_sm_pdu_dn_request_container_t *sm_pdu_dn_request_container)
{
    c_uint16_t size = sm_pdu_dn_request_container->length + sizeof(sm_pdu_dn_request_container->length);
    nas_sm_pdu_dn_request_container_t target;

    memcpy(&target, sm_pdu_dn_request_container, sizeof(nas_sm_pdu_dn_request_container_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  SM_PDU_DN_REQUEST_CONTAINER - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.12 SSC mode
 * O TV 1 */
c_int16_t nas_decode_ssc_mode(nas_ssc_mode_t *ssc_mode, pkbuf_t *pkbuf)
{
    memcpy(ssc_mode, pkbuf->payload - 1, 1);

    d_trace(25, "  SSC_MODE - ");
    d_trace_hex(25, pkbuf->payload - 1, 1);

    return 0;
}

c_int16_t nas_encode_ssc_mode(pkbuf_t *pkbuf, nas_ssc_mode_t *ssc_mode)
{
    c_uint16_t size = sizeof(nas_ssc_mode_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, ssc_mode, size);

    d_trace(25, "  SSC_MODE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.2 5GSM cause
 * O TV 2 */
c_int16_t nas_decode_5gsm_cause(nas_5gsm_cause_t *5gsm_cause, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_5gsm_cause_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(5gsm_cause, pkbuf->payload - size, size);

    d_trace(25, "  5GSM_CAUSE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_5gsm_cause(pkbuf_t *pkbuf, nas_5gsm_cause_t *5gsm_cause)
{
    c_uint16_t size = sizeof(nas_5gsm_cause_t);
    nas_5gsm_cause_t target;

    memcpy(&target, 5gsm_cause, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  5GSM_CAUSE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.3 Allowed SSC mode
 * O TV 1 */
c_int16_t nas_decode_allowed_ssc_mode(nas_allowed_ssc_mode_t *allowed_ssc_mode, pkbuf_t *pkbuf)
{
    memcpy(allowed_ssc_mode, pkbuf->payload - 1, 1);

    d_trace(25, "  ALLOWED_SSC_MODE - ");
    d_trace_hex(25, pkbuf->payload - 1, 1);

    return 0;
}

c_int16_t nas_encode_allowed_ssc_mode(pkbuf_t *pkbuf, nas_allowed_ssc_mode_t *allowed_ssc_mode)
{
    c_uint16_t size = sizeof(nas_allowed_ssc_mode_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, allowed_ssc_mode, size);

    d_trace(25, "  ALLOWED_SSC_MODE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.4 Extended protocol configuration options
 * O TLV-E 4-65538 */
c_int16_t nas_decode_extended_protocol_configuration_options(nas_extended_protocol_configuration_options_t *extended_protocol_configuration_options, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_extended_protocol_configuration_options_t *source = pkbuf->payload;

    extended_protocol_configuration_options->length = ntohs(source->length);
    size = extended_protocol_configuration_options->length + sizeof(extended_protocol_configuration_options->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    extended_protocol_configuration_options->buffer = pkbuf->payload - size + sizeof(extended_protocol_configuration_options->length);

    d_trace(25, "  EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS - ");
    d_trace_hex(25, extended_protocol_configuration_options->buffer, extended_protocol_configuration_options->length);

    return size;
}

c_int16_t nas_encode_extended_protocol_configuration_options(pkbuf_t *pkbuf, nas_extended_protocol_configuration_options_t *extended_protocol_configuration_options)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(extended_protocol_configuration_options, return -1, "Null param");
    d_assert(extended_protocol_configuration_options->buffer, return -1, "Null param");

    size = sizeof(extended_protocol_configuration_options->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(extended_protocol_configuration_options->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = extended_protocol_configuration_options->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, extended_protocol_configuration_options->buffer, size);

    d_trace(25, "  EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return extended_protocol_configuration_options->length + sizeof(extended_protocol_configuration_options->length);
}

/* 9.10.4.5 Mapped EPS bearer contexts
 * O TLV-E 7-65538 */
c_int16_t nas_decode_mapped_eps_bearer_contexts(nas_mapped_eps_bearer_contexts_t *mapped_eps_bearer_contexts, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_mapped_eps_bearer_contexts_t *source = pkbuf->payload;

    mapped_eps_bearer_contexts->length = ntohs(source->length);
    size = mapped_eps_bearer_contexts->length + sizeof(mapped_eps_bearer_contexts->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    mapped_eps_bearer_contexts->buffer = pkbuf->payload - size + sizeof(mapped_eps_bearer_contexts->length);

    d_trace(25, "  MAPPED_EPS_BEARER_CONTEXTS - ");
    d_trace_hex(25, mapped_eps_bearer_contexts->buffer, mapped_eps_bearer_contexts->length);

    return size;
}

c_int16_t nas_encode_mapped_eps_bearer_contexts(pkbuf_t *pkbuf, nas_mapped_eps_bearer_contexts_t *mapped_eps_bearer_contexts)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(mapped_eps_bearer_contexts, return -1, "Null param");
    d_assert(mapped_eps_bearer_contexts->buffer, return -1, "Null param");

    size = sizeof(mapped_eps_bearer_contexts->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(mapped_eps_bearer_contexts->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = mapped_eps_bearer_contexts->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, mapped_eps_bearer_contexts->buffer, size);

    d_trace(25, "  MAPPED_EPS_BEARER_CONTEXTS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return mapped_eps_bearer_contexts->length + sizeof(mapped_eps_bearer_contexts->length);
}

/* 9.10.4.6 Maximum number of supported packet filters
 * O TV 3 */
c_int16_t nas_decode_maximum_number_of_supported_packet_filters(nas_maximum_number_of_supported_packet_filters_t *maximum_number_of_supported_packet_filters, pkbuf_t *pkbuf)
{
    c_uint16_t size = sizeof(nas_maximum_number_of_supported_packet_filters_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(maximum_number_of_supported_packet_filters, pkbuf->payload - size, size);

    d_trace(25, "  MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_maximum_number_of_supported_packet_filters(pkbuf_t *pkbuf, nas_maximum_number_of_supported_packet_filters_t *maximum_number_of_supported_packet_filters)
{
    c_uint16_t size = sizeof(nas_maximum_number_of_supported_packet_filters_t);
    nas_maximum_number_of_supported_packet_filters_t target;

    memcpy(&target, maximum_number_of_supported_packet_filters, size);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.7 PDU address
 * O TLV 7, 11 or 15 */
c_int16_t nas_decode_pdu_address(nas_pdu_address_t *pdu_address, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_pdu_address_t *source = pkbuf->payload;

    pdu_address->length = source->length;
    size = pdu_address->length + sizeof(pdu_address->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pdu_address, pkbuf->payload - size, size);

    d_trace(25, "  PDU_ADDRESS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

c_int16_t nas_encode_pdu_address(pkbuf_t *pkbuf, nas_pdu_address_t *pdu_address)
{
    c_uint16_t size = pdu_address->length + sizeof(pdu_address->length);
    nas_pdu_address_t target;

    memcpy(&target, pdu_address, sizeof(nas_pdu_address_t));
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &target, size);

    d_trace(25, "  PDU_ADDRESS - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.8 PDU session type
 * O TV 1 */
c_int16_t nas_decode_pdu_session_type(nas_pdu_session_type_t *pdu_session_type, pkbuf_t *pkbuf)
{
    memcpy(pdu_session_type, pkbuf->payload - 1, 1);

    d_trace(25, "  PDU_SESSION_TYPE - ");
    d_trace_hex(25, pkbuf->payload - 1, 1);

    return 0;
}

c_int16_t nas_encode_pdu_session_type(pkbuf_t *pkbuf, nas_pdu_session_type_t *pdu_session_type)
{
    c_uint16_t size = sizeof(nas_pdu_session_type_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, pdu_session_type, size);

    d_trace(25, "  PDU_SESSION_TYPE - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return size;
}

/* 9.10.4.9 QoS rules
 * M LV-E 7-65538 */
c_int16_t nas_decode_qos_rules(nas_qos_rules_t *qos_rules, pkbuf_t *pkbuf)
{
    c_uint16_t size = 0;
    nas_qos_rules_t *source = pkbuf->payload;

    qos_rules->length = ntohs(source->length);
    size = qos_rules->length + sizeof(qos_rules->length);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    qos_rules->buffer = pkbuf->payload - size + sizeof(qos_rules->length);

    d_trace(25, "  QOS_RULES - ");
    d_trace_hex(25, qos_rules->buffer, qos_rules->length);

    return size;
}

c_int16_t nas_encode_qos_rules(pkbuf_t *pkbuf, nas_qos_rules_t *qos_rules)
{
    c_uint16_t size = 0;
    c_uint16_t target;

    d_assert(qos_rules, return -1, "Null param");
    d_assert(qos_rules->buffer, return -1, "Null param");

    size = sizeof(qos_rules->length);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    target = htons(qos_rules->length);
    memcpy(pkbuf->payload - size, &target, size);

    size = qos_rules->length;
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, qos_rules->buffer, size);

    d_trace(25, "  QOS_RULES - ");
    d_trace_hex(25, pkbuf->payload - size, size);

    return qos_rules->length + sizeof(qos_rules->length);
}

