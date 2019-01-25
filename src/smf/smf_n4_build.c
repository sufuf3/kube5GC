#define TRACE_MODULE _smf_n4_build

#include "core_debug.h"

#include "3gpp_types.h"
#include "pfcp/pfcp_types.h"
#include "pfcp/pfcp_conv.h"
#include "pfcp/pfcp_message.h"
#include "pfcp/pfcp_node.h"

#include "smf_context.h"
#include "smf_n4_build.h"

status_t smf_n4_build_heartbeat_request(
        pkbuf_t **pkbuf)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_heartbeat_request_t *rsp = NULL;
    c_uint32_t recovery_time_stamp = htonl(smf_self()->start_time);
    
    rsp = &pfcp_message.pfcp_heartbeat_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set Recovery Time Stamp, mandatory */
    rsp->recovery_time_stamp.presence = 1;
    rsp->recovery_time_stamp.data = &recovery_time_stamp;
    rsp->recovery_time_stamp.len = sizeof recovery_time_stamp;

    pfcp_message.h.type = PFCP_HEARTBEAT_REQUEST_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_heartbeat_response(
        pkbuf_t **pkbuf)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_heartbeat_response_t *rsp = NULL;
    c_uint32_t recovery_time_stamp = htonl(smf_self()->start_time);
    
    rsp = &pfcp_message.pfcp_heartbeat_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set Recovery Time Stamp, mandatory */
    rsp->recovery_time_stamp.presence = 1;
    rsp->recovery_time_stamp.data = &recovery_time_stamp;
    rsp->recovery_time_stamp.len = sizeof recovery_time_stamp;

    pfcp_message.h.type = PFCP_HEARTBEAT_RESPONSE_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_association_setup_request(
    pkbuf_t **pkbuf)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_setup_request_t *req = NULL;
    c_uint32_t recovery_time_stamp = htonl(smf_self()->start_time);
    c_uint8_t cp_function_features = 0;
    
    req = &pfcp_message.pfcp_association_setup_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    /* Set node id, mandatory */
    req->node_id.presence = 1;
    req->node_id.data = &smf_self()->pfcp_node_id;
    req->node_id.len = PFPC_NODE_ID_LEN(smf_self()->pfcp_node_id);
    
    /* Set Recovery Time Stamp, mandatory */
    req->recovery_time_stamp.presence = 1;
    req->recovery_time_stamp.data = &recovery_time_stamp;
    req->recovery_time_stamp.len = sizeof recovery_time_stamp;

    /* Set CP Function Features, conditional */
    req->cp_function_features.presence = 1;
    req->cp_function_features.data = &cp_function_features;
    req->cp_function_features.len = sizeof cp_function_features;
    
    pfcp_message.h.type = PFCP_ASSOCIATION_SETUP_REQUEST_TYPE;
    pfcp_message.h.seid_p = 0;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    
    return CORE_OK;
}

