#define TRACE_MODULE _upf_event

#include "upf_event.h"
#include "upf_context.h"

char* upf_event_get_name(event_t *e)
{
    if (e == NULL)
        return FSM_NAME_INIT_SIG;

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG: 
            return FSM_NAME_ENTRY_SIG;
        case FSM_EXIT_SIG: 
            return FSM_NAME_EXIT_SIG;

        case UPF_EVT_N4_MESSAGE:
            return "UPF_EVT_N4_MESSAGE";
        case UPF_EVT_N4_T3_RESPONSE:
            return "UPF_EVT_N4_T3_RESPONSE";
        case UPF_EVT_N4_T3_HOLDING:
            return "UPF_EVT_N4_T3_HOLDING";
        
        default: 
           break;
    }

    return EVT_NAME_UNKNOWN;
}
