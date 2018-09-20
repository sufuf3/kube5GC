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
 * Created on: 2018-09-20 17:24:08.867748 by waiting
 * from 24501-f00.docx
 ******************************************************************************/

#ifndef __NAS_MESSAGE_H__
#define __NAS_MESSAGE_H__

#include "nas_ies.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The Packet Buffer(pkbuf_t) for NAS message MUST make a HEADROOM. 
 * When calculating AES_CMAC, we need to use the headroom of the packet. */
#define NAS_HEADROOM 16

/***reference from TS24.007-f20 9.3.1******/

#define NAS_SECURITY_HEADER_PLAIN_NAS_MESSAGE 0
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED 1
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED 2
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_NEW_SECURITY_CONTEXT 3
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHTERD_WITH_NEW_INTEGRITY_CONTEXT 4

/***reference from TS24.007-f20 11.2.3.1**/
#define NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GSM 0x2e
#define NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GMM 0x7e

#define NAS_PDU_SESSION_IDENTITY_UNASSIGNED 0
#define NAS_PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED 0

typedef struct _nas_gmm_header_t {
    c_uint8_t extended_protocol_discriminator;
    c_uint8_t security_header_type;
    c_uint8_t message_type;
} __attribute__ ((packed)) nas_gmm_header_t;

typedef struct _nas_gsm_header_t {
    c_uint8_t extended_protocol_discriminator;
    c_uint8_t pdu_session_identity;
    c_uint8_t procedure_transaction_identity;
    c_uint8_t message_type;
} __attribute__ ((packed)) nas_gsm_header_t;

typedef struct _nas_security_header_t {
    c_uint8_t extended_protocol_discriminator;
    c_uint8_t security_header_type;
    c_uint32_t message_authentication_code;
    c_uint8_t sequence_number;
} __attribute__ ((packed)) nas_security_header_t;

#define NAS_REGISTRATION_REQUEST 65
#define NAS_REGISTRATION_ACCEPT 66
#define NAS_REGISTRATION_COMPLETE 67
#define NAS_REGISTRATION_REJECT 68
#define NAS_DEREGISTRATION_REQUEST 69
#define NAS_DEREGISTRATION_ACCEPT 70
#define NAS_SERVICE_REJECT 77
#define NAS_SERVICE_ACCEPT 78
#define NAS_CONFIGURATION_UPDATE_COMMAND 84
#define NAS_CONFIGURATION_UPDATE_COMPLETE 85
#define NAS_AUTHENTICATION_REQUEST 86
#define NAS_AUTHENTICATION_RESPONSE 87
#define NAS_AUTHENTICATION_REJECT 88
#define NAS_AUTHENTICATION_FAILURE 89
#define NAS_AUTHENTICATION_RESULT 90
#define NAS_IDENTITY_REQUEST 91
#define NAS_IDENTITY_RESPONSE 92
#define NAS_SECURITY_MODE_COMMAND 93
#define NAS_SECURITY_MODE_COMPLETE 94
#define NAS_SECURITY_MODE_REJECT 95
#define NAS_GMM_STATUS 100
#define NAS_NOTIFICATION 101
#define NAS_NOTIFICATION_RESPONSE 102
#define NAS_UL_NAS_TRANSPORT 103
#define NAS_DL_NAS_TRANSPORT 104
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST 193
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT 194
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT 195
#define NAS_PDU_SESSION_AUTHENTICATION_COMMAND 197
#define NAS_PDU_SESSION_AUTHENTICATION_COMPLETE 198
#define NAS_PDU_SESSION_AUTHENTICATION_RESULT 199
#define NAS_PDU_SESSION_MODIFICATION_REQUEST 201
#define NAS_PDU_SESSION_MODIFICATION_REJECT 202
#define NAS_PDU_SESSION_MODIFICATION_COMMAND 203
#define NAS_PDU_SESSION_MODIFICATION_COMPLETE 204
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT 205
#define NAS_PDU_SESSION_RELEASE_REQUEST 209
#define NAS_PDU_SESSION_RELEASE_REJECT 210
#define NAS_PDU_SESSION_RELEASE_COMMAND 211
#define NAS_PDU_SESSION_RELEASE_COMPLETE 212
#define NAS_GSM_STATUS 214


/*******************************************************
 * REGISTRATION REQUEST
 ******************************************************/
#define NAS_REGISTRATION_REQUEST_NON_CURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER_PRESENT (1<<0)
#define NAS_REGISTRATION_REQUEST_5GMM_CAPABILITY_PRESENT (1<<1)
#define NAS_REGISTRATION_REQUEST_UE_SECURITY_CAPABILITY_PRESENT (1<<2)
#define NAS_REGISTRATION_REQUEST_REQUESTED_NSSAI_PRESENT (1<<3)
#define NAS_REGISTRATION_REQUEST_LAST_VISITED_REGISTERED_TAI_PRESENT (1<<4)
#define NAS_REGISTRATION_REQUEST_S1_UE_NETWORK_CAPABILITY_PRESENT (1<<5)
#define NAS_REGISTRATION_REQUEST_UPLINK_DATA_STATUS_PRESENT (1<<6)
#define NAS_REGISTRATION_REQUEST_PDU_SESSION_STATUS_PRESENT (1<<7)
#define NAS_REGISTRATION_REQUEST_MICO_INDICATION_PRESENT (1<<8)
#define NAS_REGISTRATION_REQUEST_UE_STATUS_PRESENT (1<<9)
#define NAS_REGISTRATION_REQUEST_ADDITIONAL_GUTI_PRESENT (1<<10)
#define NAS_REGISTRATION_REQUEST_ALLOWED_PDU_SESSION_STATUS_PRESENT (1<<11)
#define NAS_REGISTRATION_REQUEST_UE_USAGE_SETTING_PRESENT (1<<12)
#define NAS_REGISTRATION_REQUEST_REQUESTED_DRX_PARAMETERS_PRESENT (1<<13)
#define NAS_REGISTRATION_REQUEST_EPS_NAS_MESSAGE_CONTAINER_PRESENT (1<<14)
#define NAS_REGISTRATION_REQUEST_PAYLOAD_CONTAINER_PRESENT (1<<15)
#define NAS_REGISTRATION_REQUEST_NON_CURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER_TYPE 0xC0
#define NAS_REGISTRATION_REQUEST_5GMM_CAPABILITY_TYPE 0x10
#define NAS_REGISTRATION_REQUEST_UE_SECURITY_CAPABILITY_TYPE 0x2E
#define NAS_REGISTRATION_REQUEST_REQUESTED_NSSAI_TYPE 0x2F
#define NAS_REGISTRATION_REQUEST_LAST_VISITED_REGISTERED_TAI_TYPE 0x52
#define NAS_REGISTRATION_REQUEST_S1_UE_NETWORK_CAPABILITY_TYPE 0x65
#define NAS_REGISTRATION_REQUEST_UPLINK_DATA_STATUS_TYPE 0x40
#define NAS_REGISTRATION_REQUEST_PDU_SESSION_STATUS_TYPE 0x50
#define NAS_REGISTRATION_REQUEST_MICO_INDICATION_TYPE 0xB0
#define NAS_REGISTRATION_REQUEST_UE_STATUS_TYPE 0x2B
#define NAS_REGISTRATION_REQUEST_ADDITIONAL_GUTI_TYPE 0x2C
#define NAS_REGISTRATION_REQUEST_ALLOWED_PDU_SESSION_STATUS_TYPE 0x25
#define NAS_REGISTRATION_REQUEST_UE_USAGE_SETTING_TYPE 0x60
#define NAS_REGISTRATION_REQUEST_REQUESTED_DRX_PARAMETERS_TYPE 0xTBD
#define NAS_REGISTRATION_REQUEST_EPS_NAS_MESSAGE_CONTAINER_TYPE 0x7C
#define NAS_REGISTRATION_REQUEST_PAYLOAD_CONTAINER_TYPE 0x77

