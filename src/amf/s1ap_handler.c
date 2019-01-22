#define TRACE_MODULE _s1ap_handler

#include "core_debug.h"

#include "amf4g_event.h"

#include "amf4g_kdf.h"
#include "s1ap_conv.h"
#include "s1ap_path.h"
#include "nas_path.h"
#include "amf_n11_path.h"


#include "s1ap_build.h"
#include "s1ap_handler.h"

#include "amf4g_path.h"
#include "amf4g_sm.h"

void s1ap_handle_s1_setup_request(amf4g_enb_t *enb, s1ap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i, j;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_S1SetupRequest_t *S1SetupRequest = NULL;

    S1AP_S1SetupRequestIEs_t *ie = NULL;
    S1AP_Global_ENB_ID_t *Global_ENB_ID = NULL;
    S1AP_SupportedTAs_t *SupportedTAs = NULL;
    S1AP_PagingDRX_t *PagingDRX = NULL;

    pkbuf_t *s1apbuf = NULL;
    c_uint32_t enb_id;
    S1AP_Cause_PR group = S1AP_Cause_PR_NOTHING;
    long cause = 0;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    S1SetupRequest = &initiatingMessage->value.choice.S1SetupRequest;
    d_assert(S1SetupRequest, return,);

    d_trace(3, "[AMF4G] S1-Setup request\n");

    for (i = 0; i < S1SetupRequest->protocolIEs.list.count; i++)
    {
        ie = S1SetupRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_Global_ENB_ID:
                Global_ENB_ID = &ie->value.choice.Global_ENB_ID;
                break;
            case S1AP_ProtocolIE_ID_id_SupportedTAs:
                SupportedTAs = &ie->value.choice.SupportedTAs;
                break;
            case S1AP_ProtocolIE_ID_id_DefaultPagingDRX:
                PagingDRX = &ie->value.choice.PagingDRX;
                break;
            default:
                break;
        }
    }

    d_assert(Global_ENB_ID, return,);

    s1ap_ENB_ID_to_uint32(&Global_ENB_ID->eNB_ID, &enb_id);
    d_trace(5, "    IP[%s] ENB_ID[%d]\n", CORE_ADDR(enb->addr, buf), enb_id);

    if (PagingDRX)
        d_trace(5, "    PagingDRX[%ld]\n", *PagingDRX);

    amf4g_enb_set_enb_id(enb, enb_id);

    d_assert(SupportedTAs, return,);
    /* Parse Supported TA */
    enb->num_of_supported_ta_list = 0;
    for (i = 0; i < SupportedTAs->list.count; i++)
    {
        S1AP_SupportedTAs_Item_t *SupportedTAs_Item = NULL;
        S1AP_TAC_t *tAC = NULL;

        SupportedTAs_Item = 
            (S1AP_SupportedTAs_Item_t *)SupportedTAs->list.array[i];
        d_assert(SupportedTAs_Item, return,);
        tAC = &SupportedTAs_Item->tAC;
        d_assert(tAC, return,);

        for (j = 0; j < SupportedTAs_Item->broadcastPLMNs.list.count; j++)
        {
            S1AP_PLMNidentity_t *pLMNidentity = NULL;
            pLMNidentity = (S1AP_PLMNidentity_t *)
                SupportedTAs_Item->broadcastPLMNs.list.array[j];
            d_assert(pLMNidentity, return,);

            memcpy(&enb->supported_ta_list[enb->num_of_supported_ta_list].tac,
                    tAC->buf, sizeof(c_uint16_t));
            enb->supported_ta_list[enb->num_of_supported_ta_list].tac = 
                ntohs(enb->supported_ta_list
                        [enb->num_of_supported_ta_list].tac);
            memcpy(&enb->supported_ta_list
                        [enb->num_of_supported_ta_list].plmn_id,
                    pLMNidentity->buf, sizeof(plmn_id_t));
            d_trace(5, "    PLMN_ID[MCC:%d MNC:%d] TAC[%d]\n",
                plmn_id_mcc(&enb->supported_ta_list
                    [enb->num_of_supported_ta_list].plmn_id),
                plmn_id_mnc(&enb->supported_ta_list
                    [enb->num_of_supported_ta_list].plmn_id),
                enb->supported_ta_list[enb->num_of_supported_ta_list].tac);
            enb->num_of_supported_ta_list++;
        }
    }

    if (enb->num_of_supported_ta_list == 0)
    {
        d_warn("S1-Setup failure:");
        d_warn("    No supported TA exist in S1-Setup request");
        group = S1AP_Cause_PR_misc;
        cause = S1AP_CauseMisc_unspecified;
    }
    else
    {
        int served_tai_index = -1;
        for (i = 0; i < enb->num_of_supported_ta_list; i++)
        {
            served_tai_index = 
                amf4g_find_served_tai(&enb->supported_ta_list[i]);
            if (served_tai_index >= 0 &&
                served_tai_index < MAX_NUM_OF_SERVED_TAI)
            {
                d_trace(5, "    SERVED_TAI_INDEX[%d]\n", served_tai_index);
                break;
            }
        }

        if (served_tai_index < 0)
        {
            d_warn("S1-Setup failure:");
            d_warn("    Cannot find Served TAI. Check 'amf4g.tai' configuration");
            group = S1AP_Cause_PR_misc;
            cause = S1AP_CauseMisc_unknown_PLMN;
        }
    }

    if (group == S1AP_Cause_PR_NOTHING)
    {
        d_trace(3, "[AMF4G] S1-Setup response\n");
        d_assert(s1ap_build_setup_rsp(&s1apbuf) == CORE_OK, 
                return, "s1ap_build_setup_rsp() failed");
    }
    else
    {
        d_trace(3, "[AMF4G] S1-Setup failure\n");
        d_assert(s1ap_build_setup_failure(
                &s1apbuf, group, cause, S1AP_TimeToWait_v10s) == CORE_OK, 
                return, "s1ap_build_setup_failure() failed");
    }

    d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,
            "s1ap_send_to_enb() failed");
}

void s1ap_handle_eNB_configuration_update(amf4g_enb_t *enb, s1ap_message_t *message){

    int i, j;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_ENBConfigurationUpdate_t *ENBConfigurationUpdate = NULL;

    S1AP_ENBConfigurationUpdateIEs_t *ie = NULL;
    S1AP_SupportedTAs_t *SupportedTAs = NULL;
    S1AP_PagingDRX_t *PagingDRX = NULL;

    pkbuf_t *s1apbuf = NULL;
    S1AP_Cause_PR group = S1AP_Cause_PR_NOTHING;
    long cause = 0;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    ENBConfigurationUpdate = &initiatingMessage->value.choice.ENBConfigurationUpdate;
    d_assert(ENBConfigurationUpdate, return,);

    d_trace(3, "[AMF4G] eNB Configuration update\n");

    for (i = 0; i < ENBConfigurationUpdate->protocolIEs.list.count; i++)
    {
        ie = ENBConfigurationUpdate->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_SupportedTAs:
                SupportedTAs = &ie->value.choice.SupportedTAs;
                break;
            case S1AP_ProtocolIE_ID_id_DefaultPagingDRX:
                PagingDRX = &ie->value.choice.PagingDRX;
                break;
            default:
                break;
        }
    }

    if (PagingDRX)
        d_trace(5, "    PagingDRX[%ld]\n", *PagingDRX);

    d_assert(SupportedTAs, return,);
    /* Parse Supported TA */
    enb->num_of_supported_ta_list = 0;
    for (i = 0; i < SupportedTAs->list.count; i++)
    {
        S1AP_SupportedTAs_Item_t *SupportedTAs_Item = NULL;
        S1AP_TAC_t *tAC = NULL;

        SupportedTAs_Item = 
            (S1AP_SupportedTAs_Item_t *)SupportedTAs->list.array[i];
        d_assert(SupportedTAs_Item, return,);
        tAC = &SupportedTAs_Item->tAC;
        d_assert(tAC, return,);

        for (j = 0; j < SupportedTAs_Item->broadcastPLMNs.list.count; j++)
        {
            S1AP_PLMNidentity_t *pLMNidentity = NULL;
            pLMNidentity = (S1AP_PLMNidentity_t *)
                SupportedTAs_Item->broadcastPLMNs.list.array[j];
            d_assert(pLMNidentity, return,);

            memcpy(&enb->supported_ta_list[enb->num_of_supported_ta_list].tac,
                    tAC->buf, sizeof(c_uint16_t));
            enb->supported_ta_list[enb->num_of_supported_ta_list].tac = 
                ntohs(enb->supported_ta_list
                        [enb->num_of_supported_ta_list].tac);
            memcpy(&enb->supported_ta_list
                        [enb->num_of_supported_ta_list].plmn_id,
                    pLMNidentity->buf, sizeof(plmn_id_t));
            d_trace(5, "    PLMN_ID[MCC:%d MNC:%d] TAC[%d]\n",
                plmn_id_mcc(&enb->supported_ta_list
                    [enb->num_of_supported_ta_list].plmn_id),
                plmn_id_mnc(&enb->supported_ta_list
                    [enb->num_of_supported_ta_list].plmn_id),
                enb->supported_ta_list[enb->num_of_supported_ta_list].tac);
            enb->num_of_supported_ta_list++;
        }
    }

    if (enb->num_of_supported_ta_list == 0)
    {
        d_warn("eNB Configuration update failure:");
        d_warn("    No supported TA exist in eNB Configuration update");
        group = S1AP_Cause_PR_misc;
        cause = S1AP_CauseMisc_unspecified;
    }
    else
    {
        int served_tai_index = -1;
        for (i = 0; i < enb->num_of_supported_ta_list; i++)
        {
            served_tai_index = 
                amf4g_find_served_tai(&enb->supported_ta_list[i]);
            if (served_tai_index >= 0 &&
                served_tai_index < MAX_NUM_OF_SERVED_TAI)
            {
                d_trace(5, "    SERVED_TAI_INDEX[%d]\n", served_tai_index);
                break;
            }
        }

        if (served_tai_index < 0)
        {
            d_warn("eNB Configuration update failure:");
            d_warn("    Cannot find Served TAI. Check 'amf4g.tai' configuration");
            group = S1AP_Cause_PR_misc;
            cause = S1AP_CauseMisc_unknown_PLMN;
        }
    }

    if (group == S1AP_Cause_PR_NOTHING)
    {
        d_trace(3, "[AMF4G]eNB Configuration update ACKNOWLEDGE\n");
        d_assert(s1ap_build_enb_configuration_update_acknowledge(&s1apbuf) == CORE_OK, 
                return, "s1ap_build_setup_rsp() failed");
    }
    else
    {
        d_trace(3, "[AMF4G] eNB Configuration update failure\n");
        d_assert(s1ap_build_enb_configuration_update_failure(
                &s1apbuf, group, cause, S1AP_TimeToWait_v10s) == CORE_OK, 
                return, "s1ap_build_setup_failure() failed");
    }

    d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,
            "s1ap_send_to_enb() failed");

}
void s1ap_handle_ue_context_suspend(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UEContextSuspendRequest_t *UEContextSuspendRequest = NULL;

    S1AP_UEContextSuspendRequestIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    pkbuf_t *s1apbuf = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UEContextSuspendRequest = &initiatingMessage->value.choice.UEContextSuspendRequest;
    d_assert(UEContextSuspendRequest, return,);

    d_trace(3, "[AMF4G] ue context suspend request\n");

    for (i = 0; i < UEContextSuspendRequest->protocolIEs.list.count; i++)
    {
        ie = UEContextSuspendRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            default:
                break;
        }
    }

    d_trace(3, "[AMF4G] ue context suspend response\n");
    d_assert(s1ap_build_ue_context_suspend_response(&s1apbuf,MME_UE_S1AP_ID,ENB_UE_S1AP_ID) == CORE_OK, 
            return, "s1ap_build_setup_rsp() failed");


    d_assert(s1ap_send_to_enb(enb, s1apbuf,S1AP_NON_UE_SIGNALLING) == CORE_OK,,
            "s1ap_send_to_enb() failed");

}
void s1ap_handle_ERAB_release_response(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_E_RABReleaseResponse_t *E_RABReleaseResponse = NULL;

    S1AP_E_RABReleaseResponseIEs_t *ie = NULL;
   // S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;    
//    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;    

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    E_RABReleaseResponse = &successfulOutcome->value.choice.E_RABReleaseResponse;
    d_assert(E_RABReleaseResponse, return,);

    d_trace(3, "[AMF4G] ERAB release response\n");

    for (i = 0; i < E_RABReleaseResponse->protocolIEs.list.count; i++)
    {
        ie = E_RABReleaseResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
               // ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
              //  MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            default:
                break;
        }
    }

}

// Handle MME configuration Update Acknowledge
void s1ap_handle_mme_configuration_update_acknowledge(
        amf4g_enb_t * enb,s1ap_message_t * message)
{
    //status_t rv;
    //char buf[CORE_ADDRSTRLEN];
    //int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_MMEConfigurationUpdateAcknowledge_t *MMEConfigurationUpdateAcknowledge = NULL;
    //S1AP_MMEConfigurationUpdateAcknowledgeIEs_t *ie = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    MMEConfigurationUpdateAcknowledge = 
        &successfulOutcome->value.choice.MMEConfigurationUpdateAcknowledge;

    d_assert(MMEConfigurationUpdateAcknowledge, return,);

    d_trace(3, "[AMF4G] MME configuration update acknowledge\n");
}

