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
 * This file had been created by gtp_tlv.py script v0.1.0
 * Please do not modify this file but regenerate it via script.
 * Created on: 2019-01-02 17:06:04.967693 by wirelab
 * from 29244-f40.docx
 ******************************************************************************/

#ifndef __PFCP_TLV_H__
#define __PFCP_TLV_H__

#include "core_tlv_msg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 5.1 General format */
#define PFCP_HEADER_LEN     16
#define PFCP_SEID_LEN        8
typedef struct _pfcp_header_t {
    union {
        struct {
        ED4(c_uint8_t version:3;,
            c_uint8_t spare1:3;,
            c_uint8_t mp:1;,
            c_uint8_t seid_p:1;)
        };
        c_uint8_t flags;
    };
    c_uint8_t type;
    c_uint16_t length;
    union {
        struct {
            c_uint64_t seid;
            /* sqn : 31bit ~ 8bit, spare : 7bit ~ 0bit */
#define PFCP_XID_TO_SQN(__xid) htonl(((__xid) << 8))
#define PFCP_SQN_TO_XID(__sqn) (ntohl(__sqn) >> 8)
            c_uint32_t sqn;
        };
        /* sqn : 31bit ~ 8bit, spare : 7bit ~ 0bit */
        c_uint32_t sqn_only;
    };
} __attribute__ ((packed)) pfcp_header_t;



/* PFCP message type */
#define PFCP_HEARTBEAT_REQUEST_TYPE 1
#define PFCP_HEARTBEAT_RESPONSE_TYPE 2
#define PFCP_PFD_MANAGEMENT_REQUEST_TYPE 3
#define PFCP_PFD_MANAGEMENT_RESPONSE_TYPE 4
#define PFCP_ASSOCIATION_SETUP_REQUEST_TYPE 5
#define PFCP_ASSOCIATION_SETUP_RESPONSE_TYPE 6
#define PFCP_ASSOCIATION_UPDATE_REQUEST_TYPE 7
#define PFCP_ASSOCIATION_UPDATE_RESPONSE_TYPE 8
#define PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE 9
#define PFCP_ASSOCIATION_RELEASE_RESPONSE_TYPE 10
#define PFCP_VERSION_NOT_SUPPORTED_RESPONSE_TYPE 11
#define PFCP_NODE_REPORT_REQUEST_TYPE 12
#define PFCP_NODE_REPORT_RESPONSE_TYPE 13
#define PFCP_SESSION_SET_DELETION_REQUEST_TYPE 14
#define PFCP_SESSION_SET_DELETION_RESPONSE_TYPE 15
#define PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE 50
#define PFCP_SESSION_ESTABLISHMENT_RESPONSE_TYPE 51
#define PFCP_SESSION_MODIFICATION_REQUEST_TYPE 52
#define PFCP_SESSION_MODIFICATION_RESPONSE_TYPE 53
#define PFCP_SESSION_DELETION_REQUEST_TYPE 54
#define PFCP_SESSION_DELETION_RESPONSE_TYPE 55
#define PFCP_SESSION_REPORT_REQUEST_TYPE 56
#define PFCP_SESSION_REPORT_RESPONSE_TYPE 57

