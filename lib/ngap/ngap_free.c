#define TRACE_MODULE _ngap_free

#include "ngap_message.h"

status_t ngap_free_pdu(ngap_message_t *message)
{
    d_assert(message, return CORE_ERROR,);

    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NGAP_NGAP_PDU, message);

    return CORE_OK;
}