void s1ap_handle_enb_direct_information_transfer(amf4g_enb_t *enb, s1ap_message_t *message)
{
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_ENBDirectInformationTransfer_t *ENBDirectInformationTransfer = NULL;

    S1AP_ENBDirectInformationTransferIEs_t *ie = NULL;
    //S1AP_Inter_SystemInformationTransferType_t *Inter_SystemInformationTransferType = NULL;
    
    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    // Message Type
    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    ENBDirectInformationTransfer = &initiatingMessage->value.choice.ENBDirectInformationTransfer;
    //--------------
    
    d_trace(3, "[AMF4G] ENB Direct Information Transfer\n");
    
    for (i = 0; i < ENBDirectInformationTransfer->protocolIEs.list.count; i++)
    {
        ie = ENBDirectInformationTransfer->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_Inter_SystemInformationTransferTypeEDT:
                //Inter_SystemInformationTransferType = &ie->value.choice.Inter_SystemInformationTransferType;
                break;
            default:
                break;
        }
    } 
}

// Handle MME Configuration Update Failure
void s1ap_handle_mme_configuration_update_failure(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    //status_t rv;
    //char buf[CORE_ADDRSTRLEN];
    int i;
    // Beginning of Message Type
    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_MMEConfigurationUpdateFailure_t *MMEConfigurationUpdateFailure = NULL;

    S1AP_MMEConfigurationUpdateFailureIEs_t *ie = NULL;
    //S1AP_Cause_t *Cause = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);

    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(unsuccessfulOutcome, return,);

    MMEConfigurationUpdateFailure = 
        &unsuccessfulOutcome->value.choice.MMEConfigurationUpdateFailure;
    d_assert(MMEConfigurationUpdateFailure, return,);

    d_trace(3, "[AMF4G] MME configuration update failure\n");
    // End of Message Type
    
    for (i = 0; i < MMEConfigurationUpdateFailure->protocolIEs.list.count; i++)
    {
        ie = MMEConfigurationUpdateFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_Cause:
                //Cause = &ie->value.choice.Cause;
        //printf("cause = %d\n",(int)Cause);
                break;
            default:
                break;
        }
    }
}

// Handle E-RAB release indication
void s1ap_handle_e_rab_release_indication(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    int i;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_E_RABReleaseIndication_t *E_RABReleaseIndication = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_E_RABReleaseIndicationIEs_t *ie = NULL;
    
    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    // Message Type
    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    E_RABReleaseIndication = &initiatingMessage->value.choice.E_RABReleaseIndication;
    //--------------
    
    d_trace(3, "[AMF4G] E-RAB Release Indication\n");
    
    for (i = 0; i < E_RABReleaseIndication->protocolIEs.list.count; i++)
    {
        ie = E_RABReleaseIndication->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_E_RABReleasedList:
                break;
            default:
                break;
        }
    }
    d_trace(1, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n", *ENB_UE_S1AP_ID, *MME_UE_S1AP_ID );
}

void s1ap_handle_CBC_write_replace_warning_message(amf4g_enb_t *enb)
{
    pkbuf_t *s1apbuf = NULL;
    d_trace(3, "[AMF4G] S1 send write replace warning request\n");
    d_assert(s1ap_build_write_replace_warning_request(&s1apbuf) == CORE_OK,return, "s1ap_build_write_replace_warning_request() failed");
    d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,"s1ap_send_to_enb() failed");

}

void s1ap_handle_CBC_stop_warning_message(amf4g_enb_t *enb)
{
    pkbuf_t *s1apbuf = NULL;
    d_trace(3, "[AMF4G] S1 send kill request\n");
    d_assert(s1ap_build_kill_request(&s1apbuf) == CORE_OK,return, "s1ap_build_kill_request() failed");
    d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,"s1ap_send_to_enb() failed");
}

void s1ap_handle_initial_ue_message(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    int i;
    char buf[CORE_ADDRSTRLEN];

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_InitialUEMessage_t *InitialUEMessage = NULL;

    S1AP_InitialUEMessage_IEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_NAS_PDU_t *NAS_PDU = NULL;
    S1AP_TAI_t *TAI = NULL;
    S1AP_EUTRAN_CGI_t *EUTRAN_CGI = NULL;
    S1AP_S_TMSI_t *S_TMSI = NULL;
    S1AP_UE_Application_Layer_Measurement_Capability_t *UE_Application_Layer_Measurement_Capability = NULL;

    S1AP_PLMNidentity_t    *pLMNidentity = NULL;
    S1AP_TAC_t *tAC = NULL;
    S1AP_CellIdentity_t *cell_ID = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    InitialUEMessage = &initiatingMessage->value.choice.InitialUEMessage;
    d_assert(InitialUEMessage, return,);

    d_trace(3, "[AMF4G] Initial UE Message\n");

    for (i = 0; i < InitialUEMessage->protocolIEs.list.count; i++)
    {
        ie = InitialUEMessage->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_NAS_PDU:
                NAS_PDU = &ie->value.choice.NAS_PDU;
                break;
            case S1AP_ProtocolIE_ID_id_TAI:
                TAI = &ie->value.choice.TAI;
                break;
            case S1AP_ProtocolIE_ID_id_EUTRAN_CGI:
                EUTRAN_CGI = &ie->value.choice.EUTRAN_CGI;
                break;
            case S1AP_ProtocolIE_ID_id_S_TMSI:
                S_TMSI = &ie->value.choice.S_TMSI;
                break;
            case S1AP_ProtocolIE_ID_id_UE_Application_Layer_Measurement_Capability:
                UE_Application_Layer_Measurement_Capability = &ie->value.choice.UE_Application_Layer_Measurement_Capability;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    if (!enb_ue)
    { 
        enb_ue = enb_ue_add(enb);
        d_assert(enb_ue, return, "Null param");

        enb_ue->enb_ue_s1ap_id = *ENB_UE_S1AP_ID;

        /* Find MME_UE if S_TMSI included */
        if (S_TMSI)
        {
            served_gummei_t *served_gummei = &amf4g_self()->served_gummei[0];
            guti_t guti;
            amf4g_ue_t *amf4g_ue = NULL;

            memset(&guti, 0, sizeof(guti_t));

            /* Use the first configured plmn_id and mme group id */
            memcpy(&guti.plmn_id, &served_gummei->plmn_id[0], PLMN_ID_LEN);
            guti.mme_gid = served_gummei->amf4g_gid[0];

            /* size must be 1 */
            memcpy(&guti.mme_code, S_TMSI->mMEC.buf, S_TMSI->mMEC.size);
            /* size must be 4 */
            memcpy(&guti.m_tmsi, S_TMSI->m_TMSI.buf, S_TMSI->m_TMSI.size);
            guti.m_tmsi = ntohl(guti.m_tmsi);

            amf4g_ue = amf4g_ue_find_by_guti(&guti);
            if (!amf4g_ue)
            {
                d_warn("Unknown UE by S_TMSI[G:%d,C:%d,M_TMSI:0x%x]",
                        guti.mme_gid, guti.mme_code, guti.m_tmsi);
            }
            else
            {
                d_trace(5, "    S_TMSI[G:%d,C:%d,M_TMSI:0x%x] IMSI:[%s]\n",
                        amf4g_ue->guti.mme_gid,
                        amf4g_ue->guti.mme_code,
                        amf4g_ue->guti.m_tmsi,
                        AMF4G_UE_HAVE_IMSI(amf4g_ue) 
                            ? amf4g_ue->imsi_bcd : "Unknown");

                /* If NAS(amf4g_ue_t) has already been associated with
                 * older S1(enb_ue_t) context */
                if (ECM_CONNECTED(amf4g_ue))
                {
#if 1  /* IMPLICIT_S1_RELEASE */
                   /* Implcit S1 release */
                    d_trace(5, "Implicit S1 release\n");
                    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
                          amf4g_ue->enb_ue->enb_ue_s1ap_id,
                          amf4g_ue->enb_ue->mme_ue_s1ap_id);
                    rv = enb_ue_remove(amf4g_ue->enb_ue);
                    d_assert(rv == CORE_OK,,);

#else /* S1_HOLDING_TIMER */

                    /* Previous S1(enb_ue_t) context the holding timer(30secs)
                     * is started.
                     * Newly associated S1(enb_ue_t) context holding timer
                     * is stopped. */
                    d_trace(5, "Start S1 Holding Timer\n");
                    d_trace(5, "    ENB_UE_S1AP_ID[%d] MME_UE_S1AP_ID[%d]\n",
                            amf4g_ue->enb_ue->enb_ue_s1ap_id, 
                            amf4g_ue->enb_ue->mme_ue_s1ap_id);

                    /* De-associate S1 with NAS/EMM */
                    rv = enb_ue_deassociate(amf4g_ue->enb_ue);
                    d_assert(rv == CORE_OK,,);

                    tm_start(amf4g_ue->enb_ue->holding_timer);
#endif
                }
                tm_stop(enb_ue->holding_timer);
                amf4g_ue_associate_enb_ue(amf4g_ue, enb_ue);
            }
        }
    }

    d_assert(TAI, return,);
    pLMNidentity = &TAI->pLMNidentity;
    d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
    tAC = &TAI->tAC;
    d_assert(tAC && tAC->size == sizeof(c_uint16_t), return,);

    memcpy(&enb_ue->nas.tai.plmn_id, pLMNidentity->buf, 
            sizeof(enb_ue->nas.tai.plmn_id));
    memcpy(&enb_ue->nas.tai.tac, tAC->buf, sizeof(enb_ue->nas.tai.tac));
    enb_ue->nas.tai.tac = ntohs(enb_ue->nas.tai.tac);
    
    d_assert(EUTRAN_CGI, return,);
    pLMNidentity = &EUTRAN_CGI->pLMNidentity;
    d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
    cell_ID = &EUTRAN_CGI->cell_ID;
    d_assert(cell_ID, return,);
    memcpy(&enb_ue->nas.e_cgi.plmn_id, pLMNidentity->buf, 
            sizeof(enb_ue->nas.e_cgi.plmn_id));
    memcpy(&enb_ue->nas.e_cgi.cell_id, cell_ID->buf,
            sizeof(enb_ue->nas.e_cgi.cell_id));
    enb_ue->nas.e_cgi.cell_id = (ntohl(enb_ue->nas.e_cgi.cell_id) >> 4);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d] TAC[%d]\n",
        enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id, enb_ue->nas.tai.tac);

    d_trace(10, "UE_Application_Layer_Measurement_Capability\n");
    if (UE_Application_Layer_Measurement_Capability)
    {
        memcpy(&enb_ue->amf4g_ue->ue_application_layer_measurement_capability.qoe, UE_Application_Layer_Measurement_Capability->buf,
        sizeof(enb_ue->amf4g_ue->ue_application_layer_measurement_capability.qoe));
    }
    /*if (enb_ue){
        pkbuf_t *s1apbuf = NULL;
        s1ap_build_reroute_nas_request(&s1apbuf,enb_ue);
        d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,"s1ap_send_to_enb() failed");
    }*/

    d_assert(s1ap_send_to_nas(enb_ue,
        S1AP_ProcedureCode_id_initialUEMessage, NAS_PDU) == CORE_OK,,
        "s1ap_send_to_nas failed");
}

void s1ap_handle_uplink_nas_transport(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UplinkNASTransport_t *UplinkNASTransport = NULL;

    S1AP_UplinkNASTransport_IEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_NAS_PDU_t *NAS_PDU = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UplinkNASTransport = &initiatingMessage->value.choice.UplinkNASTransport;
    d_assert(UplinkNASTransport, return,);

    d_trace(3, "[AMF4G] Uplink NAS transport\n");

    for (i = 0; i < UplinkNASTransport->protocolIEs.list.count; i++)
    {
        ie = UplinkNASTransport->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_NAS_PDU:
                NAS_PDU = &ie->value.choice.NAS_PDU;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[ENB_UE_S1AP_ID:%d]",
            *ENB_UE_S1AP_ID);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    d_assert(s1ap_send_to_nas(enb_ue,
        S1AP_ProcedureCode_id_uplinkNASTransport, NAS_PDU) == CORE_OK,,
        "s1ap_send_to_nas failed");
}

