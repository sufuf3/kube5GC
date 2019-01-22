#ifndef __AMF4G_EVENT_H__
#define __AMF4G_EVENT_H__

#include "core_event.h"
#include "core_fsm.h"
#include "core_pkbuf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* forward declaration */
typedef enum {
    AMF4G_EVT_BASE = FSM_USER_SIG,

    AMF4G_EVT_S1AP_MESSAGE,
    AMF4G_EVT_S1AP_DELAYED_SEND,
    AMF4G_EVT_S1AP_LO_ACCEPT,
    AMF4G_EVT_S1AP_LO_SCTP_COMM_UP,
    AMF4G_EVT_S1AP_LO_CONNREFUSED,
    AMF4G_EVT_S1AP_S1_HOLDING_TIMER,

    AMF_EVT_NGAP_MESSAGE,
    AMF_EVT_NGAP_DELAYED_SEND,
    AMF_EVT_NGAP_LO_ACCEPT,
    AMF_EVT_NGAP_LO_SCTP_COMM_UP,
    AMF_EVT_NGAP_LO_CONNREFUSED,
    AMF_EVT_NGAP_NG_HOLDING_TIMER,

    AMF4G_EVT_EMM_MESSAGE,
    AMF4G_EVT_EMM_T3413,

    AMF4G_EVT_ESM_MESSAGE,

    AMF4G_EVT_S11_MESSAGE,
    AMF4G_EVT_S11_T3_RESPONSE,
    AMF4G_EVT_S11_T3_HOLDING,

    AMF4G_EVT_S6A_MESSAGE,

    AMF4G_EVT_CHECK_OVERLOAD,
    
    AMF_EVT_N11_MESSAGE,
    
    AMF4G_EVT_TOP,

} event_e;

#define amf4g_event_send(__ptr_e) event_send(amf4g_self()->queue_id, (__ptr_e))

CORE_DECLARE(char*) amf4g_event_get_name(event_t *e);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AMF4G_EVENT_H__ */
