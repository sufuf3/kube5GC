#define TRACE_MODULE _json_tranform
#include "core_lib.h"
#include "core_debug.h"
#include "core_pkbuf.h"
#include "core_network.h"
#include "JsonTransform.h"
#include "string.h"
#include <arpa/inet.h>

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

void add_uint8_to_json(cJSON* json_key, c_uint8_t value, const char* key){
    char str[10];
    bzero(str, 10);
    sprintf(str, "%u", value);
    cJSON_AddStringToObject(json_key, key, str);
    return;
}
void add_uint16_to_json(cJSON* json_key, c_uint16_t value, const char* key){
    char str[10];
    bzero(str, 10);
    sprintf(str, "%u", value);
    cJSON_AddStringToObject(json_key, key, str);
    return;
}
void add_uint32_to_json(cJSON* json_key, c_uint32_t value, const char* key){
    char str[10];
    bzero(str, 10);
    sprintf(str, "%u", value);
    cJSON_AddStringToObject(json_key, key, str);
    return;
}
void add_uint64_to_json(cJSON* json_key, c_uint64_t value, const char* key){
    char str[20];
    bzero(str, 20);
    sprintf(str, "%lu", value);
    cJSON_AddStringToObject(json_key, key, str);
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
    cJSON *uli = cJSON_AddObjectToObject(json_key, JSONKEY_4G_ULI);
    cJSON *eutral = cJSON_AddObjectToObject(uli, JSONKEY_4G_ULI_EUTRAL);
    cJSON *tai = cJSON_AddObjectToObject(eutral, JSONKEY_4G_ULI_EUTRAL_TAI);
    cJSON *e_cgi = cJSON_AddObjectToObject(eutral, JSONKEY_4G_ULI_EUTRAL_ECGI);
    cJSON *tai_plmnid = cJSON_AddObjectToObject(tai, JSONKEY_4G_ULI_EUTRAL_TAI_PLMNID);
    cJSON *e_cgi_plmnid = cJSON_AddObjectToObject(e_cgi, JSONKEY_4G_ULI_EUTRAL_ECGI_PLMNID);
    add_uint16_to_json(tai, _tai.tac, JSONKEY_4G_ULI_EUTRAL_TAI_TAC);
    add_plmnid_to_json(tai_plmnid, _tai.plmn_id);
    add_uint32_to_json(e_cgi, _e_cgi.cell_id, JSONKEY_4G_ULI_EUTRAL_ECGI_EUTRACELLID);
    add_plmnid_to_json(e_cgi_plmnid, _e_cgi.plmn_id);
    return;
}

void add_servingnw_to_json(cJSON* json_key,plmn_id_t plmn_id){
    cJSON *servingnw = cJSON_AddObjectToObject(json_key, JSONKEY_4G_SERVINGNETWORK);
    add_plmnid_to_json(servingnw, plmn_id);
    return;
}

void add_pdn_to_json(cJSON* json_key, pdn_t* _pdn){
    
    cJSON *pdn = cJSON_AddObjectToObject(json_key, JSONKEY_4G_PDN);
    cJSON *paa = cJSON_AddObjectToObject(pdn, JSONKEY_4G_PDN_PAA); // PDN Address Allocation 
    cJSON *ambr = cJSON_AddObjectToObject(pdn, JSONKEY_4G_PDN_AMBR); // aggregate maximum bit rate
    
    add_uint8_to_json(pdn, _pdn->pdn_type, JSONKEY_4G_PDN_PDNTYPE);
    
    add_uint8_to_json(paa, _pdn->paa.pdn_type, JSONKEY_4G_PDN_PAA_PDNTYPE);

    char buf[CORE_ADDRSTRLEN], buf2[CORE_ADDRSTRLEN];
    
    if (_pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4){
        INET_NTOP(&_pdn->paa.addr, buf);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR, buf);
    }
    else if (_pdn->paa.pdn_type == SBI_PDN_TYPE_IPV6){
        INET6_NTOP(&_pdn->paa.addr6, buf);
        add_uint8_to_json(paa, _pdn->paa.len, JSONKEY_4G_PDN_PAA_LEN);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR6, buf);
    }
    else if (_pdn->paa.pdn_type == SBI_PDN_TYPE_IPV4V6){
        INET_NTOP(&_pdn->paa.both.addr, buf);
        INET6_NTOP(&_pdn->paa.both.addr6, buf2);
        add_uint8_to_json(paa, _pdn->paa.both.len, JSONKEY_4G_PDN_PAA_LEN);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR, buf);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR6, buf2);
    }
    else
        d_assert(0, return , "Not supported(%d)", _pdn->pdn_type);
    if (_pdn->ambr.uplink || _pdn->ambr.downlink)
    {
        add_uint64_to_json(ambr, _pdn->ambr.uplink, JSONKEY_4G_PDN_AMBR_UPLINK);
        add_uint64_to_json(ambr, _pdn->ambr.downlink, JSONKEY_4G_PDN_AMBR_DOWMLINK);
    }
}