#define TLV_PFCP_CREATE_PDR_TYPE 1
#define TLV_PFCP_PDI_TYPE 2
#define TLV_PFCP_CREATE_FAR_TYPE 3
#define TLV_PFCP_FORWARDING_PARAMETERS_TYPE 4
#define TLV_PFCP_DUPLICATING_PARAMETERS_TYPE 5
#define TLV_PFCP_CREATE_URR_TYPE 6
#define TLV_PFCP_CREATE_QER_TYPE 7
#define TLV_PFCP_CREATED_PDR_TYPE 8
#define TLV_PFCP_UPDATE_PDR_TYPE 9
#define TLV_PFCP_UPDATE_FAR_TYPE 10
#define TLV_PFCP_UPDATE_FORWARDING_PARAMETERS_TYPE 11
#define TLV_PFCP_UPDATE_BAR_PFCP_SESSION_REPORT_RESPONSE_TYPE 12
#define TLV_PFCP_UPDATE_URR_TYPE 13
#define TLV_PFCP_UPDATE_QER_TYPE 14
#define TLV_PFCP_REMOVE_PDR_TYPE 15
#define TLV_PFCP_REMOVE_FAR_TYPE 16
#define TLV_PFCP_REMOVE_URR_TYPE 17
#define TLV_PFCP_REMOVE_QER_TYPE 18
#define TLV_PFCP_CAUSE_TYPE 19
#define TLV_PFCP_SOURCE_INTERFACE_TYPE 20
#define TLV_PFCP_F_TEID_TYPE 21
#define TLV_PFCP_NETWORK_INSTANCE_TYPE 22
#define TLV_PFCP_SDF_FILTER_TYPE 23
#define TLV_PFCP_APPLICATION_ID_TYPE 24
#define TLV_PFCP_GATE_STATUS_TYPE 25
#define TLV_PFCP_MBR_TYPE 26
#define TLV_PFCP_GBR_TYPE 27
#define TLV_PFCP_QER_CORRELATION_ID_TYPE 28
#define TLV_PFCP_PRECEDENCE_TYPE 29
#define TLV_PFCP_TRANSPORT_LEVEL_MARKING_TYPE 30
#define TLV_PFCP_VOLUME_THRESHOLD_TYPE 31
#define TLV_PFCP_TIME_THRESHOLD_TYPE 32
#define TLV_PFCP_MONITORING_TIME_TYPE 33
#define TLV_PFCP_SUBSEQUENT_VOLUME_THRESHOLD_TYPE 34
#define TLV_PFCP_SUBSEQUENT_TIME_THRESHOLD_TYPE 35
#define TLV_PFCP_INACTIVITY_DETECTION_TIME_TYPE 36
#define TLV_PFCP_REPORTING_TRIGGERS_TYPE 37
#define TLV_PFCP_REDIRECT_INFORMATION_TYPE 38
#define TLV_PFCP_REPORT_TYPE_TYPE 39
#define TLV_PFCP_OFFENDING_IE_TYPE 40
#define TLV_PFCP_FORWARDING_POLICY_TYPE 41
#define TLV_PFCP_DESTINATION_INTERFACE_TYPE 42
#define TLV_PFCP_UP_FUNCTION_FEATURES_TYPE 43
#define TLV_PFCP_APPLY_ACTION_TYPE 44
#define TLV_PFCP_DOWNLINK_DATA_SERVICE_INFORMATION_TYPE 45
#define TLV_PFCP_DOWNLINK_DATA_NOTIFICATION_DELAY_TYPE 46
#define TLV_PFCP_DL_BUFFERING_DURATION_TYPE 47
#define TLV_PFCP_DL_BUFFERING_SUGGESTED_PACKET_COUNT_TYPE 48
#define TLV_PFCP_PFCPSMREQ_FLAGS_TYPE 49
#define TLV_PFCP_PFCPSRRSP_FLAGS_TYPE 50
#define TLV_PFCP_LOAD_CONTROL_INFORMATIONP_TYPE 51
#define TLV_PFCP_SEQUENCE_NUMBER_TYPE 52
#define TLV_PFCP_METRICP_TYPE 53
#define TLV_PFCP_OVERLOAD_CONTROL_INFORMATIONP_TYPE 54
#define TLV_PFCP_TIMER_TYPE 55
#define TLV_PFCP_PACKET_DETECTION_RULE_ID_TYPE 56
#define TLV_PFCP_F_SEID_TYPE 57
#define TLV_PFCP_APPLICATION_ID_S_PFDS_TYPE 58
#define TLV_PFCP_PFD_CONTEXT_TYPE 59
#define TLV_PFCP_NODE_ID_TYPE 60
#define TLV_PFCP_PFD_CONTENTS_TYPE 61
#define TLV_PFCP_MEASUREMENT_METHOD_TYPE 62
#define TLV_PFCP_USAGE_REPORT_TRIGGER_TYPE 63
#define TLV_PFCP_MEASUREMENT_PERIOD_TYPE 64
#define TLV_PFCP_FQ_CSIDP_TYPE 65
#define TLV_PFCP_VOLUME_MEASUREMENT_TYPE 66
#define TLV_PFCP_DURATION_MEASUREMENT_TYPE 67
#define TLV_PFCP_APPLICATION_DETECTION_INFORMATION_TYPE 68
#define TLV_PFCP_TIME_OF_FIRST_PACKET_TYPE 69
#define TLV_PFCP_TIME_OF_LAST_PACKET_TYPE 70
#define TLV_PFCP_QUOTA_HOLDING_TIME_TYPE 71
#define TLV_PFCP_DROPPED_DL_TRAFFIC_THRESHOLD_TYPE 72
#define TLV_PFCP_VOLUME_QUOTA_TYPE 73
#define TLV_PFCP_TIME_QUOTA_TYPE 74
#define TLV_PFCP_START_TIME_TYPE 75
#define TLV_PFCP_END_TIME_TYPE 76
#define TLV_PFCP_QUERY_URR_TYPE 77
#define TLV_PFCP_USAGE_REPORT_IN_SESSION_MODIFICATION_RESPONSE_TYPE 78
#define TLV_PFCP_USAGE_REPORT_SESSION_DELETION_RESPONSE_TYPE 79
#define TLV_PFCP_USAGE_REPORT_SESSION_REPORT_REQUEST_TYPE 80
#define TLV_PFCP_URR_ID_TYPE 81
#define TLV_PFCP_LINKED_URR_ID_TYPE 82
#define TLV_PFCP_DOWNLINK_DATA_REPORT_TYPE 83
#define TLV_PFCP_OUTER_HEADER_CREATION_TYPE 84
#define TLV_PFCP_CREATE_BAR_TYPE 85
#define TLV_PFCP_UPDATE_BAR_SESSION_MODIFICATION_REQUEST_TYPE 86
#define TLV_PFCP_REMOVE_BAR_TYPE 87
#define TLV_PFCP_BAR_ID_TYPE 88
#define TLV_PFCP_CP_FUNCTION_FEATURES_TYPE 89
#define TLV_PFCP_USAGE_INFORMATION_TYPE 90
#define TLV_PFCP_APPLICATION_INSTANCE_ID_TYPE 91
#define TLV_PFCP_FLOW_INFORMATION_TYPE 92
#define TLV_PFCP_UE_IP_ADDRESS_TYPE 93
#define TLV_PFCP_PACKET_RATE_TYPE 94
#define TLV_PFCP_OUTER_HEADER_REMOVAL_TYPE 95
#define TLV_PFCP_RECOVERY_TIME_STAMP_TYPE 96
#define TLV_PFCP_DL_FLOW_LEVEL_MARKING_TYPE 97
#define TLV_PFCP_HEADER_ENRICHMENT_TYPE 98
#define TLV_PFCP_ERROR_INDICATION_REPORT_TYPE 99
#define TLV_PFCP_MEASUREMENT_INFORMATION_TYPE 100
#define TLV_PFCP_NODE_REPORT_TYPE_TYPE 101
#define TLV_PFCP_USER_PLANE_PATH_FAILURE_REPORT_TYPE 102
#define TLV_PFCP_REMOTE_GTP_U_PEER_TYPE 103
#define TLV_PFCP_UR_SEQN_TYPE 104
#define TLV_PFCP_UPDATE_DUPLICATING_PARAMETERS_TYPE 105
#define TLV_PFCP_ACTIVATE_PREDEFINED_RULES_TYPE 106
#define TLV_PFCP_DEACTIVATE_PREDEFINED_RULES_TYPE 107
#define TLV_PFCP_FAR_ID_TYPE 108
#define TLV_PFCP_QER_ID_TYPE 109
#define TLV_PFCP_OCI_FLAGS_TYPE 110
#define TLV_PFCP_PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE 111
#define TLV_PFCP_GRACEFUL_RELEASE_PERIOD_TYPE 112
#define TLV_PFCP_PDN_TYPEP_TYPE 113
#define TLV_PFCP_FAILED_RULE_ID_TYPE 114
#define TLV_PFCP_TIME_QUOTA_MECHANISM_TYPE 115
#define TLV_PFCP_USER_PLANE_IP_RESOURCE_INFORMATION_TYPE 116
#define TLV_PFCP_USER_PLANE_INACTIVITY_TIMER_TYPE 117
#define TLV_PFCP_AGGREGATED_URRS_TYPE 118
#define TLV_PFCP_MULTIPLIER_TYPE 119
#define TLV_PFCP_AGGREGATED_URR_ID_TYPE 120
#define TLV_PFCP_SUBSEQUENT_VOLUME_QUOTA_TYPE 121
#define TLV_PFCP_SUBSEQUENT_TIME_QUOTA_TYPE 122
#define TLV_PFCP_RQI_TYPE 123
#define TLV_PFCP_QFI_TYPE 124

