#include "gtp/gtp_conv.h"
#include "smf_s11_build.h"

status_t smf_s11_build_create_session_response(
    pkbuf_t **pkbuf, smf_sess_t *sess)
{
    status_t rv;
    gtp_message_t gtp_message;
    gtp_create_session_response_t *rsp = NULL;
    gtp_cause_t cause;
    gtp_f_teid_t sgw_s11_teid;
    gtp_f_teid_t s1_u_enodeb_f_teid;
    c_sockaddr_t *s1_u_enodeb_addr = NULL;
    smf_bearer_t *bearer = NULL;
    
    rsp = &gtp_message.create_session_response;
    memset(&gtp_message, 0, sizeof(gtp_message_t));
    
    /* Set Cause */
    memset(&cause, 0, sizeof(cause));
    cause.value = GTP_CAUSE_REQUEST_ACCEPTED;
    rsp->cause.presence = 1;
    rsp->cause.len = sizeof(cause);
    rsp->cause.data = &cause;
    
    // Send Control Plane(UL) : SGW-S11
    memset(&sgw_s11_teid, 0, sizeof(gtp_f_teid_t));
    sgw_s11_teid.interface_type = GTP_F_TEID_S11_S4_SGW_GTP_C;
    sgw_s11_teid.teid = htonl(sess->sgw_s11_teid);
    rv = gtp_sockaddr_to_f_teid(
            smf_self()->gtpc_addr, smf_self()->gtpc_addr6, 
            &sgw_s11_teid, 
            (int*)&rsp->sender_f_teid_for_control_plane.len);
    d_assert(rv == CORE_OK, return rv,);
    rsp->sender_f_teid_for_control_plane.presence = 1;
    rsp->sender_f_teid_for_control_plane.data = &sgw_s11_teid;

    rsp->pdn_address_allocation.data = &sess->pdn.paa;
    if (sess->ipv4 && sess->ipv6)
        rsp->pdn_address_allocation.len = PAA_IPV4V6_LEN;
    else if (sess->ipv4)
        rsp->pdn_address_allocation.len = PAA_IPV4_LEN;
    else if (sess->ipv6)
        rsp->pdn_address_allocation.len = PAA_IPV6_LEN;
    else
        d_assert(0, return CORE_ERROR, "No IP Pool");
    rsp->pdn_address_allocation.presence = 1;

    /* APN Restriction */
    rsp->apn_restriction.presence = 1;
    rsp->apn_restriction.u8 = GTP_APN_NO_RESTRICTION;

    /* Bearer EBI */
    bearer = smf_default_bearer_in_sess(sess);
    rsp->bearer_contexts_created.presence = 1;
    rsp->bearer_contexts_created.eps_bearer_id.presence = 1;
    rsp->bearer_contexts_created.eps_bearer_id.u8 = bearer->ebi;

    memset(&s1_u_enodeb_f_teid, 0, sizeof(gtp_f_teid_t));
    s1_u_enodeb_f_teid.interface_type = GTP_F_TEID_S1_U_SGW_GTP_U;
    s1_u_enodeb_addr = sess->upf_node->sa_list;
    rv = gtp_sockaddr_to_f_teid(
            s1_u_enodeb_addr, NULL, &s1_u_enodeb_f_teid, 
            (c_int32_t *)&rsp->bearer_contexts_created.s1_u_enodeb_f_teid.len);

    rsp->bearer_contexts_created.s1_u_enodeb_f_teid.presence = 1;
    rsp->bearer_contexts_created.s1_u_enodeb_f_teid.data = &s1_u_enodeb_f_teid;

    gtp_message.h.type = GTP_CREATE_SESSION_RESPONSE_TYPE;
    rv = gtp_build_msg(pkbuf, &gtp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "gtp build failed");
    
    return CORE_OK;
}