typedef struct _nas_registration_request_t {
    /* Mandatory fields */
    nas_5gs_registration_type_t 5gs_registration_type;
    nas_key_set_identifier_t ngksi;
    nas_5gs_mobile_identity_t 5gs_mobile_identity;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_key_set_identifier_t non_current_native_nas_key_set_identifier;
    nas_5gmm_capability_t 5gmm_capability;
    nas_ue_security_capability_t ue_security_capability;
    nas_nssai_t requested_nssai;
    nas_5gs_tracking_area_identity_t last_visited_registered_tai;
    nas_s1_ue_network_capability_t s1_ue_network_capability;
    nas_uplink_data_status_t uplink_data_status;
    nas_pdu_session_status_t pdu_session_status;
    nas_mico_indication_t mico_indication;
    nas_ue_status_t ue_status;
    nas_5gs_mobile_identity_t additional_guti;
    nas_allowed_pdu_session_status_t allowed_pdu_session_status;
    nas_ue_usage_setting_t ue_usage_setting;
    nas_drx_parameters_t requested_drx_parameters;
    nas_eps_nas_message_container_t eps_nas_message_container;
    nas_payload_container_t payload_container;
} nas_registration_request_t;


/*******************************************************
 * REGISTRATION ACCEPT
 ******************************************************/
#define NAS_REGISTRATION_ACCEPT_5G_GUTI_PRESENT (1<<0)
#define NAS_REGISTRATION_ACCEPT_EQUIVALENT_PLMNS_PRESENT (1<<1)
#define NAS_REGISTRATION_ACCEPT_TAI_LIST_PRESENT (1<<2)
#define NAS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_PRESENT (1<<3)
#define NAS_REGISTRATION_ACCEPT_REJECTED_NSSAI_PRESENT (1<<4)
#define NAS_REGISTRATION_ACCEPT_CONFIGURED_NSSAI_PRESENT (1<<5)
#define NAS_REGISTRATION_ACCEPT_5GS_NETWORK_FEATURE_SUPPORT_PRESENT (1<<6)
#define NAS_REGISTRATION_ACCEPT_PDU_SESSION_STATUS_PRESENT (1<<7)
#define NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_PRESENT (1<<8)
#define NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_PRESENT (1<<9)
#define NAS_REGISTRATION_ACCEPT_LADN_INFORMATION_PRESENT (1<<10)
#define NAS_REGISTRATION_ACCEPT_MICO_INDICATION_PRESENT (1<<11)
#define NAS_REGISTRATION_ACCEPT_SERVICE_AREA_LIST_PRESENT (1<<12)
#define NAS_REGISTRATION_ACCEPT_T3512_VALUE_PRESENT (1<<13)
#define NAS_REGISTRATION_ACCEPT_NON__DE_REGISTRATION_TIMER_VALUE_PRESENT (1<<14)
#define NAS_REGISTRATION_ACCEPT_T3502_VALUE_PRESENT (1<<15)
#define NAS_REGISTRATION_ACCEPT_EMERGENCY_NUMBER_LIST_PRESENT (1<<16)
#define NAS_REGISTRATION_ACCEPT_EXTENDED_EMERGENCY_NUMBER_LIST_PRESENT (1<<17)
#define NAS_REGISTRATION_ACCEPT_TRANSPARENT_CONTAINER_PRESENT (1<<18)
#define NAS_REGISTRATION_ACCEPT_EAP_MESSAGE_PRESENT (1<<19)
#define NAS_REGISTRATION_ACCEPT_5G_GUTI_TYPE 0x2C
#define NAS_REGISTRATION_ACCEPT_EQUIVALENT_PLMNS_TYPE 0x4A
#define NAS_REGISTRATION_ACCEPT_TAI_LIST_TYPE 0x54
#define NAS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_TYPE 0x70
#define NAS_REGISTRATION_ACCEPT_REJECTED_NSSAI_TYPE 0x11
#define NAS_REGISTRATION_ACCEPT_CONFIGURED_NSSAI_TYPE 0x31
#define NAS_REGISTRATION_ACCEPT_5GS_NETWORK_FEATURE_SUPPORT_TYPE 0x64
#define NAS_REGISTRATION_ACCEPT_PDU_SESSION_STATUS_TYPE 0x50
#define NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_TYPE 0x26
#define NAS_REGISTRATION_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_TYPE 0x7E
#define NAS_REGISTRATION_ACCEPT_LADN_INFORMATION_TYPE 0x79
#define NAS_REGISTRATION_ACCEPT_MICO_INDICATION_TYPE 0xB0
#define NAS_REGISTRATION_ACCEPT_SERVICE_AREA_LIST_TYPE 0x27
#define NAS_REGISTRATION_ACCEPT_T3512_VALUE_TYPE 0x5E
#define NAS_REGISTRATION_ACCEPT_NON__DE_REGISTRATION_TIMER_VALUE_TYPE 0x5D
#define NAS_REGISTRATION_ACCEPT_T3502_VALUE_TYPE 0x16
#define NAS_REGISTRATION_ACCEPT_EMERGENCY_NUMBER_LIST_TYPE 0x34
#define NAS_REGISTRATION_ACCEPT_EXTENDED_EMERGENCY_NUMBER_LIST_TYPE 0x35
#define NAS_REGISTRATION_ACCEPT_TRANSPARENT_CONTAINER_TYPE 0xTBD
#define NAS_REGISTRATION_ACCEPT_EAP_MESSAGE_TYPE 0x78