/* Infomration Element TLV Descriptor */
extern tlv_desc_t tlv_desc_pfcpcause_0;
extern tlv_desc_t tlv_desc_source_interface_0;
extern tlv_desc_t tlv_desc_pfcpf_teid_0;
extern tlv_desc_t tlv_desc_network_instance_0;
extern tlv_desc_t tlv_desc_sdf_filter_0;
extern tlv_desc_t tlv_desc_application_id_0;
extern tlv_desc_t tlv_desc_gate_status_0;
extern tlv_desc_t tlv_desc_mbr_0;
extern tlv_desc_t tlv_desc_gbr_0;
extern tlv_desc_t tlv_desc_qer_correlation_id_0;
extern tlv_desc_t tlv_desc_precedence_0;
extern tlv_desc_t tlv_desc_transport_level_marking_0;
extern tlv_desc_t tlv_desc_volume_threshold_0;
extern tlv_desc_t tlv_desc_time_threshold_0;
extern tlv_desc_t tlv_desc_monitoring_time_0;
extern tlv_desc_t tlv_desc_subsequent_volume_threshold_0;
extern tlv_desc_t tlv_desc_subsequent_time_threshold_0;
extern tlv_desc_t tlv_desc_inactivity_detection_time_0;
extern tlv_desc_t tlv_desc_reporting_triggers_0;
extern tlv_desc_t tlv_desc_redirect_information_0;
extern tlv_desc_t tlv_desc_report_type_0;
extern tlv_desc_t tlv_desc_offending_ie_0;
extern tlv_desc_t tlv_desc_forwarding_policy_0;
extern tlv_desc_t tlv_desc_destination_interface_0;
extern tlv_desc_t tlv_desc_up_function_features_0;
extern tlv_desc_t tlv_desc_apply_action_0;
extern tlv_desc_t tlv_desc_downlink_data_service_information_0;
extern tlv_desc_t tlv_desc_downlink_data_notification_delay_0;
extern tlv_desc_t tlv_desc_dl_buffering_duration_0;
extern tlv_desc_t tlv_desc_dl_buffering_suggested_packet_count_0;
extern tlv_desc_t tlv_desc_pfcpsmreq_flags_0;
extern tlv_desc_t tlv_desc_pfcpsrrsp_flags_0;
extern tlv_desc_t tlv_desc_pfcpsequence_number_0;
extern tlv_desc_t tlv_desc_metricp_0;
extern tlv_desc_t tlv_desc_timer_0;
extern tlv_desc_t tlv_desc_packet_detection_rule_id_0;
extern tlv_desc_t tlv_desc_f_seid_0;
extern tlv_desc_t tlv_desc_node_id_0;
extern tlv_desc_t tlv_desc_pfd_contents_0;
extern tlv_desc_t tlv_desc_measurement_method_0;
extern tlv_desc_t tlv_desc_usage_report_trigger_0;
extern tlv_desc_t tlv_desc_measurement_period_0;
extern tlv_desc_t tlv_desc_fq_csidp_0;
extern tlv_desc_t tlv_desc_volume_measurement_0;
extern tlv_desc_t tlv_desc_duration_measurement_0;
extern tlv_desc_t tlv_desc_time_of_first_packet_0;
extern tlv_desc_t tlv_desc_time_of_last_packet_0;
extern tlv_desc_t tlv_desc_quota_holding_time_0;
extern tlv_desc_t tlv_desc_dropped_dl_traffic_threshold_0;
extern tlv_desc_t tlv_desc_volume_quota_0;
extern tlv_desc_t tlv_desc_time_quota_0;
extern tlv_desc_t tlv_desc_start_time_0;
extern tlv_desc_t tlv_desc_end_time_0;
extern tlv_desc_t tlv_desc_urr_id_0;
extern tlv_desc_t tlv_desc_linked_urr_id_0;
extern tlv_desc_t tlv_desc_outer_header_creation_0;
extern tlv_desc_t tlv_desc_bar_id_0;
extern tlv_desc_t tlv_desc_cp_function_features_0;
extern tlv_desc_t tlv_desc_usage_information_0;
extern tlv_desc_t tlv_desc_application_instance_id_0;
extern tlv_desc_t tlv_desc_flow_information_0;
extern tlv_desc_t tlv_desc_ue_ip_address_0;
extern tlv_desc_t tlv_desc_packet_rate_0;
extern tlv_desc_t tlv_desc_outer_header_removal_0;
extern tlv_desc_t tlv_desc_recovery_time_stamp_0;
extern tlv_desc_t tlv_desc_dl_flow_level_marking_0;
extern tlv_desc_t tlv_desc_header_enrichment_0;
extern tlv_desc_t tlv_desc_measurement_information_0;
extern tlv_desc_t tlv_desc_node_report_type_0;
extern tlv_desc_t tlv_desc_user_plane_path_failure_report_0;
extern tlv_desc_t tlv_desc_remote_gtp_u_peer_0;
extern tlv_desc_t tlv_desc_ur_seqn_0;
extern tlv_desc_t tlv_desc_activate_predefined_rules_0;
extern tlv_desc_t tlv_desc_deactivate_predefined_rules_0;
extern tlv_desc_t tlv_desc_far_id_0;
extern tlv_desc_t tlv_desc_qer_id_0;
extern tlv_desc_t tlv_desc_oci_flags_0;
extern tlv_desc_t tlv_desc_pfcp_association_release_request_0;
extern tlv_desc_t tlv_desc_graceful_release_period_0;
extern tlv_desc_t tlv_desc_pdn_typep_0;
extern tlv_desc_t tlv_desc_failed_rule_id_0;
extern tlv_desc_t tlv_desc_time_quota_mechanism_0;
extern tlv_desc_t tlv_desc_user_plane_ip_resource_information_0;
extern tlv_desc_t tlv_desc_user_plane_inactivity_timer_0;
extern tlv_desc_t tlv_desc_aggregated_urrs_0;
extern tlv_desc_t tlv_desc_multiplier_0;
extern tlv_desc_t tlv_desc_aggregated_urr_id_0;
extern tlv_desc_t tlv_desc_subsequent_volume_quota_0;
extern tlv_desc_t tlv_desc_subsequent_time_quota_0;
extern tlv_desc_t tlv_desc_rqi_0;
extern tlv_desc_t tlv_desc_qfi_0;

