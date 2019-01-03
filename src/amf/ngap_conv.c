#define TRACE_MODULE _ngap_conv

#include "core_debug.h"
#include "core_network.h"

#include "3gpp_types.h"
#include "ngap_conv.h"

void ngap_uint8_to_OCTET_STRING(c_uint8_t uint8, OCTET_STRING_t *octet_string)
{
    octet_string->size = 1;
    octet_string->buf = core_calloc(octet_string->size, sizeof(c_uint8_t));

    octet_string->buf[0] = uint8;
}

void ngap_uint16_to_OCTET_STRING(c_uint16_t uint16, OCTET_STRING_t *octet_string)
{
    octet_string->size = 2;
    octet_string->buf = core_calloc(octet_string->size, sizeof(c_uint8_t));

    octet_string->buf[0] = uint16 >> 8;
    octet_string->buf[1] = uint16;
}

void ngap_uint32_to_OCTET_STRING(c_uint32_t uint32, OCTET_STRING_t *octet_string)
{
    octet_string->size = 4;
    octet_string->buf = core_calloc(octet_string->size, sizeof(c_uint8_t));

    octet_string->buf[0] = uint32 >> 24;
    octet_string->buf[1] = uint32 >> 16;
    octet_string->buf[2] = uint32 >> 8;
    octet_string->buf[3] = uint32;
}

void ngap_buffer_to_OCTET_STRING( 
        void *buf, int size, OCTET_STRING_t *octet_string) // S1AP is TBCD-STRING
{
    octet_string->size = size;
    octet_string->buf = core_calloc(octet_string->size, sizeof(c_uint8_t));

    memcpy(octet_string->buf, buf, size);
}
void ngap_uint32_to_GNB_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_GNB_ID_t *gNB_ID)
{
    d_assert(gNB_ID, return, "Null param");

    gNB_ID->present = present;
    if (present == NGAP_GNB_ID_PR_gNB_ID)
    {
        BIT_STRING_t *bit_string = &gNB_ID->choice.gNB_ID;
        d_assert(bit_string, return, "Null param");

        bit_string->size = 4;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));

        bit_string->buf[0] = gnb_id >> 24;
        bit_string->buf[1] = gnb_id >> 16;
        bit_string->buf[2] = gnb_id >> 8;
        bit_string->buf[3] = gnb_id & 0xff;

    } 
    else
    {
        d_assert(0, return, "Invalid param");
    }

}

void ngap_GNB_ID_to_uint32(NGAP_GNB_ID_t *gNB_ID, c_uint32_t *uint32)
{
    d_assert(uint32, return, "Null param");
    d_assert(gNB_ID, return, "Null param");

    
    if (gNB_ID->present == NGAP_GNB_ID_PR_gNB_ID)
    {
        c_uint8_t *buf = gNB_ID->choice.gNB_ID.buf;
        d_assert(buf, return, "Null param");
        *uint32 = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] & 0xff);
    }
    else
    {
        d_assert(0, return, "Invalid param");
    }
}



status_t ngap_BIT_STRING_to_ip(BIT_STRING_t *bit_string, ip_t *ip)
{
    char buf[CORE_ADDRSTRLEN], buf2[CORE_ADDRSTRLEN];

    d_assert(bit_string, return CORE_ERROR,);
    d_assert(ip, return CORE_ERROR,);

    if (bit_string->size == IPV4V6_LEN)
    {
        ip->ipv4 = 1;
        ip->ipv6 = 1;
        memcpy(&ip->both.addr, bit_string->buf, IPV4_LEN);
        memcpy(&ip->both.addr6, bit_string->buf+IPV4_LEN, IPV6_LEN);
        d_trace(5, "    IPv4[%s] IPv6[%s]\n",
            INET_NTOP(&ip->both.addr, buf), INET6_NTOP(&ip->both.addr6, buf2));
    }
    else if (bit_string->size == IPV4_LEN)
    {
        ip->ipv4 = 1;
        memcpy(&ip->addr, bit_string->buf, IPV4_LEN);
        d_trace(5, "    IPv4[%s]\n", INET_NTOP(&ip->addr, buf));
    }
    else if (bit_string->size == IPV6_LEN)
    {
        ip->ipv6 = 1;
        memcpy(&ip->addr6, bit_string->buf, IPV6_LEN);
        d_trace(5, "    IPv6[%s]\n", INET_NTOP(&ip->addr6, buf));
    }
    else
        d_assert(0, return CORE_ERROR, "Invalid Length(%d)", bit_string->size);

    ip->len =  bit_string->size;

    return CORE_OK;
}
status_t ngap_ip_to_BIT_STRING(ip_t *ip, BIT_STRING_t *bit_string)
{
    char buf[CORE_ADDRSTRLEN], buf2[CORE_ADDRSTRLEN];

    d_assert(ip, return CORE_ERROR,);
    d_assert(bit_string, return CORE_ERROR,);

    if (ip->ipv4 && ip->ipv6)
    {
        bit_string->size = IPV4V6_LEN;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));
        memcpy(bit_string->buf, &ip->both.addr, IPV4_LEN);
        memcpy(bit_string->buf+IPV4_LEN, &ip->both.addr6, IPV6_LEN);
        d_trace(5, "    IPv4[%s] IPv6[%s]\n",
            INET_NTOP(&ip->both.addr, buf), INET6_NTOP(&ip->both.addr6, buf2));
    }
    else if (ip->ipv4)
    {
        bit_string->size = IPV4_LEN;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));
        memcpy(bit_string->buf, &ip->addr, IPV4_LEN);
        d_trace(5, "    IPv4[%s]\n", INET_NTOP(&ip->addr, buf));
    }
    else if (ip->ipv6)
    {
        bit_string->size = IPV6_LEN;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));
        memcpy(bit_string->buf, &ip->addr6, IPV6_LEN);
        d_trace(5, "    IPv6[%s]\n", INET_NTOP(&ip->addr6, buf));
    }
    else
        d_assert(0, return CORE_ERROR,);

    return CORE_OK;
}