typedef struct _nas_registration_accept_t {
    /* Mandatory fields */
    nas_5gs_registration_result_t 5gs_registration_result;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_5gs_mobile_identity_t 5g_guti;
    nas_plmn_list_t equivalent_plmns;
    nas_5gs_tracking_area_identity_list_t tai_list;
    nas_nssai_t allowed_nssai;
    nas_rejected_nssai_t rejected_nssai;
    nas_nssai_t configured_nssai;
    nas_5gs_network_feature_support_t 5gs_network_feature_support;
    nas_pdu_session_status_t pdu_session_status;
    nas_pdu_session_reactivation_result_t pdu_session_reactivation_result;
    nas_pdu_session_reactivation_result_error_cause_t pdu_session_reactivation_result_error_cause;
    nas_ladn_information_t ladn_information;
    nas_mico_indication_t mico_indication;
    nas_service_area_list_t service_area_list;
    nas_gprs_timer_3_t t3512_value;
    nas_gprs_timer_2_t non__de_registration_timer_value;
    nas_gprs_timer_2_t t3502_value;
    nas_emergency_number_list_t emergency_number_list;
    nas_extended_emergency_number_list_t extended_emergency_number_list;
    nas_transparent_container_t transparent_container;
    nas_eap_message_t eap_message;
} nas_registration_accept_t;


/*******************************************************
 * REGISTRATION COMPLETE
 ******************************************************/
#define NAS_REGISTRATION_COMPLETE_TRANSPARENT_CONTAINER_PRESENT (1<<0)
#define NAS_REGISTRATION_COMPLETE_TRANSPARENT_CONTAINER_TYPE 0xTBD

typedef struct _nas_registration_complete_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_transparent_container_t transparent_container;
} nas_registration_complete_t;


/*******************************************************
 * REGISTRATION REJECT
 ******************************************************/
#define NAS_REGISTRATION_REJECT_T3346_VALUE_PRESENT (1<<0)
#define NAS_REGISTRATION_REJECT_T3502_VALUE_PRESENT (1<<1)
#define NAS_REGISTRATION_REJECT_EAP_MESSAGE_PRESENT (1<<2)
#define NAS_REGISTRATION_REJECT_T3346_VALUE_TYPE 0x5F
#define NAS_REGISTRATION_REJECT_T3502_VALUE_TYPE 0x16
#define NAS_REGISTRATION_REJECT_EAP_MESSAGE_TYPE 0x78

typedef struct _nas_registration_reject_t {
    /* Mandatory fields */
    nas_5gmm_cause_t 5gmm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_gprs_timer_2_t t3346_value;
    nas_gprs_timer_2_t t3502_value;
    nas_eap_message_t eap_message;
} nas_registration_reject_t;


/*******************************************************
 * DEREGISTRATION REQUEST FROM UE
 ******************************************************/

typedef struct _nas_deregistration_request_from_ue_t {
    /* Mandatory fields */
    nas_de_registration_type_t de_registration_type;
    nas_5gs_mobile_identity_t 5gs_mobile_identity;
} nas_deregistration_request_from_ue_t;


/*******************************************************
 * DEREGISTRATION REQUEST TO UE
 ******************************************************/
#define NAS_DEREGISTRATION_REQUEST_5GMM_CAUSE_PRESENT (1<<0)
#define NAS_DEREGISTRATION_REQUEST_T3346_VALUE_PRESENT (1<<1)
#define NAS_DEREGISTRATION_REQUEST_5GMM_CAUSE_TYPE 0x58
#define NAS_DEREGISTRATION_REQUEST_T3346_VALUE_TYPE 0x5F

typedef struct _nas_deregistration_request_to_ue_t {
    /* Mandatory fields */
    nas_de_registration_type_t de_registration_type;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_5gmm_cause_t 5gmm_cause;
    nas_gprs_timer_2_t t3346_value;
} nas_deregistration_request_to_ue_t;


/*******************************************************
 * SERVICE REQUEST
 ******************************************************/
#define NAS_SERVICE_REQUEST_UPLINK_DATA_STATUS_PRESENT (1<<0)
#define NAS_SERVICE_REQUEST_PDU_SESSION_STATUS_PRESENT (1<<1)
#define NAS_SERVICE_REQUEST_ALLOWED_PDU_SESSION_STATUS_PRESENT (1<<2)
#define NAS_SERVICE_REQUEST_UPLINK_DATA_STATUS_TYPE 0x40
#define NAS_SERVICE_REQUEST_PDU_SESSION_STATUS_TYPE 0x50
#define NAS_SERVICE_REQUEST_ALLOWED_PDU_SESSION_STATUS_TYPE 0x25

typedef struct _nas_service_request_t {
    /* Mandatory fields */
    nas_key_set_identifier_t ngksi;
    nas_5gs_mobile_identity_t 5g_s_tmsi;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_uplink_data_status_t uplink_data_status;
    nas_pdu_session_status_t pdu_session_status;
    nas_allowed_pdu_session_status_t allowed_pdu_session_status;
} nas_service_request_t;


/*******************************************************
 * SERVICE REJECT
 ******************************************************/
