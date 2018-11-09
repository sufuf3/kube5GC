#ifndef _NGAP_MESSAGE_H__
#define _NGAP_MESSAGE_H__

#include "core_pkbuf.h"
#include "ngap_asn1c.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Octets */
#define NGAP_CLEAR_DATA(__dATA) \
    do { \
        d_assert((__dATA), , "Null param"); \
        if ((__dATA)->buf) \
        { \
            CORE_FREE((__dATA)->buf); \
            (__dATA)->buf = NULL; \
            (__dATA)->size = 0; \
        } \
    } while(0)
#define NGAP_STORE_DATA(__dST, __sRC) \
    do { \
        d_assert((__sRC),, "Null param") \
        d_assert((__sRC)->buf,, "Null param") \
        d_assert((__dST),, "Null param") \
        NGAP_CLEAR_DATA(__dST); \
        (__dST)->size = (__sRC)->size; \
        (__dST)->buf = core_calloc((__dST)->size, sizeof(c_uint8_t)); \
        memcpy((__dST)->buf, (__sRC)->buf, (__dST)->size); \
    } while(0)

typedef struct NGAP_NGAP_PDU ngap_message_t;

CORE_DECLARE(int) ngap_decode_pdu(ngap_message_t *message, pkbuf_t *pkbuf);
CORE_DECLARE(int) ngap_encode_pdu(pkbuf_t **pkbuf, ngap_message_t *message);
CORE_DECLARE(int) ngap_free_pdu(ngap_message_t *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

