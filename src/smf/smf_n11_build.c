#define TRACE_MODULE _smf_n11_build
#include "gtp/gtp_conv.h"
#include "gtp/gtp_types.h"
#include "smf_n11_build.h"

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

    c_sockaddr_t *s1_u_enodeb_addr;
    gtp_f_teid_t s1_u_enodeb_f_teid;
    c_int32_t f_teid_len;

    bearer = smf_default_bearer_in_sess(sess);
    createSession.upf_s1u_teid = bearer->upf_s1u_teid;

    // PDN
    memcpy(&createSession.pdn, &sess->pdn, sizeof(sess->pdn));

    d_trace(-1, "PDN type: %d", sess->pdn.pdn_type);
    d_trace(-1, "PDN PAA PDn Type: %d", sess->pdn.paa.pdn_type);


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
    gtp_f_teid_to_ip(&s1_u_enodeb_f_teid, &createSession.upf_ip);

    JSONTRANSFORM_StToJs_create_session_response(&createSession, j_createSession);
    
    string = cJSON_Print(j_createSession);
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(j_createSession);

    return CORE_OK;
}

status_t smf_n11_build_update_session_response(
    pkbuf_t **pkbuf, smf_sess_t *sess)
{
    char *string = NULL;
    cJSON *j_updateSession = cJSON_CreateObject();
    modify_bearer_t modifyBearer = {0};
     c_uint32_t length = 0;
     // IMSI
    memcpy(modifyBearer.imsi, sess->imsi, sess->imsi_len);
    modifyBearer.imsi_len = sess->imsi_len;

    modifyBearer.sm_context_update_type = sess->sm_context_update_type;
    d_trace(-1, "smf_n11_build_update_session_response SM %d\n", modifyBearer.sm_context_update_type);
    d_trace(-1, "smf_n11_build_update_session_response SM addr %d\n", sess);

    JSONTRANSFORM_StToJs_update_session_response(&modifyBearer, j_updateSession);
    
    string = cJSON_Print(j_updateSession);
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(j_updateSession);

    return CORE_OK;
}

CORE_DECLARE(status_t) smf_n11_build_delete_session_response(
        pkbuf_t **pkbuf, smf_sess_t *sess)
{
    char *string = NULL;
    cJSON *j_deleteSession = cJSON_CreateObject();
    delete_session_t deleteSession = {0};
    c_uint32_t length = 0;
    // IMSI
    memcpy(deleteSession.imsi, sess->imsi, sess->imsi_len);
    deleteSession.imsi_len = sess->imsi_len;

    /* EBI */
    deleteSession.ebi = smf_default_bearer_in_sess(sess)->ebi;

    JSONTRANSFORM_StToJs_delete_session_response(&deleteSession, j_deleteSession);
    
    string = cJSON_Print(j_deleteSession);
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(j_deleteSession);
    return CORE_OK;
}

status_t smf_n11_build_update_session_downlink_data_notification_request(
    pkbuf_t **pkbuf, smf_sess_t *sess)
{
    char *string = NULL;
    cJSON *j_updateSession = cJSON_CreateObject();
    modify_bearer_t modifyBearer = {0};
     c_uint32_t length = 0;
     // IMSI
    memcpy(modifyBearer.imsi, sess->imsi, sess->imsi_len);
    modifyBearer.imsi_len = sess->imsi_len;

    modifyBearer.sm_context_update_type = sess->sm_context_update_type;
    d_trace(-1, "smf_n11_build_update_session_downlink_data_notification_request SM %d\n", modifyBearer.sm_context_update_type);
    d_trace(-1, "smf_n11_build_update_session_downlink_data_notification_request SM %d\n", sess);

    JSONTRANSFORM_StToJs_update_session_response(&modifyBearer, j_updateSession);
    
    string = cJSON_Print(j_updateSession);
    d_info(string);
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(j_updateSession);

    return CORE_OK;
}