#define NAS_SERVICE_REJECT_PDU_SESSION_STATUS_PRESENT (1<<0)
#define NAS_SERVICE_REJECT_T3346_VALUE_PRESENT (1<<1)
#define NAS_SERVICE_REJECT_EAP_MESSAGE_PRESENT (1<<2)
#define NAS_SERVICE_REJECT_PDU_SESSION_STATUS_TYPE 0x50
#define NAS_SERVICE_REJECT_T3346_VALUE_TYPE 0x5F
#define NAS_SERVICE_REJECT_EAP_MESSAGE_TYPE 0x78

typedef struct _nas_service_reject_t {
    /* Mandatory fields */
    nas_5gmm_cause_t 5gmm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_pdu_session_status_t pdu_session_status;
    nas_gprs_timer_2_t t3346_value;
    nas_eap_message_t eap_message;
} nas_service_reject_t;


/*******************************************************
 * SERVICE ACCEPT
 ******************************************************/
#define NAS_SERVICE_ACCEPT_PDU_SESSION_STATUS_PRESENT (1<<0)
#define NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_PRESENT (1<<1)
#define NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_PRESENT (1<<2)
#define NAS_SERVICE_ACCEPT_EAP_MESSAGE_PRESENT (1<<3)
#define NAS_SERVICE_ACCEPT_PDU_SESSION_STATUS_TYPE 0x50
#define NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_TYPE 0x26
#define NAS_SERVICE_ACCEPT_PDU_SESSION_REACTIVATION_RESULT_ERROR_CAUSE_TYPE 0x7E
#define NAS_SERVICE_ACCEPT_EAP_MESSAGE_TYPE 0x78

typedef struct _nas_service_accept_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_pdu_session_status_t pdu_session_status;
    nas_pdu_session_reactivation_result_t pdu_session_reactivation_result;
    nas_pdu_session_reactivation_result_error_cause_t pdu_session_reactivation_result_error_cause;
    nas_eap_message_t eap_message;
} nas_service_accept_t;


/*******************************************************
 * CONFIGURATION UPDATE COMMAND
 ******************************************************/
#define NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURATION_UPDATE_INDICATION_PRESENT (1<<0)
#define NAS_CONFIGURATION_UPDATE_COMMAND_5G_GUTI_PRESENT (1<<1)
#define NAS_CONFIGURATION_UPDATE_COMMAND_TAI_LIST_PRESENT (1<<2)
#define NAS_CONFIGURATION_UPDATE_COMMAND_ALLOWED_NSSAI_PRESENT (1<<3)
#define NAS_CONFIGURATION_UPDATE_COMMAND_SERVICE_AREA_LIST_PRESENT (1<<4)
#define NAS_CONFIGURATION_UPDATE_COMMAND_FULL_NAME_FOR_NETWORK_PRESENT (1<<5)
#define NAS_CONFIGURATION_UPDATE_COMMAND_SHORT_NAME_FOR_NETWORK_PRESENT (1<<6)
#define NAS_CONFIGURATION_UPDATE_COMMAND_LOCAL_TIME_ZONE_PRESENT (1<<7)
#define NAS_CONFIGURATION_UPDATE_COMMAND_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_PRESENT (1<<8)
#define NAS_CONFIGURATION_UPDATE_COMMAND_NETWORK_DAYLIGHT_SAVING_TIME_PRESENT (1<<9)
#define NAS_CONFIGURATION_UPDATE_COMMAND_LADN_INFORMATION_PRESENT (1<<10)
#define NAS_CONFIGURATION_UPDATE_COMMAND_MICO_INDICATION_PRESENT (1<<11)
#define NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURED_NSSAI_PRESENT (1<<12)
#define NAS_CONFIGURATION_UPDATE_COMMAND_REJECTED_NSSAI_PRESENT (1<<13)
#define NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURATION_UPDATE_INDICATION_TYPE 0xD0
#define NAS_CONFIGURATION_UPDATE_COMMAND_5G_GUTI_TYPE 0x2C
#define NAS_CONFIGURATION_UPDATE_COMMAND_TAI_LIST_TYPE 0x54
#define NAS_CONFIGURATION_UPDATE_COMMAND_ALLOWED_NSSAI_TYPE 0x70
#define NAS_CONFIGURATION_UPDATE_COMMAND_SERVICE_AREA_LIST_TYPE 0x27
#define NAS_CONFIGURATION_UPDATE_COMMAND_FULL_NAME_FOR_NETWORK_TYPE 0x43
#define NAS_CONFIGURATION_UPDATE_COMMAND_SHORT_NAME_FOR_NETWORK_TYPE 0x45
#define NAS_CONFIGURATION_UPDATE_COMMAND_LOCAL_TIME_ZONE_TYPE 0x46
#define NAS_CONFIGURATION_UPDATE_COMMAND_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE_TYPE 0x47
#define NAS_CONFIGURATION_UPDATE_COMMAND_NETWORK_DAYLIGHT_SAVING_TIME_TYPE 0x49
#define NAS_CONFIGURATION_UPDATE_COMMAND_LADN_INFORMATION_TYPE 0x79
#define NAS_CONFIGURATION_UPDATE_COMMAND_MICO_INDICATION_TYPE 0xB0
#define NAS_CONFIGURATION_UPDATE_COMMAND_CONFIGURED_NSSAI_TYPE 0x31
#define NAS_CONFIGURATION_UPDATE_COMMAND_REJECTED_NSSAI_TYPE 0x11

typedef struct _nas_configuration_update_command_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_configuration_update_indication_t configuration_update_indication;
    nas_5gs_mobile_identity_t 5g_guti;
    nas_5gs_tracking_area_identity_list_t tai_list;
    nas_nssai_t allowed_nssai;
    nas_service_area_list_t service_area_list;
    nas_network_name_t full_name_for_network;
    nas_network_name_t short_name_for_network;
    nas_time_zone_t local_time_zone;
    nas_time_zone_and_time_t universal_time_and_local_time_zone;
    nas_daylight_saving_time_t network_daylight_saving_time;
    nas_ladn_information_t ladn_information;
    nas_mico_indication_t mico_indication;
    nas_nssai_t configured_nssai;
    nas_rejected_nssai_t rejected_nssai;
} nas_configuration_update_command_t;