status_t smf_n4_build_association_update_request(
        pkbuf_t **pkbuf)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_update_request_t *req = NULL;

    req = &pfcp_message.pfcp_association_update_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    /* Set node id, mandatory */
    req->node_id.presence = 1;
    req->node_id.data = &smf_self()->pfcp_node_id;
    req->node_id.len = PFPC_NODE_ID_LEN(smf_self()->pfcp_node_id);

    /* Set CP Function Features, conditional */
    req->cp_function_features.presence = 1;
    req->cp_function_features.data = &smf_self()->cp_function_features;
    req->cp_function_features.len = sizeof smf_self()->cp_function_features;

    pfcp_message.h.type = PFCP_ASSOCIATION_UPDATE_REQUEST_TYPE;
    pfcp_message.h.seid_p = 0;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_association_update_response(
        pkbuf_t **pkbuf, c_uint8_t cause)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_update_response_t *rsp = NULL;

    rsp = &pfcp_message.pfcp_association_update_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set node id, mandatory */
    rsp->node_id.presence = 1;
    rsp->node_id.data = &smf_self()->pfcp_node_id;
    rsp->node_id.len = PFPC_NODE_ID_LEN(smf_self()->pfcp_node_id);

    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    rsp->cause.data = &cause;
    rsp->cause.len = sizeof cause;

    /* Set CP Function Features, conditional */
    rsp->cp_function_features.presence = 1;
    rsp->cp_function_features.data = &smf_self()->cp_function_features;
    rsp->cp_function_features.len = sizeof smf_self()->cp_function_features;

    pfcp_message.h.type = PFCP_ASSOCIATION_UPDATE_RESPONSE_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_association_release_request(
        pkbuf_t **pkbuf)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_release_request_t *req = NULL;

    req = &pfcp_message.pfcp_association_release_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set node id, mandatory */
    req->node_id.presence = 1;
    req->node_id.data = &smf_self()->pfcp_node_id;
    req->node_id.len = PFPC_NODE_ID_LEN(smf_self()->pfcp_node_id);

    pfcp_message.h.type = PFCP_ASSOCIATION_RELEASE_REQUEST_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_association_release_response(
        pkbuf_t **pkbuf, c_uint8_t cause)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_association_release_response_t *rsp = NULL;

    rsp = &pfcp_message.pfcp_association_release_response;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set node id, mandatory */
    rsp->node_id.presence = 1;
    rsp->node_id.data = &smf_self()->pfcp_node_id;
    rsp->node_id.len = PFPC_NODE_ID_LEN(smf_self()->pfcp_node_id);

    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    rsp->cause.data = &cause;
    rsp->cause.len = sizeof cause;

    pfcp_message.h.type = PFCP_ASSOCIATION_RELEASE_RESPONSE_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_session_establishment_request(
        pkbuf_t **pkbuf, smf_sess_t *sess)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_establishment_request_t *req = NULL;
    pfcp_f_seid_t smf_f_seid;
    
    c_uint8_t pdn_type;

    req = &pfcp_message.pfcp_session_establishment_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));

    /* Set node id, mandatory */
    req->node_id.presence = 1;
    req->node_id.data = &smf_self()->pfcp_node_id;
    req->node_id.len = PFPC_NODE_ID_LEN(smf_self()->pfcp_node_id);

    /* Set CP F-SEID, mandatory */
    if(sess->smf_n4_seid == 0) 
    {
        sess->smf_n4_seid = sess->index;
        pfcp_message.h.seid_p = 0;
        pfcp_message.h.seid = 0;
    }
    else
    {
        pfcp_message.h.seid_p = 1;
        pfcp_message.h.seid = sess->smf_n4_seid;
    }

    req->cp_f_seid.presence = 1;
    req->cp_f_seid.data = &smf_f_seid;
    smf_f_seid.seid = htobe64(sess->smf_n4_seid);
    rv = pfcp_sockaddr_to_f_seid(smf_self()->pfcp_addr, smf_self()->pfcp_addr6,
            &smf_f_seid, (c_int32_t *)&req->cp_f_seid.len);

    /* Set Create PDR, mandatory */
    smf_n4_build_create_pdr(&req->create_pdr, sess->ul_pdr, sess);
    smf_n4_build_create_pdr(&req->create_pdr1, sess->dl_pdr, sess);
    
    smf_n4_build_create_far(&req->create_far, sess->ul_pdr->far, sess);
    smf_n4_build_create_far(&req->create_far1, sess->dl_pdr->far, sess);
    
    /* Set Create URR */
    req->create_urr.presence = 0;
    
    /* Set Create QER */
    req->create_qer.presence = 0;
    
    /* Set PDN Type */
    req->pdn_type.presence = 1;
    req->pdn_type.data = &pdn_type;
    req->pdn_type.len = 1;
    if (sess->ipv4 && sess->ipv6)
        pdn_type = PFCP_PDN_TYPE_IPV4V6;
    else if (sess->ipv4)
        pdn_type = PFCP_PDN_TYPE_IPV4;
    else if (sess->ipv6)
        pdn_type = PFCP_PDN_TYPE_IPV6;
    else
        pdn_type = PFCP_PDN_TYPE_NONIP; // not support yet
    
    /* Set User Plane Inactivity Timer */
    req->user_plane_inactivity_timer.presence = 0;
    
    pfcp_message.h.type = PFCP_SESSION_ESTABLISHMENT_REQUEST_TYPE;
    
    rv = pfcp_build_msg(pkbuf, &pfcp_message);

    smf_n4_free_create_pdr(&req->create_pdr);
    smf_n4_free_create_pdr(&req->create_pdr1);

    smf_n4_free_create_far(&req->create_far);
    smf_n4_free_create_far(&req->create_far1);

    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_session_modification_request_for_setup_downlink(
        pkbuf_t **pkbuf, smf_sess_t *sess)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_modification_request_t *req = NULL;
    pfcp_f_seid_t smf_f_seid;
    uint32_t *far_id = NULL;

    far_id = malloc(sizeof(c_uint32_t));
    *far_id = htonl(sess->dl_pdr->far->far_id);

    req = &pfcp_message.pfcp_session_modification_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    /* Set CP F-SEID, mandatory */
    d_trace(-1, "smf_n4_seid : %lu", sess->smf_n4_seid);
    req->cp_f_seid.presence = 1;
    req->cp_f_seid.data = &smf_f_seid;
    smf_f_seid.seid = htobe64(sess->smf_n4_seid);
    rv = pfcp_sockaddr_to_f_seid(smf_self()->pfcp_addr, smf_self()->pfcp_addr6,
            &smf_f_seid, (c_int32_t *)&req->cp_f_seid.len);

    pfcp_outer_hdr_t* outer_hdr = NULL;

    req->update_far.presence = 1;
    req->update_far.far_id.presence = 1;
    req->update_far.far_id.data = far_id;
    req->update_far.far_id.len = sizeof(c_uint32_t);
    req->update_far.update_forwarding_parameters.presence = 1;
    req->update_far.update_forwarding_parameters.destination_interface.presence = 1;
    req->update_far.update_forwarding_parameters.destination_interface.len = 1;
    req->update_far.update_forwarding_parameters.destination_interface.data = &sess->dl_pdr->far->destination_interface;
    req->update_far.apply_action.presence = 1;
    req->update_far.apply_action.data = &sess->dl_pdr->far->apply_action;
    req->update_far.apply_action.len = sizeof(sess->dl_pdr->far->apply_action); 
    smf_bearer_t *bearer = NULL;

    bearer = smf_default_bearer_in_sess(sess);
    d_assert(bearer, return CORE_ERROR, "No Bearer Context");

    c_uint8_t len;
    req->update_far.update_forwarding_parameters.outer_header_creation.presence = 1;            
    len = 2 + 4;
    outer_hdr = malloc(sizeof(pfcp_outer_hdr_t));
    memset(outer_hdr, 0, sizeof(pfcp_outer_hdr_t));
    outer_hdr->gtpu_ipv6 = 0;
    outer_hdr->gtpu_ipv4 = 1;
    outer_hdr->addr = bearer->addr;