void add_gummei_to_json(cJSON* json_key, guti_t guti){
    cJSON *gummei = cJSON_AddObjectToObject(json_key, JSONKEY_4G_GUMMEI);
    cJSON *gummei_plmnid = cJSON_AddObjectToObject(gummei, JSONKEY_4G_GUMMEI_PLMNID);
    add_plmnid_to_json(gummei_plmnid, guti.plmn_id);
    add_uint16_to_json(gummei, guti.mme_gid, JSONKEY_4G_GUMMEI_MMEGID);
    add_uint16_to_json(gummei, guti.mme_code, JSONKEY_4G_GUMMEI_MMECODE);
    return;
}

void add_m_bearer_ctx_to_json(cJSON* json_key, c_uint8_t ebi, ip_t ip, c_uint32_t teid){
    cJSON *m_br_ctx = cJSON_AddObjectToObject(json_key, JSONKEY_4G_M_BEARER_CTX);
    cJSON *m_br_ctx_ip = cJSON_AddObjectToObject(json_key, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP);
    add_uint8_to_json(m_br_ctx, ebi, JSONKEY_4G_M_BEARER_CTX_EBI);
    add_uint32_to_json(m_br_ctx, teid, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_TEID);
    char buf[CORE_ADDRSTRLEN] = {0}, buf2[CORE_ADDRSTRLEN] = {0};
    if(ip.ipv4 && ip.ipv6){
        INET_NTOP(&ip.both.addr, buf);
        INET6_NTOP(&ip.both.addr6, buf2);
        cJSON_AddStringToObject(m_br_ctx_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_ADDR, buf);
        cJSON_AddStringToObject(m_br_ctx_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_ADDR6, buf2);
    }
    else if(ip.ipv4){
        INET_NTOP(&ip.addr, buf);
        cJSON_AddStringToObject(m_br_ctx_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_ADDR, buf);
    }
    else if(ip.ipv6){
        INET6_NTOP(&ip.addr6, buf2);
        cJSON_AddStringToObject(m_br_ctx_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_ADDR6, buf2);
    }
    add_uint8_to_json(m_br_ctx_ip, ip.ipv4, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_IPV4);
    add_uint8_to_json(m_br_ctx_ip, ip.ipv6, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_IPV6);
    return;
}

status_t JSONTRANSFORM_StToJs_create_session_request(create_session_t *sess, cJSON *pJson)
{
    char imsi_bcd[MAX_IMSI_BCD_LEN+1] = {0};
    char conv[1000] = {0};

    d_trace(10, "JSONTRANSFORM_StToJs_create_session_request (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, return CORE_ERROR, "Null param");
        d_assert(pJson, return CORE_ERROR, "Null param");

        /* imsi */
        d_trace(15, "\tBuild IMSI\n");
        core_buffer_to_bcd(sess->imsi, sess->imsi_len, imsi_bcd);
        cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, imsi_bcd);

        /* user location information */
        d_trace(15, "\tBuild ULI\n");
        add_uli_to_json(pJson, sess->tai, sess->e_cgi);

        /* serving network */
        d_trace(15, "\tBuild ServingNetwork\n");
        add_servingnw_to_json(pJson, sess->visited_plmn_id);

        /* radio access technology */
        d_trace(15, "\tBuild RAT\n");
        cJSON_AddStringToObject(pJson, JSONKEY_4G_RATTYPE, RATTYPE_EUTRA);

        /* protocol_configuration_options(nas) */
        d_trace(15, "\tBuild PCO\n");
        if (sess->ue_pco.length && sess->ue_pco.buffer)
        {
            core_hex_to_ascii_no_space(sess->ue_pco.buffer, sess->ue_pco.length, conv, sizeof(conv));
            cJSON_AddStringToObject(pJson, JSONKEY_4G_PCO, conv);
        }
        
        /* APN */
        d_trace(15, "\tBuild APN\n");
        cJSON_AddStringToObject(pJson, JSONKEY_4G_APN, sess->apn);

        /* packet data network */
        d_trace(15, "\tBuild PDN\n");
        add_pdn_to_json(pJson , &sess->pdn);
        
        /* create bearer contexts */
        d_trace(15, "\tBuild Create Bearer Contexts\n");
        add_uint8_to_json(pJson, sess->ebi, JSONKEY_4G_EBI);

        /*TODO : ue timezone*/

        /* gummei */
        d_trace(15, "\tBuild GUMMEI\n");
        add_gummei_to_json(pJson, sess->guti);
    } while(0);

    return CORE_OK;
}