/* Group Infomration Element TLV Descriptor */
extern tlv_desc_t tlv_desc_create_pdr_0;
extern tlv_desc_t tlv_desc_create_pdr_1;
extern tlv_desc_t tlv_desc_pdi_0;
extern tlv_desc_t tlv_desc_create_far_0;
extern tlv_desc_t tlv_desc_create_far_1;
extern tlv_desc_t tlv_desc_forwarding_parameters_0;
extern tlv_desc_t tlv_desc_duplicating_parameters_0;
extern tlv_desc_t tlv_desc_create_urr_0;
extern tlv_desc_t tlv_desc_create_qer_0;
extern tlv_desc_t tlv_desc_created_pdr_0;
extern tlv_desc_t tlv_desc_update_pdr_0;
extern tlv_desc_t tlv_desc_update_pdr_1;
extern tlv_desc_t tlv_desc_update_far_0;
extern tlv_desc_t tlv_desc_update_forwarding_parameters_0;
extern tlv_desc_t tlv_desc_update_bar_pfcp_session_report_response_0;
extern tlv_desc_t tlv_desc_update_urr_0;
extern tlv_desc_t tlv_desc_update_qer_0;
extern tlv_desc_t tlv_desc_remove_pdr_0;
extern tlv_desc_t tlv_desc_remove_far_0;
extern tlv_desc_t tlv_desc_remove_urr_0;
extern tlv_desc_t tlv_desc_remove_qer_0;
extern tlv_desc_t tlv_desc_load_control_informationp_0;
extern tlv_desc_t tlv_desc_overload_control_informationp_0;
extern tlv_desc_t tlv_desc_application_id_s_pfds_0;
extern tlv_desc_t tlv_desc_pfd_context_0;
extern tlv_desc_t tlv_desc_application_detection_information_0;
extern tlv_desc_t tlv_desc_query_urr_0;
extern tlv_desc_t tlv_desc_usage_report_in_session_modification_response_0;
extern tlv_desc_t tlv_desc_usage_report_session_deletion_response_0;
extern tlv_desc_t tlv_desc_usage_report_session_report_request_0;
extern tlv_desc_t tlv_desc_downlink_data_report_0;
extern tlv_desc_t tlv_desc_create_bar_0;
extern tlv_desc_t tlv_desc_update_bar_session_modification_request_0;
extern tlv_desc_t tlv_desc_remove_bar_0;
extern tlv_desc_t tlv_desc_error_indication_report_0;
extern tlv_desc_t tlv_desc_update_duplicating_parameters_0;

/* Message Descriptor */
extern tlv_desc_t tlv_desc_pfcp_heartbeat_request;
extern tlv_desc_t tlv_desc_pfcp_heartbeat_response;
extern tlv_desc_t tlv_desc_pfcp_pfd_management_request;
extern tlv_desc_t tlv_desc_pfcp_pfd_management_response;
extern tlv_desc_t tlv_desc_pfcp_association_setup_request;
extern tlv_desc_t tlv_desc_pfcp_association_setup_response;
extern tlv_desc_t tlv_desc_pfcp_association_update_request;
extern tlv_desc_t tlv_desc_pfcp_association_update_response;
extern tlv_desc_t tlv_desc_pfcp_association_release_request;
extern tlv_desc_t tlv_desc_pfcp_association_release_response;
extern tlv_desc_t tlv_desc_pfcp_version_not_supported_response;
extern tlv_desc_t tlv_desc_pfcp_node_report_request;
extern tlv_desc_t tlv_desc_pfcp_node_report_response;
extern tlv_desc_t tlv_desc_pfcp_session_set_deletion_request;
extern tlv_desc_t tlv_desc_pfcp_session_set_deletion_response;
extern tlv_desc_t tlv_desc_pfcp_session_establishment_request;
extern tlv_desc_t tlv_desc_pfcp_session_establishment_response;
extern tlv_desc_t tlv_desc_pfcp_session_modification_request;
extern tlv_desc_t tlv_desc_pfcp_session_modification_response;
extern tlv_desc_t tlv_desc_pfcp_session_deletion_request;
extern tlv_desc_t tlv_desc_pfcp_session_deletion_response;
extern tlv_desc_t tlv_desc_pfcp_session_report_request;
extern tlv_desc_t tlv_desc_pfcp_session_report_response;