#ifndef FIVE_G_CORE
    outer_hdr->teid = htonl(bearer->enb_s1u_teid);
#else
    outer_hdr->teid = bearer->enb_s1u_teid;
#endif
    len += IPV4_LEN;
    req->update_far.update_forwarding_parameters.outer_header_creation.len = len;
    req->update_far.update_forwarding_parameters.outer_header_creation.data = outer_hdr;
    
    pfcp_message.h.type = PFCP_SESSION_MODIFICATION_REQUEST_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    free(far_id);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    
    return CORE_OK;
}

status_t smf_n4_build_session_modification_request_for_an_release(
        pkbuf_t **pkbuf, smf_sess_t *sess)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_modification_request_t *req = NULL;
    pfcp_f_seid_t smf_f_seid;
    uint32_t *far_id = NULL;

    far_id = malloc(sizeof(c_uint32_t));
    *far_id = htonl(sess->dl_pdr->far->far_id);

    req = &pfcp_message.pfcp_session_modification_request;
    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    /* Set CP F-SEID, mandatory */
    req->cp_f_seid.presence = 1;
    req->cp_f_seid.data = &smf_f_seid;
    smf_f_seid.seid = htobe64(sess->smf_n4_seid);
    rv = pfcp_sockaddr_to_f_seid(smf_self()->pfcp_addr, smf_self()->pfcp_addr6,
            &smf_f_seid, (c_int32_t *)&req->cp_f_seid.len);

    req->update_far.presence = 1;
    req->update_far.far_id.presence = 1;
    req->update_far.far_id.data = far_id;
    req->update_far.far_id.len = sizeof(c_uint32_t);

    smf_bearer_t *bearer = NULL;

    bearer = smf_default_bearer_in_sess(sess);
    d_assert(bearer, return CORE_ERROR, "No Bearer Context");

    req->update_far.apply_action.presence = 1;   
    req->update_far.apply_action.data = &sess->dl_pdr->far->apply_action;
    req->update_far.apply_action.len = sizeof(sess->dl_pdr->far->apply_action);
    
    pfcp_message.h.type = PFCP_SESSION_MODIFICATION_REQUEST_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    free(far_id);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    
    return CORE_OK;
}

