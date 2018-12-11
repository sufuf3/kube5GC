#define TRACE_MODULE _amf_json_build
#include <cJSON/cJSON.h>

#include "core_debug.h"
#include "core_pkbuf.h"
#include "3gpp_types.h"
#include "mme_context.h"
#include "sbiJson/commonJsonKey.h"
#include "sbiJson/JsonTransform.h"
#include "amf_json_build.h"

int _amf_json_build = 10;

status_t amf_json_build_create_session(pkbuf_t **pkbuf, mme_sess_t *sess) {
    
    pdn_t *pdn = NULL;
    mme_ue_t *mme_ue = NULL;
    mme_bearer_t *bearer = NULL;
    char *string = NULL;
    cJSON *session = cJSON_CreateObject();
    create_session_t createSession = {0};
    c_uint32_t length = 0;
    d_assert(sess, return CORE_ERROR, "Null param");
    pdn = sess->pdn;
    d_assert(pdn, return CORE_ERROR, "Null param");
    bearer = mme_default_bearer_in_sess(sess);
    d_assert(bearer, return CORE_ERROR, "Null param");
    mme_ue = sess->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");

    // d_info("createSession:%d", &createSession);
    memcpy(createSession.imsi_bcd,  mme_ue->imsi_bcd, sizeof( mme_ue->imsi_bcd));
#if 0
    int i = 0;
    for (i = 0; i<16; i++)
    {
        printf("%x ", createSession.imsi_bcd[i]);
    }
#endif
    memcpy(&createSession.tai, &mme_ue->tai, sizeof( mme_ue->tai));
    // printf("\ttai :\n\t\ttac : %d\n\t\tplmn_id :\n\t\t\t\n", createSession.tai.tac);  
    memcpy(&createSession.e_cgi, &mme_ue->e_cgi, sizeof( mme_ue->e_cgi));
    memcpy(&createSession.visited_plmn_id, &mme_ue->visited_plmn_id, sizeof( mme_ue->visited_plmn_id));
    if (sess->ue_pco.length && sess->ue_pco.buffer) {
        createSession.ue_pco.buffer = malloc(sizeof(char)* sess->ue_pco.length);
        memcpy(&createSession.ue_pco.length, &sess->ue_pco.length, sizeof( sess->ue_pco.length));
        memcpy(&createSession.ue_pco.buffer, &sess->ue_pco.buffer, sess->ue_pco.length);
        d_print_hex(createSession.ue_pco.buffer, createSession.ue_pco.length);
    }
    memcpy(&createSession.apn, &pdn->apn, sizeof( pdn->apn));

    c_uint8_t pdn_type;
    if (pdn->pdn_type == HSS_PDN_TYPE_IPV4 ||
        pdn->pdn_type == HSS_PDN_TYPE_IPV6 ||
        pdn->pdn_type == HSS_PDN_TYPE_IPV4V6)
    {
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
    // d_info("pdn->paa.pdn_type :%d\n", pdn->paa.pdn_type);
    memcpy(&createSession.ebi, &bearer->ebi, sizeof(bearer->ebi));
    /*TODO : ue timezone*/
    memcpy(&createSession.guti, &mme_ue->guti, sizeof(mme_ue->guti));
    
    JSONTRANSFORM_StToJs_create_session_request(&createSession, session);

    string = cJSON_Print(session);
    // d_info(string);

    length = strlen(string) + 1;
    
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(session);

    return CORE_OK;
}

CORE_DECLARE(status_t) amf_json_build_modify_bearer(pkbuf_t **pkbuf, mme_bearer_t *bearer)
{
    char *string = NULL;
    cJSON *j_modifyBearerReq = cJSON_CreateObject();
    modify_bearer_t modifyBearer = {0};
    c_uint32_t length = 0;

    d_assert(bearer, return CORE_ERROR, "Null param");
    
    // c_uint32_t      enb_s1u_teid;
    memcpy(modifyBearer.imsi_bcd,  bearer->mme_ue->imsi_bcd, sizeof( bearer->mme_ue->imsi_bcd));
    //TODO: fix it how to get uli presence?
    memcpy(&modifyBearer.uli_presence, "\x01" , sizeof(c_uint8_t));
    memcpy(&modifyBearer.tai, &bearer->mme_ue->tai, sizeof(bearer->mme_ue->tai));
    memcpy(&modifyBearer.e_cgi, &bearer->mme_ue->e_cgi, sizeof(bearer->mme_ue->e_cgi));
    memcpy(&modifyBearer.visited_plmn_id, &bearer->mme_ue->visited_plmn_id, sizeof( bearer->mme_ue->visited_plmn_id));;
    memcpy(&modifyBearer.guti, &bearer->mme_ue->guti, sizeof(bearer->mme_ue->guti));
    //TODO: fix it how to get rat type??
    //memcpy(&modifyBearer.rat_type, 0x00, sizeof(modifyBearer.rat_type));
    memcpy(&modifyBearer.ebi, &bearer->ebi, sizeof(bearer->ebi));
    memcpy(&modifyBearer.enb_s1u_ip, &bearer->enb_s1u_ip, sizeof(bearer->enb_s1u_ip));
    memcpy(&modifyBearer.enb_s1u_teid, &bearer->enb_s1u_teid, sizeof(bearer->enb_s1u_teid));
    memcpy(&modifyBearer.apn, &bearer->sess->pdn->apn, sizeof( bearer->sess->pdn->apn));
    d_trace(5, "JSONTRANSFORM_StToJs_modify_bearer_request");
    JSONTRANSFORM_StToJs_modify_bearer_request(&modifyBearer, j_modifyBearerReq);
    string = cJSON_Print(j_modifyBearerReq);
    d_info(string);
    length = strlen(string) + 1;
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    free(string);
    cJSON_Delete(j_modifyBearerReq);
    return 0;
}
