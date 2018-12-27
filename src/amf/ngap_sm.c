#define TRACE_MODULE _s1ap_sm

#include "core_debug.h"

#include "nas/nas_message.h"
#include "gtp/gtp_message.h"

#include "ngap_build.h"
#include "ngap_handler.h"

#include "amf4g_event.h"
#include "amf4g_sm.h"

void ngap_state_initial(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    amf4g_sm_trace(3, e);

    FSM_TRAN(s, &ngap_state_operational);
}

void ngap_state_final(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    amf4g_sm_trace(3, e);
}

void ngap_state_operational(fsm_t *s, event_t *e)
{
    amf_ran_t *ran = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    amf4g_sm_trace(3, e);

    ran = amf_ran_find(event_get_param1(e));
    d_assert(ran, return,);

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case AMF_EVT_NGAP_MESSAGE:
        {
            NGAP_NGAP_PDU_t *pdu = (NGAP_NGAP_PDU_t *)event_get_param4(e);
            d_assert(pdu, break, "Null param");

            switch(pdu->present)
            {
                case NGAP_NGAP_PDU_PR_initiatingMessage :
                {
                    NGAP_InitiatingMessage_t *initiatingMessage =
                        pdu->choice.initiatingMessage;
                    d_assert(initiatingMessage, break, "Null param");

                    switch(initiatingMessage->procedureCode)
                    {
                        case NGAP_ProcedureCode_id_NGSetup:
                        {
                            ngap_handle_ng_setup_request(ran, pdu);
                            break;
                        }
                        default:
                        {
                            d_warn("Not implemented(choice:%d, proc:%d)",
                                    pdu->present,
                                    initiatingMessage->procedureCode);
                            break;
                        }
                    }

                    break;
                }
                case NGAP_NGAP_PDU_PR_successfulOutcome :
                {
                    NGAP_SuccessfulOutcome_t *successfulOutcome =
                        pdu->choice.successfulOutcome;
                    d_assert(successfulOutcome, break, "Null param");

                    switch(successfulOutcome->procedureCode)
                    {
                        
                        default:
                        {
                            d_warn("Not implemented(choice:%d, proc:%d)",
                                    pdu->present,
                                    successfulOutcome->procedureCode);
                            break;
                        }
                    }
                    break;
                }
                case NGAP_NGAP_PDU_PR_unsuccessfulOutcome :
                {
                    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = 
                        pdu->choice.unsuccessfulOutcome;
                    d_assert(unsuccessfulOutcome, break, "Null param");

                    switch(unsuccessfulOutcome->procedureCode)
                    {
                        default:
                        {
                            d_warn("Not implemented(choice:%d, proc:%d)",
                                    pdu->present,
                                    unsuccessfulOutcome->procedureCode);
                            break;
                        }
                    }
                    break;
                }
                default:
                {
                    d_warn("Not implemented(choice:%d)", pdu->present);
                    break;
                }
            }

            break;
        }
        default:
        {
            d_error("Unknown event %s", amf4g_event_get_name(e));
            break;
        }
    }
}

void ngap_state_exception(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    amf4g_sm_trace(3, e);

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        default:
        {
            d_error("Unknown event %s", amf4g_event_get_name(e));
            break;
        }
    }
}