void s1ap_handle_ue_radio_capability_match_response(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    //status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_UERadioCapabilityMatchResponse_t *UERadioCapabilityMatchResponse = NULL;

    S1AP_UERadioCapabilityMatchResponseIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_VoiceSupportMatchIndicator_t *VoiceSupportMatchIndicator = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    UERadioCapabilityMatchResponse =
        &successfulOutcome->value.choice.UERadioCapabilityMatchResponse;
    d_assert(UERadioCapabilityMatchResponse, return,);

    d_trace(1, "[AMF4G] UE Radio Capability Match Response\n");

    for (i = 0; i < UERadioCapabilityMatchResponse->protocolIEs.list.count; i++)
    {
        ie = UERadioCapabilityMatchResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_VoiceSupportMatchIndicator:
                VoiceSupportMatchIndicator = &ie->value.choice.VoiceSupportMatchIndicator;
                break;
            default:
                break;
        }
    }

    d_trace(1, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    //d_trace(1, "    ENB_UE_S1AP_ID[%d] MME_UE_S1AP_ID[%d]\n",
    //        enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);
    d_trace(1, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d] VoiceSupportMatchIndicator[%d]\n",
            *ENB_UE_S1AP_ID, *MME_UE_S1AP_ID, *VoiceSupportMatchIndicator);
}

void s1ap_handle_nas_delivery_indication(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_NASDeliveryIndication_t *NASDeliveryIndication = NULL;

    S1AP_NASDeliveryIndicationIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    
    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    NASDeliveryIndication = &initiatingMessage->value.choice.NASDeliveryIndication;
    d_assert(NASDeliveryIndication, return,);

    d_trace(3, "[AMF4G] NAS DELIVERY INDICATION\n");
    d_trace(3, "[AMF4G] Sending to SCEF\n");

    //retrieve data from packet
    printf("protocolIE list count = %d\n",NASDeliveryIndication->protocolIEs.list.count);
    for (i = 0; i < NASDeliveryIndication->protocolIEs.list.count; i++)
    {
        ie = NASDeliveryIndication->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                //ENB_UE_S1AP_ID = &ie->value.choice.eNB_UE_S1AP_ID;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[ENB_UE_S1AP_ID:%d]",
            *ENB_UE_S1AP_ID);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    /*d_assert(s1ap_send_to_nas(enb_ue,
        S1AP_ProcedureCode_id_uplinkNASTransport, NAS_PDU) == CORE_OK,,
        "s1ap_send_to_nas failed");*/
}

void s1ap_handle_nas_non_delivery_indication(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_NASNonDeliveryIndication_t *NASNonDeliveryIndication = NULL;

    S1AP_NASNonDeliveryIndication_IEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_NAS_PDU_t *NAS_PDU = NULL;
    S1AP_Cause_t *Cause = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    NASNonDeliveryIndication = &initiatingMessage->value.choice.NASNonDeliveryIndication;
    d_assert(NASNonDeliveryIndication, return,);

    d_trace(3, "[AMF4G] NAS NON DELIVERY INDICATION\n");
    d_trace(3, "[AMF4G] Sending to SCEF\n");

    for (i = 0; i < NASNonDeliveryIndication->protocolIEs.list.count; i++)
    {
        ie = NASNonDeliveryIndication->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_NAS_PDU:
                NAS_PDU = &ie->value.choice.NAS_PDU;
                break;
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[ENB_UE_S1AP_ID:%d]",
            *ENB_UE_S1AP_ID);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    d_assert(Cause, return,);
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
            Cause->present, Cause->choice.radioNetwork);

    d_assert(s1ap_send_to_nas(enb_ue,
        S1AP_ProcedureCode_id_NASNonDeliveryIndication, NAS_PDU) == CORE_OK,,
        "s1ap_send_to_nas failed");
}

