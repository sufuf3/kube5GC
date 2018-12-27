#define TRACE_MODULE _amf_json_build
#include <cJSON/cJSON.h>

#include "core_debug.h"
#include "core_pkbuf.h"
#include "3gpp_types.h"
#include "amf4g_context.h"
#include "sbiJson/commonJsonKey.h"
#include "sbiJson/JsonTransform.h"
#include "amf_json_build.h"

status_t amf_json_build_create_session(pkbuf_t **pkbuf, amf4g_sess_t *sess) {
    
    pdn_t *pdn = NULL;
    amf4g_ue_t *amf4g_ue = NULL;
    amf4g_bearer_t *bearer = NULL;
    char *string = NULL;
    cJSON *session = cJSON_CreateObject();
    create_session_t createSession = {0};
    c_uint32_t length = 0;
    d_assert(sess, return CORE_ERROR, "Null param");
    pdn = sess->pdn;
    d_assert(pdn, return CORE_ERROR, "Null param");
    bearer = amf4g_default_bearer_in_sess(sess);
    d_assert(bearer, return CORE_ERROR, "Null param");
    amf4g_ue = sess->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");

    d_trace(10, "%s %d Enter", __FUNCTION__, __LINE__);

    memcpy(createSession.imsi,  amf4g_ue->imsi, amf4g_ue->imsi_len);
    createSession.imsi_len = amf4g_ue->imsi_len;
    memcpy(&createSession.tai, &amf4g_ue->tai, sizeof( amf4g_ue->tai));
    memcpy(&createSession.e_cgi, &amf4g_ue->e_cgi, sizeof( amf4g_ue->e_cgi));
    memcpy(&createSession.visited_plmn_id, &amf4g_ue->visited_plmn_id, sizeof( amf4g_ue->visited_plmn_id));
    if (sess->ue_pco.length && sess->ue_pco.buffer) {
        createSession.ue_pco.length = sess->ue_pco.length;
        memcpy(createSession.ue_pco.buffer, sess->ue_pco.buffer, sess->ue_pco.length);
    }
    memcpy(createSession.apn, pdn->apn, sizeof(pdn->apn));

    c_uint8_t pdn_type;
    d_trace(15, "%s %d Origin PDN Type: %d\n", __FUNCTION__, __LINE__, pdn->pdn_type);
    if (pdn->pdn_type == HSS_PDN_TYPE_IPV4 ||
        pdn->pdn_type == HSS_PDN_TYPE_IPV6 ||
        pdn->pdn_type == HSS_PDN_TYPE_IPV4V6)
    {
        d_trace(15, "%s %d PDNNNNNNNNNNNNNN %d, %d\n", __FUNCTION__, __LINE__, pdn->pdn_type + 1, sess->request_type.pdn_type);
        pdn_type = ((pdn->pdn_type + 1) & sess->request_type.pdn_type);
        d_assert(pdn_type != 0, return CORE_ERROR,
                "PDN Configuration Error:(%d, %d)",
                pdn->pdn_type, sess->request_type.pdn_type);
    }
    else if (pdn->pdn_type == HSS_PDN_TYPE_IPV4_OR_IPV6)
    {
        pdn_type = sess->request_type.pdn_type;
    }
    else
        d_assert(0, return CORE_ERROR,
                "HSS PDN Confiugration Error(%d)", pdn->pdn_type);
    memcpy(&createSession.pdn, pdn, sizeof(pdn_t));
    
    createSession.pdn.paa.pdn_type = pdn_type;
    d_trace(15, "%s %d  createSession.pdn.paa.pdn_type :%d\n", __FUNCTION__, __LINE__, createSession.pdn.paa.pdn_type);
    createSession.ebi = bearer->ebi;
    /*TODO : ue timezone*/
    memcpy(&createSession.guti, &amf4g_ue->guti, sizeof(amf4g_ue->guti));
    
    JSONTRANSFORM_StToJs_create_session_request(&createSession, session);

    string = cJSON_Print(session);

    length = strlen(string) + 1;
    
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(session);

    d_trace(10, "%s %d Leave", __FUNCTION__, __LINE__);

    return CORE_OK;
}

