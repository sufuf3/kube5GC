#ifndef __NGAP_CONV_H__
#define __NGAP_CONV_H__

#include "ngap/ngap_message.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(void) ngap_uint8_to_OCTET_STRING(
        c_uint8_t uint8, OCTET_STRING_t *octet_string);
CORE_DECLARE(void) ngap_uint16_to_OCTET_STRING(
        c_uint16_t uint16, OCTET_STRING_t *octet_string);
CORE_DECLARE(void) ngap_uint32_to_OCTET_STRING(
        c_uint32_t uint32, OCTET_STRING_t *octet_string);

CORE_DECLARE(void) ngap_buffer_to_OCTET_STRING(
        void *buf, int size, OCTET_STRING_t *octet_string);

CORE_DECLARE(void) ngap_uint32_to_GNB_ID(
        NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_GNB_ID_t *gNB_ID);
CORE_DECLARE(void) ngap_GNB_ID_to_uint32(
        NGAP_GNB_ID_t *gNB_ID, c_uint32_t *uint32);

CORE_DECLARE(status_t) ngap_BIT_STRING_to_ip(
        BIT_STRING_t *bit_string, ip_t *ip);
CORE_DECLARE(status_t) ngap_ip_to_BIT_STRING(
        ip_t *ip, BIT_STRING_t *bit_string);

CORE_DECLARE(status_t) ngap_copy_ie(
        const asn_TYPE_descriptor_t *td, void *src, void *dst);

CORE_DECLARE(void) ngap_uint32_to_longMacroNgENB_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_NgENB_ID_t *NgENB_ID);

CORE_DECLARE(void) ngap_uint32_to_shortMacroNgENB_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_NgENB_ID_t *NgENB_ID);

CORE_DECLARE(void) ngap_uint32_to_macroNgENB_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_NgENB_ID_t *NgENB_ID);

CORE_DECLARE(void) ngap_uint32_to_3IWF_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_N3IWF_ID_t *n3IWF_ID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NGAP_CONV_H__ */