status_t JSONTRANSFORM_StToJs_modify_bearer_request(modify_bearer_t *sess, cJSON *pJson)
{
    char imsi_bcd[MAX_IMSI_BCD_LEN+1] = {0};

    d_trace(10, "JSONTRANSFORM_StToJs_modify_bearer_request (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, return CORE_ERROR, "Null param");
        d_assert(pJson, return CORE_ERROR, "Null param");

        /* sm_context_update_type */
        add_uint8_to_json(pJson, sess->sm_context_update_type, JSONKEY_5G_SM_CONTEXT_UPDATE_TYPE);

        /* imsi */
        d_trace(15, "\tBuild IMSI\n");
        core_buffer_to_bcd(sess->imsi, sess->imsi_len, imsi_bcd);
        cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, imsi_bcd);
        
        /* uli */
        d_trace(15, "\tBuild ULI\n");
        if(sess->uli_presence)
            add_uli_to_json(pJson, sess->tai, sess->e_cgi);
        
        /* uli presence */
        d_trace(15, "\tBuild ULI PRESENCE\n");
        add_uint8_to_json(pJson, sess->uli_presence, JSONKEY_4G_ULI_PRESENCE);

        /* serving network */
        d_trace(15, "\tBuild ServingNetwork\n");
        add_servingnw_to_json(pJson, sess->visited_plmn_id);

        /* gummei */
        d_trace(15, "\tBuild GUMMEI\n");
        add_gummei_to_json(pJson, sess->guti);

        /* radio access technology */
        d_trace(15, "\tBuild RAT\n");
        cJSON_AddStringToObject(pJson, JSONKEY_4G_RATTYPE, RATTYPE_EUTRA);

        /* modified bearer context */
        d_trace(15, "\tBuild Modify Bearer Context\n");
        add_m_bearer_ctx_to_json(pJson, sess->ebi, sess->enb_s1u_ip, sess->enb_s1u_teid);

        /* apn */
        d_trace(15, "\tBuild APN\n");
        cJSON_AddStringToObject(pJson, JSONKEY_4G_APN, sess->apn);
    } while(0);
    
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


void conv_json_to_plmnid(cJSON* json_key, plmn_id_t* ptr_plmnid,char* key){
    char mcc[4];
    char mnc[4];
    bzero(mcc, 4);
    bzero(mnc, 4);
    cJSON *plmnid = cJSON_GetObjectItemCaseSensitive(json_key, key);
    cJSON *plmnid_mcc = cJSON_GetObjectItemCaseSensitive(plmnid, "mcc");
    cJSON *plmnid_mnc = cJSON_GetObjectItemCaseSensitive(plmnid, "mnc");
    strcpy(mcc, plmnid_mcc->valuestring);
    strcpy(mnc, plmnid_mnc->valuestring);
    plmn_id_build(ptr_plmnid, atoi(mcc), atoi(mnc), strlen(mnc));
    return;
}

void _add_imsi_to_struct(cJSON* json_key, c_uint8_t *pImsi, c_int32_t *imsi_len) {
    cJSON *j_imsi = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_IMSI);
    if (j_imsi != NULL)
    {
        core_bcd_to_buffer(j_imsi->valuestring, pImsi, imsi_len);
    }
}


void _add_uld_to_struct(cJSON* json_key, create_session_t *sess) {
    
    cJSON *uli = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_ULI);
    cJSON *eutra = cJSON_GetObjectItemCaseSensitive(uli, JSONKEY_4G_ULI_EUTRAL);
    cJSON *tai = cJSON_GetObjectItemCaseSensitive(eutra, JSONKEY_4G_ULI_EUTRAL_TAI);
    conv_json_to_plmnid(tai, &sess->tai.plmn_id, JSONKEY_4G_ULI_EUTRAL_TAI_PLMNID);
    cJSON *j_tai_tac = cJSON_GetObjectItemCaseSensitive(tai, JSONKEY_4G_ULI_EUTRAL_TAI_TAC);
    sess->tai.tac = atoi(j_tai_tac->valuestring);

    cJSON *e_cgi = cJSON_GetObjectItemCaseSensitive(eutra, JSONKEY_4G_ULI_EUTRAL_ECGI);
    conv_json_to_plmnid(e_cgi, &sess->e_cgi.plmn_id, JSONKEY_4G_ULI_EUTRAL_ECGI_PLMNID);
    cJSON *e_cgi_eutraCellId = cJSON_GetObjectItemCaseSensitive(e_cgi, JSONKEY_4G_ULI_EUTRAL_ECGI_EUTRACELLID);
    sess->e_cgi.cell_id = atoi(e_cgi_eutraCellId->valuestring);
}

void _add_serving_network_to_struct(cJSON* json_key, plmn_id_t *visited_plmn_id) {
    conv_json_to_plmnid(json_key, visited_plmn_id, JSONKEY_4G_SERVINGNETWORK);
}

void _add_radio_type_to_struct(cJSON* json_key, c_uint8_t *radioType) {

    memset(radioType, 0x00, 10);
    cJSON *j_ratType = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_RATTYPE);
    memcpy(radioType, j_ratType->valuestring, strlen(j_ratType->valuestring));
    
}

void _add_pco_to_struct(cJSON* json_key, ue_pco_t *ue_pco) {

    cJSON *j_pco = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_PCO);
    ue_pco->length = strlen(j_pco->valuestring) / 2;
    core_ascii_to_hex(j_pco->valuestring, ue_pco->length * 2, ue_pco->buffer, ue_pco->length);
}

