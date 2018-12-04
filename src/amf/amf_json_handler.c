#include <cJSON/cJSON.h>

#include "core_debug.h"
#include "core_pkbuf.h"
#include "3gpp_types.h"
#include "mme_context.h"
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

status_t amf_json_handle_create_session(pkbuf_t **pkbuf, mme_sess_t *pSess) {
    
    creat_session_t createSession = {0}; 
    mme_ue_t *mme_ue = NULL;
    mme_ue_t *ori_mme_ue = NULL;
    pdn_t *ori_pdn = NULL;
    pdn_t *pdn = NULL;
    mme_bearer_t *bearer = NULL;
    mme_bearer_t *ori_bearer = NULL;
    // char *string = NULL;
    d_assert(pSess, return CORE_ERROR, "Null param");
    d_assert(pkbuf, return CORE_ERROR, "Null param");
    ori_mme_ue = pSess->mme_ue;
    d_assert(ori_mme_ue, return CORE_ERROR, "Null param");
    d_assert(pSess, return CORE_ERROR, "Null param");
    ori_pdn = pSess->pdn;
    ori_bearer = mme_default_bearer_in_sess(pSess);
    d_assert(ori_bearer, return CORE_ERROR, "Null param");
    cJSON *session = cJSON_Parse((*pkbuf)->payload);
    // string = cJSON_Print(session);
    // d_info(string);
    d_assert(session, return CORE_ERROR, "Null param");

    mme_ue = core_calloc(1, sizeof(mme_ue_t));
    pdn = core_calloc(1, sizeof(pdn_t));
    bearer = core_calloc(1, sizeof(mme_bearer_t));

    JSONTRANSFORM_JsToSt_create_session_request(&createSession, session);
    memcpy(mme_ue->imsi_bcd, createSession.imsi_bcd, sizeof(createSession.imsi_bcd));
    mme_ue->imsi_len = strlen(createSession.imsi_bcd)/2;
    if (mme_ue->imsi_len %2 == 1) {
        mme_ue->imsi_len += 1;
    }

    if(strcmp(mme_ue->imsi_bcd, ori_mme_ue->imsi_bcd) != 0) {
        d_error("imsi_data Error");
    }
    
    if(mme_ue->imsi_len != ori_mme_ue->imsi_len) {
        d_error("imsi_len Error %d %d", mme_ue->imsi_len, ori_mme_ue->imsi_len);    
    }
    /* tai */
    memcpy(&mme_ue->tai.tac, &createSession.tai.tac, sizeof(createSession.tai.tac));
    memcpy(&mme_ue->tai.plmn_id, &createSession.tai.plmn_id, sizeof(createSession.tai.plmn_id));
    /* e-cgi */
    memcpy(&mme_ue->e_cgi.cell_id, &createSession.e_cgi.cell_id, sizeof(createSession.e_cgi.cell_id));
    memcpy(&mme_ue->e_cgi.plmn_id, &createSession.e_cgi.plmn_id, sizeof(createSession.e_cgi.plmn_id));
    if(!compare_plmnid(mme_ue->tai.plmn_id, ori_mme_ue->tai.plmn_id)){
        d_error("tai.plmn_id Error");
    }        
    if(mme_ue->tai.tac != ori_mme_ue->tai.tac) {
        d_error("tai.tac Error");
    }
    if(!compare_plmnid(mme_ue->e_cgi.plmn_id, ori_mme_ue->e_cgi.plmn_id)) {
        d_error("e_cgi.plmn_id Error");
    }     
    if(mme_ue->e_cgi.cell_id != ori_mme_ue->e_cgi.cell_id) {
        d_error("e_cgi.cell_id Error");
    }
    /*visited plmn id */
    memcpy(&mme_ue->visited_plmn_id, &createSession.visited_plmn_id, sizeof(createSession.visited_plmn_id));    
    if(!compare_plmnid(mme_ue->visited_plmn_id, ori_mme_ue->visited_plmn_id)){
        d_error("visited_plmn_id Error");
    }

    char rat_type[10]; 
    memcpy(rat_type, createSession.rat_type , sizeof(createSession.rat_type));   
    if(strcmp(rat_type, "EUTRA") != 0) {
        d_error("rat_type Error");
    }
        
    /* packet data network */
    pdn->pdn_type = createSession.pdn_type;
    pdn->paa.pdn_type = createSession.pdn.paa.pdn_type;
    d_info("pdn->paa.pdn_type:%d pdn->pdn_type:%d", pdn->paa.pdn_type, pdn->pdn_type);
    if(pdn->paa.pdn_type != ori_pdn->paa.pdn_type)
        d_error("paa.pdn_type Error pdn->paa.pdn_type:%d ori_pdn->paa.pdn_type:%d", pdn->paa.pdn_type, ori_pdn->paa.pdn_type);
    
    if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4){
        pdn->paa.addr = createSession.pdn.paa.addr;
    }
    else if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV6){
        pdn->paa.len = createSession.pdn.paa.len;
        memcpy(pdn->paa.addr6, createSession.pdn.paa.addr6, sizeof(createSession.pdn.paa.addr6));
    }
    else if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4V6){
        pdn->paa.both.addr = createSession.pdn.paa.addr;
        pdn->paa.both.len = createSession.pdn.paa.len;
        memcpy(pdn->paa.addr6, createSession.pdn.paa.addr6, sizeof(createSession.pdn.paa.addr6));
    }
    else {
        // d_assert(0, return CORE_ERROR, "Not supported(%d)", pdn->pdn_type);
         d_error( "Not supported(%d)", pdn->pdn_type);
    }
        
    pdn->ambr.uplink = createSession.pdn.ambr.uplink;
    pdn->ambr.downlink = createSession.pdn.ambr.downlink;
    
    if(pdn->ambr.uplink != ori_pdn->ambr.uplink) {
        d_error("ambr.uplink Error %d  ori_pdn->ambr.uplink :%d", pdn->ambr.uplink, ori_pdn->ambr.uplink);
    }
        
    if(pdn->ambr.downlink != ori_pdn->ambr.downlink) {
       d_error("ambr.downlink Error %d, ori_pdn->ambr.downlink :%d", pdn->ambr.downlink, ori_pdn->ambr.downlink);
    }  

    /* create bearer contexts */
    bearer->ebi = createSession.ebi;
    if(bearer->ebi != ori_bearer->ebi) {
        d_error("ebi Error bearer->ebi:%d, ori_bearer->ebi:%d", bearer->ebi, ori_bearer->ebi);
    }
        
    cJSON_Delete(session);
    return 0;

}

