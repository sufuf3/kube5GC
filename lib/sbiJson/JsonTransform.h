#ifndef __JSONTRANSFORM__H
#define __JSONTRANSFORM__H

#include "core.h"
#include "core_errno.h"
#include "cJSON.h"
#include "commonJsonKey.h"
#include "3gpp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _ue_pco_t {
    c_uint8_t length;
    c_uint8_t buffer[MAX_PCO_LEN * 2 + 1];
}ue_pco_t;

typedef struct _create_session_t {
    c_uint8_t       imsi[MAX_IMSI_LEN];
    c_int32_t       imsi_len;
    tai_t           tai;
    e_cgi_t         e_cgi;
    plmn_id_t       visited_plmn_id;
    c_uint32_t      upf_s1u_teid;
    ue_pco_t        ue_pco;
    c_int8_t        apn[MAX_APN_LEN+1];
    pdn_t           pdn;
    c_uint8_t       ebi;
    guti_t          guti;
    ip_t            upf_ip;
    c_uint8_t       rat_type[10];
} create_session_t;

typedef struct _modify_bearer_t {
    #define SM_CONTEXT_UPDATE_TYPE_MODIFY                           0x01
    #define SM_CONTEXT_UPDATE_TYPE_RELEASE_ACCESS                   0x02
    #define SM_CONTEXT_UPDATE_TYPE_DOWNLINK_DATA_NOTIFICATION       0x03
    #define SM_CONTEXT_UPDATE_TYPE_DOWNLINK_DATA_NOTIFICATION_ACK   0x04
    c_uint8_t       sm_context_update_type;
    c_uint8_t       imsi[MAX_IMSI_LEN];
    c_int32_t       imsi_len;
    c_uint8_t       uli_presence;
    tai_t           tai;
    e_cgi_t         e_cgi;
    plmn_id_t       visited_plmn_id;
    guti_t          guti; 
    c_uint8_t       rat_type[10];
    c_uint8_t       ebi;
    ip_t            enb_s1u_ip;
    c_uint32_t      enb_s1u_teid;
    c_int8_t        apn[MAX_APN_LEN+1];
} modify_bearer_t;

typedef struct _delete_session_t {
    c_uint8_t       imsi[MAX_IMSI_LEN];
    c_int32_t       imsi_len;
    c_int8_t        apn[MAX_APN_LEN+1];
    c_uint8_t       ebi;
} delete_session_t;

CORE_DECLARE(status_t) JSONTRANSFORM_StToJs_create_session_request(create_session_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_JsToSt_create_session_request(create_session_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_StToJs_create_session_response(create_session_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_JsToSt_create_session_response(create_session_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_StToJs_modify_bearer_request(modify_bearer_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_JsToSt_modify_bearer_request(modify_bearer_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_JsToSt_update_session_response(modify_bearer_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_StToJs_update_session_response(modify_bearer_t *sess, cJSON *pJson);

// delete session
CORE_DECLARE(status_t) JSONTRANSFORM_JsToSt_delete_session_request(delete_session_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_StToJs_delete_session_request(delete_session_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_JsToSt_delete_session_response(delete_session_t *sess, cJSON *pJson);
CORE_DECLARE(status_t) JSONTRANSFORM_StToJs_delete_session_response(delete_session_t *sess, cJSON *pJson);
#ifdef __cplusplus
}
#endif 

#endif
