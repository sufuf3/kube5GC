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
 * Created on: 2019-01-02 17:06:04.978258 by wirelab
 * from 29244-f40.docx
 ******************************************************************************/

#define TRACE_MODULE _pfcp_message
#include "core_debug.h"
#include "pfcp_message.h"

tlv_desc_t tlv_desc_pfcpcause_0 =
{
    TLV_VAR_STR,
    "Cause",
    TLV_PFCP_CAUSE_TYPE,
    0,
    0,
    sizeof(tlv_cause_t),
    { NULL }
};

tlv_desc_t tlv_desc_source_interface_0 =
{
    TLV_VAR_STR,
    "Source Interface",
    TLV_PFCP_SOURCE_INTERFACE_TYPE,
    0,
    0,
    sizeof(tlv_source_interface_t),
    { NULL }
};

tlv_desc_t tlv_desc_pfcpf_teid_0 =
{
    TLV_VAR_STR,
    "F-TEID",
    TLV_PFCP_F_TEID_TYPE,
    0,
    0,
    sizeof(tlv_f_teid_t),
    { NULL }
};

tlv_desc_t tlv_desc_network_instance_0 =
{
    TLV_VAR_STR,
    "Network Instance",
    TLV_PFCP_NETWORK_INSTANCE_TYPE,
    0,
    0,
    sizeof(tlv_network_instance_t),
    { NULL }
};

tlv_desc_t tlv_desc_sdf_filter_0 =
{
    TLV_VAR_STR,
    "SDF Filter",
    TLV_PFCP_SDF_FILTER_TYPE,
    0,
    0,
    sizeof(tlv_sdf_filter_t),
    { NULL }
};