void _add_apn_to_struct(cJSON* json_key, c_int8_t *apn) {
    cJSON *j_apn = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_APN);
    strcpy(apn, j_apn->valuestring);
}

void _add_pdn_to_struct(cJSON *json_key, pdn_t *pdn) {
    c_sockaddr_t addr;
    char paa_pdn_type[1+1] = {0};

    cJSON *j_pdn = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_PDN);
    cJSON *j_pdn_paa = cJSON_GetObjectItemCaseSensitive(j_pdn, JSONKEY_4G_PDN_PAA); // PDN Address Allocation 
    cJSON *j_pdn_paa_pdntype = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_PDNTYPE);
    cJSON *j_pdn_paa_addr = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_ADDR);
    cJSON *j_pdn_paa_addr6 = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_ADDR6);
    // cJSON *j_pdn_paa_len = cJSON_GetObjectItemCaseSensitive(j_pdn_paa, JSONKEY_4G_PDN_PAA_LEN);
    bzero(paa_pdn_type, 1+1); 
    
    // PDN Type
    strcpy(paa_pdn_type, j_pdn_paa_pdntype->valuestring);
    pdn->paa.pdn_type = atoi(paa_pdn_type);

    switch (pdn->paa.pdn_type) {
        case SBI_PDN_TYPE_IPV4:
        {
            core_inet_pton(AF_INET, j_pdn_paa_addr->valuestring, &addr);
            memcpy((void *)&pdn->paa.addr, (void *)&addr.sin.sin_addr, IPV4_LEN);
            break;
        }
        case SBI_PDN_TYPE_IPV6:
        {
            char buf[INET6_ADDRSTRLEN];
            core_inet_pton(AF_INET6, j_pdn_paa_addr6->valuestring, &addr);
            memcpy((void *)&pdn->paa.addr6, (void *)&addr.sin6.sin6_addr, IPV6_LEN);
            CORE_ADDR(&addr, buf);
            break;
        }
        case SBI_PDN_TYPE_IPV4V6:
        {
            core_inet_pton(AF_INET, j_pdn_paa_addr->valuestring, &addr);
            memcpy((void *)&pdn->paa.both.addr, (void *)&addr.sin.sin_addr, IPV4_LEN);
            core_inet_pton(AF_INET6, j_pdn_paa_addr6->valuestring, &addr);
            memcpy((void *)&pdn->paa.both.addr6, (void *)&addr.sin6.sin6_addr, IPV6_LEN);
            break;
        }
        default:
            d_error("Not support PDN Type");
    }

    cJSON *j_pdn_ambr = cJSON_GetObjectItemCaseSensitive(j_pdn, JSONKEY_4G_PDN_AMBR);
    cJSON *j_pdn_ambr_uplink = cJSON_GetObjectItemCaseSensitive(j_pdn_ambr, JSONKEY_4G_PDN_AMBR_UPLINK);
    cJSON *j_pdn_ambr_downlink = cJSON_GetObjectItemCaseSensitive(j_pdn_ambr, JSONKEY_4G_PDN_AMBR_DOWMLINK);
    if (j_pdn_ambr_uplink && j_pdn_ambr_downlink) {
        pdn->ambr.uplink = atoi(j_pdn_ambr_uplink->valuestring);
        pdn->ambr.downlink = atoi(j_pdn_ambr_downlink->valuestring);
    }
    
    cJSON *j_pdn_pdnType = cJSON_GetObjectItemCaseSensitive(j_pdn, JSONKEY_4G_PDN_PDNTYPE);
    pdn->pdn_type = atoi(j_pdn_pdnType->valuestring);

}
 void _add_ebi_to_struct(cJSON* json_key, c_uint8_t *ebi) {
    cJSON *j_modify_bearer_ctx = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_M_BEARER_CTX);
    cJSON *j_ebi = cJSON_GetObjectItemCaseSensitive(j_modify_bearer_ctx, JSONKEY_4G_EBI);
    int tmpData = 0;
    tmpData = atoi(j_ebi->valuestring);
    *ebi = tmpData;
 }

 void _add_gummei_to_struct(cJSON* json_key, guti_t *guti) {
    cJSON *gummei = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_GUMMEI);    
    conv_json_to_plmnid(gummei, &guti->plmn_id, JSONKEY_4G_GUMMEI_PLMNID);
    cJSON *gummei_mmegid = cJSON_GetObjectItemCaseSensitive(gummei, JSONKEY_4G_GUMMEI_MMEGID);
    cJSON *gummei_mmecode = cJSON_GetObjectItemCaseSensitive(gummei, JSONKEY_4G_GUMMEI_MMECODE);
    guti->mme_gid = atoi(gummei_mmegid->valuestring);
    guti->mme_code = atoi(gummei_mmecode->valuestring);
 }