/* Structure for Infomration Element */
typedef tlv_octet_t tlv_cause_t;
typedef tlv_octet_t tlv_source_interface_t;
typedef tlv_octet_t tlv_f_teid_t;
typedef tlv_octet_t tlv_network_instance_t;
typedef tlv_octet_t tlv_sdf_filter_t;
typedef tlv_octet_t tlv_application_id_t;
typedef tlv_octet_t tlv_gate_status_t;
typedef tlv_octet_t tlv_mbr_t;
typedef tlv_octet_t tlv_gbr_t;
typedef tlv_octet_t tlv_qer_correlation_id_t;
typedef tlv_octet_t tlv_precedence_t;
typedef tlv_octet_t tlv_transport_level_marking_t;
typedef tlv_octet_t tlv_volume_threshold_t;
typedef tlv_octet_t tlv_time_threshold_t;
typedef tlv_octet_t tlv_monitoring_time_t;
typedef tlv_octet_t tlv_subsequent_volume_threshold_t;
typedef tlv_octet_t tlv_subsequent_time_threshold_t;
typedef tlv_octet_t tlv_inactivity_detection_time_t;
typedef tlv_octet_t tlv_reporting_triggers_t;
typedef tlv_octet_t tlv_redirect_information_t;
typedef tlv_octet_t tlv_report_type_t;
typedef tlv_octet_t tlv_offending_ie_t;
typedef tlv_octet_t tlv_forwarding_policy_t;
typedef tlv_octet_t tlv_destination_interface_t;
typedef tlv_octet_t tlv_up_function_features_t;
typedef tlv_octet_t tlv_apply_action_t;
typedef tlv_octet_t tlv_downlink_data_service_information_t;
typedef tlv_octet_t tlv_downlink_data_notification_delay_t;
typedef tlv_octet_t tlv_dl_buffering_duration_t;
typedef tlv_octet_t tlv_dl_buffering_suggested_packet_count_t;
typedef tlv_octet_t tlv_pfcpsmreq_flags_t;
typedef tlv_octet_t tlv_pfcpsrrsp_flags_t;
typedef tlv_octet_t tlv_sequence_number_t;
typedef tlv_octet_t tlv_metricp_t;
typedef tlv_octet_t tlv_timer_t;
typedef tlv_octet_t tlv_packet_detection_rule_id_t;
typedef tlv_octet_t tlv_f_seid_t;
typedef tlv_octet_t tlv_node_id_t;
typedef tlv_octet_t tlv_pfd_contents_t;
typedef tlv_octet_t tlv_measurement_method_t;
typedef tlv_octet_t tlv_usage_report_trigger_t;
typedef tlv_octet_t tlv_measurement_period_t;
typedef tlv_octet_t tlv_fq_csidp_t;
typedef tlv_octet_t tlv_volume_measurement_t;
typedef tlv_octet_t tlv_duration_measurement_t;
typedef tlv_octet_t tlv_time_of_first_packet_t;
typedef tlv_octet_t tlv_time_of_last_packet_t;
typedef tlv_octet_t tlv_quota_holding_time_t;
typedef tlv_octet_t tlv_dropped_dl_traffic_threshold_t;
typedef tlv_octet_t tlv_volume_quota_t;
typedef tlv_octet_t tlv_time_quota_t;
typedef tlv_octet_t tlv_start_time_t;
typedef tlv_octet_t tlv_end_time_t;
typedef tlv_octet_t tlv_urr_id_t;
typedef tlv_octet_t tlv_linked_urr_id_t;
typedef tlv_octet_t tlv_outer_header_creation_t;
typedef tlv_octet_t tlv_bar_id_t;
typedef tlv_octet_t tlv_cp_function_features_t;
typedef tlv_octet_t tlv_usage_information_t;
typedef tlv_octet_t tlv_application_instance_id_t;
typedef tlv_octet_t tlv_flow_information_t;
typedef tlv_octet_t tlv_ue_ip_address_t;
typedef tlv_octet_t tlv_packet_rate_t;
typedef tlv_octet_t tlv_outer_header_removal_t;
typedef tlv_octet_t tlv_recovery_time_stamp_t;
typedef tlv_octet_t tlv_dl_flow_level_marking_t;
typedef tlv_octet_t tlv_header_enrichment_t;
typedef tlv_octet_t tlv_measurement_information_t;
typedef tlv_octet_t tlv_node_report_type_t;
typedef tlv_octet_t tlv_user_plane_path_failure_report_t;
typedef tlv_octet_t tlv_remote_gtp_u_peer_t;
typedef tlv_octet_t tlv_ur_seqn_t;
typedef tlv_octet_t tlv_activate_predefined_rules_t;
typedef tlv_octet_t tlv_deactivate_predefined_rules_t;
typedef tlv_octet_t tlv_far_id_t;
typedef tlv_octet_t tlv_qer_id_t;
typedef tlv_octet_t tlv_oci_flags_t;
typedef tlv_octet_t tlv_pfcp_association_release_request_t;
typedef tlv_octet_t tlv_graceful_release_period_t;
typedef tlv_octet_t tlv_pdn_typep_t;
typedef tlv_octet_t tlv_failed_rule_id_t;
typedef tlv_octet_t tlv_time_quota_mechanism_t;
typedef tlv_octet_t tlv_user_plane_ip_resource_information_t;
typedef tlv_octet_t tlv_user_plane_inactivity_timer_t;
typedef tlv_octet_t tlv_aggregated_urrs_t;
typedef tlv_octet_t tlv_multiplier_t;
typedef tlv_octet_t tlv_aggregated_urr_id_t;
typedef tlv_octet_t tlv_subsequent_volume_quota_t;
typedef tlv_octet_t tlv_subsequent_time_quota_t;
typedef tlv_octet_t tlv_rqi_t;
typedef tlv_octet_t tlv_qfi_t;

/* Structure for Group Infomration Element */
typedef struct _tlv_pdi_t {
    tlv_presence_t presence;
    tlv_source_interface_t source_interface;
    tlv_f_teid_t local_f_teid; /* Instance : 0 */
    tlv_network_instance_t network_instance;
    tlv_ue_ip_address_t ue_ip_address;
    tlv_sdf_filter_t sdf_filter;
    tlv_application_id_t application_id;
} tlv_pdi_t; ;

typedef struct _tlv_forwarding_parameters_t {
    tlv_presence_t presence;
    tlv_destination_interface_t destination_interface;
    tlv_network_instance_t network_instance;
    tlv_redirect_information_t redirect_information;
    tlv_outer_header_creation_t outer_header_creation;
    tlv_transport_level_marking_t transport_level_marking;
    tlv_forwarding_policy_t forwarding_policy;
    tlv_header_enrichment_t header_enrichment;
} tlv_forwarding_parameters_t; ;

typedef struct _tlv_duplicating_parameters_t {
    tlv_presence_t presence;
    tlv_destination_interface_t destination_interface;
    tlv_outer_header_creation_t outer_header_creation;
    tlv_transport_level_marking_t transport_level_marking;
    tlv_forwarding_policy_t forwarding_policy;
} tlv_duplicating_parameters_t; ;

typedef struct _tlv_create_urr_t {
    tlv_presence_t presence;
    tlv_urr_id_t urr_id;
    tlv_measurement_method_t measurement_method;
    tlv_reporting_triggers_t reporting_triggers;
    tlv_measurement_period_t measurement_period;
    tlv_volume_threshold_t volume_threshold;
    tlv_volume_quota_t volume_quota;
    tlv_time_threshold_t time_threshold;
    tlv_time_quota_t time_quota;
    tlv_quota_holding_time_t quota_holding_time;
    tlv_dropped_dl_traffic_threshold_t dropped_dl_traffic_threshold;
    tlv_monitoring_time_t monitoring_time;
    tlv_subsequent_volume_threshold_t subsequent_volume_threshold;
    tlv_subsequent_time_threshold_t subsequent_time_threshold;
    tlv_subsequent_volume_quota_t subsequent_volume_quota;
    tlv_subsequent_time_quota_t subsequent_time_quota;
    tlv_inactivity_detection_time_t inactivity_detection_time;
    tlv_linked_urr_id_t linked_urr_id;
    tlv_measurement_information_t measurement_information;
    tlv_time_quota_mechanism_t time_quota_mechanism;
    tlv_aggregated_urrs_t aggregated_urrs;
} tlv_create_urr_t; ;

typedef struct _tlv_create_qer_t {
    tlv_presence_t presence;
    tlv_qer_id_t qer_id;
    tlv_qer_correlation_id_t qer_correlation_id;
    tlv_gate_status_t gate_status;
    tlv_mbr_t maximum_bitrate;
    tlv_gbr_t guaranteed_bitrate;
    tlv_packet_rate_t packet_rate;
    tlv_dl_flow_level_marking_t dl_flow_level_marking;
    tlv_qfi_t qos_flow_identifier;
    tlv_rqi_t reflective_qos;
} tlv_create_qer_t; ;