/*******************************************************
 * CONFIGURATION UPDATE COMPLETE
 ******************************************************/
#define NAS_CONFIGURATION_UPDATE_COMPLETE_PDU_SESSION_TYPE_PRESENT (1<<0)
#define NAS_CONFIGURATION_UPDATE_COMPLETE_SSC_MODE_PRESENT (1<<1)
#define NAS_CONFIGURATION_UPDATE_COMPLETE_5GSM_CAPABILITY_PRESENT (1<<2)
#define NAS_CONFIGURATION_UPDATE_COMPLETE_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT (1<<3)
#define NAS_CONFIGURATION_UPDATE_COMPLETE_SM_PDU_DN_REQUEST_CONTAINER_PRESENT (1<<4)
#define NAS_CONFIGURATION_UPDATE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<5)
#define NAS_CONFIGURATION_UPDATE_COMPLETE_PDU_SESSION_TYPE_TYPE 0x90
#define NAS_CONFIGURATION_UPDATE_COMPLETE_SSC_MODE_TYPE 0xA0
#define NAS_CONFIGURATION_UPDATE_COMPLETE_5GSM_CAPABILITY_TYPE 0x28
#define NAS_CONFIGURATION_UPDATE_COMPLETE_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE 0x55
#define NAS_CONFIGURATION_UPDATE_COMPLETE_SM_PDU_DN_REQUEST_CONTAINER_TYPE 0xTBD
#define NAS_CONFIGURATION_UPDATE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_configuration_update_complete_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_pdu_session_type_t pdu_session_type;
    nas_ssc_mode_t ssc_mode;
    nas_5gsm_capability_t 5gsm_capability;
    nas_maximum_number_of_supported_packet_filters_t maximum_number_of_supported_packet_filters;
    nas_sm_pdu_dn_request_container_t sm_pdu_dn_request_container;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_configuration_update_complete_t;


/*******************************************************
 * AUTHENTICATION REQUEST
 ******************************************************/
#define NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_RAND_PRESENT (1<<0)
#define NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_AUTN_PRESENT (1<<1)
#define NAS_AUTHENTICATION_REQUEST_EAP_MESSAGE_PRESENT (1<<2)
#define NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_RAND_TYPE 0x21
#define NAS_AUTHENTICATION_REQUEST_AUTHENTICATION_PARAMETER_AUTN_TYPE 0x20
#define NAS_AUTHENTICATION_REQUEST_EAP_MESSAGE_TYPE 0x78

typedef struct _nas_authentication_request_t {
    /* Mandatory fields */
    nas_key_set_identifier_t ngksi;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_authentication_parameter_t authentication_parameter_rand;
    nas_authentication_parameter_autn_t authentication_parameter_autn;
    nas_eap_message_t eap_message;
} nas_authentication_request_t;


/*******************************************************
 * AUTHENTICATION RESPONSE
 ******************************************************/
#define NAS_AUTHENTICATION_RESPONSE_AUTHENTICATION_RESPONSE_PARAMETER_PRESENT (1<<0)
#define NAS_AUTHENTICATION_RESPONSE_EAP_MESSAGE_PRESENT (1<<1)
#define NAS_AUTHENTICATION_RESPONSE_AUTHENTICATION_RESPONSE_PARAMETER_TYPE 0x2D
#define NAS_AUTHENTICATION_RESPONSE_EAP_MESSAGE_TYPE 0x78

typedef struct _nas_authentication_response_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_authentication_response_parameter_t authentication_response_parameter;
    nas_eap_message_t eap_message;
} nas_authentication_response_t;


/*******************************************************
 * AUTHENTICATION FAILURE
 ******************************************************/
#define NAS_AUTHENTICATION_FAILURE_AUTHENTICATION_FAILURE_PARAMETER_PRESENT (1<<0)
#define NAS_AUTHENTICATION_FAILURE_AUTHENTICATION_FAILURE_PARAMETER_TYPE 0x30

typedef struct _nas_authentication_failure_t {
    /* Mandatory fields */
    nas_5gmm_cause_t 5gmm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_authentication_failure_parameter_t authentication_failure_parameter;
} nas_authentication_failure_t;


/*******************************************************
 * AUTHENTICATION RESULT
 ******************************************************/

typedef struct _nas_authentication_result_t {
    /* Mandatory fields */
    nas_key_set_identifier_t ngksi;
    nas_eap_message_t eap_message;
} nas_authentication_result_t;


/*******************************************************
 * IDENTITY REQUEST
 ******************************************************/

typedef struct _nas_identity_request_t {
    /* Mandatory fields */
    nas_5gs_identity_type_t identity_type;
} nas_identity_request_t;


/*******************************************************
 * IDENTITY RESPONSE
 ******************************************************/

typedef struct _nas_identity_response_t {
    /* Mandatory fields */
    nas_5gs_mobile_identity_t mobile_identity;
} nas_identity_response_t;


/*******************************************************
 * SECURITY MODE COMMAND
 ******************************************************/
#define NAS_SECURITY_MODE_COMMAND_IMEISV_REQUEST_PRESENT (1<<0)
#define NAS_SECURITY_MODE_COMMAND_HASHAMF_PRESENT (1<<1)
#define NAS_SECURITY_MODE_COMMAND_SELECTED_EPS_NAS_SECURITY_ALGORITHMS_PRESENT (1<<2)
#define NAS_SECURITY_MODE_COMMAND_EAP_MESSAGE_PRESENT (1<<3)
#define NAS_SECURITY_MODE_COMMAND_IMEISV_REQUEST_TYPE 0xE0
#define NAS_SECURITY_MODE_COMMAND_HASHAMF_TYPE 0x4F
#define NAS_SECURITY_MODE_COMMAND_SELECTED_EPS_NAS_SECURITY_ALGORITHMS_TYPE 0x57
#define NAS_SECURITY_MODE_COMMAND_EAP_MESSAGE_TYPE 0x78

