#define TRACE_MODULE _smf_sm
#include "core_debug.h"
#include "core_lib.h"

#include "smf_context.h"
#include "smf_event.h"
#include "smf_gtp_path.h"
#include "smf_sm.h"

void smf_state_initial(fsm_t *s, event_t *e)
{
    smf_sm_trace(3, e);

    d_assert(s, return, "Null param");

    FSM_TRAN(s, &smf_state_operational);
}

void smf_state_final(fsm_t *s, event_t *e)
{
    smf_sm_trace(3, e);

    d_assert(s, return, "Null param");
}

void smf_state_operational(fsm_t *s, event_t *e)
{
    status_t rv;
    smf_sm_trace(3, e);

    d_assert(s, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            rv = smf_gtp_open();
            if (rv != CORE_OK)
            {
                d_error("Can't establish S11-GTP-C path");
                break;
            }
            break;
        }
        case FSM_EXIT_SIG:
        {
            rv = smf_gtp_close();
            if (rv != CORE_OK)
            {
                d_error("Can't close S11-GTP-C path");
                break;
            }
            break;
        }
        default:
        {
            d_error("No handler for event %s", smf_event_get_name(e));
            break;
        }
    }
}