status_t ngap_copy_ie(const asn_TYPE_descriptor_t *td, void *src, void *dst)
{
    asn_enc_rval_t enc_ret = {0};
    asn_dec_rval_t dec_ret = {0};
    c_uint8_t buffer[MAX_SDU_LEN];

    d_assert(td, return CORE_ERROR,);
    d_assert(src, return CORE_ERROR,);
    d_assert(dst, return CORE_ERROR,);

    enc_ret = aper_encode_to_buffer(td, NULL, src, buffer, MAX_SDU_LEN);
    if (enc_ret.encoded < 0)
    {
        d_error("aper_encode_to_buffer() failed[%d]", enc_ret.encoded);
        return CORE_ERROR;
    }

    dec_ret = aper_decode(NULL, td, (void **)&dst,
            buffer, (enc_ret.encoded >> 3), 0, 0);

    if (dec_ret.code != RC_OK) 
    {
        d_error("aper_decode() failed[%d]", dec_ret.code);
        return CORE_ERROR;
    }

    return CORE_OK;
}

void ngap_uint32_to_macroNgENB_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_NgENB_ID_t *NgENB_ID)
{
    d_assert(NgENB_ID, return, "Null param");

    NgENB_ID->present = present;
    if (present == NGAP_GNB_ID_PR_gNB_ID)
    {
        BIT_STRING_t *bit_string = &NgENB_ID->choice.macroNgENB_ID;
        d_assert(bit_string, return, "Null param");

        bit_string->size = 3;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));

        bit_string->buf[0] = gnb_id >> 24;
        bit_string->buf[1] = gnb_id >> 16;
        bit_string->buf[2] = gnb_id >> 8;
        // bit_string->buf[3] = gnb_id & 0xff;

    } 
    else
    {
        d_assert(0, return, "Invalid param");
    }

}

void ngap_uint32_to_shortMacroNgENB_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_NgENB_ID_t *NgENB_ID)
{
    d_assert(NgENB_ID, return, "Null param");

    NgENB_ID->present = present;
    if (present == NGAP_GNB_ID_PR_gNB_ID)
    {
        BIT_STRING_t *bit_string = &NgENB_ID->choice.shortMacroNgENB_ID;
        d_assert(bit_string, return, "Null param");

        bit_string->size = 3;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));

        bit_string->buf[0] = gnb_id >> 24;
        bit_string->buf[1] = gnb_id >> 16;
        bit_string->buf[2] = gnb_id >> 8;
        // bit_string->buf[3] = gnb_id & 0xff;

    } 
    else
    {
        d_assert(0, return, "Invalid param");
    }

}

void ngap_uint32_to_longMacroNgENB_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_NgENB_ID_t *NgENB_ID)
{
    d_assert(NgENB_ID, return, "Null param");

    NgENB_ID->present = present;
    if (present == NGAP_GNB_ID_PR_gNB_ID)
    {
        BIT_STRING_t *bit_string = &NgENB_ID->choice.longMacroNgENB_ID;
        d_assert(bit_string, return, "Null param");

        bit_string->size = 3;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));

        bit_string->buf[0] = gnb_id >> 24;
        bit_string->buf[1] = gnb_id >> 16;
        bit_string->buf[2] = gnb_id >> 8;
        // bit_string->buf[3] = gnb_id & 0xff;

    } 
    else
    {
        d_assert(0, return, "Invalid param");
    }

}


void ngap_uint32_to_3IWF_ID(
    NGAP_GNB_ID_PR present, c_uint32_t gnb_id, NGAP_N3IWF_ID_t *n3IWF_ID)
{
    d_assert(n3IWF_ID, return, "Null param");

    n3IWF_ID->present = present;
    if (present == NGAP_GNB_ID_PR_gNB_ID)
    {
        BIT_STRING_t *bit_string = &n3IWF_ID->choice.n3IWF_ID;
        d_assert(bit_string, return, "Null param");

        bit_string->size = 3;
        bit_string->buf = core_calloc(bit_string->size, sizeof(c_uint8_t));

        bit_string->buf[0] = gnb_id >> 24;
        bit_string->buf[1] = gnb_id >> 16;
        bit_string->buf[2] = gnb_id >> 8;
        // bit_string->buf[3] = gnb_id & 0xff;

    } 
    else
    {
        d_assert(0, return, "Invalid param");
    }

}