typedef struct _nas_security_mode_command_t {
    /* Mandatory fields */
    nas_security_algorithms_t selected_nas_security_algorithms;
    nas_key_set_identifier_t ngksi;
    nas_ue_security_capability_t replayed_ue_security_capabilities;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_imeisv_request_t imeisv_request;
    nas_hashamf_t hashamf;
    nas_eps_nas_security_algorithms_t selected_eps_nas_security_algorithms;
    nas_eap_message_t eap_message;
} nas_security_mode_command_t;


/*******************************************************
 * SECURITY MODE COMPLETE
 ******************************************************/
#define NAS_SECURITY_MODE_COMPLETE_IMEISV_PRESENT (1<<0)
#define NAS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_PRESENT (1<<1)
#define NAS_SECURITY_MODE_COMPLETE_IMEISV_TYPE 0x2C
#define NAS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_TYPE 0x7D

typedef struct _nas_security_mode_complete_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_5g_mobile_identity_t imeisv;
    nas_message_container_t nas_message_container;
} nas_security_mode_complete_t;


/*******************************************************
 * SECURITY MODE REJECT
 ******************************************************/

typedef struct _nas_security_mode_reject_t {
    /* Mandatory fields */
    nas_5gmm_cause_t 5gmm_cause;
} nas_security_mode_reject_t;


/*******************************************************
 * GMM STATUS
 ******************************************************/

typedef struct _nas_gmm_status_t {
    /* Mandatory fields */
    nas_5gmm_cause_t 5gmm_cause;
} nas_gmm_status_t;


/*******************************************************
 * NOTIFICATION
 ******************************************************/

typedef struct _nas_notification_t {
    /* Mandatory fields */
    nas_access_type_t access_type;
} nas_notification_t;


/*******************************************************
 * NOTIFICATION RESPONSE
 ******************************************************/
#define NAS_NOTIFICATION_RESPONSE_PDU_SESSION_STATUS_PRESENT (1<<0)
#define NAS_NOTIFICATION_RESPONSE_PDU_SESSION_STATUS_TYPE 0x50

typedef struct _nas_notification_response_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_pdu_session_status_t pdu_session_status;
} nas_notification_response_t;


/*******************************************************
 * UL NAS TRANSPORT
 ******************************************************/
#define NAS_UL_NAS_TRANSPORT_OLD_PDU_SESSION_ID_PRESENT (1<<0)
#define NAS_UL_NAS_TRANSPORT_REQUEST_TYPE_PRESENT (1<<1)
#define NAS_UL_NAS_TRANSPORT_S_NSSAI_PRESENT (1<<2)
#define NAS_UL_NAS_TRANSPORT_DNN_PRESENT (1<<3)
#define NAS_UL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_PRESENT (1<<4)
#define NAS_UL_NAS_TRANSPORT_OLD_PDU_SESSION_ID_TYPE 0x61
#define NAS_UL_NAS_TRANSPORT_REQUEST_TYPE_TYPE 0x80
#define NAS_UL_NAS_TRANSPORT_S_NSSAI_TYPE 0x22
#define NAS_UL_NAS_TRANSPORT_DNN_TYPE 0x25
#define NAS_UL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_TYPE 0x24

typedef struct _nas_ul_nas_transport_t {
    /* Mandatory fields */
    nas_payload_container_type_t payload_container_type;
    nas_payload_container_t payload_container;
    nas_pdu_session_identity_2_t pdu_session_id;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_pdu_session_identity_2_t old_pdu_session_id;
    nas_request_type_t request_type;
    nas_s_nssai_t s_nssai;
    nas_dnn_t dnn;
    nas_additional_information_t additional_information;
} nas_ul_nas_transport_t;


/*******************************************************
 * DL NAS TRANSPORT
 ******************************************************/
#define NAS_DL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_PRESENT (1<<0)
#define NAS_DL_NAS_TRANSPORT_5GMM_CAUSE_PRESENT (1<<1)
#define NAS_DL_NAS_TRANSPORT_BACK_OFF_TIMER_VALUE_PRESENT (1<<2)
#define NAS_DL_NAS_TRANSPORT_ADDITIONAL_INFORMATION_TYPE 0x24
#define NAS_DL_NAS_TRANSPORT_5GMM_CAUSE_TYPE 0x58
#define NAS_DL_NAS_TRANSPORT_BACK_OFF_TIMER_VALUE_TYPE 0x37

typedef struct _nas_dl_nas_transport_t {
    /* Mandatory fields */
    nas_payload_container_type_t payload_container_type;
    nas_payload_container_t payload_container;
    nas_pdu_session_identity_2_t pdu_session_id;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_additional_information_t additional_information;
    nas_5gmm_cause_t 5gmm_cause;
    nas_gprs_timer_3_t back_off_timer_value;
} nas_dl_nas_transport_t;


/*******************************************************
 * PDU SESSION ESTABLISHMENT REQUEST
 ******************************************************/
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_PDU_SESSION_TYPE_PRESENT (1<<0)
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SSC_MODE_PRESENT (1<<1)
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_5GSM_CAPABILITY_PRESENT (1<<2)
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT (1<<3)
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SM_PDU_DN_REQUEST_CONTAINER_PRESENT (1<<4)
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<5)
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_PDU_SESSION_TYPE_TYPE 0x90
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SSC_MODE_TYPE 0xA0
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_5GSM_CAPABILITY_TYPE 0x28
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE 0x55
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_SM_PDU_DN_REQUEST_CONTAINER_TYPE 0xTBD
#define NAS_PDU_SESSION_ESTABLISHMENT_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_establishment_request_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_pdu_session_type_t pdu_session_type;
    nas_ssc_mode_t ssc_mode;
    nas_5gsm_capability_t 5gsm_capability;
    nas_maximum_number_of_supported_packet_filters_t maximum_number_of_supported_packet_filters;
    nas_sm_pdu_dn_request_container_t sm_pdu_dn_request_container;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_establishment_request_t;


