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
    smf_bearer_t *bearer = NULL;
    int len;
    
    rsp = &gtp_message.create_session_response;
    memset(&gtp_message, 0, sizeof(gtp_message_t));
    
    /* Set Cause */
    memset(&cause, 0, sizeof(cause));
    cause.value = GTP_CAUSE_REQUEST_ACCEPTED;
    rsp->cause.presence = 1;
    rsp->cause.len = sizeof(cause);
    rsp->cause.data = &cause;
    
    /* Send Control Plane(UL) : SGW-S11 */
    memset(&sgw_s11_teid, 0, sizeof(gtp_f_teid_t));
    sgw_s11_teid.interface_type = GTP_F_TEID_S11_S4_SGW_GTP_C;
    sgw_s11_teid.teid = htonl(sess->sgw_s11_teid);
    rv = gtp_sockaddr_to_f_teid(
            smf_self()->gtpc_addr, smf_self()->gtpc_addr6, &sgw_s11_teid, &len);
    d_assert(rv == CORE_OK, return rv,);
    rsp->sender_f_teid_for_control_plane.presence = 1;
    rsp->sender_f_teid_for_control_plane.data = &sgw_s11_teid;
    rsp->sender_f_teid_for_control_plane.len = len;
    
    bearer = smf_default_bearer_in_sess(sess);
    
    /* PDN Address Allocation */
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
    
    /* TODO : APN-AMBR
     * if PCRF changes APN-AMBR, this should be included. */

    /* Bearer EBI */
    rsp->bearer_contexts_created.presence = 1;
    rsp->bearer_contexts_created.eps_bearer_id.presence = 1;
    rsp->bearer_contexts_created.eps_bearer_id.u8 = bearer->ebi;

    /* TODO : Bearer QoS 
     * if PCRF changes Bearer QoS, this should be included. */

    gtp_message.h.type = GTP_CREATE_SESSION_REQUEST_TYPE;
    rv = gtp_build_msg(pkbuf, &gtp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "gtp build failed");
    
    return CORE_OK;
}
