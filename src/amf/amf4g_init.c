#define TRACE_MODULE _amf4g_init

#include "core_debug.h"
#include "core_thread.h"
#include "core_msgq.h"
#include "core_fsm.h"

#include "gtp/gtp_xact.h"

#include "app/context.h"
#include "amf4g_event.h"

#include "amf4g_fd_path.h"
#include "s1ap_path.h"

#include "amf4g_sm.h"

static thread_id sm_thread;
static void *THREAD_FUNC sm_main(thread_id id, void *data);

static thread_id net_thread;
static void *THREAD_FUNC net_main(thread_id id, void *data);

static int initialized = 0;

status_t amf4g_initialize()
{
    status_t rv;

    rv = amf4g_context_init();
    if (rv != CORE_OK) return rv;

    rv = amf4g_context_parse_config();
    if (rv != CORE_OK) return rv;

    rv = amf4g_context_setup_trace_module();
    if (rv != CORE_OK) return rv;

    rv = amf4g_m_tmsi_pool_generate();
    if (rv != CORE_OK) return rv;

    rv = amf4g_fd_init();
    if (rv != CORE_OK) return CORE_ERROR;

#define USRSCTP_LOCAL_UDP_PORT 9899
    rv = s1ap_init(
            context_self()->parameter.sctp_streams,
            USRSCTP_LOCAL_UDP_PORT);
    if (rv != CORE_OK) return rv;

    rv = thread_create(&sm_thread, NULL, sm_main, NULL);
    if (rv != CORE_OK) return rv;
    rv = thread_create(&net_thread, NULL, net_main, NULL);
    if (rv != CORE_OK) return rv;

    initialized = 1;

    return CORE_OK;
}

void amf4g_terminate(void)
{
    if (!initialized) return;

    thread_delete(net_thread);
    thread_delete(sm_thread);

    amf4g_fd_final();

    amf4g_context_final();

    s1ap_final();

    gtp_xact_final();
}

static void *THREAD_FUNC sm_main(thread_id id, void *data)
{
    event_t event;
    fsm_t amf4g_sm;
    c_time_t prev_tm, now_tm;
    status_t rv;

    memset(&event, 0, sizeof(event_t));

    amf4g_self()->queue_id = event_create(MSGQ_O_BLOCK);
    d_assert(amf4g_self()->queue_id, return NULL, 
            "AMF4G event queue creation failed");
    tm_service_init(&amf4g_self()->tm_service);
    gtp_xact_init(&amf4g_self()->tm_service,
            AMF4G_EVT_S11_T3_RESPONSE, AMF4G_EVT_S11_T3_HOLDING);

    fsm_create(&amf4g_sm, amf4g_state_initial, amf4g_state_final);
    fsm_init(&amf4g_sm, 0);

    amf4g_overload_checking_init();

    prev_tm = time_now();

#define EVENT_LOOP_TIMEOUT 50   /* 50ms */
    while ((!thread_should_stop()))
    {
        rv = event_timedrecv(amf4g_self()->queue_id, &event, EVENT_LOOP_TIMEOUT);

        d_assert(rv != CORE_ERROR, continue,
                "While receiving a event message, error occurs");

        now_tm = time_now();

        /* if the gap is over 10 ms, execute preriodic jobs */
        if (now_tm - prev_tm > EVENT_LOOP_TIMEOUT * 1000)
        {
            tm_execute_tm_service(
                    &amf4g_self()->tm_service, amf4g_self()->queue_id);

            prev_tm = now_tm;
        }

        if (rv == CORE_TIMEUP)
        {
            continue;
        }

        fsm_dispatch(&amf4g_sm, (fsm_event_t*)&event);
    }

    fsm_final(&amf4g_sm, 0);
    fsm_clear(&amf4g_sm);

    event_delete(amf4g_self()->queue_id);

    return NULL;
}

static void *THREAD_FUNC net_main(thread_id id, void *data)
{
    while (!thread_should_stop())
    {
        sock_select_loop(EVENT_LOOP_TIMEOUT); 
    }

    return NULL;
}