CORE_DECLARE(status_t) amf_json_build_modify_bearer(pkbuf_t **pkbuf, amf4g_bearer_t *bearer, c_uint8_t sm_context_update_type)
{
    char *string = NULL;
    cJSON *j_modifyBearerReq = cJSON_CreateObject();
    modify_bearer_t modifyBearer = {0};
    c_uint32_t length = 0;

    d_assert(bearer, return CORE_ERROR, "Null param");
    
    d_trace(10, "%s %d Enter \n", __FUNCTION__, __LINE__);
    
    memcpy(modifyBearer.imsi, bearer->amf4g_ue->imsi, bearer->amf4g_ue->imsi_len);
    modifyBearer.imsi_len = bearer->amf4g_ue->imsi_len; 
    memcpy(&modifyBearer.uli_presence, "\x01" , sizeof(c_uint8_t));
    memcpy(&modifyBearer.tai, &bearer->amf4g_ue->tai, sizeof(bearer->amf4g_ue->tai));
    memcpy(&modifyBearer.e_cgi, &bearer->amf4g_ue->e_cgi, sizeof(bearer->amf4g_ue->e_cgi));
    memcpy(&modifyBearer.visited_plmn_id, &bearer->amf4g_ue->visited_plmn_id, sizeof( bearer->amf4g_ue->visited_plmn_id));;
    memcpy(&modifyBearer.guti, &bearer->amf4g_ue->guti, sizeof(bearer->amf4g_ue->guti));
    //TODO: fix it how to get rat type??
    //memcpy(&modifyBearer.rat_type, 0x00, sizeof(modifyBearer.rat_type));
    memcpy(&modifyBearer.ebi, &bearer->ebi, sizeof(bearer->ebi));
    memcpy(&modifyBearer.enb_s1u_ip, &bearer->enb_s1u_ip, sizeof(bearer->enb_s1u_ip));
    memcpy(&modifyBearer.enb_s1u_teid, &bearer->enb_s1u_teid, sizeof(bearer->enb_s1u_teid));
    memcpy(&modifyBearer.apn, &bearer->sess->pdn->apn, sizeof( bearer->sess->pdn->apn));
    modifyBearer.sm_context_update_type = sm_context_update_type;
    
    JSONTRANSFORM_StToJs_modify_bearer_request(&modifyBearer, j_modifyBearerReq);
    string = cJSON_Print(j_modifyBearerReq);
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(j_modifyBearerReq);

    d_trace(10, "%s %d Leave \n", __FUNCTION__, __LINE__);
    return 0;
}

status_t amf_json_build_delete_session(pkbuf_t **pkbuf, amf4g_sess_t *sess)
{
    char *string = NULL;
    cJSON *j_deleteSession = cJSON_CreateObject();
    delete_session_t deleteSession = {0};
    c_uint32_t length = 0;
    
    d_trace(10, "%s %d Enter \n", __FUNCTION__, __LINE__);
    
    /* imsi */
    memcpy(deleteSession.imsi, sess->amf4g_ue->imsi, sess->amf4g_ue->imsi_len);
    deleteSession.imsi_len = sess->amf4g_ue->imsi_len;
    
    /* apn */
    strcpy(deleteSession.apn, sess->pdn->apn);
    /* ebi */
    deleteSession.ebi = amf4g_default_bearer_in_sess(sess)->ebi;

    JSONTRANSFORM_StToJs_delete_session_request(&deleteSession, j_deleteSession);
    string = cJSON_Print(j_deleteSession);
    d_trace(15, "Delete Session JSON: %s \n", string);
    
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    d_assert(*pkbuf != NULL, return CORE_ERROR, "*pkbuf is null");
    
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
        
    free(string);
    cJSON_Delete(j_deleteSession);

    d_trace(10, "%s %d Leave \n", __FUNCTION__, __LINE__);

    return CORE_OK;
}
