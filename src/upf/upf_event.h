#ifndef __UPF_EVENT_H__
#define __UPF_EVENT_H__

#include "core_event.h"
#include "core_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UPF_EVT_BASE = FSM_USER_SIG,

    UPF_EVT_N4_MESSAGE,
    UPF_EVT_N4_T3_RESPONSE,
    UPF_EVT_N4_T3_HOLDING,
    
    UPF_EVT_TOP,

} event_e;

#define upf_event_send(__ptr_e) event_send(upf_self()->queue_id, (__ptr_e))

CORE_DECLARE(char*) upf_event_get_name(event_t *e);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UPF_EVENT_H__ */