typedef struct _tlv_created_pdr_t {
    tlv_presence_t presence;
    tlv_packet_detection_rule_id_t pdr_id;
    tlv_f_teid_t local_f_teid; /* Instance : 0 */
} tlv_created_pdr_t; ;

typedef struct _tlv_update_pdr_t {
    tlv_presence_t presence;
    tlv_packet_detection_rule_id_t pdr_id;
    tlv_outer_header_removal_t outer_header_removal;
    tlv_precedence_t precedence;
    tlv_pdi_t pdi;
    tlv_far_id_t far_id;
    tlv_urr_id_t urr_id;
    tlv_qer_id_t qer_id;
    tlv_activate_predefined_rules_t activate_predefined_rules;
    tlv_deactivate_predefined_rules_t deactivate_predefined_rules;
} tlv_update_pdr_t; ;

typedef struct _tlv_update_forwarding_parameters_t {
    tlv_presence_t presence;
    tlv_destination_interface_t destination_interface;
    tlv_network_instance_t network_instance;
    tlv_redirect_information_t redirect_information;
    tlv_outer_header_creation_t outer_header_creation;
    tlv_transport_level_marking_t transport_level_marking;
    tlv_forwarding_policy_t forwarding_policy;
    tlv_header_enrichment_t header_enrichment;
    tlv_pfcpsmreq_flags_t sxsmreq_flags;
} tlv_update_forwarding_parameters_t; ;

typedef struct _tlv_update_bar_pfcp_session_report_response_t {
    tlv_presence_t presence;
    tlv_bar_id_t bar_id;
    tlv_downlink_data_notification_delay_t downlink_data_notification_delay;
    tlv_dl_buffering_duration_t dl_buffering_duration;
    tlv_dl_buffering_suggested_packet_count_t dl_buffering_suggested_packet_count;
} tlv_update_bar_pfcp_session_report_response_t; ;

typedef struct _tlv_update_urr_t {
    tlv_presence_t presence;
    tlv_urr_id_t urr_id;
    tlv_measurement_method_t measurement_method;
    tlv_reporting_triggers_t reporting_triggers;
    tlv_measurement_period_t measurement_period;
    tlv_volume_threshold_t volume_threshold;
    tlv_volume_quota_t volume_quota;
    tlv_time_threshold_t time_threshold;
    tlv_time_quota_t time_quota;
    tlv_quota_holding_time_t quota_holding_time;
    tlv_dropped_dl_traffic_threshold_t dropped_dl_traffic_threshold;
    tlv_monitoring_time_t monitoring_time;
    tlv_subsequent_volume_threshold_t subsequent_volume_threshold;
    tlv_subsequent_time_threshold_t subsequent_time_threshold;
    tlv_subsequent_volume_quota_t subsequent_volume_quota;
    tlv_subsequent_time_quota_t subsequent_time_quota;
    tlv_inactivity_detection_time_t inactivity_detection_time;
    tlv_linked_urr_id_t linked_urr_id;
    tlv_measurement_information_t measurement_information;
    tlv_time_quota_mechanism_t time_quota_mechanism;
    tlv_aggregated_urrs_t aggregated_urrs;
} tlv_update_urr_t; ;

typedef struct _tlv_update_qer_t {
    tlv_presence_t presence;
    tlv_qer_id_t qer_id;
    tlv_qer_correlation_id_t qer_correlation_id;
    tlv_gate_status_t gate_status;
    tlv_mbr_t maximum_bitrate;
    tlv_gbr_t guaranteed_bitrate;
    tlv_packet_rate_t packet_rate;
    tlv_dl_flow_level_marking_t dl_flow_level_marking;
    tlv_qfi_t qos_flow_identifier;
    tlv_rqi_t reflective_qos;
} tlv_update_qer_t; ;

typedef struct _tlv_remove_pdr_t {
    tlv_presence_t presence;
    tlv_packet_detection_rule_id_t pdr_id;
} tlv_remove_pdr_t; ;

typedef struct _tlv_remove_far_t {
    tlv_presence_t presence;
    tlv_far_id_t far_id;
} tlv_remove_far_t; ;

typedef struct _tlv_remove_urr_t {
    tlv_presence_t presence;
    tlv_urr_id_t urr_id;
} tlv_remove_urr_t; ;

typedef struct _tlv_remove_qer_t {
    tlv_presence_t presence;
    tlv_qer_id_t qer_id;
} tlv_remove_qer_t; ;

typedef struct _tlv_load_control_informationp_t {
    tlv_presence_t presence;
    tlv_sequence_number_t load_control_sequence_number;
    tlv_metricp_t load_metric;
} tlv_load_control_informationp_t; ;

typedef struct _tlv_overload_control_informationp_t {
    tlv_presence_t presence;
    tlv_sequence_number_t overload_control_sequence_number;
    tlv_metricp_t overload_reduction_metric;
    tlv_timer_t period_of_validity;
    tlv_oci_flags_t overload_control_information_flags;
} tlv_overload_control_informationp_t; ;

typedef struct _tlv_pfd_context_t {
    tlv_presence_t presence;
    tlv_pfd_contents_t pfd_contents;
} tlv_pfd_context_t; ;

typedef struct _tlv_application_detection_information_t {
    tlv_presence_t presence;
    tlv_application_id_t application_id;
    tlv_application_instance_id_t application_instance_id;
    tlv_flow_information_t flow_information;
} tlv_application_detection_information_t; ;

typedef struct _tlv_query_urr_t {
    tlv_presence_t presence;
    tlv_urr_id_t urr_id;
} tlv_query_urr_t; ;

typedef struct _tlv_usage_report_in_session_modification_response_t {
    tlv_presence_t presence;
    tlv_urr_id_t urr_id;
    tlv_ur_seqn_t ur_seqn;
    tlv_usage_report_trigger_t usage_report_trigger;
    tlv_start_time_t start_time;
    tlv_end_time_t end_time;
    tlv_volume_measurement_t volume_measurement;
    tlv_duration_measurement_t duration_measurement;
    tlv_time_of_first_packet_t time_of_first_packet;
    tlv_time_of_last_packet_t time_of_last_packet;
    tlv_usage_information_t usage_information;
} tlv_usage_report_in_session_modification_response_t; ;

typedef struct _tlv_usage_report_session_deletion_response_t {
    tlv_presence_t presence;
    tlv_urr_id_t urr_id;
    tlv_ur_seqn_t ur_seqn;
    tlv_usage_report_trigger_t usage_report_trigger;
    tlv_start_time_t start_time;
    tlv_end_time_t end_time;
    tlv_volume_measurement_t volume_measurement;
    tlv_duration_measurement_t duration_measurement;
    tlv_time_of_first_packet_t time_of_first_packet;
    tlv_time_of_last_packet_t time_of_last_packet;
    tlv_usage_information_t usage_information;
} tlv_usage_report_session_deletion_response_t; ;