/*******************************************************
 * PDU SESSION ESTABLISHMENT ACCEPT
 ******************************************************/
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_5GSM_CAUSE_PRESENT (1<<0)
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_PDU_ADDRESS_PRESENT (1<<1)
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_RQ_TIMER_VALUE_PRESENT (1<<2)
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_S_NSSAI_PRESENT (1<<3)
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_MAPPED_EPS_BEARER_CONTEXTS_PRESENT (1<<4)
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EAP_MESSAGE_PRESENT (1<<5)
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<6)
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_5GSM_CAUSE_TYPE 0x59
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_PDU_ADDRESS_TYPE 0x29
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_RQ_TIMER_VALUE_TYPE 0x56
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_S_NSSAI_TYPE 0x22
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_MAPPED_EPS_BEARER_CONTEXTS_TYPE 0x7F
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EAP_MESSAGE_TYPE 0x78
#define NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_establishment_accept_t {
    /* Mandatory fields */
    nas_pdu_session_type_t selected_pdu_session_type;
    nas_dnn_t dnn;
    nas_qos_rules_t authorized_qos_rules;
    nas_session_ambr_t session_ambr;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_5gsm_cause_t 5gsm_cause;
    nas_pdu_address_t pdu_address;
    nas_gprs_timer_t rq_timer_value;
    nas_s_nssai_t s_nssai;
    nas_mapped_eps_bearer_contexts_t mapped_eps_bearer_contexts;
    nas_eap_message_t eap_message;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_establishment_accept_t;


/*******************************************************
 * PDU SESSION ESTABLISHMENT REJECT
 ******************************************************/
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_BACK_OFF_TIMER_VALUE_PRESENT (1<<0)
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_ALLOWED_SSC_MODE_PRESENT (1<<1)
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EAP_MESSAGE_PRESENT (1<<2)
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<3)
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_BACK_OFF_TIMER_VALUE_TYPE 0x37
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_ALLOWED_SSC_MODE_TYPE 0xF0
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EAP_MESSAGE_TYPE 0x78
#define NAS_PDU_SESSION_ESTABLISHMENT_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_establishment_reject_t {
    /* Mandatory fields */
    nas_5gsm_cause_t 5gsm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_gprs_timer_3_t back_off_timer_value;
    nas_allowed_ssc_mode_t allowed_ssc_mode;
    nas_eap_message_t eap_message;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_establishment_reject_t;


/*******************************************************
 * PDU SESSION AUTHENTICATION COMMAND
 ******************************************************/
#define NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EAP_MESSAGE_PRESENT (1<<0)
#define NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<1)
#define NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EAP_MESSAGE_TYPE 0x78
#define NAS_PDU_SESSION_AUTHENTICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_authentication_command_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_eap_message_t eap_message;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_authentication_command_t;


/*******************************************************
 * PDU SESSION AUTHENTICATION COMPLETE
 ******************************************************/
#define NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EAP_MESSAGE_PRESENT (1<<0)
#define NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<1)
#define NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EAP_MESSAGE_TYPE 0x78
#define NAS_PDU_SESSION_AUTHENTICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_authentication_complete_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_eap_message_t eap_message;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_authentication_complete_t;


/*******************************************************
 * PDU SESSION AUTHENTICATION RESULT
 ******************************************************/
#define NAS_PDU_SESSION_AUTHENTICATION_RESULT_EAP_MESSAGE_PRESENT (1<<0)
#define NAS_PDU_SESSION_AUTHENTICATION_RESULT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<1)
#define NAS_PDU_SESSION_AUTHENTICATION_RESULT_EAP_MESSAGE_TYPE 0x78
#define NAS_PDU_SESSION_AUTHENTICATION_RESULT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_authentication_result_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_eap_message_t eap_message;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_authentication_result_t;


/*******************************************************
 * PDU SESSION MODIFICATION REQUEST
 ******************************************************/
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_5GSM_CAPABILITY_PRESENT (1<<0)
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_PRESENT (1<<1)
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_REQUESTED_QOS_RULES_PRESENT (1<<2)
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<3)
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_5GSM_CAPABILITY_TYPE 0x28
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_MAXIMUM_NUMBER_OF_SUPPORTED_PACKET_FILTERS_TYPE 0x55
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_REQUESTED_QOS_RULES_TYPE 0x7A
#define NAS_PDU_SESSION_MODIFICATION_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_modification_request_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_5gsm_capability_t 5gsm_capability;
    nas_maximum_number_of_supported_packet_filters_t maximum_number_of_supported_packet_filters;
    nas_qos_rules_t requested_qos_rules;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_modification_request_t;


/*******************************************************
 * PDU SESSION MODIFICATION REJECT
 ******************************************************/
#define NAS_PDU_SESSION_MODIFICATION_REJECT_BACK_OFF_TIMER_VALUE_PRESENT (1<<0)
#define NAS_PDU_SESSION_MODIFICATION_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<1)
#define NAS_PDU_SESSION_MODIFICATION_REJECT_BACK_OFF_TIMER_VALUE_TYPE 0x37
#define NAS_PDU_SESSION_MODIFICATION_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_modification_reject_t {
    /* Mandatory fields */
    nas_5gsm_cause_t 5gsm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_gprs_timer_3_t back_off_timer_value;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_modification_reject_t;


/*******************************************************
 * PDU SESSION MODIFICATION COMMAND
 ******************************************************/
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_5GSM_CAUSE_PRESENT (1<<0)
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_SESSION_AMBR_PRESENT (1<<1)
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_RQ_TIMER_VALUE_PRESENT (1<<2)
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_AUTHORIZED_QOS_RULES_PRESENT (1<<3)
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_MAPPED_EPS_BEARER_CONTEXTS_PRESENT (1<<4)
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<5)
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_5GSM_CAUSE_TYPE 0x59
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_SESSION_AMBR_TYPE 0x2A
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_RQ_TIMER_VALUE_TYPE 0x56
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_AUTHORIZED_QOS_RULES_TYPE 0x7A
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_MAPPED_EPS_BEARER_CONTEXTS_TYPE 0x7F
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_modification_command_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_5gsm_cause_t 5gsm_cause;
    nas_session_ambr_t session_ambr;
    nas_gprs_timer_t rq_timer_value;
    nas_qos_rules_t authorized_qos_rules;
    nas_mapped_eps_bearer_contexts_t mapped_eps_bearer_contexts;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_modification_command_t;


/*******************************************************
 * PDU SESSION MODIFICATION COMPLETE
 ******************************************************/
