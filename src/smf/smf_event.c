#define TRACE_MODULE _smf_event

#include "smf_event.h"

char* smf_event_get_name(event_t *e)
{
    if (e == NULL)
        return FSM_NAME_INIT_SIG;

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG: 
            return FSM_NAME_ENTRY_SIG;
        case FSM_EXIT_SIG: 
            return FSM_NAME_EXIT_SIG;

        case SMF_EVT_GX_MESSAGE:
            return "SMF_EVT_GX_MESSAGE";

        case SMF_EVT_N4_MESSAGE:
            return "SMF_EVT_N4_MESSAGE";
        case SMF_EVT_N4_T3_HOLDING:
            return "SMF_EVT_N4_T3_HOLDING";
        case SMF_EVT_N4_T3_RESPONSE:
            return "SMF_EVT_N4_T3_RESPONSE";
        case SMF_EVT_N4_T_HEARTBEAT:
            return "SMF_EVT_N4_T_HEARTBEAT";

        case SMF_EVT_N11_MESSAGE:
            return "SMF_EVT_N11_MESSAGE";

        default:
           break;
    }

    return EVT_NAME_UNKNOWN;
}