typedef struct _tlv_usage_report_session_report_request_t {
    tlv_presence_t presence;
    tlv_urr_id_t urr_id;
    tlv_ur_seqn_t ur_seqn;
    tlv_usage_report_trigger_t usage_report_trigger;
    tlv_start_time_t start_time;
    tlv_end_time_t end_time;
    tlv_volume_measurement_t volume_measurement;
    tlv_duration_measurement_t duration_measurement;
    tlv_application_detection_information_t application_detection_information;
    tlv_ue_ip_address_t ue_ip_address;
    tlv_network_instance_t network_instance;
    tlv_time_of_first_packet_t time_of_first_packet;
    tlv_time_of_last_packet_t time_of_last_packet;
    tlv_usage_information_t usage_information;
} tlv_usage_report_session_report_request_t; ;

typedef struct _tlv_downlink_data_report_t {
    tlv_presence_t presence;
    tlv_packet_detection_rule_id_t pdr_id;
    tlv_downlink_data_service_information_t downlink_data_service_information;
} tlv_downlink_data_report_t; ;

typedef struct _tlv_create_bar_t {
    tlv_presence_t presence;
    tlv_bar_id_t bar_id;
    tlv_downlink_data_notification_delay_t downlink_data_notification_delay;
} tlv_create_bar_t; ;

typedef struct _tlv_update_bar_session_modification_request_t {
    tlv_presence_t presence;
    tlv_bar_id_t bar_id;
    tlv_downlink_data_notification_delay_t downlink_data_notification_delay;
} tlv_update_bar_session_modification_request_t; ;

typedef struct _tlv_remove_bar_t {
    tlv_presence_t presence;
    tlv_bar_id_t bar_id;
} tlv_remove_bar_t; ;

typedef struct _tlv_error_indication_report_t {
    tlv_presence_t presence;
    tlv_f_teid_t remote_f_teid; /* Instance : 0 */
} tlv_error_indication_report_t; ;

typedef struct _tlv_update_duplicating_parameters_t {
    tlv_presence_t presence;
    tlv_destination_interface_t destination_interface;
    tlv_outer_header_creation_t outer_header_creation;
    tlv_transport_level_marking_t transport_level_marking;
    tlv_forwarding_policy_t forwarding_policy;
} tlv_update_duplicating_parameters_t; ;

typedef struct _tlv_create_pdr_t {
    tlv_presence_t presence;
    tlv_packet_detection_rule_id_t pdr_id;
    tlv_precedence_t precedence;
    tlv_pdi_t pdi;
    tlv_outer_header_removal_t outer_header_removal;
    tlv_far_id_t far_id;
    tlv_urr_id_t urr_id;
    tlv_qer_id_t qer_id;
    tlv_activate_predefined_rules_t activate_predefined_rules;
} tlv_create_pdr_t; ;

typedef struct _tlv_create_far_t {
    tlv_presence_t presence;
    tlv_far_id_t far_id;
    tlv_apply_action_t apply_action;
    tlv_forwarding_parameters_t forwarding_parameters;
    tlv_duplicating_parameters_t duplicating_parameters;
    tlv_bar_id_t bar_id;
} tlv_create_far_t; ;

typedef struct _tlv_update_far_t {
    tlv_presence_t presence;
    tlv_far_id_t far_id;
    tlv_apply_action_t apply_action;
    tlv_update_forwarding_parameters_t update_forwarding_parameters;
    tlv_update_duplicating_parameters_t update_duplicating_parameters;
    tlv_bar_id_t bar_id;
} tlv_update_far_t; ;

typedef struct _tlv_application_id_s_pfds_t {
    tlv_presence_t presence;
    tlv_application_id_t application_id;
    tlv_pfd_context_t pfd;
} tlv_application_id_s_pfds_t; ;

/* Structure for Message */
typedef struct _pfcp_heartbeat_request_t {
    tlv_recovery_time_stamp_t recovery_time_stamp;
} pfcp_heartbeat_request_t;

typedef struct _pfcp_heartbeat_response_t {
    tlv_recovery_time_stamp_t recovery_time_stamp;
} pfcp_heartbeat_response_t;

typedef struct _pfcp_association_setup_request_t {
    tlv_node_id_t node_id;
    tlv_recovery_time_stamp_t recovery_time_stamp;
    tlv_up_function_features_t up_function_features;
    tlv_cp_function_features_t cp_function_features;
    tlv_user_plane_ip_resource_information_t user_plane_ip_resource_information;
} pfcp_association_setup_request_t;

typedef struct _pfcp_association_setup_response_t {
    tlv_node_id_t node_id;
    tlv_cause_t cause;
    tlv_recovery_time_stamp_t recovery_time_stamp;
    tlv_up_function_features_t up_function_features;
    tlv_cp_function_features_t cp_function_features;
    tlv_user_plane_ip_resource_information_t user_plane_ip_resource_information;
} pfcp_association_setup_response_t;

typedef struct _pfcp_association_update_request_t {
    tlv_node_id_t node_id;
    tlv_up_function_features_t up_function_features;
    tlv_cp_function_features_t cp_function_features;
    tlv_pfcp_association_release_request_t pfcp_association_release_request;
    tlv_graceful_release_period_t graceful_release_period;
    tlv_user_plane_ip_resource_information_t user_plane_ip_resource_information;
} pfcp_association_update_request_t;

typedef struct _pfcp_association_update_response_t {
    tlv_node_id_t node_id;
    tlv_cause_t cause;
    tlv_up_function_features_t up_function_features;
    tlv_cp_function_features_t cp_function_features;
} pfcp_association_update_response_t;

typedef struct _pfcp_association_release_request_t {
    tlv_node_id_t node_id;
} pfcp_association_release_request_t;

typedef struct _pfcp_association_release_response_t {
    tlv_node_id_t node_id;
    tlv_cause_t cause;
} pfcp_association_release_response_t;

typedef struct _pfcp_version_not_supported_response_t {
} pfcp_version_not_supported_response_t;

typedef struct _pfcp_node_report_request_t {
    tlv_node_id_t node_id;
    tlv_node_report_type_t node_report_type;
    tlv_user_plane_path_failure_report_t user_plane_path_failure_report;
} pfcp_node_report_request_t;