status_t JSONTRANSFORM_JsToSt_create_session_request(create_session_t *sess, cJSON *pJson)
{
    d_trace(10, "JSONTRANSFORM_JsToSt_create_session_request\n");
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* imsi */
        d_trace(15, "\tParse IMSI\n");
        _add_imsi_to_struct(pJson, sess->imsi, &sess->imsi_len);
        
        /* user location information */
        d_trace(15, "\tParse ULI\n");
        _add_uld_to_struct(pJson, sess);
        
        /* serving network */
        d_trace(15, "\tParse ServingNetwork\n");
        _add_serving_network_to_struct(pJson, &sess->visited_plmn_id);

        /* radio access technology */
        d_trace(15, "\tParse RAT\n");
        _add_radio_type_to_struct(pJson, sess->rat_type);

        /* protocol_configuration_options(nas) */
        d_trace(15, "\tParse PCO\n");
        _add_pco_to_struct(pJson, &sess->ue_pco);
        
        /* APN */
        d_trace(15, "\tParse APN\n");
        _add_apn_to_struct(pJson, sess->apn);

        /* packet data network */
        d_trace(15, "\tParse PDN\n");
        _add_pdn_to_struct(pJson, &sess->pdn);

        /* ebi */
        d_trace(15, "\tParse EBI\n");
        c_int16_t ebi;
        cJSON *j_ebi = cJSON_GetObjectItemCaseSensitive(pJson, JSONKEY_4G_EBI);
        ebi = atoi(j_ebi->valuestring);
        sess->ebi = ebi;

        /* gummei */
        d_trace(15, "\tParse GUMMEI\n");
        _add_gummei_to_struct(pJson, &sess->guti);
    } while(0);

    return CORE_OK;
}

void _add_uld_to_modfiy_bearer_struct(cJSON* json_key, modify_bearer_t *pModifyBearer) {
    
    cJSON *uli = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_ULI);
    cJSON *eutra = cJSON_GetObjectItemCaseSensitive(uli, JSONKEY_4G_ULI_EUTRAL);
    cJSON *tai = cJSON_GetObjectItemCaseSensitive(eutra, JSONKEY_4G_ULI_EUTRAL_TAI);
    conv_json_to_plmnid(tai, &pModifyBearer->tai.plmn_id, JSONKEY_4G_ULI_EUTRAL_TAI_PLMNID);
    cJSON *j_tai_tac = cJSON_GetObjectItemCaseSensitive(tai, JSONKEY_4G_ULI_EUTRAL_TAI_TAC);
    pModifyBearer->tai.tac = atoi(j_tai_tac->valuestring);

    cJSON *e_cgi = cJSON_GetObjectItemCaseSensitive(eutra, JSONKEY_4G_ULI_EUTRAL_ECGI);
    conv_json_to_plmnid(e_cgi, &pModifyBearer->e_cgi.plmn_id, JSONKEY_4G_ULI_EUTRAL_ECGI_PLMNID);
    cJSON *e_cgi_eutraCellId = cJSON_GetObjectItemCaseSensitive(e_cgi, JSONKEY_4G_ULI_EUTRAL_ECGI_EUTRACELLID);
    pModifyBearer->e_cgi.cell_id = atoi(e_cgi_eutraCellId->valuestring);
}


void _add_json_to_struct_ui8_by_key(cJSON* json_key, c_uint8_t *ui8value, const char *jsonkeyString) {
    
    cJSON *j_sub_data = cJSON_GetObjectItemCaseSensitive(json_key, jsonkeyString);
    int tmpData = 0;
    tmpData = atoi(j_sub_data->valuestring);
    *ui8value = tmpData;
}

void _add_json_to_struct_ui32_by_key(cJSON* json_key, c_uint32_t *ui32value, const char *jsonkeyString) {
    
    cJSON *j_sub_data = cJSON_GetObjectItemCaseSensitive(json_key, jsonkeyString);
    int tmpData = 0;
    tmpData = atoi(j_sub_data->valuestring);
    *ui32value = tmpData;
 }

void _add_ipt_to_struct(cJSON* json_key, ip_t *pIP)
{
    c_sockaddr_t addr;
    cJSON *j_enb_s1u_ip = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP);
    cJSON *j_enb_s1u_ip4 = cJSON_GetObjectItemCaseSensitive(j_enb_s1u_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_IPV4);
    cJSON *j_enb_s1u_ip6 = cJSON_GetObjectItemCaseSensitive(j_enb_s1u_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_IPV6);
    cJSON *j_enb_s1u_ip_addr4 = cJSON_GetObjectItemCaseSensitive(j_enb_s1u_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_ADDR);
    cJSON *j_enb_s1u_ip_addr6 = cJSON_GetObjectItemCaseSensitive(j_enb_s1u_ip, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_IP_ADDR6);

    pIP->ipv4 = atoi(j_enb_s1u_ip4->valuestring);
    pIP->ipv6 = atoi(j_enb_s1u_ip6->valuestring);

    if ((pIP->ipv4 == 1) && (pIP->ipv6 ==1)) {
        core_inet_pton(AF_INET, j_enb_s1u_ip_addr4->valuestring, &addr);
        memcpy((void *)&pIP->both.addr, (void *)&addr.sin.sin_addr.s_addr, IPV4_LEN);
        core_inet_pton(AF_INET6, j_enb_s1u_ip_addr6->valuestring, &addr);
        memcpy((void *)&pIP->both.addr6, (void *)&addr.sin6.sin6_addr.__in6_u.__u6_addr8, IPV6_LEN);
    }
    else if ((pIP->ipv4 == 1)) {
        core_inet_pton(AF_INET, j_enb_s1u_ip_addr4->valuestring, &addr);
        memcpy((void *)&pIP->both.addr, (void *)&addr.sin.sin_addr.s_addr, IPV4_LEN);
    }
    else if ((pIP->ipv6 == 1)) {
        core_inet_pton(AF_INET6, j_enb_s1u_ip_addr6->valuestring, &addr);
        memcpy((void *)&pIP->both.addr6, (void *)&addr.sin6.sin6_addr.__in6_u.__u6_addr8, IPV6_LEN);
    }
    else {
        d_error("error ip_t");
    }
}

