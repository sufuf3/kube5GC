#define TRACE_MODULE _upf_init

#include "core_debug.h"
#include "core_thread.h"

#include "gtp/gtp_xact.h"
#include "pfcp/pfcp_xact.h"

#include "upf_context.h"
#include "upf_event.h"
#include "upf_sm.h"

static thread_id upf_thread;
static void *THREAD_FUNC upf_main(thread_id id, void *data);

static int initialized = 0;

status_t upf_initialize()
{
    status_t rv;

    rv = upf_context_init();
    if (rv != CORE_OK) return rv;

    rv = upf_context_parse_config();
    if (rv != CORE_OK) return rv;

    rv = upf_context_setup_trace_module();
    if (rv != CORE_OK) return rv;

    rv = upf_ue_pool_generate();
    if (rv != CORE_OK) return rv;

    rv = thread_create(&upf_thread, NULL, upf_main, NULL);
    if (rv != CORE_OK) return rv;

    initialized = 1;

    return CORE_OK;
}

void upf_terminate(void)
{
    if (!initialized) return;

    pfcp_xact_final();

    thread_delete(upf_thread);

    upf_context_final();
}

static void *THREAD_FUNC upf_main(thread_id id, void *data)
{
    event_t event;
    fsm_t upf_sm;
    c_time_t prev_tm, now_tm;
    status_t rv;

    memset(&event, 0, sizeof(event_t));

    upf_self()->queue_id = event_create(MSGQ_O_NONBLOCK);
    d_assert(upf_self()->queue_id, return NULL, 
            "UPF event queue creation failed");
    tm_service_init(&upf_self()->tm_service);
    
    pfcp_xact_init(&upf_self()->tm_service,
            UPF_EVT_N4_T3_RESPONSE, UPF_EVT_N4_T3_HOLDING); //$ fixme: wrong event!

    fsm_create(&upf_sm, upf_state_initial, upf_state_final);
    fsm_init(&upf_sm, 0);

    prev_tm = time_now();

#define EVENT_LOOP_TIMEOUT 10   /* 10ms */
    while ((!thread_should_stop()))
    {
        sock_select_loop(EVENT_LOOP_TIMEOUT); 
        do
        {
            rv = event_recv(upf_self()->queue_id, &event);

            d_assert(rv != CORE_ERROR, continue,
                    "While receiving a event message, error occurs");

            now_tm = time_now();

            /* if the gap is over event_loop timeout, execute preriodic jobs */
            if (now_tm - prev_tm > (EVENT_LOOP_TIMEOUT * 1000))
            {
                tm_execute_tm_service(
                        &upf_self()->tm_service, upf_self()->queue_id);

                prev_tm = now_tm;
            }

            if (rv == CORE_EAGAIN)
            {
                continue;
            }

            fsm_dispatch(&upf_sm, (fsm_event_t*)&event);
        } while(rv == CORE_OK);
    }

    fsm_final(&upf_sm, 0);
    fsm_clear(&upf_sm);

    event_delete(upf_self()->queue_id);

    return NULL;
}
