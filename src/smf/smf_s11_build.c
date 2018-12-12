#include "gtp/gtp_conv.h"
#include "smf_s11_build.h"

static c_int16_t smf_pco_build(c_uint8_t *pco_buf, tlv_pco_t *tlv_pco)
{
    status_t rv;
    pco_t ue, smf;
    pco_ipcp_t pco_ipcp;
    ipsubnet_t dns_primary, dns_secondary, dns6_primary, dns6_secondary;
    c_int8_t size = 0;
    int i = 0;

    d_assert(pco_buf, return -1, "Null param");
    d_assert(tlv_pco, return -1, "Null param");

    size = pco_parse(&ue, tlv_pco->data, tlv_pco->len);
    d_assert(size, return -1, "pco parse failed");

    memset(&smf, 0, sizeof(pco_t));
    smf.ext = ue.ext;
    smf.configuration_protocol = ue.configuration_protocol;

    for(i = 0; i < ue.num_of_id; i++)
    {
        c_uint8_t *data = ue.ids[i].data;
        switch(ue.ids[i].id)
        {
            case PCO_ID_CHALLENGE_HANDSHAKE_AUTHENTICATION_PROTOCOL:
            {
                if (data[0] == 2) /* Code : Response */
                {
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = 4;
                    smf.ids[smf.num_of_id].data =
                        (c_uint8_t *)"\x03\x00\x00\x04"; /* Code : Success */
                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_INTERNET_PROTOCOL_CONTROL_PROTOCOL:
            {
                if (data[0] == 1) /* Code : Configuration Request */
                {
                    c_uint16_t len = 16;

                    memset(&pco_ipcp, 0, sizeof(pco_ipcp_t));
                    pco_ipcp.code = 2; /* Code : Configuration Ack */
                    pco_ipcp.len = htons(len);

                    /* Primary DNS Server IP Address */
                    if (smf_self()->dns[0])
                    {
                        rv = core_ipsubnet(
                                &dns_primary, smf_self()->dns[0], NULL);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                        pco_ipcp.options[0].type = 129; 
                        pco_ipcp.options[0].len = 6; 
                        pco_ipcp.options[0].addr = dns_primary.sub[0];
                    }

                    /* Secondary DNS Server IP Address */
                    if (smf_self()->dns[1])
                    {
                        rv = core_ipsubnet(
                                &dns_secondary, smf_self()->dns[1], NULL);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                        pco_ipcp.options[1].type = 131; 
                        pco_ipcp.options[1].len = 6; 
                        pco_ipcp.options[1].addr = dns_secondary.sub[0];
                    }

                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = len;
                    smf.ids[smf.num_of_id].data = (c_uint8_t *)&pco_ipcp;

                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_DNS_SERVER_IPV4_ADDRESS_REQUEST:
            {
                if (smf_self()->dns[0])
                {
                    rv = core_ipsubnet(
                            &dns_primary, smf_self()->dns[0], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV4_LEN;
                    smf.ids[smf.num_of_id].data = dns_primary.sub;
                    smf.num_of_id++;
                }

                if (smf_self()->dns[1])
                {
                    rv = core_ipsubnet(
                            &dns_secondary, smf_self()->dns[1], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV4_LEN;
                    smf.ids[smf.num_of_id].data = dns_secondary.sub;
                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_DNS_SERVER_IPV6_ADDRESS_REQUEST:
            {
                if (smf_self()->dns6[0])
                {
                    rv = core_ipsubnet(
                            &dns6_primary, smf_self()->dns6[0], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV6_LEN;
                    smf.ids[smf.num_of_id].data = dns6_primary.sub;
                    smf.num_of_id++;
                }

                if (smf_self()->dns6[1])
                {
                    rv = core_ipsubnet(
                            &dns6_secondary, smf_self()->dns6[1], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV6_LEN;
                    smf.ids[smf.num_of_id].data = dns6_secondary.sub;
                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_P_CSCF_IPV4_ADDRESS_REQUEST:
            {
                /* TODO */
            }
            case PCO_ID_P_CSCF_IPV6_ADDRESS_REQUEST:
            {
                /* TODO */
                break;
            }
            case PCO_ID_IP_ADDRESS_ALLOCATION_VIA_NAS_SIGNALLING:
                /* TODO */
                break;
            case PCO_ID_IPV4_LINK_MTU_REQUEST:
                /* TODO */
                break;
            default:
                d_warn("Unknown PCO ID:(0x%x)", ue.ids[i].id);
        }
    }

    size = pco_build(pco_buf, MAX_PCO_LEN, &smf);
    return size;
}

static c_int16_t smf_sbi_pco_build(c_uint8_t *pco_buf, c_uint8_t *pco_ori, c_uint32_t pco_ori_len)
{
    status_t rv;
    pco_t ue, smf;
    pco_ipcp_t pco_ipcp;
    ipsubnet_t dns_primary, dns_secondary, dns6_primary, dns6_secondary;
    c_int8_t size = 0;
    int i = 0;

    d_assert(pco_buf, return -1, "Null param");
    d_assert(pco_ori, return -1, "Null param");

    size = pco_parse(&ue, pco_ori, pco_ori_len);
    d_assert(size, return -1, "pco parse failed");

    memset(&smf, 0, sizeof(pco_t));
    smf.ext = ue.ext;
    smf.configuration_protocol = ue.configuration_protocol;

    for(i = 0; i < ue.num_of_id; i++)
    {
        c_uint8_t *data = ue.ids[i].data;
        switch(ue.ids[i].id)
        {
            case PCO_ID_CHALLENGE_HANDSHAKE_AUTHENTICATION_PROTOCOL:
            {
                if (data[0] == 2) /* Code : Response */
                {
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = 4;
                    smf.ids[smf.num_of_id].data =
                        (c_uint8_t *)"\x03\x00\x00\x04"; /* Code : Success */
                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_INTERNET_PROTOCOL_CONTROL_PROTOCOL:
            {
                if (data[0] == 1) /* Code : Configuration Request */
                {
                    c_uint16_t len = 16;

                    memset(&pco_ipcp, 0, sizeof(pco_ipcp_t));
                    pco_ipcp.code = 2; /* Code : Configuration Ack */
                    pco_ipcp.len = htons(len);

                    /* Primary DNS Server IP Address */
                    if (smf_self()->dns[0])
                    {
                        rv = core_ipsubnet(
                                &dns_primary, smf_self()->dns[0], NULL);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                        pco_ipcp.options[0].type = 129; 
                        pco_ipcp.options[0].len = 6; 
                        pco_ipcp.options[0].addr = dns_primary.sub[0];
                    }

                    /* Secondary DNS Server IP Address */
                    if (smf_self()->dns[1])
                    {
                        rv = core_ipsubnet(
                                &dns_secondary, smf_self()->dns[1], NULL);
                        d_assert(rv == CORE_OK, return CORE_ERROR,);
                        pco_ipcp.options[1].type = 131; 
                        pco_ipcp.options[1].len = 6; 
                        pco_ipcp.options[1].addr = dns_secondary.sub[0];
                    }

                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = len;
                    smf.ids[smf.num_of_id].data = (c_uint8_t *)&pco_ipcp;

                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_DNS_SERVER_IPV4_ADDRESS_REQUEST:
            {
                if (smf_self()->dns[0])
                {
                    rv = core_ipsubnet(
                            &dns_primary, smf_self()->dns[0], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV4_LEN;
                    smf.ids[smf.num_of_id].data = dns_primary.sub;
                    smf.num_of_id++;
                }

                if (smf_self()->dns[1])
                {
                    rv = core_ipsubnet(
                            &dns_secondary, smf_self()->dns[1], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV4_LEN;
                    smf.ids[smf.num_of_id].data = dns_secondary.sub;
                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_DNS_SERVER_IPV6_ADDRESS_REQUEST:
            {
                if (smf_self()->dns6[0])
                {
                    rv = core_ipsubnet(
                            &dns6_primary, smf_self()->dns6[0], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV6_LEN;
                    smf.ids[smf.num_of_id].data = dns6_primary.sub;
                    smf.num_of_id++;
                }

                if (smf_self()->dns6[1])
                {
                    rv = core_ipsubnet(
                            &dns6_secondary, smf_self()->dns6[1], NULL);
                    d_assert(rv == CORE_OK, return CORE_ERROR,);
                    smf.ids[smf.num_of_id].id = ue.ids[i].id;
                    smf.ids[smf.num_of_id].len = IPV6_LEN;
                    smf.ids[smf.num_of_id].data = dns6_secondary.sub;
                    smf.num_of_id++;
                }
                break;
            }
            case PCO_ID_P_CSCF_IPV4_ADDRESS_REQUEST:
            {
                /* TODO */
            }
            case PCO_ID_P_CSCF_IPV6_ADDRESS_REQUEST:
            {
                /* TODO */
                break;
            }
            case PCO_ID_IP_ADDRESS_ALLOCATION_VIA_NAS_SIGNALLING:
                /* TODO */
                break;
            case PCO_ID_IPV4_LINK_MTU_REQUEST:
                /* TODO */
                break;
            default:
                d_warn("Unknown PCO ID:(0x%x)", ue.ids[i].id);
        }
    }

    size = pco_build(pco_buf, MAX_PCO_LEN, &smf);
    return size;
}

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
    gtp_message_t *message = smf_default_bearer_in_sess(sess)->gtp_pkbuf->payload;
    gtp_create_session_request_t *req = &message->create_session_request;

    c_uint8_t pco_buf[MAX_PCO_LEN];
    c_int16_t pco_len;
    
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

    pco_len = smf_pco_build(pco_buf, &req->protocol_configuration_options);
    d_assert(pco_len > 0, return CORE_ERROR, "pco build failed");
    rsp->protocol_configuration_options.presence = 1;
    rsp->protocol_configuration_options.data = pco_buf;
    rsp->protocol_configuration_options.len = pco_len;

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
    s1_u_enodeb_f_teid.teid = htonl(bearer->sgw_s1u_teid);
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

status_t smf_s11_build_modify_bearer_response(
        pkbuf_t **pkbuf, smf_sess_t *sess)
{
    status_t rv;
    gtp_message_t gtp_message;
    gtp_modify_bearer_response_t *rsp = NULL;
    gtp_cause_t cause;

    rsp = &gtp_message.modify_bearer_response;
    memset(&gtp_message, 0, sizeof(gtp_message_t));
    
    /* Set Cause */
    memset(&cause, 0, sizeof(cause));
    cause.value = GTP_CAUSE_REQUEST_ACCEPTED;
    rsp->cause.presence = 1;
    rsp->cause.len = sizeof(cause);
    rsp->cause.data = &cause;

    gtp_message.h.type = GTP_MODIFY_BEARER_RESPONSE_TYPE;
    rv = gtp_build_msg(pkbuf, &gtp_message);
    d_assert(rv == CORE_OK, return CORE_ERROR, "gtp build failed");
    return CORE_OK;
}

status_t smf_n11_build_create_session_response(
    pkbuf_t **pkbuf, smf_sess_t *sess)
{
    char *string = NULL;
    cJSON *j_createSession = cJSON_CreateObject();
    create_session_t createSession = {0};
    smf_bearer_t *bearer = NULL;
    c_uint32_t length = 0;
    c_uint16_t pco_len;
    c_uint8_t pco_buf[MAX_PCO_LEN] = {0};
    // gtp_message_t *message = smf_default_bearer_in_sess(sess)->gtp_pkbuf->payload;

    c_sockaddr_t *s1_u_enodeb_addr;
    gtp_f_teid_t s1_u_enodeb_f_teid;
    c_int32_t f_teid_len;

    // Send Control Plane(UL) : SGW-S11
    bearer = smf_default_bearer_in_sess(sess);
    createSession.sgw_s1u_teid = bearer->sgw_s1u_teid;

    memcpy(&createSession.pdn, &sess->pdn, sizeof(sess->pdn));

    // IMSI
    memcpy(createSession.imsi, sess->imsi, sess->imsi_len);
    createSession.imsi_len = sess->imsi_len;

    // PCO
    pco_len = smf_sbi_pco_build(pco_buf, (c_uint8_t *)&sess->pco_buf, sess->pco_len);
    d_assert(pco_len > 0, return CORE_ERROR, "pco build failed");
    memcpy(createSession.ue_pco.buffer, pco_buf, pco_len);
    createSession.ue_pco.length = pco_len;

    /* APN Restriction */
    memcpy(createSession.apn, sess->pdn.apn, MAX_APN_LEN);

    /* Bearer EBI */
    createSession.ebi = bearer->ebi;

    /* S1U TEID */
    s1_u_enodeb_addr = sess->upf_node->sa_list;
    gtp_sockaddr_to_f_teid(
            s1_u_enodeb_addr, NULL, &s1_u_enodeb_f_teid, 
            &f_teid_len);
    gtp_f_teid_to_ip(&s1_u_enodeb_f_teid, &createSession.sgw_ip);

    JSONTRANSFORM_StToJs_create_session_response(&createSession, j_createSession);
    
    string = cJSON_Print(j_createSession);
    d_info(string);
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(j_createSession);

    return CORE_OK;
}