void _add_enb_s1u_teid(cJSON *json_key, c_uint32_t *teid)
{
    cJSON *m_bearer_ctx = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_M_BEARER_CTX);
    _add_json_to_struct_ui32_by_key(m_bearer_ctx, teid, JSONKEY_4G_M_BEARER_CTX_ENB_S1U_TEID);
}

status_t JSONTRANSFORM_JsToSt_modify_bearer_request(modify_bearer_t *pModifyBearer, cJSON *pJson)
{
    d_trace(10, "JSONTRANSFORM_JsToSt_modify_bearer_request (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(pModifyBearer, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* sm_context_update_type */
        d_trace(15, "\tParse sm_context_update_type\n");
        _add_json_to_struct_ui8_by_key(pJson, &pModifyBearer->sm_context_update_type, JSONKEY_5G_SM_CONTEXT_UPDATE_TYPE);

        /* imsi */
        d_trace(15, "\tParse IMSI\n");
        _add_imsi_to_struct(pJson, pModifyBearer->imsi, &pModifyBearer->imsi_len);
        
        // c_uint8_t       uli_presence;
        d_trace(15, "\tParse ULI_PRESENCE\n");
        _add_json_to_struct_ui8_by_key(pJson, &pModifyBearer->uli_presence, JSONKEY_4G_ULI_PRESENCE);

        /* user location information */
        d_trace(15, "\tParse ULI\n");
        _add_uld_to_modfiy_bearer_struct(pJson, pModifyBearer);  
        
        /* serving network */
        d_trace(15, "\tParse ServingNetwork\n");
        _add_serving_network_to_struct(pJson, &pModifyBearer->visited_plmn_id);

        /* radio access technology */
        d_trace(15, "\tParse RAT\n");
        _add_radio_type_to_struct(pJson, pModifyBearer->rat_type);

        /* ebi */
        d_trace(15, "\tParse EBI\n");
        _add_ebi_to_struct(pJson, &pModifyBearer->ebi);

        /* gummei */
        d_trace(15, "\tParse GUMMEI\n");
        _add_gummei_to_struct(pJson, &pModifyBearer->guti);

        // ip_t            enb_s1u_ip;
        d_trace(15, "\tParse eNB_S1U_IP\n");
        _add_ipt_to_struct(pJson, &pModifyBearer->enb_s1u_ip);

        // c_uint32_t      enb_s1u_teid;
        d_trace(15, "\tParse eNB_S1U_TEID\n");
        _add_enb_s1u_teid(pJson, &pModifyBearer->enb_s1u_teid);

        /* APN */
        d_trace(15, "\tParse APN\n");
        _add_apn_to_struct(pJson, pModifyBearer->apn);
    } while(0);
    
    return CORE_OK;
}

void _add_upf_ipt_to_struct(cJSON* json_key, ip_t *pIP)
{
    c_sockaddr_t addr;
    cJSON *j_upf_s1u_addr4 = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_SGW_S1U_IP_IPV4);
    cJSON *j_upf_s1u_addr6 = cJSON_GetObjectItemCaseSensitive(json_key, JSONKEY_4G_SGW_S1U_IP_IPV6);

    if (j_upf_s1u_addr4 && j_upf_s1u_addr6) {
        pIP->ipv6 = 1; pIP->ipv4 = 1;
        core_inet_pton(AF_INET, j_upf_s1u_addr4->valuestring, &addr);
        memcpy((void *)&pIP->both.addr, (void *)&addr.sin.sin_addr.s_addr, IPV4_LEN);
        core_inet_pton(AF_INET6, j_upf_s1u_addr6->valuestring, &addr);
        memcpy((void *)&pIP->both.addr6, (void *)&addr.sin6.sin6_addr.__in6_u.__u6_addr8, IPV6_LEN);
    }
    else if (j_upf_s1u_addr4) {
        core_inet_pton(AF_INET, j_upf_s1u_addr4->valuestring, &addr);
        pIP->ipv6 = 0; pIP->ipv4 = 1;
        memcpy((void *)&pIP->addr, (void *)&addr.sin.sin_addr.s_addr, IPV4_LEN);
    }
    else if (j_upf_s1u_addr6) {
        core_inet_pton(AF_INET6, j_upf_s1u_addr6->valuestring, &addr);
        pIP->ipv6 = 1; pIP->ipv4 = 0;
        memcpy((void *)&pIP->addr6, (void *)&addr.sin6.sin6_addr.__in6_u.__u6_addr8, IPV6_LEN);
    }
    else {
        d_error("error ip_t");
    }
}

