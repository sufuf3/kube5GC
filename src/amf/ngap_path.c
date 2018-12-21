#define TRACE_MODULE _ngap_path

#include "core_debug.h"
#include "core_thread.h"

#include "amf4g_event.h"

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

    rv = ngap_server_list(&amf4g_self()->ngap_list, type);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    rv = ngap_server_list(&amf4g_self()->ngap_list6, type);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    return CORE_OK;
}

status_t ngap_close()
{
    ngap_delete_list(&amf4g_self()->ngap_list);
    ngap_delete_list(&amf4g_self()->ngap_list6);

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
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);

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
                &amf4g_self()->tm_service, AMF_EVT_NGAP_DELAYED_SEND, duration);
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

status_t ngap_send_initial_context_setup_request(amf_ue_t *amf_ue)
{
    status_t rv;
    pkbuf_t *ngapbuf = NULL;

    d_assert(amf_ue, return CORE_ERROR, "Null param");

    rv = ngap_build_initial_context_setup_request(&ngapbuf, amf_ue, NULL);
    d_assert(rv == CORE_OK && ngapbuf, return CORE_ERROR, "ngapbuf build error");

    // rv = nas_send_to_enb(amf_ue, ngapbuf);
    // d_assert(rv == CORE_OK, return CORE_ERROR, "ngap send error");

    return CORE_OK;
}

status_t ngap_send_handover_cancel_acknowledge(ran_ue_t *source_ue)
{
    status_t rv;
    pkbuf_t *ngapbuf = NULL;

    d_assert(source_ue, return CORE_ERROR,);

    rv = ngap_build_handover_cancel_acknowledge(&ngapbuf, source_ue);
    d_assert(rv == CORE_OK && ngapbuf, return CORE_ERROR, "ngap build error");

    rv = ngap_send_to_ran_ue(source_ue, ngapbuf);
    d_assert(rv == CORE_OK,, "ngap send error");

    return rv;
}

status_t ngap_send_ue_context_release_command(
    ran_ue_t *ran_ue, NGAP_Cause_PR group, long cause,
    c_uint8_t action, c_uint32_t delay)
{
    status_t rv;
    pkbuf_t *ngapbuf = NULL;

    //d_assert(action != S1AP_UE_CTX_REL_INVALID_ACTION, return CORE_ERROR,
    //        "Should give valid action for UE Context Release Command");

    d_assert(action != 0, return CORE_ERROR,
           "Should give valid action for UE Context Release Command");

    d_assert(ran_ue, return CORE_ERROR, "Null param");
    //TODO: add ue ctx rel action 
    // ran_ue->ue_ctx_rel_action = action;

    d_trace(3, "[AMF] UE Context release command\n");
    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);
    d_trace(5, "    Group[%d] Cause[%d] Action[%d] Delay[%d]\n",
            group, cause, action, delay);

    rv = ngap_build_ue_context_release_command(&ngapbuf, ran_ue, group, cause);
    d_assert(rv == CORE_OK && ngapbuf, return CORE_ERROR, "ngapbuf build error");

    rv = ngap_delayed_send_to_ran_ue(ran_ue, ngapbuf, delay);
    d_assert(rv == CORE_OK,, "ngapbuf send error");
    
    return CORE_OK;
}

status_t ngap_send_handover_preparation_failure(ran_ue_t *source_ue, NGAP_Cause_t *cause)
{
    status_t rv;
    pkbuf_t *ngapbuf = NULL;

    d_assert(source_ue, return CORE_ERROR,);
    d_assert(cause, return CORE_ERROR,);

    rv = ngap_build_handover_preparation_failure(&ngapbuf, source_ue, cause);
    d_assert(rv == CORE_OK && ngapbuf, return CORE_ERROR, "ngapbuf build error");

    rv = ngap_send_to_ran_ue(source_ue, ngapbuf);
    d_assert(rv == CORE_OK,, "ngapbuf send error");

    return rv;
}

status_t ngap_send_handover_request(
        amf_ue_t *amf_ue,
        amf_ran_t *target_ran,
        NGAP_RAN_UE_NGAP_ID_t *ran_ue_ngap_id,
        NGAP_AMF_UE_NGAP_ID_t *amf_ue_ngap_id,
        NGAP_HandoverType_t *handovertype,
        NGAP_Cause_t *cause,
        NGAP_SourceToTarget_TransparentContainer_t
            *SourceToTarget_TransparentContainer)
{
    status_t rv;
    pkbuf_t *ngapbuf = NULL;

    ran_ue_t *source_ue = NULL, *target_ue = NULL;

    d_trace(3, "[AMF] Handover request\n");
    
    d_assert(target_ran, return CORE_ERROR, "Cannot find target eNB");

    d_assert(amf_ue, return CORE_ERROR,);
    source_ue = amf_ue->ran_ue;
    d_assert(source_ue, return CORE_ERROR,);
    d_assert(source_ue->target_ue == NULL, return CORE_ERROR,
            "Handover Required Duplicated");

    target_ue = ran_ue_add(target_ran);
    d_assert(target_ue, return CORE_ERROR,);

    d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            source_ue->ran_ue_ngap_id, source_ue->amf_ue_ngap_id);
    d_trace(5, "    Target : RAN_UE_NGAP_ID[Unknown] AMF_UE_NGAP_ID[%d]\n",
            target_ue->amf_ue_ngap_id);

    rv = source_ue_associate_target_5g(source_ue, target_ue);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    rv = ngap_build_handover_request(&ngapbuf, amf_ue, target_ue,
            ran_ue_ngap_id, amf_ue_ngap_id,
            handovertype, cause,
            SourceToTarget_TransparentContainer);
    d_assert(rv == CORE_OK && ngapbuf, return CORE_ERROR, "ngap build error");

    rv = ngap_send_to_ran_ue(target_ue, ngapbuf);
    d_assert(rv == CORE_OK,, "ngap send error");

    return rv;
}

status_t ngap_send_handover_command(ran_ue_t *source_ue)
{
    status_t rv;
    pkbuf_t *ngapbuf = NULL;

    d_assert(source_ue, return CORE_ERROR,);

    rv = ngap_build_handover_command(&ngapbuf, source_ue);
    d_assert(rv == CORE_OK && ngapbuf, return CORE_ERROR, "ngap build error");

    rv = ngap_send_to_ran_ue(source_ue, ngapbuf);
    d_assert(rv == CORE_OK,, "ngap send error");

    return rv;
}