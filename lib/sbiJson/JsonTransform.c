#include "core_lib.h"
#include "core_debug.h"
#include "core_pkbuf.h"
#include "core_network.h"
#include "JsonTransform.h"
#include "string.h"
#include <arpa/inet.h>

#define JSON_DEBUG 0

void plmn_id_to_buffer(plmn_id_t plmn_id, char* mcc, char* mnc){
    bzero(mcc, 4);
    bzero(mnc, 4);
    sprintf(mcc, "%d%d%d", plmn_id.mcc1, plmn_id.mcc2, plmn_id.mcc3);  
    if(plmn_id.mnc1 != 0xf) 
        sprintf(mnc, "%d%d%d", plmn_id.mnc1, plmn_id.mnc2, plmn_id.mnc3);
    else
        sprintf(mnc, "%d%d", plmn_id.mnc2, plmn_id.mnc3);

    return;
}

void add_plmnid_to_json(cJSON* json_key, plmn_id_t plmn_id){
    char mnc[4];
    char mcc[4];
    plmn_id_to_buffer(plmn_id, mcc, mnc);
    cJSON_AddStringToObject(json_key, "mcc", mcc);
    cJSON_AddStringToObject(json_key, "mnc", mnc);
    return;
}


void add_uli_to_json(cJSON* json_key, tai_t _tai, e_cgi_t _e_cgi){
    char str[10];
    cJSON *uli = cJSON_AddObjectToObject(json_key, JSONKEY_4G_ULI);
    cJSON *eutral = cJSON_AddObjectToObject(uli, JSONKEY_4G_ULI_EUTRAL);
    cJSON *tai = cJSON_AddObjectToObject(eutral, JSONKEY_4G_ULI_EUTRAL_TAI);
    cJSON *e_cgi = cJSON_AddObjectToObject(eutral, JSONKEY_4G_ULI_EUTRAL_ECGI);
    cJSON *tai_plmnid = cJSON_AddObjectToObject(tai, JSONKEY_4G_ULI_EUTRAL_TAI_PLMNID);
    cJSON *e_cgi_plmnid = cJSON_AddObjectToObject(e_cgi, JSONKEY_4G_ULI_EUTRAL_ECGI_PLMNID);
    bzero(str, 10);
    sprintf(str, "%u", _tai.tac);
    cJSON_AddStringToObject(tai, JSONKEY_4G_ULI_EUTRAL_TAI_TAC, str);
    add_plmnid_to_json(tai_plmnid, _tai.plmn_id);
    bzero(str, 10);
    sprintf(str, "%u", _e_cgi.cell_id);
    cJSON_AddStringToObject(e_cgi, JSONKEY_4G_ULI_EUTRAL_ECGI_EUTRACELLID, str);
    add_plmnid_to_json(e_cgi_plmnid, _e_cgi.plmn_id);

    return;
}

void add_servingnw_to_json(cJSON* json_key,plmn_id_t plmn_id){
    cJSON *servingnw = cJSON_AddObjectToObject(json_key, JSONKEY_4G_SERVINGNETWORK);
    add_plmnid_to_json(servingnw, plmn_id);
    return;
}

void add_ebi_to_json(cJSON* json_key, c_uint8_t ebi){
    char str[10];
    bzero(str, 10);
    sprintf(str, "%u", ebi);
    cJSON_AddStringToObject(json_key, JSONKEY_4G_EBI, str);
    return;
}