status_t JSONTRANSFORM_JsToSt_create_session_response(create_session_t *sess, cJSON *pJson)
{
    d_trace(10, "JSONTRANSFORM_JsToSt_create_session_response (%s:%d)\n", __FILE__, __LINE__);
    
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* imsi */
        d_trace(15, "\tParse IMSI\n");
        _add_imsi_to_struct(pJson, sess->imsi, &sess->imsi_len);

        /* upf_s1u_teid */
        d_trace(15, "\tParse UPF_S1U_TEID\n");
        _add_json_to_struct_ui32_by_key(pJson, &sess->upf_s1u_teid, JSONKEY_4G_SGW_S1U_TEID);

        /* protocol_configuration_options(nas) */
        d_trace(15, "\tParse PCO\n");
        _add_pco_to_struct(pJson, &sess->ue_pco);

        /* APN */
        d_trace(15, "\tParse APN\n");
        _add_apn_to_struct(pJson, sess->apn);

        /* APN */
        d_trace(15, "\tParse APN\n");
        _add_apn_to_struct(pJson, sess->apn);

        /* packet data network */
        d_trace(15, "\tParse PDN\n");
        _add_pdn_to_struct(pJson, &sess->pdn);

        /* ebi */
        d_trace(15, "\tParse EBI\n");
        c_int16_t ebi;
        cJSON *j_ebi = cJSON_GetObjectItemCaseSensitive(pJson, JSONKEY_4G_EBI);
        ebi = atoi(j_ebi->valuestring);
        sess->ebi = ebi;

        /* upf_ip */
        d_trace(15, "\tParse upf_IP\n");
        _add_upf_ipt_to_struct(pJson, &sess->upf_ip);
    } while(0);
    
    return CORE_OK;
}

status_t JSONTRANSFORM_StToJs_create_session_response(create_session_t *sess, cJSON *pJson)
{
    char ip4_buf[INET6_ADDRSTRLEN];
    char ip6_buf[INET6_ADDRSTRLEN];

    d_trace(10, "JSONTRANSFORM_StToJs_create_session_response (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* imsi */
        d_trace(15, "\tBuild IMSI\n");
        char imsi_bcd[MAX_IMSI_BCD_LEN+1] = {0};
        core_buffer_to_bcd(sess->imsi, sess->imsi_len, imsi_bcd);
        cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, imsi_bcd);

        /* ebi */
        d_trace(15, "\tBuild EBI\n");
        add_uint8_to_json(pJson, sess->ebi, JSONKEY_4G_EBI);

        /* upf_s1u_teid */
        d_trace(15, "\tBuild UPF_S1U_TEID\n");
        add_uint32_to_json(pJson, sess->upf_s1u_teid, JSONKEY_4G_SGW_S1U_TEID);
        
        /* PCO */
        d_trace(15, "\tBuild PCO\n");
        if (sess->ue_pco.length && sess->ue_pco.buffer)
        {
            char conv[1000] = {0};
            core_hex_to_ascii_no_space(sess->ue_pco.buffer, sess->ue_pco.length, conv, sizeof(conv));
            cJSON_AddStringToObject(pJson, JSONKEY_4G_PCO, conv);
        }

        /* upf_s1u_ip */
        d_trace(15, "\tBuild UPF_S1U_IP\n");
        if(sess->upf_ip.ipv4 && sess->upf_ip.ipv6){
            INET_NTOP(&sess->upf_ip.both.addr, ip4_buf);
            INET6_NTOP(&sess->upf_ip.both.addr6, ip6_buf);
            cJSON_AddStringToObject(pJson, JSONKEY_4G_SGW_S1U_IP_IPV4, ip4_buf);
            cJSON_AddStringToObject(pJson, JSONKEY_4G_SGW_S1U_IP_IPV6, ip6_buf);
        }
        else if(sess->upf_ip.ipv4){
            INET_NTOP(&sess->upf_ip.addr, ip4_buf);
            cJSON_AddStringToObject(pJson, JSONKEY_4G_SGW_S1U_IP_IPV4, ip4_buf);
        }
        else if(sess->upf_ip.ipv6){
            INET6_NTOP(&sess->upf_ip.addr6, ip6_buf);
            cJSON_AddStringToObject(pJson, JSONKEY_4G_SGW_S1U_IP_IPV6, ip6_buf);
        }

        /* APN */
        d_trace(15, "\tBuild APN\n");
        cJSON_AddStringToObject(pJson, JSONKEY_4G_APN, sess->apn);

        /* PDN Address Allocation */
        d_trace(15, "\tBuild PAA\n");
        add_pdn_to_json(pJson , &sess->pdn);
    } while(0);

    return CORE_OK;
}
         
