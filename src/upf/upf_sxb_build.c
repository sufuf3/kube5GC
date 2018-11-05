#define TRACE_MODULE _upf_sxb_build

#include "core_debug.h"

#include "3gpp_types.h"
#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_conv.h"
#include "pfcp/pfcp_message.h"
#include "pfcp/pfcp_node.h"
#include "gtp/gtp_message.h"

#include "ipfw/ipfw2.h"

#include "upf_context.h"


status_t upf_sxb_build_session_establishment_response(
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
    upf_seid.seid = sess->upf_seid;
    rv =  pfcp_sockaddr_to_f_seid(upf_self()->gtpu_addr, upf_self()->gtpu_addr6, &upf_seid, &len);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build session establishment response failed"); 
    rsp->up_f_seid.len = len;

    /* Set UPF-U FQ-CSID */

    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t upf_sxb_build_session_modification_response(
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

status_t upf_sxb_build_session_deletion_response(
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

status_t upf_sxb_build_association_setup_response(
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
        //$ if up_fun all zero, should not present
        rsp->up_function_features.presence = 0;
    }
    
    pfcp_message.h.type = type;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    
    return CORE_OK;
}

status_t upf_sxb_build_association_release_response(
        pkbuf_t **pkbuf, c_uint8_t type)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_release_response_t *rsp = NULL;
    pfcp_node_id_t node_id;
    c_uint8_t cause;
//#include<sys/time.h>    
    
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

status_t upf_sxb_build_heartbeat_response(
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
