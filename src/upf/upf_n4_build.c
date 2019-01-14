#define TRACE_MODULE _upf_n4_build

#include "core_debug.h"

#include "3gpp_types.h"
#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_conv.h"
#include "pfcp/pfcp_message.h"
#include "pfcp/pfcp_node.h"
#include "gtp/gtp_message.h"

#include "ipfw/ipfw2.h"

#include "upf_context.h"

status_t upf_n4_build_session_establishment_response(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess,
        pfcp_session_establishment_request_t *pfcp_req)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_establishment_response_t *rsp = NULL;
    c_uint8_t cause;
    pfcp_f_seid_t upf_seid;    
    pfcp_node_id_t node_id;
    int len;

    rsp = &pfcp_message.pfcp_session_establishment_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set node id, mandatory */
    rsp->node_id.presence = 1;
    node_id.type = PFCP_NODE_ID_IPV4;
    node_id.addr = upf_self()->gtpu_addr->sin.sin_addr.s_addr;
    rsp->node_id.data = &node_id;
    rsp->node_id.len = 1+4;

    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    cause = PFCP_CAUSE_SUCCESS;
    rsp->cause.data = &cause;
    rsp->cause.len = 1;
    
    /* Set UP F-SEID, mandatory */
    rsp->up_f_seid.presence = 1;
    rsp->up_f_seid.data = &upf_seid;
    upf_seid.seid = htobe64(sess->upf_seid);
    rv =  pfcp_sockaddr_to_f_seid(upf_self()->gtpu_addr, upf_self()->gtpu_addr6, &upf_seid, &len);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build session establishment response failed"); 
    rsp->up_f_seid.len = len;

    /* Set UPF-U FQ-CSID */

    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t upf_n4_build_session_modification_response(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess,
        pfcp_session_modification_request_t *pfcp_req)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_modification_response_t *rsp = NULL;
    c_uint8_t cause;

    rsp = &pfcp_message.pfcp_session_modification_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    cause = PFCP_CAUSE_SUCCESS;
    rsp->cause.data = &cause;
    rsp->cause.len = 1;

    /* Set Offending IE, Created PDR, Load Control Information, Overload Control Information, Usage Report, Failed Rule ID, Additional Usage Reports Information, Created/Updated Traffic Endpoint */

    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t upf_n4_build_session_deletion_response(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess,
        pfcp_session_deletion_request_t *pfcp_req)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_deletion_response_t *rsp = NULL;
    c_uint8_t cause;
    //pfcp_f_seid_t upf_seid;    
    //pfcp_node_id_t node_id;
    //int len;

    rsp = &pfcp_message.pfcp_session_deletion_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    cause = PFCP_CAUSE_SUCCESS;
    rsp->cause.data = &cause;
    rsp->cause.len = 1;

    /* Set Offending IE, Load Control Information, Overload Control Information, Usage Report */

    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t upf_n4_build_session_report_request_downlink_data_report(
        pkbuf_t **pkbuf, c_uint8_t type, upf_sess_t *sess, c_int16_t pdr_id,
        pfcp_session_report_request_t *pfcp_req)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_report_request_t *req = NULL;
    pfcp_report_type_t report_type;
    pfcp_downlink_data_service_information_t downlink_data_service_information_data;

    req = &pfcp_message.pfcp_session_report_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    memset(&report_type, 0, sizeof(pfcp_report_type_t));
    memset(&downlink_data_service_information_data, 0, sizeof(pfcp_downlink_data_service_information_t));

    report_type.DLDR = 1;

    req->report_type.presence = 1;
    req->report_type.data = &report_type;
    req->report_type.len = sizeof(pfcp_report_type_t);

    tlv_downlink_data_report_t *downlink_data_report = &req->downlink_data_report;
    downlink_data_report->presence = 1;

    downlink_data_report->pdr_id.presence = 1;
    downlink_data_report->pdr_id.data = &pdr_id;
    downlink_data_report->pdr_id.len = sizeof(pdr_id);

    tlv_downlink_data_service_information_t *downlink_data_service_information = 
            &downlink_data_report->downlink_data_service_information;

    downlink_data_service_information_data.PPI = 0;
    downlink_data_service_information_data.QFII = 0;
    downlink_data_service_information->presence = 1;
    downlink_data_service_information->data = &downlink_data_service_information_data;
    downlink_data_service_information->len = 
        PFCP_DOWNLINK_DATA_SERVICE_INFORMATION_LEN(downlink_data_service_information_data);

    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t upf_n4_build_association_setup_response(
        pkbuf_t **pkbuf, c_uint8_t type)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_setup_response_t *rsp = NULL;
    pfcp_node_id_t node_id;
    c_uint8_t cause;
    c_uint16_t up_fun;
    //struct  timeval  tv;
//#include<sys/time.h>    
    
    rsp = &pfcp_message.pfcp_association_setup_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    /* Set node id, mandatory */
    rsp->node_id.presence = 1;
    node_id.type = PFCP_NODE_ID_IPV4;
    node_id.addr = upf_self()->gtpu_addr->sin.sin_addr.s_addr;
    rsp->node_id.data = &node_id;
    rsp->node_id.len = 1+4;
    
    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    cause = PFCP_CAUSE_SUCCESS;
    rsp->cause.data = &cause;
    rsp->cause.len = 1;
    
    /* Set Recovery Time Stamp, mandatory */
    //gettimeofday(&tv,NULL);
    //tv.tv_sec = htonl(tv.tv_sec);
    rsp->recovery_time_stamp.presence = 1;
    //rsp->recovery_time_stamp.data = &tv.tv_sec;
    rsp->recovery_time_stamp.data = &upf_self()->recovery_time;
    rsp->recovery_time_stamp.len = 4;        
    
    /* Set UP Function Features */
    up_fun = 0; 
    if (up_fun)
    {
        rsp->up_function_features.presence = 1;
        rsp->up_function_features.data = &up_fun;
        rsp->up_function_features.len = 2;
    }
    else
    {
        // If up_fun all zero, should not present
        rsp->up_function_features.presence = 0;
    }
    
    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    
    return CORE_OK;
}

status_t upf_n4_build_association_release_response(
        pkbuf_t **pkbuf, c_uint8_t type)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_release_response_t *rsp = NULL;
    pfcp_node_id_t node_id;
    c_uint8_t cause;
    
    rsp = &pfcp_message.pfcp_association_release_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    /* Set node id, mandatory */
    rsp->node_id.presence = 1;
    node_id.type = PFCP_NODE_ID_IPV4;
    node_id.addr = upf_self()->gtpu_addr->sin.sin_addr.s_addr;
    rsp->node_id.data = &node_id;
    rsp->node_id.len = 1+4;
    
    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    cause = PFCP_CAUSE_SUCCESS;
    rsp->cause.data = &cause;
    rsp->cause.len = 1;
    
    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    
    return CORE_OK;
}

status_t upf_n4_build_heartbeat_response(
        pkbuf_t **pkbuf, c_uint8_t type)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_heartbeat_response_t *rsp = NULL;    
    
    rsp = &pfcp_message.pfcp_heartbeat_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    /* Set Recovery Time Stamp, mandatory */
    rsp->recovery_time_stamp.presence = 1;
    rsp->recovery_time_stamp.data = &upf_self()->recovery_time;
    rsp->recovery_time_stamp.len = 4;
    
    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    
    return CORE_OK;
}
