#ifndef __UPF_SM_H__
#define __UPF_SM_H__

#include "core_param.h"
#include "core_fsm.h"
#include "core_event.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void upf_state_initial(fsm_t *s, event_t *e);
void upf_state_final(fsm_t *s, event_t *e);
void upf_state_operational(fsm_t *s, event_t *e);
void upf_state_exception(fsm_t *s, event_t *e);

#define upf_sm_print(__pe) \
    d_print("%s(): %s\n", __func__, upf_event_get_name(__pe))

#define upf_sm_trace(__l, __pe) \
    d_trace(__l, "%s(): %s\n", __func__, upf_event_get_name(__pe))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__UPF_SM_H__ */