status_t handle_create_session(const char* json_string, mme_sess_t* _sess) {
    
    mme_sess_t* sess = NULL;
    pdn_t *pdn = NULL;
    mme_ue_t *mme_ue = NULL;
    mme_bearer_t *bearer = NULL;
    pdn_t *_pdn = NULL;
    mme_ue_t *_mme_ue = NULL;
    mme_bearer_t *_bearer = NULL;
    char mcc[4];
    char mnc[4];
    char rat_type[10];
    cJSON *session = cJSON_Parse(json_string);

    d_assert(_sess, return CORE_ERROR, "Null param");
    _pdn = _sess->pdn;
    d_assert(_pdn, return CORE_ERROR, "Null param");
    _bearer = mme_default_bearer_in_sess(_sess);
    d_assert(_bearer, return CORE_ERROR, "Null param");
    _mme_ue = _sess->mme_ue;
    d_assert(_mme_ue, return CORE_ERROR, "Null param");

    mme_ue = core_calloc(1, sizeof(mme_ue_t));
    bearer = core_calloc(1, sizeof(mme_bearer_t));
    pdn = core_calloc(1, sizeof(pdn_t));
    sess = core_calloc(1, sizeof(mme_sess_t));

    /* imsi */
    cJSON *j_imsi = cJSON_GetObjectItemCaseSensitive(session, "imsi");
    strcpy(mme_ue->imsi_bcd , cJSON_GetObjectItemCaseSensitive(j_imsi, "data") -> valuestring);
    mme_ue->imsi_len = cJSON_GetObjectItemCaseSensitive(j_imsi, "len") -> valueint;
    
    // printf("imsi :\n\tdata : %s\n\tlen : %d\n", mme_ue->imsi_bcd, mme_ue->imsi_len);
    if(strcmp(mme_ue->imsi_bcd, _mme_ue->imsi_bcd) != 0)
        d_error("imsi_data Error");
    if(mme_ue->imsi_len != _mme_ue->imsi_len)
        d_error("imsi_len Error");

    /* user location information */
    cJSON *j_uli = cJSON_GetObjectItemCaseSensitive(session, "uli");
    cJSON *j_tai = cJSON_GetObjectItemCaseSensitive(j_uli, "tai");
    cJSON *j_e_cgi = cJSON_GetObjectItemCaseSensitive(j_uli, "e_cgi");
    cJSON *j_tai_plmnid = cJSON_GetObjectItemCaseSensitive(j_tai, "plmn_id");
    cJSON *j_e_cgi_plmnid = cJSON_GetObjectItemCaseSensitive(j_e_cgi, "plmn_id");
    mme_ue->tai.tac = (c_int16_t)cJSON_GetObjectItemCaseSensitive(j_tai, "tac") -> valueint;
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, cJSON_GetObjectItemCaseSensitive(j_tai_plmnid, "mcc") -> valuestring);
    strcpy(mnc, cJSON_GetObjectItemCaseSensitive(j_tai_plmnid, "mnc") -> valuestring);
    plmn_id_build(&mme_ue->tai.plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
    // printf("uli :\n");
    // printf("\ttai :\n\t\ttac : %d\n\t\tplmn_id :\n\t\t\tmcc : %s\n\t\t\t mnc : %s\n", mme_ue->tai.tac, mcc, mnc);
    mme_ue->e_cgi.cell_id = (c_uint32_t)cJSON_GetObjectItemCaseSensitive(j_e_cgi, "cell_id") -> valueint;
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, cJSON_GetObjectItemCaseSensitive(j_e_cgi_plmnid, "mcc") -> valuestring);
    strcpy(mnc, cJSON_GetObjectItemCaseSensitive(j_e_cgi_plmnid, "mnc") -> valuestring);
    plmn_id_build(&mme_ue->e_cgi.plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
    // printf("\te_cgi :\n\t\tcell_id : %d\n\t\tplmn_id :\n\t\t\tmcc : %s\n\t\t\t mnc : %s\n", mme_ue->e_cgi.cell_id, mcc, mnc);
    if(!compare_plmnid(mme_ue->tai.plmn_id, _mme_ue->tai.plmn_id))
        d_error("tai.plmn_id Error");
    if(mme_ue->tai.tac != _mme_ue->tai.tac)
        d_error("tai.tac Error");
    if(!compare_plmnid(mme_ue->e_cgi.plmn_id, _mme_ue->e_cgi.plmn_id))
        d_error("e_cgi.plmn_id Error");
    if(mme_ue->e_cgi.cell_id != _mme_ue->e_cgi.cell_id)
        d_error("e_cgi.cell_id Error");

    

    /* serving network */
    cJSON *j_vpi = cJSON_GetObjectItemCaseSensitive(session, "visited_plmn_id");
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, cJSON_GetObjectItemCaseSensitive(j_vpi, "mcc") -> valuestring);
    strcpy(mnc, cJSON_GetObjectItemCaseSensitive(j_vpi, "mnc") -> valuestring);
    plmn_id_build(&mme_ue->visited_plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
    // printf("visited_plmn_id :\n\tmcc : %s\n\t mnc : %s\n", mcc, mnc);
    if(!compare_plmnid(mme_ue->visited_plmn_id, _mme_ue->visited_plmn_id))
        d_error("visited_plmn_id Error");

    /* radio access technology */
    strcpy(rat_type, cJSON_GetObjectItemCaseSensitive(session, "rat_type") -> valuestring);
    // printf("rat_type : %s\n", rat_type);
    if(strcmp(rat_type, "EUTRA") != 0)
        d_error("rat_type Error");


    /* protocol_configuration_options(nas) */
    cJSON *j_pco = cJSON_GetObjectItemCaseSensitive(session, "pco");
    int pco_presence = 0;
    pco_presence = cJSON_GetObjectItemCaseSensitive(j_pco, "presence") -> valueint;
    // printf("pco :\n\tpresence : %d\n", pco_presence);

    if (pco_presence)
    {
        sess->ue_pco.length = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_pco, "len") -> valueint;
        // printf("\tlen : %d\n\tbuff : %s\n", sess->ue_pco.length, cJSON_GetObjectItemCaseSensitive(j_pco, "buff") -> valuestring);
        sess->ue_pco.buffer = core_calloc(sess->ue_pco.length, sizeof(c_uint8_t));
        buffer_to_uint(sess->ue_pco.buffer, cJSON_GetObjectItemCaseSensitive(j_pco, "buff") -> valuestring, sess->ue_pco.length);
        if(sess->ue_pco.length != _sess->ue_pco.length)
            d_error("e_cgi.cell_id Error");
        if(memcmp(sess->ue_pco.buffer, _sess->ue_pco.buffer, _sess->ue_pco.length) != 0){
            d_error("ue_pco.buffer Error");
            print_uint_array(sess->ue_pco.buffer, sess->ue_pco.length);
            print_uint_array(_sess->ue_pco.buffer, _sess->ue_pco.length);

        }
    }
    
    /* packet data network */
    cJSON *j_pdn = cJSON_GetObjectItemCaseSensitive(session, "pdn");
    cJSON *j_paa = cJSON_GetObjectItemCaseSensitive(j_pdn, "paa"); // PDN Address Allocation 
    cJSON *j_ambr = cJSON_GetObjectItemCaseSensitive(j_pdn, "ambr"); // aggregate maximum bit rate
    strcpy(pdn->apn, cJSON_GetObjectItemCaseSensitive(j_pdn, "apn") -> valuestring);
    pdn->pdn_type = (c_int8_t)cJSON_GetObjectItemCaseSensitive(j_pdn, "pdn_type") -> valueint;
    pdn->paa.pdn_type = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_paa, "pdn_type") -> valueint;
    // printf("pdn :\n\tapn : %s\n\tpdn_type : %d\n", pdn->apn, pdn->pdn_type);
    if(pdn->paa.pdn_type != _pdn->paa.pdn_type)
        d_error("paa.pdn_type Error");
    
    if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4){
        pdn->paa.addr = (c_uint32_t)cJSON_GetObjectItemCaseSensitive(j_paa, "addr") -> valueint;
    }
    else if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV6){
        pdn->paa.len = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_paa, "len") -> valueint;
        buffer_to_uint(pdn->paa.addr6, cJSON_GetObjectItemCaseSensitive(j_paa, "len") -> valuestring, sizeof(pdn->paa.addr6));
    }
    else if (pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4V6){
        pdn->paa.both.addr = (c_uint32_t)cJSON_GetObjectItemCaseSensitive(j_paa, "addr") -> valueint;
        pdn->paa.both.len = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_paa, "len") -> valueint;
        buffer_to_uint(pdn->paa.both.addr6, cJSON_GetObjectItemCaseSensitive(j_paa, "len") -> valuestring, pdn->paa.both.len);
    }
    else
        d_assert(0, return CORE_ERROR, "Not supported(%d)", pdn->pdn_type);
    int pdn_ambr_presence = cJSON_GetObjectItemCaseSensitive(j_ambr, "presence") -> valueint;
    if (pdn_ambr_presence)
    {
        pdn->ambr.uplink = (c_uint64_t)cJSON_GetObjectItemCaseSensitive(j_ambr, "uplink") -> valuedouble;
        pdn->ambr.downlink = (c_uint64_t)cJSON_GetObjectItemCaseSensitive(j_ambr, "downlink") -> valuedouble;
        if(pdn->ambr.uplink != _pdn->ambr.uplink)
            d_error("ambr.uplink Error");
        if(pdn->ambr.downlink != _pdn->ambr.downlink)
            d_error("ambr.downlink Error");
    }    
    // printf("\tambr :\n\t\tuplink : %lu\n\t\tdownlink : %lu\n", pdn->ambr.uplink, pdn->ambr.downlink);
    

    /* create bearer contexts */
    cJSON *j_bearer = cJSON_GetObjectItemCaseSensitive(session, "bearer_contexts");
    cJSON *j_bearer_qos = cJSON_GetObjectItemCaseSensitive(j_bearer, "bearer_qos");
    bearer->ebi = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_bearer, "ebi") -> valueint;
    pdn->qos.qci = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_bearer_qos, "qci") -> valueint; //QoS class identifier
    pdn->qos.arp.priority_level = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_bearer_qos, "priority") -> valueint; //priority level
    pdn->qos.arp.pre_emption_capability = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_bearer_qos, "pec") -> valueint; //pre emption capability
    pdn->qos.arp.pre_emption_vulnerability = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_bearer_qos, "pev") -> valueint; //pre emption vulnerability
    // printf("bearer_contexts :\n\tebi : %d\n\tbearer_qos :\n", bearer->ebi);
    // printf("\t\tqci : %d\n\t\tpriority : %d\n\t\tpec : %d\n\t\t pev : %d\n", pdn->qos.qci, pdn->qos.arp.priority_level, pdn->qos.arp.pre_emption_capability, pdn->qos.arp.pre_emption_vulnerability);
    if(bearer->ebi != _bearer->ebi)
        d_error("ebi Error");
    if(pdn->qos.qci != _pdn->qos.qci)
        d_error("qos.qci Error");
    if(pdn->qos.arp.priority_level != _pdn->qos.arp.priority_level)
        d_error("qos.arp.priority_level Error");
    if(pdn->qos.arp.pre_emption_capability != _pdn->qos.arp.pre_emption_capability)
        d_error("qos.arp.pre_emption_capability Error");
    if(pdn->qos.arp.pre_emption_vulnerability != _pdn->qos.arp.pre_emption_vulnerability)
        d_error("qos.arp.pre_emption_vulnerability Error");
    


    /*TODO : ue timezone*/

    /* gummei */
    cJSON *j_gummei = cJSON_GetObjectItemCaseSensitive(session, "gummei");
    cJSON *j_gummei_plmnid = cJSON_GetObjectItemCaseSensitive(j_gummei, "plmn_id");
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, cJSON_GetObjectItemCaseSensitive(j_gummei_plmnid, "mcc") -> valuestring);
    strcpy(mnc, cJSON_GetObjectItemCaseSensitive(j_gummei_plmnid, "mnc") -> valuestring);
    plmn_id_build(&mme_ue->guti.plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
    mme_ue->guti.mme_gid = (c_uint16_t)cJSON_GetObjectItemCaseSensitive(j_gummei, "mme_gid") -> valueint;
    mme_ue->guti.mme_code = (c_uint8_t)cJSON_GetObjectItemCaseSensitive(j_gummei, "mme_code") -> valueint;
    // printf("gummei :\n\tmme_gid : %d\n\t mme_code : %d\n", mme_ue->guti.mme_gid, mme_ue->guti.mme_code);
    // printf("\t\tmcc : %s\n\t\tmnc : %s\n", mcc, mnc);
    if(!compare_plmnid(mme_ue->guti.plmn_id, _mme_ue->guti.plmn_id))
        d_error("guti.plmn_id Error");
    if(mme_ue->guti.mme_gid != _mme_ue->guti.mme_gid)
        d_error("mme_gid Error");
    if(mme_ue->guti.mme_code != _mme_ue->guti.mme_code)
        d_error("mme_code Error");


    CORE_FREE(mme_ue);
    CORE_FREE(bearer);
    CORE_FREE(pdn);
    CORE_FREE(sess);
    cJSON_Delete(session);

    return CORE_OK;
}