typedef struct _pfcp_node_report_response_t {
    tlv_node_id_t node_id;
    tlv_cause_t cause;
    tlv_offending_ie_t offending_ie;
} pfcp_node_report_response_t;

typedef struct _pfcp_session_set_deletion_request_t {
    tlv_node_id_t node_id;
    tlv_fq_csidp_t sgw_c_fq_csid;
} pfcp_session_set_deletion_request_t;

typedef struct _pfcp_session_set_deletion_response_t {
    tlv_node_id_t node_id;
    tlv_cause_t cause;
    tlv_offending_ie_t offending_ie;
} pfcp_session_set_deletion_response_t;

typedef struct _pfcp_session_establishment_request_t {
    tlv_node_id_t node_id;
    tlv_f_seid_t cp_f_seid;
    tlv_create_pdr_t create_pdr;
    tlv_create_pdr_t create_pdr1;
    tlv_create_far_t create_far;
    tlv_create_far_t create_far1;
    tlv_create_urr_t create_urr;
    tlv_create_qer_t create_qer;
    tlv_create_bar_t create_bar;
    tlv_pdn_typep_t pdn_type;
    tlv_fq_csidp_t sgw_c_fq_csid;
    tlv_user_plane_inactivity_timer_t user_plane_inactivity_timer;
} pfcp_session_establishment_request_t;

typedef struct _pfcp_session_establishment_response_t {
    tlv_node_id_t node_id;
    tlv_cause_t cause;
    tlv_offending_ie_t offending_ie;
    tlv_f_seid_t up_f_seid;
    tlv_created_pdr_t created_pdr;
    tlv_load_control_informationp_t load_control_information;
    tlv_overload_control_informationp_t overload_control_information;
    tlv_fq_csidp_t sgw_u_fq_csid;
    tlv_failed_rule_id_t failed_rule_id;
} pfcp_session_establishment_response_t;

typedef struct _pfcp_session_modification_request_t {
    tlv_f_seid_t cp_f_seid;
    tlv_remove_pdr_t remove_pdr;
    tlv_remove_far_t remove_far;
    tlv_remove_urr_t remove_urr;
    tlv_remove_qer_t remove_qer;
    tlv_remove_bar_t remove_bar;
    tlv_create_pdr_t create_pdr;
    tlv_create_pdr_t create_pdr1;
    tlv_create_far_t create_far;
    tlv_create_far_t create_far1;
    tlv_create_urr_t create_urr;
    tlv_create_qer_t create_qer;
    tlv_create_bar_t create_bar;
    tlv_update_pdr_t update_pdr;
    tlv_update_pdr_t update_pdr1;
    tlv_update_far_t update_far;
    tlv_update_urr_t update_urr;
    tlv_update_qer_t update_qer;
    tlv_update_bar_session_modification_request_t update_bar;
    tlv_pfcpsmreq_flags_t pfcpsmreq_flags;
    tlv_query_urr_t query_urr;
    tlv_fq_csidp_t pgw_c_fq_csid;
    tlv_user_plane_inactivity_timer_t user_plane_inactivity_timer;
} pfcp_session_modification_request_t;

typedef struct _pfcp_session_modification_response_t {
    tlv_cause_t cause;
    tlv_offending_ie_t offending_ie;
    tlv_created_pdr_t created_pdr;
    tlv_load_control_informationp_t load_control_information;
    tlv_overload_control_informationp_t overload_control_information;
    tlv_usage_report_in_session_modification_response_t usage_report;
    tlv_failed_rule_id_t failed_rule_id;
} pfcp_session_modification_response_t;

typedef struct _pfcp_session_deletion_request_t {
} pfcp_session_deletion_request_t;

typedef struct _pfcp_session_deletion_response_t {
    tlv_cause_t cause;
    tlv_offending_ie_t offending_ie;
    tlv_load_control_informationp_t load_control_information;
    tlv_overload_control_informationp_t overload_control_information;
    tlv_usage_report_session_deletion_response_t usage_report;
} pfcp_session_deletion_response_t;

typedef struct _pfcp_session_report_request_t {
    tlv_report_type_t report_type;
    tlv_downlink_data_report_t downlink_data_report;
    tlv_usage_report_session_report_request_t usage_report;
    tlv_error_indication_report_t error_indication_report;
    tlv_load_control_informationp_t load_control_information;
    tlv_overload_control_informationp_t overload_control_information;
} pfcp_session_report_request_t;

typedef struct _pfcp_session_report_response_t {
    tlv_cause_t cause;
    tlv_offending_ie_t offending_ie;
    tlv_update_bar_pfcp_session_report_response_t update_bar;
    tlv_pfcpsrrsp_flags_t pfcpsrrsp_flags;
} pfcp_session_report_response_t;

typedef struct _pfcp_message_t {
   pfcp_header_t h;
   union {
        pfcp_heartbeat_request_t pfcp_heartbeat_request;
        pfcp_heartbeat_response_t pfcp_heartbeat_response;
        pfcp_association_setup_request_t pfcp_association_setup_request;
        pfcp_association_setup_response_t pfcp_association_setup_response;
        pfcp_association_update_request_t pfcp_association_update_request;
        pfcp_association_update_response_t pfcp_association_update_response;
        pfcp_association_release_request_t pfcp_association_release_request;
        pfcp_association_release_response_t pfcp_association_release_response;
        pfcp_version_not_supported_response_t pfcp_version_not_supported_response;
        pfcp_node_report_request_t pfcp_node_report_request;
        pfcp_node_report_response_t pfcp_node_report_response;
        pfcp_session_set_deletion_request_t pfcp_session_set_deletion_request;
        pfcp_session_set_deletion_response_t pfcp_session_set_deletion_response;
        pfcp_session_establishment_request_t pfcp_session_establishment_request;
        pfcp_session_establishment_response_t pfcp_session_establishment_response;
        pfcp_session_modification_request_t pfcp_session_modification_request;
        pfcp_session_modification_response_t pfcp_session_modification_response;
        pfcp_session_deletion_request_t pfcp_session_deletion_request;
        pfcp_session_deletion_response_t pfcp_session_deletion_response;
        pfcp_session_report_request_t pfcp_session_report_request;
        pfcp_session_report_response_t pfcp_session_report_response;
   };
} pfcp_message_t;

CORE_DECLARE(status_t) pfcp_parse_msg(
        pfcp_message_t *pfcp_message, pkbuf_t *pkbuf);
CORE_DECLARE(status_t) pfcp_build_msg(
        pkbuf_t **pkbuf, pfcp_message_t *pfcp_message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PFCP_TLV_H__ */
