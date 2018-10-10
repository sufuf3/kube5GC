#define TRACE_MODULE _smf_sm
#include "core_debug.h"
#include "core_lib.h"
#include "core_pkbuf.h"

#include "smf_context.h"
#include "smf_event.h"
#include "smf_gtp_path.h"
#include "smf_sm.h"

#include "gtp/gtp_message.h"

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
        case SMF_EVT_S11_MESSAGE:
        {
            status_t rv;
            pkbuf_t *recvbuf = (pkbuf_t *)event_get_param1(e);
            gtp_message_t s11_message;
            smf_sess_t *sess = NULL;

            rv = gtp_parse_msg(&s11_message, recvbuf);
            d_assert(rv == CORE_OK, goto release_s11_pkbuf;,);
            
            sess = smf_sess_add_or_find_by_message(&s11_message);
            d_assert(sess, goto release_s11_pkbuf;,);
            
            d_info("%lu", sess->ipv4->addr[0]);

        release_s11_pkbuf:
            pkbuf_free(recvbuf);
            break;
        }
        default:
        {
            d_error("No handler for event %s", smf_event_get_name(e));
            break;
        }
    }
}