void add_pdn_to_json(cJSON* json_key, c_uint8_t pdn_type, pdn_t* _pdn){
    char str[20];
    cJSON *pdn = cJSON_AddObjectToObject(json_key, JSONKEY_4G_PDN);
    cJSON *paa = cJSON_AddObjectToObject(pdn, JSONKEY_4G_PDN_PAA); // PDN Address Allocation 
    cJSON *ambr = cJSON_AddObjectToObject(pdn, JSONKEY_4G_PDN_AMBR); // aggregate maximum bit rate
    bzero(str, 20);
    sprintf(str, "%u", pdn_type);
#if JSON_DEBUG
    d_info("%d %s pdn_type :%d\n", __LINE__, __FUNCTION__, pdn_type);
    d_info("%d %s str :%s\n", __LINE__, __FUNCTION__, str);
#endif
    cJSON_AddStringToObject(pdn, JSONKEY_4G_PDN_PDNTYPE, str);
    bzero(str, 20);
    sprintf(str, "%u", _pdn->paa.pdn_type);
#if JSON_DEBUG
    d_info("%d %s str :%s\n", __LINE__, __FUNCTION__, str);
#endif
    cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_PDNTYPE, str);
    char buf[CORE_ADDRSTRLEN], buf2[CORE_ADDRSTRLEN];
    
    if (pdn_type == SBI_PDN_TYPE_IPV4){
        INET_NTOP(&_pdn->paa.addr, buf);
#if JSON_DEBUG
        d_info("%s %d str _pdn->paa.addr,:%s\n", __LINE__, __FUNCTION__, buf);
#endif
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR, buf);
    }
    else if (pdn_type == SBI_PDN_TYPE_IPV6){
        INET6_NTOP(&_pdn->paa.addr6, buf);
#if JSON_DEBUG
        d_info("%d %s str _pdn->paa.addr6,:%s\n", __LINE__, __FUNCTION__, buf);
#endif
        bzero(str, 20);
        sprintf(str, "%u", _pdn->paa.both.len);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_LEN, str);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR6, buf);
    }
    else if (pdn_type == SBI_PDN_TYPE_IPV4V6){
        INET_NTOP(&_pdn->paa.both.addr, buf);
#if JSON_DEBUG
        d_info("%d %s str _pdn->paa.addr,:%s\n", __LINE__, __FUNCTION__, buf);
#endif
        INET6_NTOP(&_pdn->paa.both.addr6, buf2);
#if JSON_DEBUG
        d_info("%d %s str _pdn->paa.addr6,:%s\n", __LINE__, __FUNCTION__, buf2);
#endif
        bzero(str, 20);
        sprintf(str, "%u", _pdn->paa.both.len);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR, buf);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_LEN, str);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR6, buf2);
    }
    else
        d_assert(0, return , "Not supported(%d)", pdn_type);
    if (_pdn->ambr.uplink || _pdn->ambr.downlink)
    {
        bzero(str, 20);
        sprintf(str, "%lu", _pdn->ambr.uplink);
        cJSON_AddStringToObject(ambr, "uplink", str);
        bzero(str, 20);
        sprintf(str, "%lu", _pdn->ambr.downlink);
        cJSON_AddStringToObject(ambr, "downlink", str);
    }
}

void add_gummei_to_json(cJSON* json_key, guti_t guti){
    char str[10];
    cJSON *gummei = cJSON_AddObjectToObject(json_key, JSONKEY_4G_GUMMEI);
    cJSON *gummei_plmnid = cJSON_AddObjectToObject(gummei, JSONKEY_4G_GUMMEI_PLMNID);
    add_plmnid_to_json(gummei_plmnid, guti.plmn_id);
    bzero(str, 10);
    sprintf(str, "%u", guti.mme_gid);
    cJSON_AddStringToObject(gummei, JSONKEY_4G_GUMMEI_MMEGID, str);
    bzero(str, 10);    
    sprintf(str, "%u", guti.mme_code);
    cJSON_AddStringToObject(gummei, JSONKEY_4G_GUMMEI_MMECODE, str);
    return;
}

char* uint_to_buffer(char* conv, c_uint8_t* input, int len){
    bzero(conv, 1000);
    for(int i = 0; i < len; i++){
#if WORDS_BIGENDIAN
        sprintf(conv, "%s%x%x", conv, (input[i]>>4)&0x0f, input[i]&0x0f);
#else
        sprintf(conv, "%s%x%x", conv, input[i]&0x0f, (input[i]>>4)&0x0f);
#endif
    }
    return conv;
}

status_t JSONTRANSFORM_StToJs_create_session_request(creat_session_t *sess, cJSON *pJson)
{
    d_assert(sess, return CORE_ERROR, "Null param");
    d_assert(pJson, return CORE_ERROR, "Null param");

    /* imsi */
#if JSON_DEBUG
    d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
    cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, sess->imsi_bcd);
    /* user location information */
    add_uli_to_json(pJson, sess->tai, sess->e_cgi);

    /* serving network */
#if JSON_DEBUG
    d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
    add_servingnw_to_json(pJson, sess->visited_plmn_id);

    /* radio access technology */
#if JSON_DEBUG
    d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
    cJSON_AddStringToObject(pJson, JSONKEY_4G_RATTYPE, EUTRA);

    /* protocol_configuration_options(nas) */
    if (sess->ue_pco.length && sess->ue_pco.buffer)
    {
        char conv[1000] = {0};
#if JSON_DEBUG
        d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
        cJSON_AddStringToObject(pJson, JSONKEY_4G_PCO, uint_to_buffer(conv, sess->ue_pco.buffer, sess->ue_pco.length));
    }
    
    /* APN */
