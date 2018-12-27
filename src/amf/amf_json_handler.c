#define TRACE_MODULE _amf_json_handler
#include <cJSON/cJSON.h>

#include "core_debug.h"
#include "core_lib.h"
#include "core_pkbuf.h"
#include "3gpp_types.h"
#include "amf4g_context.h"
#include "amf_json_handler.h"
#include "sbiJson/JsonTransform.h"

void buffer_to_uint(c_uint8_t* output, const char* input, int len){
    c_uint8_t from, end;
    for(int i = 0; i < len; i++){
        if(input[i*2] >= 'a')
            from = input[i*2] - 'a' + 10;
        else 
            from = input[i*2] - '0';
        if(input[i*2+1] >= 'a')
            end = input[i*2+1] - 'a' + 10;
        else 
            end = input[i*2+1] - '0';
#if WORDS_BIGENDIAN
        output[i] = ((from<<4) & 0xf0) + (end & 0x0f);
#else
         
        output[i] = ((end<<4) & 0xf0) + (from & 0x0f);
#endif
    }
    return ;
}

void print_uint_array(c_uint8_t* input, int len){
    char temp[1000];
    bzero(temp, 1000);
    for(int i = 0; i < len; i++){
        sprintf(temp, "%s\t%d", temp, input[i]);
    }
    printf("buffer : %s\n", temp);
    return;
}

bool compare_plmnid(plmn_id_t id1, plmn_id_t id2){
    if(id1.mnc1 != id2.mnc1)
        return false;
    if(id1.mnc2 != id2.mnc2)
        return false;
    if(id1.mnc3 != id2.mnc3)
        return false;
    if(id1.mcc1 != id2.mcc1)
        return false;
    if(id1.mcc2 != id2.mcc2)
        return false;
    if(id1.mcc3 != id2.mcc3)
        return false;
    return true;

}

