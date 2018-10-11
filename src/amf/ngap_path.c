#define TRACE_MODULE _ngap_path

#include "core_debug.h"
#include "core_thread.h"

#include "mme_event.h"

#include "nas_security.h"
#include "nas_path.h"

#include "ngap_conv.h"
#include "ngap_build.h"
#include "ngap_path.h"

static status_t ngap_server_list(list_t *list, int type);
static status_t ngap_delete_list(list_t *list);

status_t ngap_open(void)
{
    status_t rv;
#if USE_USRSCTP != 1
    int type = SOCK_STREAM;
#else
    int type = SOCK_SEQPACKET;
#endif

    rv = ngap_server_list(&mme_self()->ngap_list, type);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    rv = ngap_server_list(&mme_self()->ngap_list6, type);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    return CORE_OK;
}

status_t ngap_close()
{
    ngap_delete_list(&mme_self()->ngap_list);
    ngap_delete_list(&mme_self()->ngap_list6);

    return CORE_OK;
}

static status_t ngap_server_list(list_t *list, int type)
{
    status_t rv;
    sock_node_t *snode = NULL;

    d_assert(list, return CORE_ERROR,);

    for (snode = list_first(list); snode; snode = list_next(snode))
    {
        rv = ngap_server(snode, type);
        d_assert(rv == CORE_OK, return CORE_ERROR,);
    }

    return CORE_OK;
}


static status_t ngap_delete_list(list_t *list)
{
    sock_node_t *snode = NULL;

    for (snode = list_first(list); snode; snode = list_next(snode))
    {
        ngap_delete(snode->sock);
    }

    return CORE_OK;
}

status_t ngap_send_to_ran(amf_ran_t *ran, pkbuf_t *pkbuf, c_uint16_t stream_no)
{
    char buf[CORE_ADDRSTRLEN];
    status_t rv;

    d_assert(ran, return CORE_ERROR,);
    d_assert(pkbuf, return CORE_ERROR,);
    d_assert(ran->sock, return CORE_ERROR,);

    d_trace(5, "    IP[%s] GNB_ID[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->gnb_id);

    rv = ngap_send(ran->sock, pkbuf,
            ran->sock_type == SOCK_STREAM ? NULL : ran->addr,
            stream_no);
    if (rv != CORE_OK)
    {
        d_error("ng_send error");
        pkbuf_free(pkbuf);
    }

    return CORE_OK;;
}

status_t ngap_send_to_ran_ue(ran_ue_t *ran_ue, pkbuf_t *pkbuf)
{
    amf_ran_t *ran = NULL;
    amf_ue_t *amf_ue = NULL;

    d_assert(ran_ue, return CORE_ERROR,);
    ran = ran_ue->ran;
    d_assert(ran, return CORE_ERROR,);
    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return CORE_ERROR,);

    return ngap_send_to_ran(ran, pkbuf, amf_ue->ostream_id);
}

status_t ngap_delayed_send_to_ran_ue(
        ran_ue_t *ran_ue, pkbuf_t *pkbuf, c_uint32_t duration)
{
    tm_block_id timer = 0;

    d_assert(ran_ue, return CORE_ERROR,);
    d_assert(pkbuf, return CORE_ERROR,);

    if (duration)
    {
        timer = timer_create(
                &mme_self()->tm_service, AMF_EVT_NGAP_DELAYED_SEND, duration);
        d_assert(timer, return CORE_ERROR,);

        timer_set_param1(timer, (c_uintptr_t)ran_ue->index);
        timer_set_param2(timer, (c_uintptr_t)pkbuf);
        timer_set_param3(timer, timer);

        tm_start(timer);

        return CORE_OK;
    }
    else
    {
        amf_ran_t *ran = NULL;
        ran = ran_ue->ran;
        d_assert(ran, return CORE_ERROR,);
        return ngap_send_to_ran_ue(ran_ue, pkbuf);
    }
}