#if JSON_DEBUG
    d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
    cJSON_AddStringToObject(pJson, JSONKEY_4G_APN, sess->apn);

    /* packet data network */
    // d_info("sess->pdn_type :%d\n", sess->pdn_type);
#if JSON_DEBUG
    d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
    add_pdn_to_json(pJson, sess->pdn_type, &sess->pdn);
    
    /* create bearer contexts */
#if JSON_DEBUG
    d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
    add_ebi_to_json(pJson, sess->ebi);
    // cJSON *j_bearer = cJSON_AddObjectToObject(session, "bearer_contexts");
    // cJSON *j_bearer_qos = cJSON_AddObjectToObject(j_bearer, "bearer_qos");
    // cJSON_AddNumberToObject(j_bearer, "ebi", bearer->ebi);
    // cJSON_AddNumberToObject(j_bearer_qos, "qci", pdn->qos.qci); //QoS class identifier
    // cJSON_AddNumberToObject(j_bearer_qos, "priority", pdn->qos.arp.priority_level); //priority level
    // cJSON_AddNumberToObject(j_bearer_qos, "pec", pdn->qos.arp.pre_emption_capability); //pre emption capability
    // cJSON_AddNumberToObject(j_bearer_qos, "pev", pdn->qos.arp.pre_emption_vulnerability); //pre emption vulnerability

    /*TODO : ue timezone*/

    /* gummei */
#if JSON_DEBUG
    d_info("%d %s \n", __LINE__, __FUNCTION__);
#endif
    add_gummei_to_json(pJson, sess->guti);
    
    return CORE_OK;
}

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

void _add_imsi_to_struct(cJSON* json_key, c_int8_t *pImsi) {
    
    cJSON *j_imsi = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_IMSI);
    memcpy(pImsi, j_imsi->valuestring, strlen(j_imsi->valuestring));
    memset(&pImsi[15], 0x00, 1);
#if 0
    d_info("j_imsi->valuestring %s ", j_imsi->valuestring);
    int i = 0;
    for (i = 0; i<16; i++)
    {
        printf("%x ", pImsi[i]);
    }
#endif
}

void _add_uld_to_struct(cJSON* json_key, creat_session_t *sess) {
    char mcc[4];
    char mnc[4];
    bzero(mcc, 4);
    bzero(mnc, 4);
    cJSON *j_uli = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_ULI);
    cJSON *j_eutra = cJSON_GetObjectItemCaseSensitive(j_uli, JSONKEY_4G_ULI_EUTRAL);
    cJSON *j_tai = cJSON_GetObjectItemCaseSensitive(j_eutra, JSONKEY_4G_ULI_EUTRAL_TAI);
    cJSON *j_tai_plmnid = cJSON_GetObjectItemCaseSensitive(j_tai, JSONKEY_4G_ULI_EUTRAL_TAI_PLMNID);
    cJSON *j_tai_plmnid_mmc = cJSON_GetObjectItemCaseSensitive(j_tai_plmnid, JSONKEY_4G_ULI_EUTRAL_TAI_PLMNID_MCC);
    cJSON *j_tai_plmnid_mnc = cJSON_GetObjectItemCaseSensitive(j_tai_plmnid, JSONKEY_4G_ULI_EUTRAL_TAI_PLMNID_MNC);
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, j_tai_plmnid_mmc->valuestring);
    // d_info("j_tai_plmnid_mmc->valuestring %s ",j_tai_plmnid_mmc->valuestring);
    strcpy(mnc, j_tai_plmnid_mnc->valuestring);
    // d_info("j_tai_plmnid_mmc->valuestring %s ",j_tai_plmnid_mmc->valuestring);
    plmn_id_build(&sess->tai.plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
    cJSON *j_tai_tac = cJSON_GetObjectItemCaseSensitive(j_tai, JSONKEY_4G_ULI_EUTRAL_TAI_TAC);
    // d_info("j_tai_tac->valuestring %s ",j_tai_tac->valuestring);
    // _str2short(j_tai_tac->valuestring, strlen(j_tai_tac->valuestring),&sess->tai.tac);
    sess->tai.tac = atoi(j_tai_tac->valuestring);
    // printf("\ttai :\n\t\ttac : %d\n\t\tplmn_id :\n\t\t\tmcc : %s\n\t\t\t mnc : %s\n", sess->tai.tac, mcc, mnc);    

    cJSON *j_e_cgi = cJSON_GetObjectItemCaseSensitive(j_eutra, JSONKEY_4G_ULI_EUTRAL_ECGI);
    cJSON *j_e_cgi_plmnid = cJSON_GetObjectItemCaseSensitive(j_e_cgi, JSONKEY_4G_ULI_EUTRAL_ECGI_PLMNID);
    cJSON *j_e_cgi_plmnid_mmc = cJSON_GetObjectItemCaseSensitive(j_e_cgi_plmnid, JSONKEY_4G_ULI_EUTRAL_ECGI_PLMNID_MCC);
    cJSON *j_e_cgi_plmnid_mnc = cJSON_GetObjectItemCaseSensitive(j_e_cgi_plmnid, JSONKEY_4G_ULI_EUTRAL_ECGI_PLMNID_MNC);
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, j_e_cgi_plmnid_mmc->valuestring);
    // d_info("j_tai_plmnid_mmc->valuestring %s ",j_tai_plmnid_mmc->valuestring);
    strcpy(mnc, j_e_cgi_plmnid_mnc->valuestring);
    // d_info("j_tai_plmnid_mmc->valuestring %s ",j_tai_plmnid_mmc->valuestring);
    plmn_id_build(&sess->e_cgi.plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
    cJSON *j_e_cgi_eutraCellId = cJSON_GetObjectItemCaseSensitive(j_e_cgi, JSONKEY_4G_ULI_EUTRAL_ECGI_EUTRACELLID);
    sess->e_cgi.cell_id = atoi(j_e_cgi_eutraCellId->valuestring);
    // printf("\tecgi :\n\t\tcell id : %d\n \t\t\tmcc : %s\n\t\t\t mnc : %s\n", sess->e_cgi.cell_id, mcc, mnc);    
}

