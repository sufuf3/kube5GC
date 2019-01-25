#ifndef __SMF_EVENT_H__
#define __SMF_EVENT_H__

#include "core_event.h"
#include "core_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    SMF_EVT_BASE = FSM_USER_SIG,

    SMF_EVT_GX_MESSAGE,

    SMF_EVT_N4_MESSAGE,
    SMF_EVT_N4_T3_RESPONSE,
    SMF_EVT_N4_T3_HOLDING,
    SMF_EVT_N4_T_HEARTBEAT,

    SMF_EVT_N11_MESSAGE,

    SMF_EVT_TOP,

} event_e;

#define smf_event_send(__ptr_e) event_send(smf_self()->queue_id, (__ptr_e))

CORE_DECLARE(char*) smf_event_get_name(event_t *e);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SMF_EVENT_H__ */
