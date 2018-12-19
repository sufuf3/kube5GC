#define TRACE_MODULE _smf_init

#include "core_debug.h"
#include "core_network.h"
#include "core_thread.h"

#include "pfcp/pfcp_xact.h"
#include "smf_fd_path.h"

#include "smf_context.h"
#include "smf_event.h"
#include "smf_sm.h"

static thread_id smf_thread;

static void *THREAD_FUNC smf_main(thread_id id, void *data);

static int initialized = 0;

status_t smf_initialize()
{
    status_t rv;

    rv = smf_context_init();
    if (rv != CORE_OK) return rv;

    rv = smf_context_parse_config();
    if (rv != CORE_OK) return rv;

    rv = smf_context_setup_trace_module();
    if (rv != CORE_OK) return rv;

    rv = smf_ue_pool_generate();
    if (rv != CORE_OK) return rv;

    rv = smf_fd_init();
    if (rv != CORE_OK) return CORE_ERROR;

    rv = thread_create(&smf_thread, NULL, smf_main, NULL);
    if (rv != CORE_OK) return rv;

    initialized = 1;

    return CORE_OK;
}

void smf_terminate(void)
{
    if (!initialized) return;

    thread_delete(smf_thread);

    pfcp_xact_final();
    
    smf_fd_final();

    smf_context_final();
}

static void *THREAD_FUNC smf_main(thread_id id, void *data)
{
    event_t event;
    fsm_t smf_sm;
    c_time_t prev_tm, now_tm;
    status_t rv;

    memset(&event, 0, sizeof(event_t));

    smf_self()->queue_id = event_create(MSGQ_O_NONBLOCK);
    d_assert(smf_self()->queue_id, return NULL, 
            "SMF event queue creation failed");
    tm_service_init(&smf_self()->tm_service);
    pfcp_xact_init(&smf_self()->tm_service, 
            SMF_EVT_N4_T3_RESPONSE, SMF_EVT_N4_T3_RESPONSE);
    fsm_create(&smf_sm, smf_state_initial, smf_state_final);
    fsm_init(&smf_sm, 0);

    prev_tm = time_now();

#define EVENT_LOOP_TIMEOUT 10   /* 10ms */
    while ((!thread_should_stop()))
    {
        sock_select_loop(EVENT_LOOP_TIMEOUT); 
        do
        {
            rv = event_recv(smf_self()->queue_id, &event);

            d_assert(rv != CORE_ERROR, continue,
                    "While receiving a event message, error occurs");

            now_tm = time_now();

            /* if the gap is over event_loop timeout, execute preriodic jobs */
            if (now_tm - prev_tm > (EVENT_LOOP_TIMEOUT * 1000))
            {
                tm_execute_tm_service(
                        &smf_self()->tm_service, smf_self()->queue_id);

                prev_tm = now_tm;
            }

            if (rv == CORE_EAGAIN)
            {
                continue;
            }

            fsm_dispatch(&smf_sm, (fsm_event_t*)&event);
        } while(rv == CORE_OK);
    }

    fsm_final(&smf_sm, 0);
    fsm_clear(&smf_sm);

    event_delete(smf_self()->queue_id);

    return NULL;
}