void _add_serving_network_to_struct(cJSON* json_key, plmn_id_t *visited_plmn_id) {
    char mcc[4];
    char mnc[4];
    bzero(mcc, 4);
    bzero(mnc, 4);
    cJSON *j_serving_network = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_SERVINGNETWORK);
    cJSON *j_serving_network_mmc = cJSON_GetObjectItemCaseSensitive(j_serving_network, JSONKEY_4G_SERVINGNETWORK_MMC);
    cJSON *j_serving_network_mnc= cJSON_GetObjectItemCaseSensitive(j_serving_network, JSONKEY_4G_SERVINGNETWORK_MNC);
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, j_serving_network_mmc->valuestring);
    strcpy(mnc, j_serving_network_mnc->valuestring);
    plmn_id_build(visited_plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
}

void _add_radio_type_to_struct(cJSON* json_key, c_uint8_t *radioType) {

    memset(radioType, 0x00, 10);
    cJSON *j_ratType = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_RATTYPE);
    memcpy(radioType, j_ratType->valuestring, strlen(j_ratType->valuestring));
    
}

void _add_pco_to_struct(cJSON* json_key, ue_pco_t *ue_pco) {

    cJSON *j_pco = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_PCO);
    
    ue_pco->length = strlen(j_pco->valuestring);
    ue_pco->buffer = core_calloc(ue_pco->length +1 , sizeof(c_uint8_t));
    // strcpy(sess->ue_pco.buffer, cJSON_GetObjectItemCaseSensitive(pJson, JSONKEY_4G_PCO) -> valuestring);
    core_ascii_to_hex(j_pco->valuestring, ue_pco->length, ue_pco->buffer, (ue_pco->length)/2 );
}

void _add_apn_to_struct(cJSON* json_key, c_int8_t *apn) {
    cJSON *j_apn = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_APN);
    strcpy(apn, j_apn->valuestring);
}