#define NAS_PDU_SESSION_MODIFICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<0)
#define NAS_PDU_SESSION_MODIFICATION_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_modification_complete_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_modification_complete_t;


/*******************************************************
 * PDU SESSION MODIFICATION COMMAND REJECT
 ******************************************************/
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<0)
#define NAS_PDU_SESSION_MODIFICATION_COMMAND_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_modification_command_reject_t {
    /* Mandatory fields */
    nas_5gsm_cause_t 5gsm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_modification_command_reject_t;


/*******************************************************
 * PDU SESSION RELEASE REQUEST
 ******************************************************/
#define NAS_PDU_SESSION_RELEASE_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<0)
#define NAS_PDU_SESSION_RELEASE_REQUEST_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_release_request_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_release_request_t;


/*******************************************************
 * PDU SESSION RELEASE REJECT
 ******************************************************/
#define NAS_PDU_SESSION_RELEASE_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<0)
#define NAS_PDU_SESSION_RELEASE_REJECT_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_release_reject_t {
    /* Mandatory fields */
    nas_5gsm_cause_t 5gsm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_release_reject_t;


/*******************************************************
 * PDU SESSION RELEASE COMMAND
 ******************************************************/
#define NAS_PDU_SESSION_RELEASE_COMMAND_BACK_OFF_TIMER_VALUE_PRESENT (1<<0)
#define NAS_PDU_SESSION_RELEASE_COMMAND_EAP_MESSAGE_PRESENT (1<<1)
#define NAS_PDU_SESSION_RELEASE_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<2)
#define NAS_PDU_SESSION_RELEASE_COMMAND_BACK_OFF_TIMER_VALUE_TYPE 0x37
#define NAS_PDU_SESSION_RELEASE_COMMAND_EAP_MESSAGE_TYPE 0x78
#define NAS_PDU_SESSION_RELEASE_COMMAND_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_release_command_t {
    /* Mandatory fields */
    nas_5gsm_cause_t 5gsm_cause;

    /* Optional fields */
    c_uint32_t presencemask;
    nas_gprs_timer_3_t back_off_timer_value;
    nas_eap_message_t eap_message;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_release_command_t;


/*******************************************************
 * PDU SESSION RELEASE COMPLETE
 ******************************************************/
#define NAS_PDU_SESSION_RELEASE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_PRESENT (1<<0)
#define NAS_PDU_SESSION_RELEASE_COMPLETE_EXTENDED_PROTOCOL_CONFIGURATION_OPTIONS_TYPE 0x7B

typedef struct _nas_pdu_session_release_complete_t {

    /* Optional fields */
    c_uint32_t presencemask;
    nas_extended_protocol_configuration_options_t extended_protocol_configuration_options;
} nas_pdu_session_release_complete_t;


/*******************************************************
 * GSM STATUS
 ******************************************************/

typedef struct _nas_gsm_status_t {
    /* Mandatory fields */
    nas_5gsm_cause_t 5gsm_cause;
} nas_gsm_status_t;


typedef struct _nas_gmm_message_t {
    nas_gmm_header_t h;
    union {
        nas_registration_request_t registration_request;
        nas_registration_accept_t registration_accept;
        nas_registration_complete_t registration_complete;
        nas_registration_reject_t registration_reject;
        nas_deregistration_request_from_ue_t deregistration_request_from_ue;
        nas_deregistration_request_to_ue_t deregistration_request_to_ue;
        nas_service_request_t service_request;
        nas_service_reject_t service_reject;
        nas_service_accept_t service_accept;
        nas_configuration_update_command_t configuration_update_command;
        nas_configuration_update_complete_t configuration_update_complete;
        nas_authentication_request_t authentication_request;
        nas_authentication_response_t authentication_response;
        nas_authentication_failure_t authentication_failure;
        nas_authentication_result_t authentication_result;
        nas_identity_request_t identity_request;
        nas_identity_response_t identity_response;
        nas_security_mode_command_t security_mode_command;
        nas_security_mode_complete_t security_mode_complete;
        nas_security_mode_reject_t security_mode_reject;
        nas_gmm_status_t gmm_status;
        nas_notification_t notification;
        nas_notification_response_t notification_response;
        nas_ul_nas_transport_t ul_nas_transport;
        nas_dl_nas_transport_t dl_nas_transport;
    };
} nas_gmm_message_t;

typedef struct _nas_gsm_message_t {
    nas_gsm_header_t h;
    union {
        nas_pdu_session_establishment_request_t pdu_session_establishment_request;
        nas_pdu_session_establishment_accept_t pdu_session_establishment_accept;
        nas_pdu_session_establishment_reject_t pdu_session_establishment_reject;
        nas_pdu_session_authentication_command_t pdu_session_authentication_command;
        nas_pdu_session_authentication_complete_t pdu_session_authentication_complete;
        nas_pdu_session_authentication_result_t pdu_session_authentication_result;
        nas_pdu_session_modification_request_t pdu_session_modification_request;
        nas_pdu_session_modification_reject_t pdu_session_modification_reject;
        nas_pdu_session_modification_command_t pdu_session_modification_command;
        nas_pdu_session_modification_complete_t pdu_session_modification_complete;
        nas_pdu_session_modification_command_reject_t pdu_session_modification_command_reject;
        nas_pdu_session_release_request_t pdu_session_release_request;
        nas_pdu_session_release_reject_t pdu_session_release_reject;
        nas_pdu_session_release_command_t pdu_session_release_command;
        nas_pdu_session_release_complete_t pdu_session_release_complete;
        nas_gsm_status_t gsm_status;
    };
} nas_gsm_message_t;

typedef struct _nas_message_t {
    nas_security_header_t h;
    union {
        nas_gmm_message_t gmm;
        nas_gsm_message_t gsm;
    };
} nas_message_t;

CORE_DECLARE(status_t) nas_gmm_decode(nas_message_t *message, pkbuf_t *pkbuf);
CORE_DECLARE(status_t) nas_gsm_decode(nas_message_t *message, pkbuf_t *pkbuf);
CORE_DECLARE(status_t) nas_plain_encode(
        pkbuf_t **pkbuf, nas_message_t *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NAS_MESSAGE_H__ */