status_t amf_json_handle_create_session(pkbuf_t **pkbuf, amf4g_sess_t *pSess) {
    
    create_session_t createSession = {0}; 
    amf4g_ue_t *amf4g_ue = NULL;
    amf4g_ue_t *ori_amf4g_ue = NULL;
    pdn_t *ori_pdn = NULL;
    pdn_t *pdn = NULL;
    amf4g_bearer_t *bearer = NULL;
    amf4g_bearer_t *ori_bearer = NULL;

    d_trace(10, "%s %d Enter \n", __FUNCTION__, __LINE__);

    d_assert(pSess, return CORE_ERROR, "Null param");
    d_assert(pkbuf, return CORE_ERROR, "Null param");
    ori_amf4g_ue = pSess->amf4g_ue;
    d_assert(ori_amf4g_ue, return CORE_ERROR, "Null param");
    d_assert(pSess, return CORE_ERROR, "Null param");
    ori_pdn = pSess->pdn;
    ori_bearer = amf4g_default_bearer_in_sess(pSess);
    d_assert(ori_bearer, return CORE_ERROR, "Null param");
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    
    d_assert(session, return CORE_ERROR, "Null param");

    amf4g_ue = core_calloc(1, sizeof(amf4g_ue_t));
    pdn = core_calloc(1, sizeof(pdn_t));
    bearer = core_calloc(1, sizeof(amf4g_bearer_t));

    JSONTRANSFORM_JsToSt_create_session_request(&createSession, session);

    core_buffer_to_bcd(createSession.imsi, createSession.imsi_len, amf4g_ue->imsi_bcd);
    memcpy(amf4g_ue->imsi, createSession.imsi, createSession.imsi_len);
    amf4g_ue->imsi_len = createSession.imsi_len;

    if(strcmp(amf4g_ue->imsi_bcd, ori_amf4g_ue->imsi_bcd) != 0) 
    {
        d_error("imsi_data Error");
    }
    
    if(amf4g_ue->imsi_len != ori_amf4g_ue->imsi_len) 
    {
        d_error("imsi_len Error %d %d", amf4g_ue->imsi_len, ori_amf4g_ue->imsi_len);    
    }

    /* tai */
    memcpy(&amf4g_ue->tai.tac, &createSession.tai.tac, sizeof(createSession.tai.tac));
    memcpy(&amf4g_ue->tai.plmn_id, &createSession.tai.plmn_id, sizeof(createSession.tai.plmn_id));
    /* e-cgi */
    memcpy(&amf4g_ue->e_cgi.cell_id, &createSession.e_cgi.cell_id, sizeof(createSession.e_cgi.cell_id));
    memcpy(&amf4g_ue->e_cgi.plmn_id, &createSession.e_cgi.plmn_id, sizeof(createSession.e_cgi.plmn_id));
    if(!compare_plmnid(amf4g_ue->tai.plmn_id, ori_amf4g_ue->tai.plmn_id))
    {
        d_error("tai.plmn_id Error");
    }        
    if(amf4g_ue->tai.tac != ori_amf4g_ue->tai.tac) 
    {
        d_error("tai.tac Error");
    }
    if(!compare_plmnid(amf4g_ue->e_cgi.plmn_id, ori_amf4g_ue->e_cgi.plmn_id)) 
    {
        d_error("e_cgi.plmn_id Error");
    }     
    if(amf4g_ue->e_cgi.cell_id != ori_amf4g_ue->e_cgi.cell_id) 
    {
        d_error("e_cgi.cell_id Error");
    }
    /*visited plmn id */
    memcpy(&amf4g_ue->visited_plmn_id, &createSession.visited_plmn_id, sizeof(createSession.visited_plmn_id));    
    if(!compare_plmnid(amf4g_ue->visited_plmn_id, ori_amf4g_ue->visited_plmn_id))
    {
        d_error("visited_plmn_id Error");
    }

    char rat_type[10]; 
    memcpy(rat_type, createSession.rat_type , sizeof(createSession.rat_type));   
    if(strcmp(rat_type, "EUTRA") != 0) 
    {
        d_error("rat_type Error");
    }
        
    /* packet data network */
    pdn->pdn_type = createSession.pdn.pdn_type;
    pdn->paa.pdn_type = createSession.pdn.paa.pdn_type;

    if(pdn->paa.pdn_type != ori_pdn->paa.pdn_type)
    {
        d_error("paa.pdn_type Error pdn->paa.pdn_type:%d ori_pdn->paa.pdn_type:%d", pdn->paa.pdn_type, ori_pdn->paa.pdn_type);
    }

    if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4)
    {
        pdn->paa.addr = createSession.pdn.paa.addr;
    }
    else if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV6)
    {
        pdn->paa.len = createSession.pdn.paa.len;
        memcpy(pdn->paa.addr6, createSession.pdn.paa.addr6, sizeof(createSession.pdn.paa.addr6));
    }
    else if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4V6)
    {
        pdn->paa.both.addr = createSession.pdn.paa.addr;
        pdn->paa.both.len = createSession.pdn.paa.len;
        memcpy(pdn->paa.addr6, createSession.pdn.paa.addr6, sizeof(createSession.pdn.paa.addr6));
    }
    else 
    {
         d_error( "Not supported(%d)", pdn->pdn_type);
    }
        
    pdn->ambr.uplink = createSession.pdn.ambr.uplink;
    pdn->ambr.downlink = createSession.pdn.ambr.downlink;
    
    if(pdn->ambr.uplink != ori_pdn->ambr.uplink) 
    {
        d_error("ambr.uplink Error %d  ori_pdn->ambr.uplink :%d", pdn->ambr.uplink, ori_pdn->ambr.uplink);
    }
        
    if(pdn->ambr.downlink != ori_pdn->ambr.downlink) 
    {
       d_error("ambr.downlink Error %d, ori_pdn->ambr.downlink :%d", pdn->ambr.downlink, ori_pdn->ambr.downlink);
    }  

    /* create bearer contexts */
    bearer->ebi = createSession.ebi;
    if(bearer->ebi != ori_bearer->ebi) 
    {
        d_error("ebi Error bearer->ebi:%d, ori_bearer->ebi:%d", bearer->ebi, ori_bearer->ebi);
    }
    
    /*TODO : ue timezone*/
    
    /* gummei */
    memcpy(&amf4g_ue->guti, &createSession.guti, sizeof(createSession.guti));
    if(!compare_plmnid(amf4g_ue->guti.plmn_id, ori_amf4g_ue->guti.plmn_id))
        d_error("guti.plmn_id Error");
    if(amf4g_ue->guti.mme_gid != ori_amf4g_ue->guti.mme_gid)
        d_error("amf4g_gid Error");
    if(amf4g_ue->guti.mme_code != ori_amf4g_ue->guti.mme_code)
        d_error("amf4g_code Error");

    CORE_FREE(amf4g_ue);
    CORE_FREE(bearer);
    CORE_FREE(pdn);
    
    cJSON_Delete(session);

    d_trace(10, "%s %d Leave \n", __FUNCTION__, __LINE__);

    return 0;

}

status_t amf_json_handler_create_session_response(pkbuf_t **pkbuf, create_session_t *pCreateRspSess)
{
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    
    d_trace(10, "%s %d Enter \n", __FUNCTION__, __LINE__);
    
    d_assert(session, return CORE_ERROR, "Null param");

    JSONTRANSFORM_JsToSt_create_session_response(pCreateRspSess, session);
    
    cJSON_Delete(session);
    
    d_trace(10, "%s %d Leave \n", __FUNCTION__, __LINE__);
    
    return CORE_OK;
}

status_t amf_json_handler_update_session_response(pkbuf_t **pkbuf, modify_bearer_t *modifyBearer)
{
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    
    d_trace(10, "%s %d Enter \n", __FUNCTION__, __LINE__);

    d_assert(session, return CORE_ERROR, "Null param");

    JSONTRANSFORM_JsToSt_update_session_response(modifyBearer, session);
    
    cJSON_Delete(session);
    
    d_trace(10, "%s %d Leave \n", __FUNCTION__, __LINE__);

    return CORE_OK;
}

status_t amf_json_handler_delete_session_response(pkbuf_t **pkbuf, delete_session_t *deleteSession)
{
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    d_assert(session, return CORE_ERROR, "Null param");
    
    d_trace(10, "%s %d Enter \n", __FUNCTION__, __LINE__);

    JSONTRANSFORM_JsToSt_delete_session_response(deleteSession, session);
    
    cJSON_Delete(session);
     
    d_trace(10, "%s %d Leave \n", __FUNCTION__, __LINE__);
    return CORE_OK;
}