void _add_pdn_to_struct(cJSON *json_key, pdn_t *pdn) {
    char paa_pdn_type[1+1] = {0};
    char paa_pdn_addr[32]= {0};
    char paa_pdn_len[16]= {0};

    cJSON *j_pdn = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_PDN);
    cJSON *j_pdn_paa = cJSON_GetObjectItemCaseSensitive(j_pdn, JSONKEY_4G_PDN_PAA); // PDN Address Allocation 
    cJSON *j_pdn_paa_pdntype = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_PDNTYPE);
    cJSON *j_pdn_paa_addr = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_ADDR);
    cJSON *j_pdn_paa_addr6 = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_ADDR6);
    cJSON *j_pdn_paa_len = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_LEN);
    bzero(paa_pdn_type, 1+1); 
    bzero(paa_pdn_addr, 32);
    bzero(paa_pdn_len, 16);
    
    strcpy(paa_pdn_type, j_pdn_paa_pdntype->valuestring);
    // d_info(paa_pdn_type);
    pdn->paa.pdn_type = atoi(paa_pdn_type);
    strcpy(paa_pdn_addr,j_pdn_paa_addr->valuestring);
    // d_info(paa_pdn_type);
    pdn->paa.addr = atoi(paa_pdn_addr);
    strcpy(paa_pdn_len,j_pdn_paa_len->valuestring);
    // d_info(paa_pdn_len);
    pdn->paa.len = atoi(paa_pdn_len);
    //TODO : FIX ipv6 input
    // d_info(j_pdn_paa_addr6->valuestring);
    inet_pton(AF_INET6, j_pdn_paa_addr6->valuestring, pdn->paa.addr6);

    cJSON *j_pdn_ambr = cJSON_GetObjectItemCaseSensitive(j_pdn, JSONKEY_4G_PDN_AMBR);
    cJSON *j_pdn_ambr_uplink = cJSON_GetObjectItemCaseSensitive(j_pdn_ambr, JSONKEY_4G_PDN_AMBR_UPLINK);
    cJSON *j_pdn_ambr_downlink = cJSON_GetObjectItemCaseSensitive(j_pdn_ambr, JSONKEY_4G_PDN_AMBR_DOWMLINK);
    pdn->ambr.uplink = atoi(j_pdn_ambr_uplink->valuestring);
    pdn->ambr.downlink = atoi(j_pdn_ambr_downlink->valuestring);
    
    cJSON *j_pdn_pdnType = cJSON_GetObjectItemCaseSensitive(j_pdn, JSONKEY_4G_PDN_PDNTYPE);
    pdn->pdn_type = atoi(j_pdn_pdnType->valuestring);

}
 void _add_ebi_to_struct(cJSON* json_key, c_uint8_t *ebi) {
    
    cJSON *j_ebi = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_EBI);
    // d_info(j_ebi->valuestring);
    int tmpData = 0;
    tmpData = atoi(j_ebi->valuestring);
    *ebi = tmpData;
     
 }

 void _add_gummei_to_struct(cJSON* json_key, guti_t *guti) {
    char mcc[4];
    char mnc[4];
    bzero(mcc, 4);
    bzero(mnc, 4);
    cJSON *j_gummei = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_GUMMEI);
    cJSON *j_gummei_plmnid = cJSON_GetObjectItemCaseSensitive(j_gummei, JSONKEY_4G_GUMMEI_PLMNID);
    cJSON *j_gummei_plmnid_mcc = cJSON_GetObjectItemCaseSensitive(j_gummei_plmnid, JSONKEY_4G_GUMMEI_PLMNID_MCC);
    cJSON *j_gummei_plmnid_mnc = cJSON_GetObjectItemCaseSensitive(j_gummei_plmnid, JSONKEY_4G_GUMMEI_PLMNID_MNC);
    bzero(mcc, 4);
    bzero(mnc, 4);
    strcpy(mcc, j_gummei_plmnid_mcc->valuestring);
    strcpy(mnc, j_gummei_plmnid_mnc->valuestring);
    plmn_id_build(&guti->plmn_id, atoi(mcc), atoi(mnc), strlen(mnc));
    cJSON *j_gummei_mmegid = cJSON_GetObjectItemCaseSensitive(j_gummei, JSONKEY_4G_GUMMEI_MMEGID);
    cJSON *j_gummei_mmecode = cJSON_GetObjectItemCaseSensitive(j_gummei, JSONKEY_4G_GUMMEI_MMECODE);

    guti->mme_gid = atoi(j_gummei_mmegid->valuestring);
    guti->mme_code = atoi(j_gummei_mmecode->valuestring);
 }


status_t JSONTRANSFORM_JsToSt_create_session_request(creat_session_t *sess, cJSON *pJson)
{
    /* imsi */
    _add_imsi_to_struct(pJson, sess->imsi_bcd);
    
    /* user location information */
    _add_uld_to_struct(pJson, sess);
    
    // /* serving network */
    _add_serving_network_to_struct(pJson, &sess->visited_plmn_id);

    /* radio access technology */
    _add_radio_type_to_struct(pJson, sess->rat_type);

    /* protocol_configuration_options(nas) */
    _add_pco_to_struct(pJson, &sess->ue_pco);
    
    /* APN */
    _add_apn_to_struct(pJson, sess->apn);

    /* packet data network */
    _add_pdn_to_struct(pJson, &sess->pdn);

    /* ebi */
    _add_ebi_to_struct(pJson, &sess->ebi);
    
    /* gummei */
    _add_gummei_to_struct(pJson, &sess->guti);

    return CORE_OK;
}