status_t JSONTRANSFORM_JsToSt_update_session_response(modify_bearer_t *sess, cJSON *pJson)
{
    d_trace(10, "JSONTRANSFORM_JsToSt_update_session_response (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* sm_context_update_type */
        d_trace(15, "\tParse sm_context_update_type\n");
        _add_json_to_struct_ui8_by_key(pJson, &sess->sm_context_update_type, JSONKEY_5G_SM_CONTEXT_UPDATE_TYPE);

        /* imsi */
        d_trace(15, "\tParse IMSI\n");
        _add_imsi_to_struct(pJson, sess->imsi, &sess->imsi_len);
    } while(0);

    return CORE_OK;
}


status_t JSONTRANSFORM_StToJs_update_session_response(modify_bearer_t *sess, cJSON *pJson)
{
    d_trace(10, "JSONTRANSFORM_StToJs_update_session_response (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* sm_context_update_type */
        d_trace(15, "\tBuild sm_context_update_type\n");
        add_uint8_to_json(pJson, sess->sm_context_update_type, JSONKEY_5G_SM_CONTEXT_UPDATE_TYPE);
        
        /* imsi */
        d_trace(15, "\tBuild IMSI\n");
        char imsi_bcd[MAX_IMSI_BCD_LEN+1] = {0};
        core_buffer_to_bcd(sess->imsi, sess->imsi_len, imsi_bcd);
        cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, imsi_bcd);
    } while(0);

    return CORE_OK;
}

status_t JSONTRANSFORM_JsToSt_delete_session_request(delete_session_t *sess, cJSON *pJson)
{
    d_trace(10, "JSONTRANSFORM_JsToSt_delete_session_request (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* imsi */
        d_trace(15, "\tParse IMSI\n");
        _add_imsi_to_struct(pJson, sess->imsi, &sess->imsi_len);

        /* apn */
        d_trace(15, "\tParse APN\n");
        _add_apn_to_struct(pJson, sess->apn);

        /* EBI */
        d_trace(15, "\tParse EBI\n");
        cJSON *j_ebi = cJSON_GetObjectItemCaseSensitive(pJson, JSONKEY_4G_EBI);
        int tmpData = 0;
        tmpData = atoi(j_ebi->valuestring);
        sess->ebi = tmpData;
    } while(0);
    return CORE_OK;
}

status_t JSONTRANSFORM_StToJs_delete_session_request(delete_session_t *sess, cJSON *pJson)
{
    char imsi_bcd[MAX_IMSI_BCD_LEN+1] = {0};

    d_trace(10, "JSONTRANSFORM_StToJs_delete_session_request (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* imsi */
        d_trace(15, "\tBuild IMSI\n");
        core_buffer_to_bcd(sess->imsi, sess->imsi_len, imsi_bcd);
        cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, imsi_bcd);

        /* apn */
        d_trace(15, "\tBuild APN\n");
        cJSON_AddStringToObject(pJson, JSONKEY_4G_APN, sess->apn);

        /* EBI */
        d_trace(15, "\tBuild EBI\n");
        add_uint8_to_json(pJson, sess->ebi, JSONKEY_4G_EBI);
    } while(0);

    return CORE_OK;
}

status_t JSONTRANSFORM_JsToSt_delete_session_response(delete_session_t *sess, cJSON *pJson)
{
    d_trace(10, "JSONTRANSFORM_JsToSt_delete_session_response (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* imsi */
        d_trace(15, "\tParse IMSI\n");
        _add_imsi_to_struct(pJson, sess->imsi, &sess->imsi_len);

        /* EBI */
        d_trace(15, "\tParse EBI\n");
        cJSON *j_ebi = cJSON_GetObjectItemCaseSensitive(pJson, JSONKEY_4G_EBI);
        int tmpData = 0;
        tmpData = atoi(j_ebi->valuestring);
        sess->ebi = tmpData;
    } while(0);

    return CORE_OK;
}

status_t JSONTRANSFORM_StToJs_delete_session_response(delete_session_t *sess, cJSON *pJson)
{
    char imsi_bcd[MAX_IMSI_BCD_LEN + 1] = {0};

    d_trace(10, "JSONTRANSFORM_StToJs_delete_session_response (%s:%d)\n", __FILE__, __LINE__);
    do {
        d_assert(sess, break, "Null Param");
        d_assert(pJson, break, "Null Param");

        /* imsi */
        d_trace(15, "\tBuild IMSI\n");
        core_buffer_to_bcd(sess->imsi, sess->imsi_len, imsi_bcd);
        cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, imsi_bcd);

        /* EBI */
        d_trace(15, "\tBuild EBI\n");
        add_uint8_to_json(pJson, sess->ebi, JSONKEY_4G_EBI);
    } while(0);

    return CORE_OK;
}
