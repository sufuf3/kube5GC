#define TRACE_MODULE _ngap_decoder

#include "ngap_message.h"

status_t ngap_decode_pdu(ngap_message_t *message, pkbuf_t *pkbuf)
{
    asn_dec_rval_t dec_ret = {0};

    d_assert(message, return CORE_ERROR,);
    d_assert(pkbuf, return CORE_ERROR,);
    d_assert(pkbuf->payload, return CORE_ERROR,);

    memset((void *)message, 0, sizeof(ngap_message_t));
    dec_ret = aper_decode(NULL, &asn_DEF_NGAP_NGAP_PDU, (void **)&message, 
            pkbuf->payload, pkbuf->len, 0, 0);

    if (dec_ret.code != RC_OK) 
    {
        d_error("Failed to decode NGAP-PDU[%d]", dec_ret.code);
        return CORE_ERROR;
    }

    if (g_trace_mask && TRACE_MODULE >= 25)
        asn_fprint(stdout, &asn_DEF_NGAP_NGAP_PDU, message);

    return CORE_OK;
}
