#include <cJSON/cJSON.h>

#include "core_debug.h"
#include "core_pkbuf.h"
#include "3gpp_types.h"
#include "mme_context.h"
#include "sbiJson/commonJsonKey.h"
#include "sbiJson/JsonTransform.h"
#include "amf_json_build.h"

status_t amf_json_build_create_session(pkbuf_t **pkbuf, mme_sess_t *sess) {
    
    pdn_t *pdn = NULL;
    mme_ue_t *mme_ue = NULL;
    mme_bearer_t *bearer = NULL;
    char *string = NULL;
    cJSON *session = cJSON_CreateObject();
    creat_session_t createSession = {0};
    c_uint32_t length = 0;
    d_assert(sess, return CORE_ERROR, "Null param");
    pdn = sess->pdn;
    d_assert(pdn, return CORE_ERROR, "Null param");
    bearer = mme_default_bearer_in_sess(sess);
    d_assert(bearer, return CORE_ERROR, "Null param");
    mme_ue = sess->mme_ue;
    d_assert(mme_ue, return CORE_ERROR, "Null param");

    d_info("createSession:%d", &createSession);
    memcpy(createSession.imsi_bcd,  mme_ue->imsi_bcd, sizeof( mme_ue->imsi_bcd));
    memcpy(&createSession.tai, &mme_ue->tai, sizeof( mme_ue->tai));
    memcpy(&createSession.e_cgi, &mme_ue->e_cgi, sizeof( mme_ue->e_cgi));
    memcpy(&createSession.visited_plmn_id, &mme_ue->visited_plmn_id, sizeof( mme_ue->visited_plmn_id));
    if (sess->ue_pco.length && sess->ue_pco.buffer) {
        createSession.ue_pco.buffer = malloc(sizeof(char)* sess->ue_pco.length);
        memcpy(&createSession.ue_pco.length, &sess->ue_pco.length, sizeof( sess->ue_pco.length));
        memcpy(&createSession.ue_pco.buffer, &sess->ue_pco.buffer, sizeof( sess->ue_pco.length));
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
    memcpy(&createSession.pdn_type, &pdn_type, sizeof(pdn_type));
    memcpy(&createSession.pdn, &pdn, sizeof(pdn_t));

    memcpy(&createSession.ebi, &bearer->ebi, sizeof(bearer->ebi));
    /*TODO : ue timezone*/
    memcpy(&createSession.guti, &mme_ue->guti, sizeof(mme_ue->guti));
    
    JSONTRANSFORM_create_session_request(&createSession, session);

    string = cJSON_Print(session);
    d_info(string);

    length = strlen(string) + 1;
    
    *pkbuf = pkbuf_alloc(0, length);
    (*pkbuf)->len = length;
    memcpy((*pkbuf)->payload, string, length -1);
    cJSON_Delete(session);

    return CORE_OK;
}