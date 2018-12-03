#include "core_debug.h"
#include "core_pkbuf.h"
#include "core_network.h"
#include "JsonTransform.h"

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
    cJSON_AddStringToObject(pdn, JSONKEY_4G_PDN_PDNTYPE, str);
    cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_PDNTYPE, str);
    char buf[CORE_ADDRSTRLEN], buf2[CORE_ADDRSTRLEN];
    
    if (pdn_type == SBI_PDN_TYPE_IPV4){
        INET_NTOP(&_pdn->paa.addr, buf);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR, buf);
    }
    else if (pdn_type == SBI_PDN_TYPE_IPV6){
        INET6_NTOP(&_pdn->paa.addr6, buf);
        bzero(str, 20);
        sprintf(str, "%u", _pdn->paa.both.len);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_LEN, str);
        cJSON_AddStringToObject(paa, JSONKEY_4G_PDN_PAA_ADDR6, buf);
    }
    else if (pdn_type == SBI_PDN_TYPE_IPV4V6){
        INET_NTOP(&_pdn->paa.both.addr, buf);
        INET6_NTOP(&_pdn->paa.both.addr6, buf2);
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
    cJSON_AddStringToObject(pJson, JSONKEY_4G_IMSI, sess->imsi_bcd);
    /* user location information */
    add_uli_to_json(pJson, sess->tai, sess->e_cgi);

    /* serving network */
    add_servingnw_to_json(pJson, sess->visited_plmn_id);

    /* radio access technology */
    cJSON_AddStringToObject(pJson, JSONKEY_4G_RATTYPE, EUTRA);

    /* protocol_configuration_options(nas) */
    if (sess->ue_pco.length && sess->ue_pco.buffer)
    {
        char conv[1000] = {0};
        cJSON_AddStringToObject(pJson, JSONKEY_4G_PCO, uint_to_buffer(conv, sess->ue_pco.buffer, sess->ue_pco.length));
    }
    
    /* APN */
    cJSON_AddStringToObject(pJson, JSONKEY_4G_APN, sess->apn);

    /* packet data network */
    add_pdn_to_json(pJson, sess->pdn_type, &sess->pdn);
    
    /* create bearer contexts */
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
    add_gummei_to_json(pJson, sess->guti);
    
    return CORE_OK;
}

status_t JSONTRANSFORM_JsToSt_create_session_request(creat_session_t *sess, cJSON *pJson)
{
    return CORE_OK;
}