tlv_desc_t tlv_desc_application_id_0 =
{
    TLV_VAR_STR,
    "Application ID",
    TLV_PFCP_APPLICATION_ID_TYPE,
    0,
    0,
    sizeof(tlv_application_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_gate_status_0 =
{
    TLV_VAR_STR,
    "Gate Status",
    TLV_PFCP_GATE_STATUS_TYPE,
    0,
    0,
    sizeof(tlv_gate_status_t),
    { NULL }
};

tlv_desc_t tlv_desc_mbr_0 =
{
    TLV_VAR_STR,
    "MBR",
    TLV_PFCP_MBR_TYPE,
    0,
    0,
    sizeof(tlv_mbr_t),
    { NULL }
};

tlv_desc_t tlv_desc_gbr_0 =
{
    TLV_VAR_STR,
    "GBR",
    TLV_PFCP_GBR_TYPE,
    0,
    0,
    sizeof(tlv_gbr_t),
    { NULL }
};

tlv_desc_t tlv_desc_qer_correlation_id_0 =
{
    TLV_VAR_STR,
    "QER Correlation ID",
    TLV_PFCP_QER_CORRELATION_ID_TYPE,
    0,
    0,
    sizeof(tlv_qer_correlation_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_precedence_0 =
{
    TLV_VAR_STR,
    "Precedence",
    TLV_PFCP_PRECEDENCE_TYPE,
    0,
    0,
    sizeof(tlv_precedence_t),
    { NULL }
};

tlv_desc_t tlv_desc_transport_level_marking_0 =
{
    TLV_VAR_STR,
    "Transport Level Marking",
    TLV_PFCP_TRANSPORT_LEVEL_MARKING_TYPE,
    0,
    0,
    sizeof(tlv_transport_level_marking_t),
    { NULL }
};

tlv_desc_t tlv_desc_volume_threshold_0 =
{
    TLV_VAR_STR,
    "Volume Threshold",
    TLV_PFCP_VOLUME_THRESHOLD_TYPE,
    0,
    0,
    sizeof(tlv_volume_threshold_t),
    { NULL }
};

tlv_desc_t tlv_desc_time_threshold_0 =
{
    TLV_VAR_STR,
    "Time Threshold",
    TLV_PFCP_TIME_THRESHOLD_TYPE,
    0,
    0,
    sizeof(tlv_time_threshold_t),
    { NULL }
};

tlv_desc_t tlv_desc_monitoring_time_0 =
{
    TLV_VAR_STR,
    "Monitoring Time",
    TLV_PFCP_MONITORING_TIME_TYPE,
    0,
    0,
    sizeof(tlv_monitoring_time_t),
    { NULL }
};

tlv_desc_t tlv_desc_subsequent_volume_threshold_0 =
{
    TLV_VAR_STR,
    "Subsequent Volume Threshold",
    TLV_PFCP_SUBSEQUENT_VOLUME_THRESHOLD_TYPE,
    0,
    0,
    sizeof(tlv_subsequent_volume_threshold_t),
    { NULL }
};

tlv_desc_t tlv_desc_subsequent_time_threshold_0 =
{
    TLV_VAR_STR,
    "Subsequent Time Threshold",
    TLV_PFCP_SUBSEQUENT_TIME_THRESHOLD_TYPE,
    0,
    0,
    sizeof(tlv_subsequent_time_threshold_t),
    { NULL }
};

tlv_desc_t tlv_desc_inactivity_detection_time_0 =
{
    TLV_VAR_STR,
    "Inactivity Detection Time",
    TLV_PFCP_INACTIVITY_DETECTION_TIME_TYPE,
    0,
    0,
    sizeof(tlv_inactivity_detection_time_t),
    { NULL }
};

tlv_desc_t tlv_desc_reporting_triggers_0 =
{
    TLV_VAR_STR,
    "Reporting Triggers",
    TLV_PFCP_REPORTING_TRIGGERS_TYPE,
    0,
    0,
    sizeof(tlv_reporting_triggers_t),
    { NULL }
};

tlv_desc_t tlv_desc_redirect_information_0 =
{
    TLV_VAR_STR,
    "Redirect Information",
    TLV_PFCP_REDIRECT_INFORMATION_TYPE,
    0,
    0,
    sizeof(tlv_redirect_information_t),
    { NULL }
};

tlv_desc_t tlv_desc_report_type_0 =
{
    TLV_VAR_STR,
    "Report Type",
    TLV_PFCP_REPORT_TYPE_TYPE,
    0,
    0,
    sizeof(tlv_report_type_t),
    { NULL }
};

tlv_desc_t tlv_desc_offending_ie_0 =
{
    TLV_VAR_STR,
    "Offending IE",
    TLV_PFCP_OFFENDING_IE_TYPE,
    0,
    0,
    sizeof(tlv_offending_ie_t),
    { NULL }
};

tlv_desc_t tlv_desc_forwarding_policy_0 =
{
    TLV_VAR_STR,
    "Forwarding Policy",
    TLV_PFCP_FORWARDING_POLICY_TYPE,
    0,
    0,
    sizeof(tlv_forwarding_policy_t),
    { NULL }
};

tlv_desc_t tlv_desc_destination_interface_0 =
{
    TLV_VAR_STR,
    "Destination Interface",
    TLV_PFCP_DESTINATION_INTERFACE_TYPE,
    0,
    0,
    sizeof(tlv_destination_interface_t),
    { NULL }
};

tlv_desc_t tlv_desc_up_function_features_0 =
{
    TLV_VAR_STR,
    "UP Function Features",
    TLV_PFCP_UP_FUNCTION_FEATURES_TYPE,
    0,
    0,
    sizeof(tlv_up_function_features_t),
    { NULL }
};

tlv_desc_t tlv_desc_apply_action_0 =
{
    TLV_VAR_STR,
    "Apply Action",
    TLV_PFCP_APPLY_ACTION_TYPE,
    0,
    0,
    sizeof(tlv_apply_action_t),
    { NULL }
};

tlv_desc_t tlv_desc_downlink_data_service_information_0 =
{
    TLV_VAR_STR,
    "Downlink Data Service Information",
    TLV_PFCP_DOWNLINK_DATA_SERVICE_INFORMATION_TYPE,
    0,
    0,
    sizeof(tlv_downlink_data_service_information_t),
    { NULL }
};

tlv_desc_t tlv_desc_downlink_data_notification_delay_0 =
{
    TLV_VAR_STR,
    "Downlink Data Notification Delay",
    TLV_PFCP_DOWNLINK_DATA_NOTIFICATION_DELAY_TYPE,
    0,
    0,
    sizeof(tlv_downlink_data_notification_delay_t),
    { NULL }
};

tlv_desc_t tlv_desc_dl_buffering_duration_0 =
{
    TLV_VAR_STR,
    "DL Buffering Duration",
    TLV_PFCP_DL_BUFFERING_DURATION_TYPE,
    0,
    0,
    sizeof(tlv_dl_buffering_duration_t),
    { NULL }
};

tlv_desc_t tlv_desc_dl_buffering_suggested_packet_count_0 =
{
    TLV_VAR_STR,
    "DL Buffering Suggested Packet Count",
    TLV_PFCP_DL_BUFFERING_SUGGESTED_PACKET_COUNT_TYPE,
    0,
    0,
    sizeof(tlv_dl_buffering_suggested_packet_count_t),
    { NULL }
};

tlv_desc_t tlv_desc_pfcpsmreq_flags_0 =
{
    TLV_VAR_STR,
    "PFCPSMReq-Flags",
    TLV_PFCP_PFCPSMREQ_FLAGS_TYPE,
    0,
    0,
    sizeof(tlv_pfcpsmreq_flags_t),
    { NULL }
};

tlv_desc_t tlv_desc_pfcpsrrsp_flags_0 =
{
    TLV_VAR_STR,
    "PFCPSRRsp-Flags",
    TLV_PFCP_PFCPSRRSP_FLAGS_TYPE,
    0,
    0,
    sizeof(tlv_pfcpsrrsp_flags_t),
    { NULL }
};

tlv_desc_t tlv_desc_pfcpsequence_number_0 =
{
    TLV_VAR_STR,
    "Sequence Number",
    TLV_PFCP_SEQUENCE_NUMBER_TYPE,
    0,
    0,
    sizeof(tlv_sequence_number_t),
    { NULL }
};

tlv_desc_t tlv_desc_metricp_0 =
{
    TLV_VAR_STR,
    "Metricp",
    TLV_PFCP_METRICP_TYPE,
    0,
    0,
    sizeof(tlv_metricp_t),
    { NULL }
};

tlv_desc_t tlv_desc_timer_0 =
{
    TLV_VAR_STR,
    "Timer",
    TLV_PFCP_TIMER_TYPE,
    0,
    0,
    sizeof(tlv_timer_t),
    { NULL }
};

tlv_desc_t tlv_desc_packet_detection_rule_id_0 =
{
    TLV_VAR_STR,
    "Packet Detection Rule ID",
    TLV_PFCP_PACKET_DETECTION_RULE_ID_TYPE,
    0,
    0,
    sizeof(tlv_packet_detection_rule_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_f_seid_0 =
{
    TLV_VAR_STR,
    "F-SEID",
    TLV_PFCP_F_SEID_TYPE,
    0,
    0,
    sizeof(tlv_f_seid_t),
    { NULL }
};

tlv_desc_t tlv_desc_node_id_0 =
{
    TLV_VAR_STR,
    "Node ID",
    TLV_PFCP_NODE_ID_TYPE,
    0,
    0,
    sizeof(tlv_node_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_pfd_contents_0 =
{
    TLV_VAR_STR,
    "PFD contents",
    TLV_PFCP_PFD_CONTENTS_TYPE,
    0,
    0,
    sizeof(tlv_pfd_contents_t),
    { NULL }
};

tlv_desc_t tlv_desc_measurement_method_0 =
{
    TLV_VAR_STR,
    "Measurement Method",
    TLV_PFCP_MEASUREMENT_METHOD_TYPE,
    0,
    0,
    sizeof(tlv_measurement_method_t),
    { NULL }
};

tlv_desc_t tlv_desc_usage_report_trigger_0 =
{
    TLV_VAR_STR,
    "Usage Report Trigger",
    TLV_PFCP_USAGE_REPORT_TRIGGER_TYPE,
    0,
    0,
    sizeof(tlv_usage_report_trigger_t),
    { NULL }
};

tlv_desc_t tlv_desc_measurement_period_0 =
{
    TLV_VAR_STR,
    "Measurement Period",
    TLV_PFCP_MEASUREMENT_PERIOD_TYPE,
    0,
    0,
    sizeof(tlv_measurement_period_t),
    { NULL }
};

tlv_desc_t tlv_desc_fq_csidp_0 =
{
    TLV_VAR_STR,
    "FQ-CSIDp",
    TLV_PFCP_FQ_CSIDP_TYPE,
    0,
    0,
    sizeof(tlv_fq_csidp_t),
    { NULL }
};

tlv_desc_t tlv_desc_volume_measurement_0 =
{
    TLV_VAR_STR,
    "Volume Measurement",
    TLV_PFCP_VOLUME_MEASUREMENT_TYPE,
    0,
    0,
    sizeof(tlv_volume_measurement_t),
    { NULL }
};

tlv_desc_t tlv_desc_duration_measurement_0 =
{
    TLV_VAR_STR,
    "Duration Measurement",
    TLV_PFCP_DURATION_MEASUREMENT_TYPE,
    0,
    0,
    sizeof(tlv_duration_measurement_t),
    { NULL }
};

tlv_desc_t tlv_desc_time_of_first_packet_0 =
{
    TLV_VAR_STR,
    "Time of First Packet",
    TLV_PFCP_TIME_OF_FIRST_PACKET_TYPE,
    0,
    0,
    sizeof(tlv_time_of_first_packet_t),
    { NULL }
};

tlv_desc_t tlv_desc_time_of_last_packet_0 =
{
    TLV_VAR_STR,
    "Time of Last Packet",
    TLV_PFCP_TIME_OF_LAST_PACKET_TYPE,
    0,
    0,
    sizeof(tlv_time_of_last_packet_t),
    { NULL }
};

tlv_desc_t tlv_desc_quota_holding_time_0 =
{
    TLV_VAR_STR,
    "Quota Holding Time",
    TLV_PFCP_QUOTA_HOLDING_TIME_TYPE,
    0,
    0,
    sizeof(tlv_quota_holding_time_t),
    { NULL }
};

tlv_desc_t tlv_desc_dropped_dl_traffic_threshold_0 =
{
    TLV_VAR_STR,
    "Dropped DL Traffic Threshold",
    TLV_PFCP_DROPPED_DL_TRAFFIC_THRESHOLD_TYPE,
    0,
    0,
    sizeof(tlv_dropped_dl_traffic_threshold_t),
    { NULL }
};

tlv_desc_t tlv_desc_volume_quota_0 =
{
    TLV_VAR_STR,
    "Volume Quota",
    TLV_PFCP_VOLUME_QUOTA_TYPE,
    0,
    0,
    sizeof(tlv_volume_quota_t),
    { NULL }
};

tlv_desc_t tlv_desc_time_quota_0 =
{
    TLV_VAR_STR,
    "Time Quota",
    TLV_PFCP_TIME_QUOTA_TYPE,
    0,
    0,
    sizeof(tlv_time_quota_t),
    { NULL }
};

tlv_desc_t tlv_desc_start_time_0 =
{
    TLV_VAR_STR,
    "Start Time",
    TLV_PFCP_START_TIME_TYPE,
    0,
    0,
    sizeof(tlv_start_time_t),
    { NULL }
};

tlv_desc_t tlv_desc_end_time_0 =
{
    TLV_VAR_STR,
    "End Time",
    TLV_PFCP_END_TIME_TYPE,
    0,
    0,
    sizeof(tlv_end_time_t),
    { NULL }
};

tlv_desc_t tlv_desc_urr_id_0 =
{
    TLV_VAR_STR,
    "URR ID",
    TLV_PFCP_URR_ID_TYPE,
    0,
    0,
    sizeof(tlv_urr_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_linked_urr_id_0 =
{
    TLV_VAR_STR,
    "Linked URR ID",
    TLV_PFCP_LINKED_URR_ID_TYPE,
    0,
    0,
    sizeof(tlv_linked_urr_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_outer_header_creation_0 =
{
    TLV_VAR_STR,
    "Outer Header Creation",
    TLV_PFCP_OUTER_HEADER_CREATION_TYPE,
    0,
    0,
    sizeof(tlv_outer_header_creation_t),
    { NULL }
};

tlv_desc_t tlv_desc_bar_id_0 =
{
    TLV_VAR_STR,
    "BAR ID",
    TLV_PFCP_BAR_ID_TYPE,
    0,
    0,
    sizeof(tlv_bar_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_cp_function_features_0 =
{
    TLV_VAR_STR,
    "CP Function Features",
    TLV_PFCP_CP_FUNCTION_FEATURES_TYPE,
    0,
    0,
    sizeof(tlv_cp_function_features_t),
    { NULL }
};

tlv_desc_t tlv_desc_usage_information_0 =
{
    TLV_VAR_STR,
    "Usage Information",
    TLV_PFCP_USAGE_INFORMATION_TYPE,
    0,
    0,
    sizeof(tlv_usage_information_t),
    { NULL }
};

tlv_desc_t tlv_desc_application_instance_id_0 =
{
    TLV_VAR_STR,
    "Application Instance ID",
    TLV_PFCP_APPLICATION_INSTANCE_ID_TYPE,
    0,
    0,
    sizeof(tlv_application_instance_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_flow_information_0 =
{
    TLV_VAR_STR,
    "Flow Information",
    TLV_PFCP_FLOW_INFORMATION_TYPE,
    0,
    0,
    sizeof(tlv_flow_information_t),
    { NULL }
};

tlv_desc_t tlv_desc_ue_ip_address_0 =
{
    TLV_VAR_STR,
    "UE IP Address",
    TLV_PFCP_UE_IP_ADDRESS_TYPE,
    0,
    0,
    sizeof(tlv_ue_ip_address_t),
    { NULL }
};

tlv_desc_t tlv_desc_packet_rate_0 =
{
    TLV_VAR_STR,
    "Packet Rate",
    TLV_PFCP_PACKET_RATE_TYPE,
    0,
    0,
    sizeof(tlv_packet_rate_t),
    { NULL }
};

tlv_desc_t tlv_desc_outer_header_removal_0 =
{
    TLV_VAR_STR,
    "Outer Header Removal",
    TLV_PFCP_OUTER_HEADER_REMOVAL_TYPE,
    0,
    0,
    sizeof(tlv_outer_header_removal_t),
    { NULL }
};

tlv_desc_t tlv_desc_recovery_time_stamp_0 =
{
    TLV_VAR_STR,
    "Recovery Time Stamp",
    TLV_PFCP_RECOVERY_TIME_STAMP_TYPE,
    0,
    0,
    sizeof(tlv_recovery_time_stamp_t),
    { NULL }
};

tlv_desc_t tlv_desc_dl_flow_level_marking_0 =
{
    TLV_VAR_STR,
    "DL Flow Level Marking",
    TLV_PFCP_DL_FLOW_LEVEL_MARKING_TYPE,
    0,
    0,
    sizeof(tlv_dl_flow_level_marking_t),
    { NULL }
};

tlv_desc_t tlv_desc_header_enrichment_0 =
{
    TLV_VAR_STR,
    "Header Enrichment",
    TLV_PFCP_HEADER_ENRICHMENT_TYPE,
    0,
    0,
    sizeof(tlv_header_enrichment_t),
    { NULL }
};

tlv_desc_t tlv_desc_measurement_information_0 =
{
    TLV_VAR_STR,
    "Measurement Information",
    TLV_PFCP_MEASUREMENT_INFORMATION_TYPE,
    0,
    0,
    sizeof(tlv_measurement_information_t),
    { NULL }
};

tlv_desc_t tlv_desc_node_report_type_0 =
{
    TLV_VAR_STR,
    "Node Report Type",
    TLV_PFCP_NODE_REPORT_TYPE_TYPE,
    0,
    0,
    sizeof(tlv_node_report_type_t),
    { NULL }
};

tlv_desc_t tlv_desc_user_plane_path_failure_report_0 =
{
    TLV_VAR_STR,
    "User Plane Path Failure Report",
    TLV_PFCP_USER_PLANE_PATH_FAILURE_REPORT_TYPE,
    0,
    0,
    sizeof(tlv_user_plane_path_failure_report_t),
    { NULL }
};

tlv_desc_t tlv_desc_remote_gtp_u_peer_0 =
{
    TLV_VAR_STR,
    "Remote GTP-U Peer",
    TLV_PFCP_REMOTE_GTP_U_PEER_TYPE,
    0,
    0,
    sizeof(tlv_remote_gtp_u_peer_t),
    { NULL }
};

tlv_desc_t tlv_desc_ur_seqn_0 =
{
    TLV_VAR_STR,
    "UR-SEQN",
    TLV_PFCP_UR_SEQN_TYPE,
    0,
    0,
    sizeof(tlv_ur_seqn_t),
    { NULL }
};

tlv_desc_t tlv_desc_activate_predefined_rules_0 =
{
    TLV_VAR_STR,
    "Activate Predefined Rules",
    TLV_PFCP_ACTIVATE_PREDEFINED_RULES_TYPE,
    0,
    0,
    sizeof(tlv_activate_predefined_rules_t),
    { NULL }
};

tlv_desc_t tlv_desc_deactivate_predefined_rules_0 =
{
    TLV_VAR_STR,
    "Deactivate Predefined Rules",
    TLV_PFCP_DEACTIVATE_PREDEFINED_RULES_TYPE,
    0,
    0,
    sizeof(tlv_deactivate_predefined_rules_t),
    { NULL }
};

tlv_desc_t tlv_desc_far_id_0 =
{
    TLV_VAR_STR,
    "FAR ID",
    TLV_PFCP_FAR_ID_TYPE,
    0,
    0,
    sizeof(tlv_far_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_qer_id_0 =
{
    TLV_VAR_STR,
    "QER ID",
    TLV_PFCP_QER_ID_TYPE,
    0,
    0,
    sizeof(tlv_qer_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_oci_flags_0 =
{
    TLV_VAR_STR,
    "OCI Flags",
    TLV_PFCP_OCI_FLAGS_TYPE,
    0,
    0,
    sizeof(tlv_oci_flags_t),
    { NULL }
};

tlv_desc_t tlv_desc_pfcp_association_release_request_0 =
{
    TLV_VAR_STR,
    "PFCP Association Release Request",
    TLV_PFCP_PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE,
    0,
    0,
    sizeof(tlv_pfcp_association_release_request_t),
    { NULL }
};

tlv_desc_t tlv_desc_graceful_release_period_0 =
{
    TLV_VAR_STR,
    "Graceful Release Period",
    TLV_PFCP_GRACEFUL_RELEASE_PERIOD_TYPE,
    0,
    0,
    sizeof(tlv_graceful_release_period_t),
    { NULL }
};

tlv_desc_t tlv_desc_pdn_typep_0 =
{
    TLV_VAR_STR,
    "PDN Typep",
    TLV_PFCP_PDN_TYPEP_TYPE,
    0,
    0,
    sizeof(tlv_pdn_typep_t),
    { NULL }
};

tlv_desc_t tlv_desc_failed_rule_id_0 =
{
    TLV_VAR_STR,
    "Failed Rule ID",
    TLV_PFCP_FAILED_RULE_ID_TYPE,
    0,
    0,
    sizeof(tlv_failed_rule_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_time_quota_mechanism_0 =
{
    TLV_VAR_STR,
    "Time Quota Mechanism",
    TLV_PFCP_TIME_QUOTA_MECHANISM_TYPE,
    0,
    0,
    sizeof(tlv_time_quota_mechanism_t),
    { NULL }
};

tlv_desc_t tlv_desc_user_plane_ip_resource_information_0 =
{
    TLV_VAR_STR,
    "User Plane IP Resource Information",
    TLV_PFCP_USER_PLANE_IP_RESOURCE_INFORMATION_TYPE,
    0,
    0,
    sizeof(tlv_user_plane_ip_resource_information_t),
    { NULL }
};

tlv_desc_t tlv_desc_user_plane_inactivity_timer_0 =
{
    TLV_VAR_STR,
    "User Plane Inactivity Timer",
    TLV_PFCP_USER_PLANE_INACTIVITY_TIMER_TYPE,
    0,
    0,
    sizeof(tlv_user_plane_inactivity_timer_t),
    { NULL }
};

tlv_desc_t tlv_desc_aggregated_urrs_0 =
{
    TLV_VAR_STR,
    "Aggregated URRs",
    TLV_PFCP_AGGREGATED_URRS_TYPE,
    0,
    0,
    sizeof(tlv_aggregated_urrs_t),
    { NULL }
};

tlv_desc_t tlv_desc_multiplier_0 =
{
    TLV_VAR_STR,
    "Multiplier",
    TLV_PFCP_MULTIPLIER_TYPE,
    0,
    0,
    sizeof(tlv_multiplier_t),
    { NULL }
};

tlv_desc_t tlv_desc_aggregated_urr_id_0 =
{
    TLV_VAR_STR,
    "Aggregated URR ID",
    TLV_PFCP_AGGREGATED_URR_ID_TYPE,
    0,
    0,
    sizeof(tlv_aggregated_urr_id_t),
    { NULL }
};

tlv_desc_t tlv_desc_subsequent_volume_quota_0 =
{
    TLV_VAR_STR,
    "Subsequent Volume Quota",
    TLV_PFCP_SUBSEQUENT_VOLUME_QUOTA_TYPE,
    0,
    0,
    sizeof(tlv_subsequent_volume_quota_t),
    { NULL }
};

tlv_desc_t tlv_desc_subsequent_time_quota_0 =
{
    TLV_VAR_STR,
    "Subsequent Time Quota",
    TLV_PFCP_SUBSEQUENT_TIME_QUOTA_TYPE,
    0,
    0,
    sizeof(tlv_subsequent_time_quota_t),
    { NULL }
};

tlv_desc_t tlv_desc_rqi_0 =
{
    TLV_VAR_STR,
    "RQI",
    TLV_PFCP_RQI_TYPE,
    0,
    0,
    sizeof(tlv_rqi_t),
    { NULL }
};

tlv_desc_t tlv_desc_qfi_0 =
{
    TLV_VAR_STR,
    "QFI",
    TLV_PFCP_QFI_TYPE,
    0,
    0,
    sizeof(tlv_qfi_t),
    { NULL }
};

tlv_desc_t tlv_desc_create_pdr_0 =
{
    TLV_COMPOUND,
    "Create PDR",
    TLV_PFCP_CREATE_PDR_TYPE,
    0,
    0,
    sizeof(tlv_create_pdr_t),
    {
        &tlv_desc_packet_detection_rule_id_0,
        &tlv_desc_precedence_0,
        &tlv_desc_pdi_0,
        &tlv_desc_outer_header_removal_0,
        &tlv_desc_far_id_0,
        &tlv_desc_urr_id_0,
        &tlv_desc_qer_id_0,
        &tlv_desc_activate_predefined_rules_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_create_pdr_1 =
{
    TLV_COMPOUND,
    "Create PDR",
    TLV_PFCP_CREATE_PDR_TYPE,
    0,
    1,
    sizeof(tlv_create_pdr_t),
    {
        &tlv_desc_packet_detection_rule_id_0,
        &tlv_desc_precedence_0,
        &tlv_desc_pdi_0,
        &tlv_desc_outer_header_removal_0,
        &tlv_desc_far_id_0,
        &tlv_desc_urr_id_0,
        &tlv_desc_qer_id_0,
        &tlv_desc_activate_predefined_rules_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_pdi_0 =
{
    TLV_COMPOUND,
    "PDI",
    TLV_PFCP_PDI_TYPE,
    0,
    0,
    sizeof(tlv_pdi_t),
    {
        &tlv_desc_source_interface_0,
        &tlv_desc_pfcpf_teid_0,
        &tlv_desc_network_instance_0,
        &tlv_desc_ue_ip_address_0,
        &tlv_desc_sdf_filter_0,
        &tlv_desc_application_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_create_far_0 =
{
    TLV_COMPOUND,
    "Create FAR",
    TLV_PFCP_CREATE_FAR_TYPE,
    0,
    0,
    sizeof(tlv_create_far_t),
    {
        &tlv_desc_far_id_0,
        &tlv_desc_apply_action_0,
        &tlv_desc_forwarding_parameters_0,
        &tlv_desc_duplicating_parameters_0,
        &tlv_desc_bar_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_create_far_1 =
{
    TLV_COMPOUND,
    "Create FAR",
    TLV_PFCP_CREATE_FAR_TYPE,
    0,
    1,
    sizeof(tlv_create_far_t),
    {
        &tlv_desc_far_id_0,
        &tlv_desc_apply_action_0,
        &tlv_desc_forwarding_parameters_0,
        &tlv_desc_duplicating_parameters_0,
        &tlv_desc_bar_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_forwarding_parameters_0 =
{
    TLV_COMPOUND,
    "Forwarding Parameters",
    TLV_PFCP_FORWARDING_PARAMETERS_TYPE,
    0,
    0,
    sizeof(tlv_forwarding_parameters_t),
    {
        &tlv_desc_destination_interface_0,
        &tlv_desc_network_instance_0,
        &tlv_desc_redirect_information_0,
        &tlv_desc_outer_header_creation_0,
        &tlv_desc_transport_level_marking_0,
        &tlv_desc_forwarding_policy_0,
        &tlv_desc_header_enrichment_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_duplicating_parameters_0 =
{
    TLV_COMPOUND,
    "Duplicating Parameters",
    TLV_PFCP_DUPLICATING_PARAMETERS_TYPE,
    0,
    0,
    sizeof(tlv_duplicating_parameters_t),
    {
        &tlv_desc_destination_interface_0,
        &tlv_desc_outer_header_creation_0,
        &tlv_desc_transport_level_marking_0,
        &tlv_desc_forwarding_policy_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_create_urr_0 =
{
    TLV_COMPOUND,
    "Create URR",
    TLV_PFCP_CREATE_URR_TYPE,
    0,
    0,
    sizeof(tlv_create_urr_t),
    {
        &tlv_desc_urr_id_0,
        &tlv_desc_measurement_method_0,
        &tlv_desc_reporting_triggers_0,
        &tlv_desc_measurement_period_0,
        &tlv_desc_volume_threshold_0,
        &tlv_desc_volume_quota_0,
        &tlv_desc_time_threshold_0,
        &tlv_desc_time_quota_0,
        &tlv_desc_quota_holding_time_0,
        &tlv_desc_dropped_dl_traffic_threshold_0,
        &tlv_desc_monitoring_time_0,
        &tlv_desc_subsequent_volume_threshold_0,
        &tlv_desc_subsequent_time_threshold_0,
        &tlv_desc_subsequent_volume_quota_0,
        &tlv_desc_subsequent_time_quota_0,
        &tlv_desc_inactivity_detection_time_0,
        &tlv_desc_linked_urr_id_0,
        &tlv_desc_measurement_information_0,
        &tlv_desc_time_quota_mechanism_0,
        &tlv_desc_aggregated_urrs_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_create_qer_0 =
{
    TLV_COMPOUND,
    "Create QER",
    TLV_PFCP_CREATE_QER_TYPE,
    0,
    0,
    sizeof(tlv_create_qer_t),
    {
        &tlv_desc_qer_id_0,
        &tlv_desc_qer_correlation_id_0,
        &tlv_desc_gate_status_0,
        &tlv_desc_mbr_0,
        &tlv_desc_gbr_0,
        &tlv_desc_packet_rate_0,
        &tlv_desc_dl_flow_level_marking_0,
        &tlv_desc_qfi_0,
        &tlv_desc_rqi_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_created_pdr_0 =
{
    TLV_COMPOUND,
    "Created PDR",
    TLV_PFCP_CREATED_PDR_TYPE,
    0,
    0,
    sizeof(tlv_created_pdr_t),
    {
        &tlv_desc_packet_detection_rule_id_0,
        &tlv_desc_pfcpf_teid_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_pdr_0 =
{
    TLV_COMPOUND,
    "Update PDR",
    TLV_PFCP_UPDATE_PDR_TYPE,
    0,
    0,
    sizeof(tlv_update_pdr_t),
    {
        &tlv_desc_packet_detection_rule_id_0,
        &tlv_desc_outer_header_removal_0,
        &tlv_desc_precedence_0,
        &tlv_desc_pdi_0,
        &tlv_desc_far_id_0,
        &tlv_desc_urr_id_0,
        &tlv_desc_qer_id_0,
        &tlv_desc_activate_predefined_rules_0,
        &tlv_desc_deactivate_predefined_rules_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_pdr_1 =
{
    TLV_COMPOUND,
    "Update PDR",
    TLV_PFCP_UPDATE_PDR_TYPE,
    0,
    1,
    sizeof(tlv_update_pdr_t),
    {
        &tlv_desc_packet_detection_rule_id_0,
        &tlv_desc_outer_header_removal_0,
        &tlv_desc_precedence_0,
        &tlv_desc_pdi_0,
        &tlv_desc_far_id_0,
        &tlv_desc_urr_id_0,
        &tlv_desc_qer_id_0,
        &tlv_desc_activate_predefined_rules_0,
        &tlv_desc_deactivate_predefined_rules_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_far_0 =
{
    TLV_COMPOUND,
    "Update FAR",
    TLV_PFCP_UPDATE_FAR_TYPE,
    0,
    0,
    sizeof(tlv_update_far_t),
    {
        &tlv_desc_far_id_0,
        &tlv_desc_apply_action_0,
        &tlv_desc_update_forwarding_parameters_0,
        &tlv_desc_update_duplicating_parameters_0,
        &tlv_desc_bar_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_forwarding_parameters_0 =
{
    TLV_COMPOUND,
    "Update Forwarding Parameters",
    TLV_PFCP_UPDATE_FORWARDING_PARAMETERS_TYPE,
    0,
    0,
    sizeof(tlv_update_forwarding_parameters_t),
    {
        &tlv_desc_destination_interface_0,
        &tlv_desc_network_instance_0,
        &tlv_desc_redirect_information_0,
        &tlv_desc_outer_header_creation_0,
        &tlv_desc_transport_level_marking_0,
        &tlv_desc_forwarding_policy_0,
        &tlv_desc_header_enrichment_0,
        &tlv_desc_pfcpsmreq_flags_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_bar_pfcp_session_report_response_0 =
{
    TLV_COMPOUND,
    "Update BAR PFCP Session Report Response",
    TLV_PFCP_UPDATE_BAR_PFCP_SESSION_REPORT_RESPONSE_TYPE,
    0,
    0,
    sizeof(tlv_update_bar_pfcp_session_report_response_t),
    {
        &tlv_desc_bar_id_0,
        &tlv_desc_downlink_data_notification_delay_0,
        &tlv_desc_dl_buffering_duration_0,
        &tlv_desc_dl_buffering_suggested_packet_count_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_urr_0 =
{
    TLV_COMPOUND,
    "Update URR",
    TLV_PFCP_UPDATE_URR_TYPE,
    0,
    0,
    sizeof(tlv_update_urr_t),
    {
        &tlv_desc_urr_id_0,
        &tlv_desc_measurement_method_0,
        &tlv_desc_reporting_triggers_0,
        &tlv_desc_measurement_period_0,
        &tlv_desc_volume_threshold_0,
        &tlv_desc_volume_quota_0,
        &tlv_desc_time_threshold_0,
        &tlv_desc_time_quota_0,
        &tlv_desc_quota_holding_time_0,
        &tlv_desc_dropped_dl_traffic_threshold_0,
        &tlv_desc_monitoring_time_0,
        &tlv_desc_subsequent_volume_threshold_0,
        &tlv_desc_subsequent_time_threshold_0,
        &tlv_desc_subsequent_volume_quota_0,
        &tlv_desc_subsequent_time_quota_0,
        &tlv_desc_inactivity_detection_time_0,
        &tlv_desc_linked_urr_id_0,
        &tlv_desc_measurement_information_0,
        &tlv_desc_time_quota_mechanism_0,
        &tlv_desc_aggregated_urrs_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_qer_0 =
{
    TLV_COMPOUND,
    "Update QER",
    TLV_PFCP_UPDATE_QER_TYPE,
    0,
    0,
    sizeof(tlv_update_qer_t),
    {
        &tlv_desc_qer_id_0,
        &tlv_desc_qer_correlation_id_0,
        &tlv_desc_gate_status_0,
        &tlv_desc_mbr_0,
        &tlv_desc_gbr_0,
        &tlv_desc_packet_rate_0,
        &tlv_desc_dl_flow_level_marking_0,
        &tlv_desc_qfi_0,
        &tlv_desc_rqi_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_remove_pdr_0 =
{
    TLV_COMPOUND,
    "Remove PDR",
    TLV_PFCP_REMOVE_PDR_TYPE,
    0,
    0,
    sizeof(tlv_remove_pdr_t),
    {
        &tlv_desc_packet_detection_rule_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_remove_far_0 =
{
    TLV_COMPOUND,
    "Remove FAR",
    TLV_PFCP_REMOVE_FAR_TYPE,
    0,
    0,
    sizeof(tlv_remove_far_t),
    {
        &tlv_desc_far_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_remove_urr_0 =
{
    TLV_COMPOUND,
    "Remove URR",
    TLV_PFCP_REMOVE_URR_TYPE,
    0,
    0,
    sizeof(tlv_remove_urr_t),
    {
        &tlv_desc_urr_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_remove_qer_0 =
{
    TLV_COMPOUND,
    "Remove QER",
    TLV_PFCP_REMOVE_QER_TYPE,
    0,
    0,
    sizeof(tlv_remove_qer_t),
    {
        &tlv_desc_qer_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_load_control_informationp_0 =
{
    TLV_COMPOUND,
    "Load Control Informationp",
    TLV_PFCP_LOAD_CONTROL_INFORMATIONP_TYPE,
    0,
    0,
    sizeof(tlv_load_control_informationp_t),
    {
        &tlv_desc_pfcpsequence_number_0,
        &tlv_desc_metricp_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_overload_control_informationp_0 =
{
    TLV_COMPOUND,
    "Overload Control Informationp",
    TLV_PFCP_OVERLOAD_CONTROL_INFORMATIONP_TYPE,
    0,
    0,
    sizeof(tlv_overload_control_informationp_t),
    {
        &tlv_desc_pfcpsequence_number_0,
        &tlv_desc_metricp_0,
        &tlv_desc_timer_0,
        &tlv_desc_oci_flags_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_application_id_s_pfds_0 =
{
    TLV_COMPOUND,
    "Application ID's PFDs",
    TLV_PFCP_APPLICATION_ID_S_PFDS_TYPE,
    0,
    0,
    sizeof(tlv_application_id_s_pfds_t),
    {
        &tlv_desc_application_id_0,
        &tlv_desc_pfd_context_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_pfd_context_0 =
{
    TLV_COMPOUND,
    "PFD context",
    TLV_PFCP_PFD_CONTEXT_TYPE,
    0,
    0,
    sizeof(tlv_pfd_context_t),
    {
        &tlv_desc_pfd_contents_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_application_detection_information_0 =
{
    TLV_COMPOUND,
    "Application Detection Information",
    TLV_PFCP_APPLICATION_DETECTION_INFORMATION_TYPE,
    0,
    0,
    sizeof(tlv_application_detection_information_t),
    {
        &tlv_desc_application_id_0,
        &tlv_desc_application_instance_id_0,
        &tlv_desc_flow_information_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_query_urr_0 =
{
    TLV_COMPOUND,
    "Query URR",
    TLV_PFCP_QUERY_URR_TYPE,
    0,
    0,
    sizeof(tlv_query_urr_t),
    {
        &tlv_desc_urr_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_usage_report_in_session_modification_response_0 =
{
    TLV_COMPOUND,
    "Usage Report in Session Modification Response",
    TLV_PFCP_USAGE_REPORT_IN_SESSION_MODIFICATION_RESPONSE_TYPE,
    0,
    0,
    sizeof(tlv_usage_report_in_session_modification_response_t),
    {
        &tlv_desc_urr_id_0,
        &tlv_desc_ur_seqn_0,
        &tlv_desc_usage_report_trigger_0,
        &tlv_desc_start_time_0,
        &tlv_desc_end_time_0,
        &tlv_desc_volume_measurement_0,
        &tlv_desc_duration_measurement_0,
        &tlv_desc_time_of_first_packet_0,
        &tlv_desc_time_of_last_packet_0,
        &tlv_desc_usage_information_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_usage_report_session_deletion_response_0 =
{
    TLV_COMPOUND,
    "Usage Report Session Deletion Response",
    TLV_PFCP_USAGE_REPORT_SESSION_DELETION_RESPONSE_TYPE,
    0,
    0,
    sizeof(tlv_usage_report_session_deletion_response_t),
    {
        &tlv_desc_urr_id_0,
        &tlv_desc_ur_seqn_0,
        &tlv_desc_usage_report_trigger_0,
        &tlv_desc_start_time_0,
        &tlv_desc_end_time_0,
        &tlv_desc_volume_measurement_0,
        &tlv_desc_duration_measurement_0,
        &tlv_desc_time_of_first_packet_0,
        &tlv_desc_time_of_last_packet_0,
        &tlv_desc_usage_information_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_usage_report_session_report_request_0 =
{
    TLV_COMPOUND,
    "Usage Report Session Report Request",
    TLV_PFCP_USAGE_REPORT_SESSION_REPORT_REQUEST_TYPE,
    0,
    0,
    sizeof(tlv_usage_report_session_report_request_t),
    {
        &tlv_desc_urr_id_0,
        &tlv_desc_ur_seqn_0,
        &tlv_desc_usage_report_trigger_0,
        &tlv_desc_start_time_0,
        &tlv_desc_end_time_0,
        &tlv_desc_volume_measurement_0,
        &tlv_desc_duration_measurement_0,
        &tlv_desc_application_detection_information_0,
        &tlv_desc_ue_ip_address_0,
        &tlv_desc_network_instance_0,
        &tlv_desc_time_of_first_packet_0,
        &tlv_desc_time_of_last_packet_0,
        &tlv_desc_usage_information_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_downlink_data_report_0 =
{
    TLV_COMPOUND,
    "Downlink Data Report",
    TLV_PFCP_DOWNLINK_DATA_REPORT_TYPE,
    0,
    0,
    sizeof(tlv_downlink_data_report_t),
    {
        &tlv_desc_packet_detection_rule_id_0,
        &tlv_desc_downlink_data_service_information_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_create_bar_0 =
{
    TLV_COMPOUND,
    "Create BAR",
    TLV_PFCP_CREATE_BAR_TYPE,
    0,
    0,
    sizeof(tlv_create_bar_t),
    {
        &tlv_desc_bar_id_0,
        &tlv_desc_downlink_data_notification_delay_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_bar_session_modification_request_0 =
{
    TLV_COMPOUND,
    "Update BAR Session Modification Request",
    TLV_PFCP_UPDATE_BAR_SESSION_MODIFICATION_REQUEST_TYPE,
    0,
    0,
    sizeof(tlv_update_bar_session_modification_request_t),
    {
        &tlv_desc_bar_id_0,
        &tlv_desc_downlink_data_notification_delay_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_remove_bar_0 =
{
    TLV_COMPOUND,
    "Remove BAR",
    TLV_PFCP_REMOVE_BAR_TYPE,
    0,
    0,
    sizeof(tlv_remove_bar_t),
    {
        &tlv_desc_bar_id_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_error_indication_report_0 =
{
    TLV_COMPOUND,
    "Error Indication Report",
    TLV_PFCP_ERROR_INDICATION_REPORT_TYPE,
    0,
    0,
    sizeof(tlv_error_indication_report_t),
    {
        &tlv_desc_pfcpf_teid_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_update_duplicating_parameters_0 =
{
    TLV_COMPOUND,
    "Update Duplicating Parameters",
    TLV_PFCP_UPDATE_DUPLICATING_PARAMETERS_TYPE,
    0,
    0,
    sizeof(tlv_update_duplicating_parameters_t),
    {
        &tlv_desc_destination_interface_0,
        &tlv_desc_outer_header_creation_0,
        &tlv_desc_transport_level_marking_0,
        &tlv_desc_forwarding_policy_0,
        NULL,
    }
};

tlv_desc_t tlv_desc_pfcp_heartbeat_request =
{
    TLV_MESSAGE,
    "PFCP Heartbeat Request",
    0, 0, 0, 0, {
        &tlv_desc_recovery_time_stamp_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_heartbeat_response =
{
    TLV_MESSAGE,
    "PFCP Heartbeat Response",
    0, 0, 0, 0, {
        &tlv_desc_recovery_time_stamp_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_association_setup_request =
{
    TLV_MESSAGE,
    "PFCP Association Setup Request",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_recovery_time_stamp_0,
        &tlv_desc_up_function_features_0,
        &tlv_desc_cp_function_features_0,
        &tlv_desc_user_plane_ip_resource_information_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_association_setup_response =
{
    TLV_MESSAGE,
    "PFCP Association Setup Response",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_pfcpcause_0,
        &tlv_desc_recovery_time_stamp_0,
        &tlv_desc_up_function_features_0,
        &tlv_desc_cp_function_features_0,
        &tlv_desc_user_plane_ip_resource_information_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_association_update_request =
{
    TLV_MESSAGE,
    "PFCP Association Update Request",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_up_function_features_0,
        &tlv_desc_cp_function_features_0,
        &tlv_desc_pfcp_association_release_request_0,
        &tlv_desc_graceful_release_period_0,
        &tlv_desc_user_plane_ip_resource_information_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_association_update_response =
{
    TLV_MESSAGE,
    "PFCP Association Update Response",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_pfcpcause_0,
        &tlv_desc_up_function_features_0,
        &tlv_desc_cp_function_features_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_association_release_request =
{
    TLV_MESSAGE,
    "PFCP Association Release Request",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_association_release_response =
{
    TLV_MESSAGE,
    "PFCP Association Release Response",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_pfcpcause_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_version_not_supported_response =
{
    TLV_MESSAGE,
    "PFCP Version Not Supported Response",
    0, 0, 0, 0, {
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_node_report_request =
{
    TLV_MESSAGE,
    "PFCP Node Report Request",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_node_report_type_0,
        &tlv_desc_user_plane_path_failure_report_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_node_report_response =
{
    TLV_MESSAGE,
    "PFCP Node Report Response",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_pfcpcause_0,
        &tlv_desc_offending_ie_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_set_deletion_request =
{
    TLV_MESSAGE,
    "PFCP Session Set Deletion Request",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_fq_csidp_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_set_deletion_response =
{
    TLV_MESSAGE,
    "PFCP Session Set Deletion Response",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_pfcpcause_0,
        &tlv_desc_offending_ie_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_establishment_request =
{
    TLV_MESSAGE,
    "PFCP Session Establishment Request",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_f_seid_0,
        &tlv_desc_create_pdr_0,
        &tlv_desc_create_pdr_1,
        &tlv_desc_create_far_0,
        &tlv_desc_create_far_1,
        &tlv_desc_create_urr_0,
        &tlv_desc_create_qer_0,
        &tlv_desc_create_bar_0,
        &tlv_desc_pdn_typep_0,
        &tlv_desc_fq_csidp_0,
        &tlv_desc_user_plane_inactivity_timer_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_establishment_response =
{
    TLV_MESSAGE,
    "PFCP Session Establishment Response",
    0, 0, 0, 0, {
        &tlv_desc_node_id_0,
        &tlv_desc_pfcpcause_0,
        &tlv_desc_offending_ie_0,
        &tlv_desc_f_seid_0,
        &tlv_desc_created_pdr_0,
        &tlv_desc_load_control_informationp_0,
        &tlv_desc_overload_control_informationp_0,
        &tlv_desc_fq_csidp_0,
        &tlv_desc_failed_rule_id_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_modification_request =
{
    TLV_MESSAGE,
    "PFCP Session Modification Request",
    0, 0, 0, 0, {
        &tlv_desc_f_seid_0,
        &tlv_desc_remove_pdr_0,
        &tlv_desc_remove_far_0,
        &tlv_desc_remove_urr_0,
        &tlv_desc_remove_qer_0,
        &tlv_desc_remove_bar_0,
        &tlv_desc_create_pdr_0,
        &tlv_desc_create_pdr_1,
        &tlv_desc_create_far_0,
        &tlv_desc_create_far_1,
        &tlv_desc_create_urr_0,
        &tlv_desc_create_qer_0,
        &tlv_desc_create_bar_0,
        &tlv_desc_update_pdr_0,
        &tlv_desc_update_pdr_1,
        &tlv_desc_update_far_0,
        &tlv_desc_update_urr_0,
        &tlv_desc_update_qer_0,
        &tlv_desc_update_bar_session_modification_request_0,
        &tlv_desc_pfcpsmreq_flags_0,
        &tlv_desc_query_urr_0,
        &tlv_desc_fq_csidp_0,
        &tlv_desc_user_plane_inactivity_timer_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_modification_response =
{
    TLV_MESSAGE,
    "PFCP Session Modification Response",
    0, 0, 0, 0, {
        &tlv_desc_pfcpcause_0,
        &tlv_desc_offending_ie_0,
        &tlv_desc_created_pdr_0,
        &tlv_desc_load_control_informationp_0,
        &tlv_desc_overload_control_informationp_0,
        &tlv_desc_usage_report_in_session_modification_response_0,
        &tlv_desc_failed_rule_id_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_deletion_request =
{
    TLV_MESSAGE,
    "PFCP Session Deletion Request",
    0, 0, 0, 0, {
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_deletion_response =
{
    TLV_MESSAGE,
    "PFCP Session Deletion Response",
    0, 0, 0, 0, {
        &tlv_desc_pfcpcause_0,
        &tlv_desc_offending_ie_0,
        &tlv_desc_load_control_informationp_0,
        &tlv_desc_overload_control_informationp_0,
        &tlv_desc_usage_report_session_deletion_response_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_report_request =
{
    TLV_MESSAGE,
    "PFCP Session Report Request",
    0, 0, 0, 0, {
        &tlv_desc_report_type_0,
        &tlv_desc_downlink_data_report_0,
        &tlv_desc_usage_report_session_report_request_0,
        &tlv_desc_error_indication_report_0,
        &tlv_desc_load_control_informationp_0,
        &tlv_desc_overload_control_informationp_0,
    NULL,
}};

tlv_desc_t tlv_desc_pfcp_session_report_response =
{
    TLV_MESSAGE,
    "PFCP Session Report Response",
    0, 0, 0, 0, {
        &tlv_desc_pfcpcause_0,
        &tlv_desc_offending_ie_0,
        &tlv_desc_update_bar_pfcp_session_report_response_0,
        &tlv_desc_pfcpsrrsp_flags_0,
    NULL,
}};


status_t pfcp_parse_msg(pfcp_message_t *pfcp_message, pkbuf_t *pkbuf)
{
    status_t rv = CORE_ERROR;
    pfcp_header_t *h = NULL;
    c_uint16_t size = 0;

    d_assert(pfcp_message, return CORE_ERROR, "Null param");
    d_assert(pkbuf, return CORE_ERROR, "Null param");
    d_assert(pkbuf->payload, return CORE_ERROR, "Null param");

    d_trace(50, "[PFCP] RECV : ");
    d_trace_hex(50, pkbuf->payload, pkbuf->len);

    h = pkbuf->payload;
    d_assert(h, return CORE_ERROR, "Null param");
    
    memset(pfcp_message, 0, sizeof(pfcp_message_t));

    if (h->seid_p)
        size = PFCP_HEADER_LEN;
    else
        size = PFCP_HEADER_LEN-PFCP_SEID_LEN;

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK,
            return CORE_ERROR, "pkbuf_header error");
    memcpy(&pfcp_message->h, pkbuf->payload - size, size);

    if (h->seid_p)
        pfcp_message->h.seid = be64toh(pfcp_message->h.seid);
    else
    {
        pfcp_message->h.sqn  = pfcp_message->h.sqn_only;
        pfcp_message->h.sqn_only  = pfcp_message->h.sqn_only;
    }

    if (pkbuf->len == 0)
        return CORE_OK;

    switch(pfcp_message->h.type)
    {
        case PFCP_HEARTBEAT_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_heartbeat_request,
                    &tlv_desc_pfcp_heartbeat_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_HEARTBEAT_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_heartbeat_response,
                    &tlv_desc_pfcp_heartbeat_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_SETUP_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_association_setup_request,
                    &tlv_desc_pfcp_association_setup_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_SETUP_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_association_setup_response,
                    &tlv_desc_pfcp_association_setup_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_UPDATE_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_association_update_request,
                    &tlv_desc_pfcp_association_update_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_UPDATE_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_association_update_response,
                    &tlv_desc_pfcp_association_update_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_association_release_request,
                    &tlv_desc_pfcp_association_release_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_RELEASE_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_association_release_response,
                    &tlv_desc_pfcp_association_release_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_VERSION_NOT_SUPPORTED_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_version_not_supported_response,
                    &tlv_desc_pfcp_version_not_supported_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_NODE_REPORT_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_node_report_request,
                    &tlv_desc_pfcp_node_report_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_NODE_REPORT_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_node_report_response,
                    &tlv_desc_pfcp_node_report_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_SET_DELETION_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_set_deletion_request,
                    &tlv_desc_pfcp_session_set_deletion_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_SET_DELETION_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_set_deletion_response,
                    &tlv_desc_pfcp_session_set_deletion_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_establishment_request,
                    &tlv_desc_pfcp_session_establishment_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_ESTABLISHMENT_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_establishment_response,
                    &tlv_desc_pfcp_session_establishment_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_MODIFICATION_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_modification_request,
                    &tlv_desc_pfcp_session_modification_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_MODIFICATION_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_modification_response,
                    &tlv_desc_pfcp_session_modification_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_DELETION_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_deletion_request,
                    &tlv_desc_pfcp_session_deletion_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_DELETION_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_deletion_response,
                    &tlv_desc_pfcp_session_deletion_response, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_REPORT_REQUEST_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_report_request,
                    &tlv_desc_pfcp_session_report_request, pkbuf, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_REPORT_RESPONSE_TYPE:
            rv = tlv_parse_msg(&pfcp_message->pfcp_session_report_response,
                    &tlv_desc_pfcp_session_report_response, pkbuf, TLV_MODE_T2_L2);
            break;
        default:
            d_warn("Not implmeneted(type:%d)", pfcp_message->h.type);
            break;
    }

    return rv;
}

status_t pfcp_build_msg(pkbuf_t **pkbuf, pfcp_message_t *pfcp_message)
{
    status_t rv = CORE_ERROR;

    d_assert(pfcp_message, return rv, "Null param");
    switch(pfcp_message->h.type)
    {
        case PFCP_HEARTBEAT_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_heartbeat_request,
                    &pfcp_message->pfcp_heartbeat_request, TLV_MODE_T2_L2);
            break;
        case PFCP_HEARTBEAT_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_heartbeat_response,
                    &pfcp_message->pfcp_heartbeat_response, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_SETUP_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_association_setup_request,
                    &pfcp_message->pfcp_association_setup_request, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_SETUP_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_association_setup_response,
                    &pfcp_message->pfcp_association_setup_response, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_UPDATE_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_association_update_request,
                    &pfcp_message->pfcp_association_update_request, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_UPDATE_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_association_update_response,
                    &pfcp_message->pfcp_association_update_response, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_association_release_request,
                    &pfcp_message->pfcp_association_release_request, TLV_MODE_T2_L2);
            break;
        case PFCP_ASSOCIATION_RELEASE_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_association_release_response,
                    &pfcp_message->pfcp_association_release_response, TLV_MODE_T2_L2);
            break;
        case PFCP_VERSION_NOT_SUPPORTED_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_version_not_supported_response,
                    &pfcp_message->pfcp_version_not_supported_response, TLV_MODE_T2_L2);
            break;
        case PFCP_NODE_REPORT_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_node_report_request,
                    &pfcp_message->pfcp_node_report_request, TLV_MODE_T2_L2);
            break;
        case PFCP_NODE_REPORT_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_node_report_response,
                    &pfcp_message->pfcp_node_report_response, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_SET_DELETION_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_set_deletion_request,
                    &pfcp_message->pfcp_session_set_deletion_request, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_SET_DELETION_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_set_deletion_response,
                    &pfcp_message->pfcp_session_set_deletion_response, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_establishment_request,
                    &pfcp_message->pfcp_session_establishment_request, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_ESTABLISHMENT_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_establishment_response,
                    &pfcp_message->pfcp_session_establishment_response, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_MODIFICATION_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_modification_request,
                    &pfcp_message->pfcp_session_modification_request, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_MODIFICATION_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_modification_response,
                    &pfcp_message->pfcp_session_modification_response, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_DELETION_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_deletion_request,
                    &pfcp_message->pfcp_session_deletion_request, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_DELETION_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_deletion_response,
                    &pfcp_message->pfcp_session_deletion_response, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_REPORT_REQUEST_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_report_request,
                    &pfcp_message->pfcp_session_report_request, TLV_MODE_T2_L2);
            break;
        case PFCP_SESSION_REPORT_RESPONSE_TYPE:
            rv = tlv_build_msg(pkbuf, &tlv_desc_pfcp_session_report_response,
                    &pfcp_message->pfcp_session_report_response, TLV_MODE_T2_L2);
            break;
        default:
            d_warn("Not implmeneted(type:%d)", pfcp_message->h.type);
            break;
    }

    if ((*pkbuf) && (*pkbuf)->payload)
    {
        d_trace(50, "[PFCP] SEND : ");
        d_trace_hex(50, (*pkbuf)->payload, (*pkbuf)->len);
    }

    return rv;
}