status_t smf_n4_build_session_deletion_request(
        pkbuf_t **pkbuf, smf_sess_t *sess)
{
    status_t rv;
    pfcp_message_t pfcp_message;

    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    
    pfcp_message.h.type = PFCP_SESSION_DELETION_REQUEST_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");

    return CORE_OK;
}

status_t smf_n4_build_session_report_response(
        pkbuf_t **pkbuf, smf_sess_t *sess)
{
    status_t rv;
    pfcp_message_t pfcp_message;
    pfcp_session_report_response_t *rsp = NULL;
    c_uint8_t cause;

    memset(&pfcp_message, 0, sizeof(pfcp_message_t));
    rsp = &pfcp_message.pfcp_session_report_response;

    /* Set cause, mandatory */
    rsp->cause.presence = 1;
    cause = PFCP_CAUSE_SUCCESS;
    rsp->cause.data = &cause;
    rsp->cause.len = 1;
    
    pfcp_message.h.type = PFCP_SESSION_REPORT_RESPONSE_TYPE;
    rv = pfcp_build_msg(pkbuf, &pfcp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build failed");
    return CORE_OK;
}

status_t smf_n4_build_create_pdr(
        tlv_create_pdr_t *create_pdr, smf_pdr_t *pdr, smf_sess_t *sess)
{
    status_t rv;
    pfcp_f_teid_t *upf_f_teid;
    pfcp_ue_ip_addr_t *ue_ip = malloc(sizeof(pfcp_ue_ip_addr_t));
    smf_bearer_t *bearer = NULL;
    c_uint16_t *pdr_id = NULL;

    pdr_id = malloc(sizeof(c_uint16_t));
    *pdr_id = htons(pdr->pdr_id);
    
    bearer = smf_default_bearer_in_sess(sess);
    d_assert(bearer, return CORE_ERROR, "No Bearer Context!");

    /* Set Create PDR, mandatory */
    create_pdr->presence = 1;
    
    /* [Create PDR] Packet Detection Rule ID (PDR ID) */
    create_pdr->pdr_id.presence = 1;
    create_pdr->pdr_id.data = pdr_id;
    create_pdr->pdr_id.len = sizeof(c_uint16_t);
    
    /* [Create PDR] Precedence */
    create_pdr->precedence.presence = 1;
    create_pdr->precedence.len = 4;
    create_pdr->precedence.data = &pdr->precedence;
    
    /* [Create PDR] PDI, mandatory */
    tlv_pdi_t *pdi = &create_pdr->pdi;
    pdi->presence = 1;
    
    /* [PDI] Source Interface */
    pdi->source_interface.presence = 1;
    pdi->source_interface.len = 1; 
    pdi->source_interface.data = &pdr->source_interface;
    
    if (pdr->source_interface == PFCP_SRC_INTF_ACCESS)
    {
        int len;
        upf_f_teid = malloc(sizeof(pfcp_f_teid_t));
        memset(upf_f_teid, 0, sizeof(pfcp_f_teid_t));
        pdi->local_f_teid.presence = 1;    
        pdi->local_f_teid.data = upf_f_teid;
        upf_f_teid->teid = htonl(bearer->upf_s1u_teid);
        if (sess->upf_node->sa_list->next)
            rv = pfcp_sockaddr_to_f_teid(sess->upf_node->sa_list,
                    sess->upf_node->sa_list->next,
                    upf_f_teid, &len);
        else
            rv = pfcp_sockaddr_to_f_teid(sess->upf_node->sa_list, 
                    NULL, 
                    upf_f_teid, &len);
        d_assert(rv == CORE_OK, return CORE_ERROR, "pfcp build pdr failed");
        pdi->local_f_teid.len = len;
    }
    
    /* [PDI] Network instance: a Domain Name or an Access Point Name (APN) */
    pdi->network_instance.presence = 1;
    pdi->network_instance.len =  strlen(sess->pdn.apn);
    pdi->network_instance.data = sess->pdn.apn;
    
    create_pdr->pdi.ue_ip_address.presence = 1;
    create_pdr->pdi.ue_ip_address.data = ue_ip;
    memset(ue_ip, 0, sizeof(pfcp_ue_ip_addr_t));
    
    /* [Create PDR] UE IP Address */
    if (sess->ipv4 && sess->ipv6)
    {
        create_pdr->pdi.ue_ip_address.len = PFCP_UE_IP_ADDR_IPV4V6_LEN;
        ue_ip->both.addr = sess->ipv4->addr[0];
        memcpy(ue_ip->both.addr6, (c_uint8_t*)sess->ipv6->addr, IPV6_LEN);
        ue_ip->ipv4 = 1; ue_ip->ipv6 = 1;
    }
    else if (sess->ipv4)
    {
        create_pdr->pdi.ue_ip_address.len = PFCP_UE_IP_ADDR_IPV4_LEN;
        ue_ip->addr = sess->ipv4->addr[0];
        ue_ip->ipv4 = 1;
    }
    else if (sess->ipv6)
    {
        create_pdr->pdi.ue_ip_address.len = PFCP_UE_IP_ADDR_IPV6_LEN;
        memcpy(ue_ip->addr6, (c_uint8_t*)sess->ipv6->addr, IPV6_LEN);
        ue_ip->ipv6 = 1;
    }
    else
        d_assert(0, return CORE_ERROR, "No IP Pool");
    if (pdr->source_interface == PFCP_SRC_INTF_ACCESS)
        ue_ip->sd = PFCP_UE_IP_ADDR_SOURCE; // for UL
    else
        ue_ip->sd = PFCP_UE_IP_ADDR_DESITINATION; // for DL
    
    /* [Create PDR] Outer Header Removal */
    if (pdr->outer_header_removal != PFCP_OUTER_HDR_RMV_DESC_NULL)
    {
        create_pdr->outer_header_removal.presence = 1;
        create_pdr->outer_header_removal.len = 1;
        create_pdr->outer_header_removal.data = &pdr->outer_header_removal;
    }    
    
    /* [Create PDR] FAR ID */
    if (pdr->far)
    {
        c_uint32_t *far_id = NULL;
        far_id = malloc(sizeof(c_uint32_t));
        *far_id = htonl(pdr->far->far_id);

        create_pdr->far_id.presence = 1;
        create_pdr->far_id.data = far_id;
        create_pdr->far_id.len = sizeof(c_uint32_t);
    }
    
    /* [Create PDR] URR ID */
    if (pdr->urr)
    {
        create_pdr->urr_id.presence = 1;
        create_pdr->urr_id.data = &pdr->urr->urr_id;
        create_pdr->urr_id.len = 4;
    }
    
    /* [Create PDR] QER ID */
    if (pdr->qer)
    {
        create_pdr->qer_id.presence = 1;
        create_pdr->qer_id.data = &pdr->qer->qer_id;
        create_pdr->qer_id.len = 4;
    }
    
    return CORE_OK;
}

status_t smf_n4_free_create_pdr(tlv_create_pdr_t *create_pdr)
{
    if (create_pdr->presence && create_pdr->pdr_id.data)
        free(create_pdr->pdr_id.data);
    if (create_pdr->presence && create_pdr->pdi.local_f_teid.data)
        free(create_pdr->pdi.local_f_teid.data);
    if (create_pdr->presence && create_pdr->pdi.ue_ip_address.data)
        free(create_pdr->pdi.ue_ip_address.data);
    return CORE_OK;
}

status_t smf_n4_build_create_far(
        tlv_create_far_t *create_far, smf_far_t *far, smf_sess_t *sess)
{
    pfcp_outer_hdr_t* outer_hdr = NULL;
    c_uint32_t *far_id = NULL;

    far_id = malloc(sizeof(c_uint32_t));
    *far_id = htonl(far->far_id);

    /* Set Create FAR, mandatory */
    create_far->presence = 1;
    
    /* Create FAR: FAR ID */
    create_far->far_id.presence = 1;
    create_far->far_id.data = far_id;
    create_far->far_id.len = sizeof(c_uint32_t);
    
    /* Create FAR: Apply Action */
    create_far->apply_action.presence = 1;
    create_far->apply_action.len = 1;
    create_far->apply_action.data = &far->apply_action;
    if (far->apply_action & PFCP_FAR_APPLY_ACTION_FORW)
    {
        create_far->forwarding_parameters.presence = 1;
        create_far->forwarding_parameters.destination_interface.presence = 1;
        create_far->forwarding_parameters.destination_interface.len = 1;
        create_far->forwarding_parameters.destination_interface.data = &far->destination_interface;
        
        if (far->destination_interface == PFCP_FAR_DEST_INTF_ACCESS)
        {
            smf_bearer_t *bearer = NULL;

            bearer = smf_default_bearer_in_sess(sess);
            d_assert(bearer, return CORE_ERROR, "No Bearer Context");

            c_uint8_t len;
            create_far->forwarding_parameters.outer_header_creation.presence = 1;            
            len = 2 + 4;
            outer_hdr = malloc(sizeof(pfcp_outer_hdr_t));
            memset(outer_hdr, 0, sizeof(pfcp_outer_hdr_t));
            outer_hdr->gtpu_ipv6 = 0;
            outer_hdr->gtpu_ipv4 = 1;
            outer_hdr->addr = bearer->addr;
            outer_hdr->teid = htonl(bearer->enb_s1u_teid);
            len += IPV4_LEN;
            create_far->forwarding_parameters.outer_header_creation.len = len;
            create_far->forwarding_parameters.outer_header_creation.data = outer_hdr;
        }
    }
    
    return CORE_OK;
}

status_t smf_n4_free_create_far(tlv_create_far_t *create_far)
{
    if (create_far->far_id.presence && 
            create_far->far_id.data)
    {
        free(create_far->far_id.data);
    }
    if (create_far->forwarding_parameters.outer_header_creation.presence && 
            create_far->forwarding_parameters.outer_header_creation.data)
    {
        free(create_far->forwarding_parameters.outer_header_creation.data);
    }
    return CORE_OK;
}