void s1ap_handle_e_rab_modification_indication(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_E_RABModificationIndication_t *E_RABModificationIndication = NULL;
    
    S1AP_E_RABModificationIndicationIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    //S1AP_E_RABToBeModifiedListBearerModInd_t*     E_RABToBeModifiedListBearerModInd = NULL;
    //S1AP_E_RABNotToBeModifiedListBearerModInd_t*     E_RABNotToBeModifiedListBearerModInd = NULL;
    //S1AP_CSGMembershipInfo_t*     CSGMembershipInfo = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    E_RABModificationIndication = &initiatingMessage->value.choice.E_RABModificationIndication;
    d_assert(E_RABModificationIndication, return,);

    d_trace(1, "[AMF4G] E-RAB Modification Indication\n");

    for (i = 0; i < E_RABModificationIndication->protocolIEs.list.count; i++)
    {
        ie = E_RABModificationIndication->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;

            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;

            /*case S1AP_ProtocolIE_ID_id_E_RABToBeModifiedListBearerModInd:
                E_RABToBeModifiedListBearerModInd = &ie->value.choice.E_RABToBeModifiedListBearerModInd;
                break;*/
 
            /*case S1AP_ProtocolIE_ID_id_E_RABNotToBeModifiedListBearerModInd:
                E_RABNotToBeModifiedListBearerModInd = &ie->value.choice.E_RABNotToBeModifiedListBearerModInd;
                break;

            case S1AP_ProtocolIE_ID_id_CSGMembershipInfo:
                CSGMembershipInfo = &ie->value.choice.CSGMembershipInfo;
                break;*/

            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[ENB_UE_S1AP_ID:%d]",
            *ENB_UE_S1AP_ID);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%s]\n",
            enb_ue->enb_ue_s1ap_id, *MME_UE_S1AP_ID);

    d_assert(s1ap_send_e_rab_modification_confirm(enb_ue) == CORE_OK,,
            "s1ap_send_e_rab_modification_confirm() failed");

}
void s1ap_handle_ue_capability_info_indication(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UECapabilityInfoIndication_t *UECapabilityInfoIndication = NULL;

    S1AP_UECapabilityInfoIndicationIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_UERadioCapability_t *UERadioCapability = NULL;
    S1AP_UE_Application_Layer_Measurement_Capability_t *UE_Application_Layer_Measurement_Capability = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UECapabilityInfoIndication =
        &initiatingMessage->value.choice.UECapabilityInfoIndication;
    d_assert(UECapabilityInfoIndication, return,);

    d_trace(3, "[AMF4G] UE capability info indication\n");

    for (i = 0; i < UECapabilityInfoIndication->protocolIEs.list.count; i++)
    {
        ie = UECapabilityInfoIndication->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_UERadioCapability:
                UERadioCapability = &ie->value.choice.UERadioCapability;
                break;
            case S1AP_ProtocolIE_ID_id_UE_Application_Layer_Measurement_Capability:
                UE_Application_Layer_Measurement_Capability = &ie->value.choice.UE_Application_Layer_Measurement_Capability;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[%d]", *ENB_UE_S1AP_ID);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    if (enb_ue->amf4g_ue)
    {
#if 0
        S1AP_UERadioCapability_t *radio_capa = NULL;
        amf4g_ue_t *amf4g_ue = enb_ue->amf4g_ue;

        /* Release the previous one */
        if (amf4g_ue->radio_capa)
        {
            radio_capa = (S1AP_UERadioCapability_t *)amf4g_ue->radio_capa;

            if (radio_capa->buf)
                CORE_FREE(radio_capa->buf);
            CORE_FREE(amf4g_ue->radio_capa);
        }
        /* Save UE radio capability */ 
        amf4g_ue->radio_capa = core_calloc(1, sizeof(S1AP_UERadioCapability_t));
        radio_capa = (S1AP_UERadioCapability_t *)amf4g_ue->radio_capa;
        d_assert(radio_capa, return,);

        d_assert(UERadioCapability, return,);
        radio_capa->size = UERadioCapability->size;
        radio_capa->buf = core_calloc(radio_capa->size, sizeof(c_uint8_t)); 
        d_assert(radio_capa->buf, return, "core_calloc error(size=%d)",
                radio_capa->size);
        memcpy(radio_capa->buf, UERadioCapability->buf, radio_capa->size);
#else
        d_assert(UERadioCapability, return,);
        S1AP_STORE_DATA(&enb_ue->amf4g_ue->ueRadioCapability, UERadioCapability);
#endif
    }
    d_trace(10, "UE_Application_Layer_Measurement_Capability\n");
    if (UE_Application_Layer_Measurement_Capability)
    {
        memcpy(&enb_ue->amf4g_ue->ue_application_layer_measurement_capability.qoe, UE_Application_Layer_Measurement_Capability->buf,
        sizeof(enb_ue->amf4g_ue->ue_application_layer_measurement_capability.qoe));
    }
}

void s1ap_handle_initial_context_setup_response(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_InitialContextSetupResponse_t *InitialContextSetupResponse = NULL;

    S1AP_InitialContextSetupResponseIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_E_RABSetupListCtxtSURes_t *E_RABSetupListCtxtSURes = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    InitialContextSetupResponse =
        &successfulOutcome->value.choice.InitialContextSetupResponse;
    d_assert(InitialContextSetupResponse, return,);

    d_trace(3, "[AMF4G] Initial context setup response\n");

    for (i = 0; i < InitialContextSetupResponse->protocolIEs.list.count; i++)
    {
        ie = InitialContextSetupResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_E_RABSetupListCtxtSURes:
                E_RABSetupListCtxtSURes =
                    &ie->value.choice.E_RABSetupListCtxtSURes;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[%d]", *ENB_UE_S1AP_ID);
    amf4g_ue = enb_ue->amf4g_ue;
    d_assert(amf4g_ue, return,);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    d_assert(E_RABSetupListCtxtSURes, return,);
    for (i = 0; i < E_RABSetupListCtxtSURes->list.count; i++)
    {
        S1AP_E_RABSetupItemCtxtSUResIEs_t *ie2 = NULL;
        S1AP_E_RABSetupItemCtxtSURes_t *e_rab = NULL;

        amf4g_sess_t *sess = NULL;
        amf4g_bearer_t *bearer = NULL;

        ie2 = (S1AP_E_RABSetupItemCtxtSUResIEs_t *)
            E_RABSetupListCtxtSURes->list.array[i];
        d_assert(ie2, return,);

        e_rab = &ie2->value.choice.E_RABSetupItemCtxtSURes;
        d_assert(e_rab, return, "Null param");

        sess = amf4g_sess_find_by_ebi(amf4g_ue, e_rab->e_RAB_ID);
        d_assert(sess, return, "Null param");
        bearer = amf4g_default_bearer_in_sess(sess);
        d_assert(bearer, return, "Null param");
        memcpy(&bearer->enb_s1u_teid, e_rab->gTP_TEID.buf, 
                sizeof(bearer->enb_s1u_teid));
        bearer->enb_s1u_teid = ntohl(bearer->enb_s1u_teid);
        rv = s1ap_BIT_STRING_to_ip(
                &e_rab->transportLayerAddress, &bearer->enb_s1u_ip);
        d_assert(rv == CORE_OK, return,);

        d_trace(5, "    EBI[%d] ENB-S1U-TEID[%d]\n",
                bearer->ebi, bearer->enb_s1u_teid);

        if (FSM_CHECK(&bearer->sm, esm_state_active))
        {
            d_trace(5, "    NAS_EPS Type[%d]\n", amf4g_ue->nas_eps.type);
            int uli_presence = 0;
            if (amf4g_ue->nas_eps.type != AMF4G_EPS_TYPE_ATTACH_REQUEST)
            {
                d_trace(5, "    ### ULI PRESENT ###\n");
                uli_presence = 1;
            }
            rv = amf_n11_send_modify_bearer_request(bearer, uli_presence);
            d_assert(rv == CORE_OK, return, "gtp send failed");
        }
        
    }
    
}

void s1ap_handle_eNB_cp_relocation_indication(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    // status_t rv;
    // char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_ENBCPRelocationIndication_t *ENBCPRelocationIndication = NULL;

    S1AP_ENBCPRelocationIndicationIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    // S1AP_S_TMSI_t *S_TMSI = NULL;
    // S1AP_EUTRAN_CGI_t *EUTRAN_CGI = NULL;
    // S1AP_TAI_t *TAI = NULL;
    // S1AP_UL_CP_SecurityInformation_t *UL_CP_SecurityInformation = NULL;
    
    // pkbuf_t *s1apbuf = NULL;

    // S1AP_PLMNidentity_t *pLMNidentity = NULL;
    // S1AP_CellIdentity_t    *cell_ID = NULL;
    // S1AP_TAC_t *tAC = NULL;

    // amf4g_ue_t *amf4g_ue = NULL;
    // S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id = NULL;
    // enb_ue_t *enb_ue = NULL;

    // amf4g_ue = enb_ue->amf4g_ue;
    // d_assert(amf4g_ue, return,);

    // *mme_ue_s1ap_id = enb_ue->mme_ue_s1ap_id;
    // d_assert(mme_ue_s1ap_id, return,);

    // d_assert(enb, return,);
    // d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    ENBCPRelocationIndication = &initiatingMessage->value.choice.ENBCPRelocationIndication;
    d_assert(ENBCPRelocationIndication, return,);

    d_trace(3, "[AMF4G] eNB CP Relocation Indication\n");

    for (i = 0; i < ENBCPRelocationIndication->protocolIEs.list.count; i++)
    {
        ie = ENBCPRelocationIndication->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
    //         case S1AP_ProtocolIE_ID_id_S_TMSI:
    //             S_TMSI = &ie->value.choice.S_TMSI;
    //             break;
    //         case S1AP_ProtocolIE_ID_id_EUTRAN_CGI:
    //             EUTRAN_CGI = &ie->value.choice.EUTRAN_CGI;
    //             break;
    //         case S1AP_ProtocolIE_ID_id_TAI:
    //             TAI = &ie->value.choice.TAI;
    //             break;
    // case S1AP_ProtocolIE_ID_id_UL_CP_SecurityInformation:
    //             UL_CP_SecurityInformation = &ie->value.choice.UL_CP_SecurityInformation;
    //             break;
            default:
                break;
        }
    }

     d_trace(5, "ENB_UE_S1AP_ID = %d\n",ENB_UE_S1AP_ID);
    //  d_trace(5, "S_TMSI = %d\n",S_TMSI);
    //  d_trace(5, "EUTRAN_CGI = %d\n",EUTRAN_CGI);
    //  d_trace(5, "TAI = %d\n",TAI);
    //  d_trace(5, "rv = %d\n",rv);
    //  d_trace(5, "cell_ID = %d\n",*cell_ID);
    //  d_trace(5, "tAC = %d\n",*tAC);
    //  d_trace(5, "pLMNidentity = %d\n",*pLMNidentity);
    //  d_trace(5, "UL_CP_SecurityInformation = %d\n",*UL_CP_SecurityInformation);
    //  d_trace(5, "buf = %d\n",buf);

//     d_assert(ENB_UE_S1AP_ID, return,);
//     enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
//     if (!enb_ue)
//     {
//         enb_ue = enb_ue_add(enb);
//         d_assert(enb_ue, return, "Null param");

//         enb_ue->enb_ue_s1ap_id = *ENB_UE_S1AP_ID;

//         /* Find MME_UE if S_TMSI included */
//         if (S_TMSI)
//         {
//             served_gummei_t *served_gummei = &amf4g_self()->served_gummei[0];
//             guti_t guti;
//             amf4g_ue_t *amf4g_ue = NULL;

//             memset(&guti, 0, sizeof(guti_t));

//             /* Use the first configured plmn_id and mme group id */
//             memcpy(&guti.plmn_id, &served_gummei->plmn_id[0], PLMN_ID_LEN);
//             guti.mme_gid = served_gummei->amf4g_gid[0];

//             /* size must be 1 */
//             memcpy(&guti.mme_code, S_TMSI->mMEC.buf, S_TMSI->mMEC.size);
//             /* size must be 4 */
//             memcpy(&guti.m_tmsi, S_TMSI->m_TMSI.buf, S_TMSI->m_TMSI.size);
//             guti.m_tmsi = ntohl(guti.m_tmsi);

//             amf4g_ue = amf4g_ue_find_by_guti(&guti);
//             if (!amf4g_ue)
//             {
//                 d_warn("Unknown UE by S_TMSI[G:%d,C:%d,M_TMSI:0x%x]",
//                         guti.mme_gid, guti.mme_code, guti.m_tmsi);
//             }
//             else
//             {
//                 d_trace(5, "    S_TMSI[G:%d,C:%d,M_TMSI:0x%x] IMSI:[%s]\n",
//                         amf4g_ue->guti.mme_gid,
//                         amf4g_ue->guti.mme_code,
//                         amf4g_ue->guti.m_tmsi,
//                         MME_UE_HAVE_IMSI(amf4g_ue) 
//                             ? amf4g_ue->imsi_bcd : "Unknown");

//                 /* If NAS(amf4g_ue_t) has already been associated with
//                  * older S1(enb_ue_t) context */
//                 if (ECM_CONNECTED(amf4g_ue))
//                 {
// #if 1  /* IMPLICIT_S1_RELEASE */
//                    /* Implcit S1 release */
//                     d_warn("Implicit S1 release");
//                     d_warn("    ENB_UE_S1AP_ID[%d] MME_UE_S1AP_ID[%d]",
//                           amf4g_ue->enb_ue->enb_ue_s1ap_id,
//                           amf4g_ue->enb_ue->mme_ue_s1ap_id);
//                     rv = enb_ue_remove(amf4g_ue->enb_ue);
//                     d_assert(rv == CORE_OK,,);

// #else /* S1_HOLDING_TIMER */

//                     /* Previous S1(enb_ue_t) context the holding timer(30secs)
//                      * is started.
//                      * Newly associated S1(enb_ue_t) context holding timer
//                      * is stopped. */
//                     d_trace(5, "Start S1 Holding Timer\n");
//                     d_trace(5, "    ENB_UE_S1AP_ID[%d] MME_UE_S1AP_ID[%d]\n",
//                             amf4g_ue->enb_ue->enb_ue_s1ap_id, 
//                             amf4g_ue->enb_ue->mme_ue_s1ap_id);

//                     /* De-associate S1 with NAS/EMM */
//                     rv = enb_ue_deassociate(amf4g_ue->enb_ue);
//                     d_assert(rv == CORE_OK,,);

//                     tm_start(amf4g_ue->enb_ue->holding_timer);
// #endif
//                 }
//                 tm_stop(enb_ue->holding_timer);
//                 amf4g_ue_associate_enb_ue(amf4g_ue, enb_ue);
//             }
//         }
//     }

//     d_assert(TAI, return,);
//     pLMNidentity = &TAI->pLMNidentity;
//     d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
//     tAC = &TAI->tAC;
//     d_assert(tAC && tAC->size == sizeof(c_uint16_t), return,);

//     memcpy(&enb_ue->nas.tai.plmn_id, pLMNidentity->buf, 
//             sizeof(enb_ue->nas.tai.plmn_id));
//     memcpy(&enb_ue->nas.tai.tac, tAC->buf, sizeof(enb_ue->nas.tai.tac));
//     enb_ue->nas.tai.tac = ntohs(enb_ue->nas.tai.tac);
    
//     d_assert(EUTRAN_CGI, return,);
//     pLMNidentity = &EUTRAN_CGI->pLMNidentity;
//     d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
//     cell_ID = &EUTRAN_CGI->cell_ID;
//     d_assert(cell_ID, return,);
//     memcpy(&enb_ue->nas.e_cgi.plmn_id, pLMNidentity->buf, 
//             sizeof(enb_ue->nas.e_cgi.plmn_id));
//     memcpy(&enb_ue->nas.e_cgi.cell_id, cell_ID->buf,
//             sizeof(enb_ue->nas.e_cgi.cell_id));
//     enb_ue->nas.e_cgi.cell_id = (ntohl(enb_ue->nas.e_cgi.cell_id) >> 4);

    // d_trace(5, "    ENB_UE_S1AP_ID[%d] MME_UE_S1AP_ID[%d] TAC[%d]\n",
    //     enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id, enb_ue->nas.tai.tac);

    // rv = s1ap_build_connection_establishment_indication(
    //     &s1apbuf,mme_ue_s1ap_id,ENB_UE_S1AP_ID,amf4g_ue);

    
}

void s1ap_handle_initial_context_setup_failure(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_InitialContextSetupFailure_t *InitialContextSetupFailure = NULL;

    S1AP_InitialContextSetupFailureIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(unsuccessfulOutcome, return,);
    InitialContextSetupFailure =
        &unsuccessfulOutcome->value.choice.InitialContextSetupFailure;
    d_assert(InitialContextSetupFailure, return,);

    d_trace(3, "[AMF4G] Initial context setup failure\n");

    for (i = 0; i < InitialContextSetupFailure->protocolIEs.list.count; i++)
    {
        ie = InitialContextSetupFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    d_assert(Cause, return,);

    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    if (enb_ue == NULL)
    {
        d_warn("Initial context setup failure : cannot find eNB-UE-S1AP-ID[%d]",
                *ENB_UE_S1AP_ID);
        return;
    }
    amf4g_ue = enb_ue->amf4g_ue;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
            Cause->present, Cause->choice.radioNetwork);

    d_assert(amf4g_ue,,);
    if (amf4g_ue && FSM_CHECK(&amf4g_ue->sm, emm_state_registered))
    {
        d_trace(5, "    EMM-Registered\n");

        /*
         * 19.2.2.3 in Spec 36.300
         *
         * In case of failure, eNB and MME behaviours are not mandated.
         *
         * Both implicit release (local release at each node) and
         * explicit release (MME-initiated UE Context Release procedure)
         * may in principle be adopted. The eNB should ensure
         * that no hanging resources remain at the eNB.
         */

#if 0 /* NOTHING TO DO */

#if 0 /* FIXME : Does it needed? */
        rv = nas_send_service_reject(amf4g_ue,
            EMM_CAUSE_PROTOCOL_ERROR_UNSPECIFIED);
        d_assert(rv == CORE_OK,,
                "nas_send_service_reject() failed");
#endif

#if 1 /* Explicit Release */

        d_trace(5, "    Explicit Release\n");
        rv = s1ap_send_ue_context_release_command(enb_ue,
                S1AP_Cause_PR_nas,
#if 1 /* NAS Cause: Normal Relase */
                S1AP_CauseNas_normal_release,
#else /* NAS Cause : Detach */
                S1AP_CauseNas_detach,
#endif
                S1AP_UE_CTX_REL_UNLINK_MME_UE_CONTEXT, 0);
        d_assert(rv == CORE_OK,, "s1ap send error");

#else  /* Implicit Release */

        d_trace(5, "    Implicit Release\n");
        rv = enb_ue_remove(enb_ue);
        d_assert(rv == CORE_OK,, "enb_ue_remove() failed");

        rv = amf4g_ue_deassociate(amf4g_ue);
        d_assert(rv == CORE_OK,, "amf4g_ue_deassociate() failed");

#endif

#endif
    }
    else
    {
        d_trace(5, "    NOT EMM-Registered\n");
        d_assert(amf4g_ue,,);
        rv = amf4g_send_delete_session_or_ue_context_release(amf4g_ue, enb_ue);
        d_assert(rv == CORE_OK,,
                "mme_send_delete_session_or_ue_context_release() failed");
    }
}

void s1ap_handle_e_rab_setup_response(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_E_RABSetupResponse_t *E_RABSetupResponse = NULL;

    S1AP_E_RABSetupResponseIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_E_RABSetupListBearerSURes_t *E_RABSetupListBearerSURes = NULL;

    enb_ue_t *enb_ue = NULL;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    E_RABSetupResponse = &successfulOutcome->value.choice.E_RABSetupResponse;
    d_assert(E_RABSetupResponse, return,);

    d_trace(3, "[AMF4G] E-RAB setup response\n");

    for (i = 0; i < E_RABSetupResponse->protocolIEs.list.count; i++)
    {
        ie = E_RABSetupResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_E_RABSetupListBearerSURes:
                E_RABSetupListBearerSURes =
                    &ie->value.choice.E_RABSetupListBearerSURes;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[%d]", *ENB_UE_S1AP_ID);
    amf4g_ue = enb_ue->amf4g_ue;
    d_assert(amf4g_ue, return,);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    d_assert(E_RABSetupListBearerSURes, return,);
    for (i = 0; i < E_RABSetupListBearerSURes->list.count; i++)
    {
        S1AP_E_RABSetupItemBearerSUResIEs_t *ie2 = NULL;
        S1AP_E_RABSetupItemBearerSURes_t *e_rab = NULL;

        amf4g_bearer_t *bearer = NULL;

        ie2 = (S1AP_E_RABSetupItemBearerSUResIEs_t *)
            E_RABSetupListBearerSURes->list.array[i];
        d_assert(ie2, return,);

        e_rab = &ie2->value.choice.E_RABSetupItemBearerSURes;
        d_assert(e_rab, return, "Null param");

        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, e_rab->e_RAB_ID);
        d_assert(bearer, return, "Null param");

        memcpy(&bearer->enb_s1u_teid, e_rab->gTP_TEID.buf, 
                sizeof(bearer->enb_s1u_teid));
        bearer->enb_s1u_teid = ntohl(bearer->enb_s1u_teid);
        rv = s1ap_BIT_STRING_to_ip(
                &e_rab->transportLayerAddress, &bearer->enb_s1u_ip);
        d_assert(rv == CORE_OK, return,);

        d_trace(5, "    EBI[%d]\n", bearer->ebi);

        if (FSM_CHECK(&bearer->sm, esm_state_active))
        {
            status_t rv;

            amf4g_bearer_t *linked_bearer = amf4g_linked_bearer(bearer);
            d_assert(linked_bearer, return, "Null param");
            d_trace(5, "    Linked-EBI[%d]\n", linked_bearer->ebi);

            if (bearer->ebi == linked_bearer->ebi)
            {
                rv = amf_n11_send_modify_bearer_request(bearer, 0);
                d_assert(rv == CORE_OK, return, "gtp send failed");
            }
            else
            {
                // TODO : SEND CREATE BEARER RESPONSE
            }
        }
    }
}

void s1ap_handle_e_rab_modify_response(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    //status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_E_RABModifyResponse_t *E_RABModifyResponse = NULL;

    S1AP_E_RABModifyResponseIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_E_RABModifyListBearerModRes_t *E_RABModifyListBearerModRes = NULL;
    //S1AP_SecondaryRATDataUsageReportList_t *SecondaryRATDataUsageReportList = NULL;

    enb_ue_t *enb_ue = NULL;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    E_RABModifyResponse = &successfulOutcome->value.choice.E_RABModifyResponse;
    d_assert(E_RABModifyResponse, return,);

    d_trace(3, "[AMF4G] E-RAB modify response\n");

    for (i = 0; i < E_RABModifyResponse->protocolIEs.list.count; i++)
    {
        ie = E_RABModifyResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_E_RABModifyListBearerModRes:
                E_RABModifyListBearerModRes =
                    &ie->value.choice.E_RABModifyListBearerModRes;
                break;
            /*case S1AP_ProtocolIE_ID_id_SecondaryRATDataUsageReportList:
                SecondaryRATDataUsageReportList =
                    &ie->value.choice.SecondaryRATDataUsageReportList;
        break;*/
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(enb_ue, return, "No UE Context[%d]", *ENB_UE_S1AP_ID);
    amf4g_ue = enb_ue->amf4g_ue;
    d_assert(amf4g_ue, return,);

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    d_assert(E_RABModifyListBearerModRes, return,);
    for (i = 0; i < E_RABModifyListBearerModRes->list.count; i++)
    {
        S1AP_E_RABModifyItemBearerModResIEs_t *ie2 = NULL;
        S1AP_E_RABModifyItemBearerModRes_t *e_rab = NULL;

        amf4g_bearer_t *bearer = NULL;

        ie2 = (S1AP_E_RABModifyItemBearerModResIEs_t *)
            E_RABModifyListBearerModRes->list.array[i];
        d_assert(ie2, return,);

        e_rab = &ie2->value.choice.E_RABModifyItemBearerModRes;
        d_assert(e_rab, return, "Null param");

        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, e_rab->e_RAB_ID);
        d_assert(bearer, return, "Null param");

        if (FSM_CHECK(&bearer->sm, esm_state_active))
        {
            status_t rv;
            if (bearer->enb_s1u_teid != bearer->upf_s1u_teid)
            {
                //if(SecondaryRATDataUsageReportList != NULL)
                {
                    //rv = amf_n11_send_change_notification_request(amf4g_ue, NULL);
                    //d_assert(rv == CORE_OK, return, "gtp send failed");
                }
        //else
        {
            rv = amf_n11_send_delete_session_request(bearer->sess);
            d_assert(rv == CORE_OK, return, "gtp send failed");
        } 
            }
            else
            {
                // TODO : SEND UPDATE BEARER RESPONSE
            }  
        }
    }
}

void s1ap_handle_ue_context_release_request(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UEContextReleaseRequest_t *UEContextReleaseRequest = NULL;

    S1AP_UEContextReleaseRequest_IEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    enb_ue_t *enb_ue = NULL;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UEContextReleaseRequest =
        &initiatingMessage->value.choice.UEContextReleaseRequest;
    d_assert(UEContextReleaseRequest, return,);

    d_trace(3, "[AMF4G] UE Context release request\n");

    for (i = 0; i < UEContextReleaseRequest->protocolIEs.list.count; i++)
    {
        ie = UEContextReleaseRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(MME_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    if (!enb_ue)
    {
        d_warn("No ENB UE Context : AMF4G_UE_S1AP_ID[%d]", *MME_UE_S1AP_ID);
        rv = s1ap_send_error_indication(enb, 
                MME_UE_S1AP_ID, ENB_UE_S1AP_ID,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK, return, "s1ap send error");
        return;
    }

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    d_assert(Cause, return,);
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
            Cause->present, Cause->choice.radioNetwork);

    switch(Cause->present)
    {
        case S1AP_Cause_PR_radioNetwork:
        case S1AP_Cause_PR_transport:
        case S1AP_Cause_PR_protocol:
        case S1AP_Cause_PR_misc:
            break;
        case S1AP_Cause_PR_nas:
            d_warn("NAS-Cause[%d]", Cause->choice.nas);
        default:
            d_warn("Invalid cause group[%d]", Cause->present);
            break;
    }

    amf4g_ue = enb_ue->amf4g_ue;
    if (amf4g_ue)
    {
        if (FSM_CHECK(&amf4g_ue->sm, emm_state_registered))
        {
            d_trace(5, "    EMM-Registered\n");
            rv = amf4g_send_release_access_bearer_or_ue_context_release(
                    amf4g_ue, enb_ue);
            d_assert(rv == CORE_OK,, "amf4g_send_release_access_bearer_or_"
                    "ue_context_release() failed");
        }
        else
        {
            d_trace(5, "    NOT EMM-Registered\n");
            rv = amf4g_send_delete_session_or_ue_context_release(amf4g_ue, enb_ue);
            d_assert(rv == CORE_OK,,
                    "amf4g_send_delete_session_or_ue_context_release() failed");
        }
    }
    else
    {
        d_trace(5, "    S1 Context Not Associated\n");
        rv = s1ap_send_ue_context_release_command(enb_ue, 
                S1AP_Cause_PR_nas, S1AP_CauseNas_normal_release,
                S1AP_UE_CTX_REL_NO_ACTION, 0);
        d_assert(rv == CORE_OK,, "s1ap send error");
    }
}

void s1ap_handle_ue_context_release_complete(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_UEContextReleaseComplete_t *UEContextReleaseComplete = NULL;

    S1AP_UEContextReleaseComplete_IEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    UEContextReleaseComplete =
        &successfulOutcome->value.choice.UEContextReleaseComplete;
    d_assert(UEContextReleaseComplete, return,);

    d_trace(3, "[AMF4G] UE Context release complete\n");

    for (i = 0; i < UEContextReleaseComplete->protocolIEs.list.count; i++)
    {
        ie = UEContextReleaseComplete->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(MME_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    if (!enb_ue)
    {
        d_warn("No ENB UE Context : AMF4G_UE_S1AP_ID[%d]", *MME_UE_S1AP_ID);
        rv = s1ap_send_error_indication(enb, 
                MME_UE_S1AP_ID, NULL,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK, return, "s1ap send error");
        return;
    }
    amf4g_ue = enb_ue->amf4g_ue;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    switch (enb_ue->ue_ctx_rel_action)
    {
        case S1AP_UE_CTX_REL_NO_ACTION:
        {
            d_trace(5, "    No Action\n");
            rv = enb_ue_remove(enb_ue);
            d_assert(rv == CORE_OK,, "enb_ue_remove() failed");
            break;
        }
        case S1AP_UE_CTX_REL_S1_NORMAL_RELEASE:
        {
            d_trace(5, "    Action: S1 normal release\n");
            rv = enb_ue_remove(enb_ue);
            d_assert(rv == CORE_OK,, "enb_ue_remove() failed");

            d_assert(amf4g_ue,,);
            rv = amf4g_ue_deassociate(amf4g_ue);
            d_assert(rv == CORE_OK,, "amf4g_ue_deassociate() failed");
            break;
        }
        case S1AP_UE_CTX_REL_UE_CONTEXT_REMOVE:
        {
            d_trace(5, "    Action: UE context remove()\n");
            rv = enb_ue_remove(enb_ue);
            d_assert(rv == CORE_OK,, "enb_ue_removeI() failed");

            d_assert(amf4g_ue,,);
            rv = amf4g_ue_remove(amf4g_ue);
            d_assert(rv == CORE_OK,, "amf4g_ue_remove() failed");
            break;
        }
        case S1AP_UE_CTX_REL_DELETE_INDIRECT_TUNNEL:
        {
            d_trace(5, "    Action: Delete indirect tunnel\n");

            rv = source_ue_deassociate_target_ue(enb_ue);
            d_assert(rv == CORE_OK,,
                    "source_ue_deassociate_target_ue() failed");

            rv = enb_ue_remove(enb_ue);
            d_assert(rv == CORE_OK,, "enb_ue_removeI() failed");

            d_assert(amf4g_ue,,);
            if (amf4g_ue_have_indirect_tunnel(amf4g_ue))
            {
                // TODO : SEND DELETE INDIRECT DATA FORWARDINg TUNNEL REQUEST
            }
            else
            {
                d_warn("Check your eNodeB");
                d_warn("  There is no INDIRECT TUNNEL");
                d_warn("  Packet could be dropped during S1-Handover");
                rv = amf4g_ue_clear_indirect_tunnel(amf4g_ue);
                d_assert(rv == CORE_OK,,
                        "amf4g_ue_clear_indirect_tunnel() failed");
            }
            break;
        }
        default:
        {
            d_assert(0,, "Invalid Action[%d]", enb_ue->ue_ctx_rel_action);
            break;
        }
    }
}

// Handle UE Context Resume Request
void s1ap_handle_ue_context_resume_request(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
	S1AP_UEContextResumeRequest_t *UEContextResumeRequest = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
	S1AP_UEContextResumeRequestIEs_t *ie = NULL;
    // S1AP_E_RABFailedToResumeListResumeReq_t	 *E_RABFailedToResumeListResumeReq;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

	// Message Type
    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UEContextResumeRequest= &initiatingMessage->value.choice.UEContextResumeRequest;
	//--------------
	
    d_trace(3, "[AMF4G] UE Context Resume Request\n");
	
    for (i = 0; i <UEContextResumeRequest->protocolIEs.list.count; i++)
    {
        ie = UEContextResumeRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
			case S1AP_ProtocolIE_ID_id_E_RABFailedToResumeListResumeReq:
				// E_RABFailedToResumeListResumeReq = &ie->value.choice.E_RABFailedToResumeListResumeReq;
				break;
            default:
                break;
        }
    }

    // Build response and send back to enb
    pkbuf_t *s1apbuf = NULL;

    d_trace(3, "[AMF4G] s1 ue_context_resume response\n");
    d_assert(s1ap_build_ue_context_resume_response(&s1apbuf, ENB_UE_S1AP_ID, MME_UE_S1AP_ID) == CORE_OK, 
            return, "s1ap_build_ue_context_resume_response() failed");

    d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,
            "s1ap_send_to_enb() failed");
}

void s1ap_handle_ue_context_modification_response(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_UEContextModificationResponse_t *UEContextModificationResponse = NULL;

    S1AP_UEContextModificationResponseIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    UEContextModificationResponse =
        &successfulOutcome->value.choice.UEContextModificationResponse;
    d_assert(UEContextModificationResponse, return,);

    d_trace(3, "[AMF4G] Receive UE Context modification response\n");
    
    for (i = 0; i < UEContextModificationResponse->protocolIEs.list.count; i++)
    {
        ie = UEContextModificationResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    d_assert(MME_UE_S1AP_ID, return,);

    enb_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    if (!enb_ue)
    {
        d_warn("No ENB UE Context : AMF4G_UE_S1AP_ID[%d]", *MME_UE_S1AP_ID);
        rv = s1ap_send_error_indication(enb, 
                MME_UE_S1AP_ID, NULL,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK, return, "s1ap send error");
        return;
    }
    d_trace(3, "UE Context modification. AMF4G_UE_S1AP_ID[%lu], eNB_UE_S1AP_ID[%lu]\n", *MME_UE_S1AP_ID, *ENB_UE_S1AP_ID);
}

void s1ap_handle_ue_context_modification_failure(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_UEContextModificationFailure_t *UEContextModificationFailure = NULL;

    S1AP_UEContextModificationFailureIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(unsuccessfulOutcome, return,);
    UEContextModificationFailure =
        &unsuccessfulOutcome->value.choice.UEContextModificationFailure;
    d_assert(UEContextModificationFailure, return,);

    d_trace(3, "[AMF4G] Receive UE Context modification response\n");
    
    for (i = 0; i < UEContextModificationFailure->protocolIEs.list.count; i++)
    {
        ie = UEContextModificationFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    d_assert(MME_UE_S1AP_ID, return,);

    enb_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    if (!enb_ue)
    {
        d_warn("No ENB UE Context : AMF4G_UE_S1AP_ID[%d]", *MME_UE_S1AP_ID);
        rv = s1ap_send_error_indication(enb, 
                MME_UE_S1AP_ID, NULL,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK, return, "s1ap send error");
        return;
    }
    d_trace(3, "UE Context modification failure. AMF4G_UE_S1AP_ID[%lu], eNB_UE_S1AP_ID[%lu]\n", *MME_UE_S1AP_ID, *ENB_UE_S1AP_ID);
    d_trace(3, "Group[%d]Cause[%d]\n", Cause->present, Cause->choice.radioNetwork);
}

void s1ap_handle_ue_context_modification_indication(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UEContextModificationIndication_t *UEContextModificationIndication = NULL;

    S1AP_UEContextModificationIndicationIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UEContextModificationIndication =
        &initiatingMessage->value.choice.UEContextModificationIndication;
    d_assert(UEContextModificationIndication, return,);

    d_trace(3, "[AMF4G] Receive UE Context confirm\n");
    
    for (i = 0; i < UEContextModificationIndication->protocolIEs.list.count; i++)
    {
        ie = UEContextModificationIndication->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    d_assert(MME_UE_S1AP_ID, return,);
    d_assert(ENB_UE_S1AP_ID, return,);

    enb_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    if (!enb_ue)
    {
        d_warn("No ENB UE Context : AMF4G_UE_S1AP_ID[%d]", *MME_UE_S1AP_ID);
        rv = s1ap_send_error_indication(enb, 
                MME_UE_S1AP_ID, NULL,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK, return, "s1ap send error");
        return;
    }
    amf4g_ue = enb_ue->amf4g_ue;
    rv = s1ap_send_ue_context_modification_confirm(enb_ue);
    d_trace(3, "UE Context modification confirm, GUTI: %d, ISMI: %s\n", amf4g_ue->guti_present, amf4g_ue->imsi);
}

void s1ap_handle_paging(amf4g_ue_t *amf4g_ue)
{
    pkbuf_t *s1apbuf = NULL;
    hash_index_t *hi = NULL;
    amf4g_enb_t *enb = NULL;
    int i;
    status_t rv;

    /* Find enB with matched TAI */
    for (hi = amf4g_enb_first(); hi; hi = amf4g_enb_next(hi))
    {
        enb = amf4g_enb_this(hi);
        for (i = 0; i < enb->num_of_supported_ta_list; i++)
        {
            if (!memcmp(&enb->supported_ta_list[i],
                        &amf4g_ue->tai, sizeof(tai_t)))
            {
                if (amf4g_ue->last_paging_msg)
                {
                    s1apbuf = amf4g_ue->last_paging_msg;
                    /* Save it for later use */
                    amf4g_ue->last_paging_msg = pkbuf_copy(s1apbuf);
                }
                else
                {
                    /* Buidl S1Ap Paging message */
                    rv = s1ap_build_paging(&s1apbuf, amf4g_ue);
                    d_assert(rv == CORE_OK && s1apbuf, return, 
                            "s1ap build error");

                    /* Save it for later use */
                    amf4g_ue->last_paging_msg = pkbuf_copy(s1apbuf);
                }

                /* Send to enb */
                d_assert(s1ap_send_to_enb(
                        enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,
                        return, "s1ap send error");
            }
        }
    }
}

void s1ap_handle_path_switch_request(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_PathSwitchRequest_t *PathSwitchRequest = NULL;

    S1AP_PathSwitchRequestIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_E_RABToBeSwitchedDLList_t *E_RABToBeSwitchedDLList = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_EUTRAN_CGI_t *EUTRAN_CGI = NULL;
    S1AP_TAI_t *TAI = NULL;
    S1AP_UESecurityCapabilities_t *UESecurityCapabilities = NULL;

    S1AP_PLMNidentity_t *pLMNidentity = NULL;
    S1AP_CellIdentity_t    *cell_ID = NULL;
    S1AP_TAC_t *tAC = NULL;
    S1AP_EncryptionAlgorithms_t    *encryptionAlgorithms = NULL;
    S1AP_IntegrityProtectionAlgorithms_t *integrityProtectionAlgorithms = NULL;
    c_uint16_t eea = 0, eia = 0;

    enb_ue_t *enb_ue = NULL;
    amf4g_ue_t *amf4g_ue = NULL;
    pkbuf_t *s1apbuf = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    PathSwitchRequest = &initiatingMessage->value.choice.PathSwitchRequest;
    d_assert(PathSwitchRequest, return,);

    d_trace(3, "[AMF4G] Path switch request\n");

    for (i = 0; i < PathSwitchRequest->protocolIEs.list.count; i++)
    {
        ie = PathSwitchRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_E_RABToBeSwitchedDLList:
                E_RABToBeSwitchedDLList =
                    &ie->value.choice.E_RABToBeSwitchedDLList;
                break;
            case S1AP_ProtocolIE_ID_id_SourceMME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_EUTRAN_CGI:
                EUTRAN_CGI = &ie->value.choice.EUTRAN_CGI;
                break;
            case S1AP_ProtocolIE_ID_id_TAI:
                TAI = &ie->value.choice.TAI;
                break;
            case S1AP_ProtocolIE_ID_id_UESecurityCapabilities:
                UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(EUTRAN_CGI, return,);
    pLMNidentity = &EUTRAN_CGI->pLMNidentity;
    d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
    cell_ID = &EUTRAN_CGI->cell_ID;
    d_assert(cell_ID, return,);

    d_assert(TAI, return,);
    pLMNidentity = &TAI->pLMNidentity;
    d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
    tAC = &TAI->tAC;
    d_assert(tAC && tAC->size == sizeof(c_uint16_t), return,);

    d_assert(UESecurityCapabilities, return,);
    encryptionAlgorithms =
        &UESecurityCapabilities->encryptionAlgorithms;
    integrityProtectionAlgorithms =
        &UESecurityCapabilities->integrityProtectionAlgorithms;

    d_assert(MME_UE_S1AP_ID, return,);
    d_assert(ENB_UE_S1AP_ID, return,);
    enb_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    if (!enb_ue)
    {
        d_error("Cannot find UE from sourceMME-UE-S1AP-ID[%d] and eNB[%s:%d]",
                *MME_UE_S1AP_ID, CORE_ADDR(enb->addr, buf), enb->enb_id);

        rv = s1ap_build_path_switch_failure(&s1apbuf,
                *ENB_UE_S1AP_ID, *MME_UE_S1AP_ID,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK && s1apbuf, return, "s1ap build error");

        rv = s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING);
        d_assert(rv == CORE_OK,, "s1ap send error");
        return;
    }

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    amf4g_ue = enb_ue->amf4g_ue;
    d_assert(amf4g_ue, return, "Null param");

    if (SECURITY_CONTEXT_IS_VALID(amf4g_ue))
    {
        amf4g_ue->nhcc++;
        amf4g_kdf_nh(amf4g_ue->kasme, amf4g_ue->nh, amf4g_ue->nh);
    }
    else
    {
        rv = s1ap_build_path_switch_failure(&s1apbuf,
                *ENB_UE_S1AP_ID, *MME_UE_S1AP_ID,
                S1AP_Cause_PR_nas, S1AP_CauseNas_authentication_failure);
        d_assert(rv == CORE_OK && s1apbuf, return, "s1ap build error");

        rv = s1ap_send_to_enb_ue(enb_ue, s1apbuf);
        d_assert(rv == CORE_OK,, "s1ap send error");
        return;
    }

    enb_ue->enb_ue_s1ap_id = *ENB_UE_S1AP_ID;

    memcpy(&enb_ue->nas.tai.plmn_id, pLMNidentity->buf, 
            sizeof(enb_ue->nas.tai.plmn_id));
    memcpy(&enb_ue->nas.tai.tac, tAC->buf, sizeof(enb_ue->nas.tai.tac));
    enb_ue->nas.tai.tac = ntohs(enb_ue->nas.tai.tac);

    memcpy(&enb_ue->nas.e_cgi.plmn_id, pLMNidentity->buf, 
            sizeof(enb_ue->nas.e_cgi.plmn_id));
    memcpy(&enb_ue->nas.e_cgi.cell_id, cell_ID->buf,
            sizeof(enb_ue->nas.e_cgi.cell_id));
    enb_ue->nas.e_cgi.cell_id = (ntohl(enb_ue->nas.e_cgi.cell_id) >> 4);

    d_trace(5, "    OLD TAI[PLMN_ID:0x%x,TAC:%d]\n",
            amf4g_ue->tai.plmn_id, amf4g_ue->tai.tac);
    d_trace(5, "    OLD E_CGI[PLMN_ID:0x%x,CELL_ID:%d]\n",
            amf4g_ue->e_cgi.plmn_id, amf4g_ue->e_cgi.cell_id);
    d_trace(5, "    TAI[PLMN_ID:0x%x,TAC:%d]\n",
            enb_ue->nas.tai.plmn_id, enb_ue->nas.tai.tac);
    d_trace(5, "    E_CGI[PLMN_ID:0x%x,CELL_ID:%d]\n",
            enb_ue->nas.e_cgi.plmn_id, enb_ue->nas.e_cgi.cell_id);

    /* Copy TAI and ECGI from enb_ue */
    memcpy(&amf4g_ue->tai, &enb_ue->nas.tai, sizeof(tai_t));
    memcpy(&amf4g_ue->e_cgi, &enb_ue->nas.e_cgi, sizeof(e_cgi_t));

    memcpy(&eea, encryptionAlgorithms->buf, sizeof(eea));
    eea = ntohs(eea);
    amf4g_ue->ue_network_capability.eea = eea >> 9;
    amf4g_ue->ue_network_capability.eea0 = 1;

    memcpy(&eia, integrityProtectionAlgorithms->buf, sizeof(eia));
    eia = ntohs(eia);
    amf4g_ue->ue_network_capability.eia = eia >> 9;
    amf4g_ue->ue_network_capability.eia0 = 0;

    d_assert(E_RABToBeSwitchedDLList, return,);
    for (i = 0; i < E_RABToBeSwitchedDLList->list.count; i++)
    {
        S1AP_E_RABToBeSwitchedDLItemIEs_t *ie2 = NULL;
        S1AP_E_RABToBeSwitchedDLItem_t *e_rab = NULL;

        amf4g_bearer_t *bearer = NULL;

        ie2 = (S1AP_E_RABToBeSwitchedDLItemIEs_t *)
            E_RABToBeSwitchedDLList->list.array[i];
        d_assert(ie2, return,);

        e_rab = &ie2->value.choice.E_RABToBeSwitchedDLItem;
        d_assert(e_rab, return, "Null param");

        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, e_rab->e_RAB_ID);
        d_assert(bearer, return, "Cannot find e_RAB_ID[%d]", e_rab->e_RAB_ID);

        memcpy(&bearer->enb_s1u_teid, e_rab->gTP_TEID.buf, 
                sizeof(bearer->enb_s1u_teid));
        bearer->enb_s1u_teid = ntohl(bearer->enb_s1u_teid);
        rv = s1ap_BIT_STRING_to_ip(
                &e_rab->transportLayerAddress, &bearer->enb_s1u_ip);
        d_assert(rv == CORE_OK, return,);

        GTP_COUNTER_INCREMENT(
                amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_PATH_SWITCH);

        rv = amf_n11_send_modify_bearer_request(bearer, 1);
        d_assert(rv == CORE_OK, return, "gtp send failed");
    }

    /* Switch to enb */
    enb_ue_switch_to_enb(enb_ue, enb);
}

void s1ap_handle_enb_configuration_transfer(
        amf4g_enb_t *enb, s1ap_message_t *message, pkbuf_t *pkbuf)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_ENBConfigurationTransfer_t *ENBConfigurationTransfer = NULL;

    S1AP_ENBConfigurationTransferIEs_t *ie = NULL;
    S1AP_SONConfigurationTransfer_t *SONConfigurationTransfer = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    ENBConfigurationTransfer =
        &initiatingMessage->value.choice.ENBConfigurationTransfer;
    d_assert(ENBConfigurationTransfer, return,);

    d_trace(3, "[AMF4G] ENB configuration transfer\n");
    for (i = 0; i < ENBConfigurationTransfer->protocolIEs.list.count; i++)
    {
        ie = ENBConfigurationTransfer->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_SONConfigurationTransferECT:
                SONConfigurationTransfer =
                    &ie->value.choice.SONConfigurationTransfer;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    if (SONConfigurationTransfer)
    {
        S1AP_TargeteNB_ID_t *targeteNB_ID =
            &SONConfigurationTransfer->targeteNB_ID;
        S1AP_SourceeNB_ID_t *sourceeNB_ID =
            &SONConfigurationTransfer->sourceeNB_ID;

        amf4g_enb_t *target_enb = NULL;
        c_uint32_t source_enb_id, target_enb_id;
        c_uint16_t source_tac, target_tac;

        s1ap_ENB_ID_to_uint32(
                &sourceeNB_ID->global_ENB_ID.eNB_ID, &source_enb_id);
        s1ap_ENB_ID_to_uint32(
                &targeteNB_ID->global_ENB_ID.eNB_ID, &target_enb_id);

        memcpy(&source_tac, sourceeNB_ID->selected_TAI.tAC.buf,
                sizeof(source_tac));
        source_tac = ntohs(source_tac);
        memcpy(&target_tac, targeteNB_ID->selected_TAI.tAC.buf,
                sizeof(target_tac));
        target_tac = ntohs(target_tac);

        d_trace(5, "    Source : ENB_ID[%s:%d], TAC[%d]\n",
                sourceeNB_ID->global_ENB_ID.eNB_ID.present == 
                    S1AP_ENB_ID_PR_homeENB_ID ? "Home" : 
                sourceeNB_ID->global_ENB_ID.eNB_ID.present == 
                    S1AP_ENB_ID_PR_macroENB_ID ? "Macro" : "Others",
                source_enb_id, source_tac);
        d_trace(5, "    Target : ENB_ID[%s:%d], TAC[%d]\n",
                targeteNB_ID->global_ENB_ID.eNB_ID.present == 
                    S1AP_ENB_ID_PR_homeENB_ID ? "Home" : 
                targeteNB_ID->global_ENB_ID.eNB_ID.present == 
                    S1AP_ENB_ID_PR_macroENB_ID ? "Macro" : "Others",
                target_enb_id, target_tac);

        target_enb = amf4g_enb_find_by_enb_id(target_enb_id);
        if (target_enb == NULL)
        {
            d_warn("eNB configuration transfer : cannot find target eNB-id[%d]",
                    target_enb_id);
            return;
        }

        rv = s1ap_send_mme_configuration_transfer(
                target_enb, SONConfigurationTransfer);
        d_assert(rv == CORE_OK,,);
    }
}

void s1ap_handle_handover_required(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_HandoverRequired_t *HandoverRequired = NULL;

    S1AP_HandoverRequiredIEs_t *ie = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_HandoverType_t *HandoverType = NULL;
    S1AP_Cause_t *Cause = NULL;
    S1AP_TargetID_t *TargetID = NULL;
    S1AP_Source_ToTarget_TransparentContainer_t
        *Source_ToTarget_TransparentContainer = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    HandoverRequired = &initiatingMessage->value.choice.HandoverRequired;
    d_assert(HandoverRequired, return,);

    enb_ue_t *source_ue = NULL;
    amf4g_ue_t *amf4g_ue = NULL;
    amf4g_enb_t *target_enb = NULL;
    c_uint32_t target_enb_id = 0;

    d_trace(3, "[AMF4G] Handover required\n");
    for (i = 0; i < HandoverRequired->protocolIEs.list.count; i++)
    {
        ie = HandoverRequired->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_HandoverType:
                HandoverType = &ie->value.choice.HandoverType;
                break;
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            case S1AP_ProtocolIE_ID_id_TargetID:
                TargetID = &ie->value.choice.TargetID;
                break;
            case S1AP_ProtocolIE_ID_id_Source_ToTarget_TransparentContainer:
                Source_ToTarget_TransparentContainer =
                    &ie->value.choice.Source_ToTarget_TransparentContainer;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(TargetID, return,);
    switch(TargetID->present)
    {
        case S1AP_TargetID_PR_targeteNB_ID:
        {
            s1ap_ENB_ID_to_uint32(
                &TargetID->choice.targeteNB_ID->global_ENB_ID.eNB_ID,
                &target_enb_id);
            break;
        }
        default:
        {
            d_error("Not implemented(%d)", TargetID->present);
            return;
        }
    }

    target_enb = amf4g_enb_find_by_enb_id(target_enb_id);
    if (target_enb == NULL)
    {
        d_warn("Handover required : cannot find target eNB-id[%d]",
                target_enb_id);
        return;
    }

    d_assert(ENB_UE_S1AP_ID, return,);
    d_assert(MME_UE_S1AP_ID, return,);
    source_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(source_ue, return,
            "Cannot find UE for eNB-UE-S1AP-ID[%d] and eNB[%s:%d]",
            *ENB_UE_S1AP_ID,
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    d_assert(source_ue->mme_ue_s1ap_id == *MME_UE_S1AP_ID, return,
            "Conflict MME-UE-S1AP-ID : %d != %d\n",
            source_ue->mme_ue_s1ap_id, *MME_UE_S1AP_ID);

    d_trace(5, "    Source : ENB_UE_S1AP_ID[%d] MME_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);

    amf4g_ue = source_ue->amf4g_ue;
    d_assert(amf4g_ue, return,);

    if (SECURITY_CONTEXT_IS_VALID(amf4g_ue))
    {
        amf4g_ue->nhcc++;
        amf4g_kdf_nh(amf4g_ue->kasme, amf4g_ue->nh, amf4g_ue->nh);
    }
    else
    {
        d_assert(Cause, return,);

        rv = s1ap_send_handover_preparation_failure(source_ue, Cause);
        d_assert(rv == CORE_OK, return, "s1ap send error");

        return;
    }

    d_assert(HandoverType, return,);
    source_ue->handover_type = *HandoverType;

    rv = s1ap_send_handover_request(amf4g_ue, target_enb,
            ENB_UE_S1AP_ID, MME_UE_S1AP_ID,
            HandoverType, Cause,
            Source_ToTarget_TransparentContainer);
    d_assert(rv == CORE_OK,, "s1ap send error");
}

void s1ap_handle_handover_request_ack(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_HandoverRequestAcknowledge_t *HandoverRequestAcknowledge = NULL;

    S1AP_HandoverRequestAcknowledgeIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_E_RABAdmittedList_t *E_RABAdmittedList = NULL;
    S1AP_Target_ToSource_TransparentContainer_t
        *Target_ToSource_TransparentContainer = NULL;

    enb_ue_t *source_ue = NULL;
    enb_ue_t *target_ue = NULL;
    amf4g_ue_t *amf4g_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    HandoverRequestAcknowledge =
        &successfulOutcome->value.choice.HandoverRequestAcknowledge;
    d_assert(HandoverRequestAcknowledge, return,);

    d_trace(3, "[AMF4G] Handover request acknowledge\n");
    for (i = 0; i < HandoverRequestAcknowledge->protocolIEs.list.count; i++)
    {
        ie = HandoverRequestAcknowledge->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_E_RABAdmittedList:
                E_RABAdmittedList = &ie->value.choice.E_RABAdmittedList;
                break;
            case S1AP_ProtocolIE_ID_id_Target_ToSource_TransparentContainer:
                Target_ToSource_TransparentContainer =
                    &ie->value.choice.Target_ToSource_TransparentContainer;
                break;
            default:
                break;
        }
    }
    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(MME_UE_S1AP_ID, return,);
    d_assert(ENB_UE_S1AP_ID, return,);
    d_assert(E_RABAdmittedList, return,);
    d_assert(Target_ToSource_TransparentContainer, return,);

    target_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    d_assert(target_ue, return,
            "Cannot find UE for MME-UE-S1AP-ID[%d] and eNB[%s:%d]",
            *MME_UE_S1AP_ID,
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    target_ue->enb_ue_s1ap_id = *ENB_UE_S1AP_ID;

    source_ue = target_ue->source_ue;
    d_assert(source_ue, return,);
    amf4g_ue = source_ue->amf4g_ue;
    d_assert(amf4g_ue, return,);

    d_trace(5, "    Source : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);
    d_trace(5, "    Target : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            target_ue->enb_ue_s1ap_id, target_ue->mme_ue_s1ap_id);

    for (i = 0; i < E_RABAdmittedList->list.count; i++)
    {
        S1AP_E_RABAdmittedItemIEs_t *ie2 = NULL;
        S1AP_E_RABAdmittedItem_t *e_rab = NULL;

        amf4g_bearer_t *bearer = NULL;

        ie2 = (S1AP_E_RABAdmittedItemIEs_t *)E_RABAdmittedList->list.array[i];
        d_assert(ie2, return,);

        e_rab = &ie2->value.choice.E_RABAdmittedItem;
        d_assert(e_rab, return,);

        bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, e_rab->e_RAB_ID);
        d_assert(bearer, return,);

        memcpy(&bearer->target_s1u_teid, e_rab->gTP_TEID.buf, 
                sizeof(bearer->target_s1u_teid));
        bearer->target_s1u_teid = ntohl(bearer->target_s1u_teid);
        rv = s1ap_BIT_STRING_to_ip(
                &e_rab->transportLayerAddress, &bearer->target_s1u_ip);
        d_assert(rv == CORE_OK, return,);

        if (e_rab->dL_transportLayerAddress && e_rab->dL_gTP_TEID)
        {
            d_assert(e_rab->dL_gTP_TEID->buf, return,);
            d_assert(e_rab->dL_transportLayerAddress->buf, return,);
            memcpy(&bearer->enb_dl_teid, e_rab->dL_gTP_TEID->buf, 
                    sizeof(bearer->enb_dl_teid));
            bearer->enb_dl_teid = ntohl(bearer->enb_dl_teid);
            rv = s1ap_BIT_STRING_to_ip(
                    e_rab->dL_transportLayerAddress, &bearer->enb_dl_ip);
            d_assert(rv == CORE_OK, return,);
        }

        if (e_rab->uL_TransportLayerAddress && e_rab->uL_GTP_TEID)
        {
            d_assert(e_rab->uL_GTP_TEID->buf, return,);
            d_assert(e_rab->uL_TransportLayerAddress->buf, return,);
            memcpy(&bearer->enb_ul_teid, e_rab->uL_GTP_TEID->buf, 
                    sizeof(bearer->enb_ul_teid));
            bearer->enb_ul_teid = ntohl(bearer->enb_ul_teid);
            rv = s1ap_BIT_STRING_to_ip(
                    e_rab->uL_TransportLayerAddress, &bearer->enb_ul_ip);
            d_assert(rv == CORE_OK, return,);
        }
    }

    S1AP_STORE_DATA(&amf4g_ue->container, Target_ToSource_TransparentContainer);

    if (amf4g_ue_have_indirect_tunnel(amf4g_ue) == 1)
    {
        // TODO : SEND CREATE INDIRECT DATA FORWARDING TUNNEL REQUEST
    }
    else
    {
        rv = s1ap_send_handover_command(source_ue);
        d_assert(rv == CORE_OK, return, "gtp send failed");
    }
}

void s1ap_handle_handover_failure(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_HandoverFailure_t *HandoverFailure = NULL;

    S1AP_HandoverFailureIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    enb_ue_t *target_ue = NULL;
    enb_ue_t *source_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(unsuccessfulOutcome, return,);
    HandoverFailure = &unsuccessfulOutcome->value.choice.HandoverFailure;
    d_assert(HandoverFailure, return,);

    d_trace(3, "[AMF4G] Handover failure\n");
    for (i = 0; i < HandoverFailure->protocolIEs.list.count; i++)
    {
        ie = HandoverFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(MME_UE_S1AP_ID, return,);
    d_assert(Cause, return,);

    target_ue = enb_ue_find_by_mme_ue_s1ap_id(*MME_UE_S1AP_ID);
    d_assert(target_ue, return,
            "Cannot find UE for MME-UE-S1AP-ID[%d] and eNB[%s:%d]",
            *MME_UE_S1AP_ID,
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    source_ue = target_ue->source_ue;
    d_assert(source_ue, return,);

    d_trace(5, "    Source : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);
    d_trace(5, "    Target : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            target_ue->enb_ue_s1ap_id, target_ue->mme_ue_s1ap_id);

    rv = s1ap_send_handover_preparation_failure(source_ue, Cause);
    d_assert(rv == CORE_OK, return, "s1ap send error");

    rv = s1ap_send_ue_context_release_command(
        target_ue, S1AP_Cause_PR_radioNetwork,
        S1AP_CauseRadioNetwork_ho_failure_in_target_EPC_eNB_or_target_system,
        S1AP_UE_CTX_REL_DELETE_INDIRECT_TUNNEL, 0);
    d_assert(rv == CORE_OK, return, "s1ap send error");
}

void s1ap_handle_handover_cancel(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_HandoverCancel_t *HandoverCancel = NULL;

    S1AP_HandoverCancelIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    enb_ue_t *source_ue = NULL;
    enb_ue_t *target_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    HandoverCancel = &initiatingMessage->value.choice.HandoverCancel;
    d_assert(HandoverCancel, return,);

    d_trace(3, "[AMF4G] Handover cancel\n");
    for (i = 0; i < HandoverCancel->protocolIEs.list.count; i++)
    {
        ie = HandoverCancel->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            default:
                break;
        }
    }
    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(MME_UE_S1AP_ID, return,);
    d_assert(ENB_UE_S1AP_ID, return,);
    d_assert(Cause, return,);

    source_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(source_ue, return,
            "Cannot find UE for eNB-UE-S1AP-ID[%d] and eNB[%s:%d]",
            *ENB_UE_S1AP_ID,
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    d_assert(source_ue->mme_ue_s1ap_id == *MME_UE_S1AP_ID, return,
            "Conflict amf4g-UE-S1AP-ID : %d != %d\n",
            source_ue->mme_ue_s1ap_id, *MME_UE_S1AP_ID);

    target_ue = source_ue->target_ue;
    d_assert(target_ue, return,);

    d_trace(5, "    Source : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);
    d_trace(5, "    Target : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            target_ue->enb_ue_s1ap_id, target_ue->mme_ue_s1ap_id);

    rv = s1ap_send_handover_cancel_ack(source_ue);
    d_assert(rv == CORE_OK,, "s1ap send error");

    rv = s1ap_send_ue_context_release_command(
            target_ue, S1AP_Cause_PR_radioNetwork,
            S1AP_CauseRadioNetwork_handover_cancelled,
            S1AP_UE_CTX_REL_DELETE_INDIRECT_TUNNEL, 300);
    d_assert(rv == CORE_OK, return, "s1ap send error");

    d_trace(3, "[AMF4G] Handover Cancel : "
            "UE[eNB-UE-S1AP-ID(%d)] --> eNB[%s:%d]\n",
            source_ue->enb_ue_s1ap_id,
            CORE_ADDR(enb->addr, buf), enb->enb_id);
}

void s1ap_handle_enb_status_transfer(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_ENBStatusTransfer_t *ENBStatusTransfer = NULL;

    S1AP_ENBStatusTransferIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_ENB_StatusTransfer_TransparentContainer_t
        *ENB_StatusTransfer_TransparentContainer = NULL;

    enb_ue_t *source_ue = NULL, *target_ue = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    ENBStatusTransfer = &initiatingMessage->value.choice.ENBStatusTransfer;
    d_assert(ENBStatusTransfer, return,);

    d_trace(3, "[AMF4G] ENB status transfer\n");
    for (i = 0; i < ENBStatusTransfer->protocolIEs.list.count; i++)
    {
        ie = ENBStatusTransfer->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer:
                ENB_StatusTransfer_TransparentContainer =
                    &ie->value.choice.ENB_StatusTransfer_TransparentContainer;
                break;
            default:
                break;
        }
    }
    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(MME_UE_S1AP_ID, return,);
    d_assert(ENB_UE_S1AP_ID, return,);
    d_assert(ENB_StatusTransfer_TransparentContainer, return,);

    source_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(source_ue, return,
            "Cannot find UE for eNB-UE-S1AP-ID[%d] and eNB[%s:%d]",
            *ENB_UE_S1AP_ID,
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    d_assert(source_ue->mme_ue_s1ap_id == *MME_UE_S1AP_ID, return,
            "Conflict AMF4G-UE-S1AP-ID : %d != %d\n",
            source_ue->mme_ue_s1ap_id, *MME_UE_S1AP_ID);

    target_ue = source_ue->target_ue;
    d_assert(target_ue, return,);

    d_trace(5, "    Source : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);
    d_trace(5, "    Target : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            target_ue->enb_ue_s1ap_id, target_ue->mme_ue_s1ap_id);

    rv = s1ap_send_mme_status_transfer(target_ue,
            ENB_StatusTransfer_TransparentContainer);
    d_assert(rv == CORE_OK,,);
}

void s1ap_handle_handover_notification(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_HandoverNotify_t *HandoverNotify = NULL;

    S1AP_HandoverNotifyIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_EUTRAN_CGI_t *EUTRAN_CGI = NULL;
    S1AP_TAI_t *TAI = NULL;

    S1AP_PLMNidentity_t *pLMNidentity = NULL;
    S1AP_CellIdentity_t    *cell_ID = NULL;
    S1AP_TAC_t *tAC = NULL;

    enb_ue_t *source_ue = NULL;
    enb_ue_t *target_ue = NULL;
    amf4g_ue_t *amf4g_ue = NULL;
    amf4g_sess_t *sess = NULL;
    amf4g_bearer_t *bearer = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    HandoverNotify = &initiatingMessage->value.choice.HandoverNotify;
    d_assert(HandoverNotify, return,);

    d_trace(3, "[AMF4G] Handover notification\n");
    for (i = 0; i < HandoverNotify->protocolIEs.list.count; i++)
    {
        ie = HandoverNotify->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID:
                MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;
                break;
            case S1AP_ProtocolIE_ID_id_EUTRAN_CGI:
                EUTRAN_CGI = &ie->value.choice.EUTRAN_CGI;
                break;
            case S1AP_ProtocolIE_ID_id_TAI:
                TAI = &ie->value.choice.TAI;
                break;
            default:
                break;
        }
    }
    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(EUTRAN_CGI, return,);
    pLMNidentity = &EUTRAN_CGI->pLMNidentity;
    d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
    cell_ID = &EUTRAN_CGI->cell_ID;
    d_assert(cell_ID, return,);

    d_assert(TAI, return,);
    pLMNidentity = &TAI->pLMNidentity;
    d_assert(pLMNidentity && pLMNidentity->size == sizeof(plmn_id_t), return,);
    tAC = &TAI->tAC;
    d_assert(tAC && tAC->size == sizeof(c_uint16_t), return,);

    d_assert(ENB_UE_S1AP_ID, return,);
    d_assert(MME_UE_S1AP_ID, return,);
    target_ue = enb_ue_find_by_enb_ue_s1ap_id(enb, *ENB_UE_S1AP_ID);
    d_assert(target_ue, return,
            "Cannot find UE for eNB-UE-S1AP-ID[%d] and eNB[%s:%d]",
            *ENB_UE_S1AP_ID,
            CORE_ADDR(enb->addr, buf), enb->enb_id);
    d_assert(target_ue->mme_ue_s1ap_id == *MME_UE_S1AP_ID, return,
            "Conflict MME-UE-S1AP-ID : %d != %d\n",
            target_ue->mme_ue_s1ap_id, *MME_UE_S1AP_ID);

    source_ue = target_ue->source_ue;
    d_assert(source_ue, return,);
    amf4g_ue = source_ue->amf4g_ue;
    d_assert(amf4g_ue, return,);

    d_trace(5, "    Source : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);
    d_trace(5, "    Target : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            target_ue->enb_ue_s1ap_id, target_ue->mme_ue_s1ap_id);

    amf4g_ue_associate_enb_ue(amf4g_ue, target_ue);

    memcpy(&target_ue->nas.tai.plmn_id, pLMNidentity->buf, 
            sizeof(target_ue->nas.tai.plmn_id));
    memcpy(&target_ue->nas.tai.tac, tAC->buf, sizeof(target_ue->nas.tai.tac));
    target_ue->nas.tai.tac = ntohs(target_ue->nas.tai.tac);

    memcpy(&target_ue->nas.e_cgi.plmn_id, pLMNidentity->buf, 
            sizeof(target_ue->nas.e_cgi.plmn_id));
    memcpy(&target_ue->nas.e_cgi.cell_id, cell_ID->buf,
            sizeof(target_ue->nas.e_cgi.cell_id));
    target_ue->nas.e_cgi.cell_id = (ntohl(target_ue->nas.e_cgi.cell_id) >> 4);

    d_trace(5, "    OLD TAI[PLMN_ID:0x%x,TAC:%d]\n",
            amf4g_ue->tai.plmn_id, amf4g_ue->tai.tac);
    d_trace(5, "    OLD E_CGI[PLMN_ID:0x%x,CELL_ID:%d]\n",
            amf4g_ue->e_cgi.plmn_id, amf4g_ue->e_cgi.cell_id);
    d_trace(5, "    TAI[PLMN_ID:0x%x,TAC:%d]\n",
            target_ue->nas.tai.plmn_id, target_ue->nas.tai.tac);
    d_trace(5, "    E_CGI[PLMN_ID:0x%x,CELL_ID:%d]\n",
            target_ue->nas.e_cgi.plmn_id, target_ue->nas.e_cgi.cell_id);

    /* Copy TAI and ECGI from enb_ue */
    memcpy(&amf4g_ue->tai, &target_ue->nas.tai, sizeof(tai_t));
    memcpy(&amf4g_ue->e_cgi, &target_ue->nas.e_cgi, sizeof(e_cgi_t));

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            bearer->enb_s1u_teid = bearer->target_s1u_teid;
            memcpy(&bearer->enb_s1u_ip, &bearer->target_s1u_ip, sizeof(ip_t));

            GTP_COUNTER_INCREMENT(
                    amf4g_ue, GTP_COUNTER_MODIFY_BEARER_BY_HANDOVER_NOTIFY);

            rv = amf_n11_send_modify_bearer_request(bearer, 1);
            d_assert(rv == CORE_OK, return, "gtp send failed");

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }
}

void s1ap_handle_s1_reset(
        amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    char buf[CORE_ADDRSTRLEN];
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_Reset_t *Reset = NULL;

    S1AP_ResetIEs_t *ie = NULL;
    S1AP_Cause_t *Cause = NULL;
    S1AP_ResetType_t *ResetType = NULL;
    S1AP_UE_associatedLogicalS1_ConnectionListRes_t *partOfS1_Interface = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    Reset = &initiatingMessage->value.choice.Reset;
    d_assert(Reset, return,);

    d_trace(3, "[AMF4G] Reset\n");

    for (i = 0; i < Reset->protocolIEs.list.count; i++)
    {
        ie = Reset->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            case S1AP_ProtocolIE_ID_id_ResetType:
                ResetType = &ie->value.choice.ResetType;
                break;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] ENB_ID[%d]\n",
            CORE_ADDR(enb->addr, buf), enb->enb_id);

    d_assert(Cause, return,);
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
            Cause->present, Cause->choice.radioNetwork);

    switch(Cause->present)
    {
        case S1AP_Cause_PR_radioNetwork:
        case S1AP_Cause_PR_transport:
        case S1AP_Cause_PR_protocol:
        case S1AP_Cause_PR_misc:
            break;
        case S1AP_Cause_PR_nas:
            d_warn("NAS-Cause[%d]", Cause->choice.nas);
        default:
            d_warn("Invalid cause group[%d]", Cause->present);
            break;
    }

    d_assert(ResetType, return,);
    switch(ResetType->present)
    {
        case S1AP_ResetType_PR_s1_Interface:
        {
            d_trace(5, "    S1AP_ResetType_PR_s1_Interface\n");

            rv = enb_ue_remove_in_enb(enb);
            d_assert(rv == CORE_OK,,);
            break;
        }
        case S1AP_ResetType_PR_partOfS1_Interface:
        {
            d_trace(5, "    S1AP_ResetType_PR_partOfS1_Interface\n");

            partOfS1_Interface = ResetType->choice.partOfS1_Interface;
            d_assert(partOfS1_Interface, return,);
            for (i = 0; i < partOfS1_Interface->list.count; i++)
            {
                S1AP_UE_associatedLogicalS1_ConnectionItemRes_t *ie2 = NULL;
                S1AP_UE_associatedLogicalS1_ConnectionItem_t *item = NULL;

                enb_ue_t *enb_ue = NULL;

                ie2 = (S1AP_UE_associatedLogicalS1_ConnectionItemRes_t *)
                    partOfS1_Interface->list.array[i];
                d_assert(ie2, return,);

                item = &ie2->value.choice.UE_associatedLogicalS1_ConnectionItem;
                d_assert(item, return,);
                
                d_trace(5, "    AMF4G_UE_S1AP_ID[%d] ENB_UE_S1AP_ID[%d]\n",
                        item->mME_UE_S1AP_ID ? *item->mME_UE_S1AP_ID : -1,
                        item->eNB_UE_S1AP_ID ? *item->eNB_UE_S1AP_ID : -1);

                if (item->mME_UE_S1AP_ID)
                    enb_ue = enb_ue_find_by_mme_ue_s1ap_id(
                            *item->mME_UE_S1AP_ID);
                else if (item->eNB_UE_S1AP_ID)
                    enb_ue = enb_ue_find_by_enb_ue_s1ap_id(enb,
                            *item->eNB_UE_S1AP_ID);

                if (enb_ue == NULL)
                {
                    d_warn("Cannot find S1 Context "
                            "(AMF4G_UE_S1AP_ID[%d] ENB_UE_S1AP_ID[%d])\n",
                            item->mME_UE_S1AP_ID ? *item->mME_UE_S1AP_ID : -1,
                            item->eNB_UE_S1AP_ID ? *item->eNB_UE_S1AP_ID : -1);
                    continue;
                }

                rv = enb_ue_remove(enb_ue);
                d_assert(rv == CORE_OK,,);
            }
            break;
        }
        default:
            d_warn("Invalid ResetType[%d]", ResetType->present);
            break;
    }

    rv = s1ap_send_s1_reset_ack(enb, partOfS1_Interface);
    d_assert(rv == CORE_OK,,);
}

void s1ap_handle_retrieve_ue_information(amf4g_enb_t *enb, s1ap_message_t *message)
{
    status_t rv;
    int i;

    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_RetrieveUEInformation_t *RetrieveUEInformation = NULL;
    
    S1AP_RetrieveUEInformationIEs_t *ie = NULL;
    S1AP_S_TMSI_t *S_TMSI = NULL;

    d_assert(enb, return,);
    d_assert(enb->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    RetrieveUEInformation = &initiatingMessage->value.choice.RetrieveUEInformation;
    d_assert(RetrieveUEInformation, return,);

d_trace(3, "[AMF4G] Retrieve UE Information\n");
    
// Retrieve s-tmsi
    for (i = 0; i < RetrieveUEInformation->protocolIEs.list.count; i++)
    {
        ie = RetrieveUEInformation->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case S1AP_ProtocolIE_ID_id_S_TMSI:
                S_TMSI = &ie->value.choice.S_TMSI;
                break;
            default:
                break;
        }
    }
    
    // Find AMF4G_UE if S_TMSI included
    if (S_TMSI)
    {
        served_gummei_t *served_gummei = &amf4g_self()->served_gummei[0];
        guti_t guti;
        amf4g_ue_t *amf4g_ue = NULL;

        memset(&guti, 0, sizeof(guti_t));

        /* Use the first configured plmn_id and mme group id */
        memcpy(&guti.plmn_id, &served_gummei->plmn_id[0], PLMN_ID_LEN);
        guti.mme_gid = served_gummei->amf4g_gid[0];

        /* size must be 1 */
        memcpy(&guti.mme_code, S_TMSI->mMEC.buf, S_TMSI->mMEC.size);
        /* size must be 4 */
        memcpy(&guti.m_tmsi, S_TMSI->m_TMSI.buf, S_TMSI->m_TMSI.size);
        guti.m_tmsi = ntohl(guti.m_tmsi);

        amf4g_ue = amf4g_ue_find_by_guti(&guti);
           // Call s1ap_send_ue_information_transfer and pass amf4g_ue and S_TMSI 
        rv = s1ap_send_ue_information_transfer(amf4g_ue, S_TMSI);
        d_assert(rv == CORE_OK,,);
    }
}


