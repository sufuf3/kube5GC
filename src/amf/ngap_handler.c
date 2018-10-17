#define TRACE_MODULE _ngap_handler
#include "core_debug.h"

/* TODO: remove 3gpp_type.h head file, until ngap_conv.c done*/
#include "3gpp_types.h"

#include "ngap_path.h"
#include "ngap_conv.h"
#include "ngap_build.h"
#include "ngap_handler.h"

void ngap_handle_ng_setup_request(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0, j = 0, k = 0;
                              
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_NGSetupRequest_t *NGSetupRequest = NULL;

    NGAP_NGSetupRequestIEs_t *ie = NULL;
    NGAP_GlobalRANNodeID_t *Global_RAN_Node_ID = NULL;
    NGAP_SupportedTAList_t *SupportedTAList = NULL;
	// NGAP_RANNodeName_t	 *RANNodeName = NULL; //Optional
	NGAP_PagingDRX_t *PagingDRX = NULL;

    pkbuf_t *ngapbuf = NULL;
    //c_uint32_t gnb_id = 0;
    NGAP_Cause_PR group = NGAP_Cause_PR_NOTHING;
    long cause = 0;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    NGSetupRequest = &initiatingMessage->value.choice.NGSetupRequest;
    d_assert(NGSetupRequest, return,);

    d_trace(3, "[AMF] NG-Setup request\n");

    for (i = 0; i < NGSetupRequest->protocolIEs.list.count; i++)
    {
        ie = NGSetupRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_GlobalRANNodeID:
                Global_RAN_Node_ID = &ie->value.choice.GlobalRANNodeID;
                d_assert(Global_RAN_Node_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_SupportedTAList:
                SupportedTAList = &ie->value.choice.SupportedTAList;
                d_assert(SupportedTAList, return,);
                break;
            case NGAP_ProtocolIE_ID_id_DefaultPagingDRX:
                PagingDRX = &ie->value.choice.PagingDRX;
                d_assert(PagingDRX, return,);
                break;
            /* */    
            // case NGAP_ProtocolIE_ID_id_RANNodeName:
            //     RANNodeName = &ie->value.choice.RANNodeName;
            //     break;
            default:
                break;
        }
    }
    
    // ngap_RAN_ID_to_unit32(&Global_RAN_Node_ID->gNB_ID, &gnb_id);
    // amf_ran_set_gnb_id(ran, gnb_id);

    /* Parse Supported TA */
    ran->num_of_supported_ta_list = 0;
    for (i = 0; i < SupportedTAList->list.count; i++)
    {
        NGAP_SupportedTAItem_t *SupportedTAs_Item = NULL;
        NGAP_TAC_t *tAC = NULL;

        SupportedTAs_Item = 
            (NGAP_SupportedTAItem_t *)SupportedTAList->list.array[i];
        d_assert(SupportedTAs_Item, return,);
        tAC = &SupportedTAs_Item->tAC;
        d_assert(tAC, return,);

        for (j = 0; j < SupportedTAs_Item->broadcastPLMNList.list.count; j++)
        {
            NGAP_BroadcastPLMNItem_t *pBoradcastPLMNItem = NULL;
            pBoradcastPLMNItem = (NGAP_BroadcastPLMNItem_t *) 
                SupportedTAs_Item->broadcastPLMNList.list.array[j];
            d_assert(pBoradcastPLMNItem, return,);

            memcpy(&ran->supported_ta_list[ran->num_of_supported_ta_list].tai.tac,
                    tAC->buf, sizeof(c_uint16_t));
            ran->supported_ta_list[ran->num_of_supported_ta_list].tai.tac = 
                ntohs(ran->supported_ta_list
                        [ran->num_of_supported_ta_list].tai.tac);
            memcpy(&ran->supported_ta_list
                        [ran->num_of_supported_ta_list].tai.plmn_id,
                    pBoradcastPLMNItem->pLMNIdentity.buf, sizeof(plmn_id_t));

            c_uint16_t  tmp_num_of_s_nssai = ran->supported_ta_list[ran->num_of_supported_ta_list].num_of_s_nssai;
            for( k = 0; k < pBoradcastPLMNItem->tAISliceSupportList.list.count; k++)
            {
                NGAP_S_NSSAI_t *s_NSSAI;
                
                NGAP_SliceSupportItem_t *pSliceSupportItem = NULL;
                pSliceSupportItem = (NGAP_SliceSupportItem_t *)
                    pBoradcastPLMNItem->tAISliceSupportList.list.array[j];
                    d_assert(pSliceSupportItem, return,);
                s_NSSAI = &pSliceSupportItem->s_NSSAI;
                memcpy(&ran->supported_ta_list[ran->num_of_supported_ta_list].s_nssai[tmp_num_of_s_nssai],
                s_NSSAI->sST.buf, sizeof(c_uint16_t));
                tmp_num_of_s_nssai++;
                ran->supported_ta_list[ran->num_of_supported_ta_list].num_of_s_nssai++;
            }

            d_trace(5, "    PLMN_ID[MCC:%d MNC:%d] TAC[%d]\n",
                plmn_id_mcc(&ran->supported_ta_list
                    [ran->num_of_supported_ta_list].tai.plmn_id),
                plmn_id_mnc(&ran->supported_ta_list
                    [ran->num_of_supported_ta_list].tai.plmn_id),
                ran->supported_ta_list[ran->num_of_supported_ta_list].tai.tac);
            ran->num_of_supported_ta_list++;
        }
    }

    if (ran->num_of_supported_ta_list == 0) 
    {
        d_warn("NG-Setup failure:");
        d_warn("    No supported TA exist in NG-Setup request");
        group = NGAP_Cause_PR_misc;
        cause = NGAP_CauseMisc_unspecified;
    }
    else
    {
        int served_tai_index = -1;
        for (i = 0; i < ran->num_of_supported_ta_list; i++)
        {
            served_tai_index =  amf_find_served_tai(&ran->supported_ta_list[i].tai);
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
            d_warn("    Cannot find Served TAI. Check 'amf.tai' configuration");
            group = S1AP_Cause_PR_misc;
            cause = S1AP_CauseMisc_unknown_PLMN;
        }
    }

    if (group == NGAP_Cause_PR_NOTHING)
    {
        d_trace(3, "[AMF] NG-Setup response\n");
        d_assert(ngap_build_setup_rsp(&ngapbuf) == CORE_OK, 
                return, "ngap_build_setup_rsp() failed");
    }
    else
    {
        d_trace(3, "[AMF] N{-Setup failure\n");
        d_assert(ngap_build_setup_failure(
                &ngapbuf, group, cause, NGAP_TimeToWait_v10s) == CORE_OK, 
                return, "ngap_build_setup_failure() failed");
    }

    d_assert(ngap_send_to_ran(ran, ngapbuf, NGAP_NON_UE_SIGNALLING) == CORE_OK,,
             "ngap_send_to_ran() failed");
}

/**
 * NG-RAN node -> AMF
 **/
void ngap_handle_initial_context_setup_response(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;

    char buf[CORE_ADDRSTRLEN];
                              
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_InitialContextSetupResponse_t *InitialContextSetupResponse = NULL;

    NGAP_InitialContextSetupResponseIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceSetupListCxtRes_t *PDUSessionResourceSetupListCxtRes = NULL;
    d_assert(ran, return,);
    d_assert(message, return,);

    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    InitialContextSetupResponse =
        &successfulOutcome->value.choice.InitialContextSetupResponse;
    d_assert(InitialContextSetupResponse, return,);
    
    for (i = 0; i < InitialContextSetupResponse->protocolIEs.list.count; i++)
    {
        ie = InitialContextSetupResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID =
                    &ie->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupListCxtRes:
                PDUSessionResourceSetupListCxtRes =
                    &ie->value.choice.PDUSessionResourceSetupListCxtRes;
            default:
                break;
        }
    }
    d_trace(5, "    IP[%s] gnb_ID[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->gnb_id);

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return, );

    ran_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    d_trace(5, "    AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d]\n",
            ran_ue->amf_ue_ngap_id, ran_ue->ran_ue_ngap_id);

    d_assert(PDUSessionResourceSetupListCxtRes, return, );
    for (i = 0 ; i < PDUSessionResourceSetupListCxtRes->list.count; i++)
    {   
        NGAP_PDUSessionResourceSetupItemCxtReq_t *ie2 = NULL;
        // NGAP_PDUSessionResourceSetupResponseTransfer_t *PDUSessionResourceSetupResponseTransfer = NULL;
        ie2 = (NGAP_PDUSessionResourceSetupItemCxtReq_t *) 
            PDUSessionResourceSetupListCxtRes->list.array[i];
        d_assert(ie2, return,);

        amf_ue->psi = ie2->pDUSessionID;
        //ie2->pDUSessionResourceSetupRequestTransfer;
       
    }

}

void ngap_handle_initial_context_setup_failure(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];

    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    NGAP_InitialContextSetupFailure_t *InitialContextSetupFailure = NULL;

    NGAP_InitialContextSetupFailureIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_Cause_t *Cause = NULL;

    d_assert(ran, return,);
    d_assert(message, return,);

    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(unsuccessfulOutcome, return,);
    InitialContextSetupFailure =
        &unsuccessfulOutcome->value.choice.InitialContextSetupFailure;
    d_assert(InitialContextSetupFailure, return,);

    for (i = 0; i < InitialContextSetupFailure->protocolIEs.list.count; i++)
    {
        ie = InitialContextSetupFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->gnb_id);

    d_assert(AMF_UE_NGAP_ID, return,);
    d_assert(RAN_UE_NGAP_ID, return,);
    d_assert(Cause, return,);

    ran_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    d_trace(5, "    AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d]\n",
            ran_ue->amf_ue_ngap_id, ran_ue->ran_ue_ngap_id);

    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    amf_ue = ran_ue->amf_ue;
    if (amf_ue == NULL) 
    {
        d_warn("Initial context setup failure : cannot find RAN-UE-NGAP-ID[%d]",
        *RAN_UE_NGAP_ID);
        return;
    }

    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
             ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
            Cause->present, Cause->choice.radioNetwork);

    d_assert(amf_ue, ,);
    //TODO : add  FSM_CHECK
    if (0)
    {

    }
    else
    {
        //d_trace(5, "    NOT EMM-Registered\n");
        //d_assert(mme_ue,,);
        //rv = mme_send_delete_session_or_ue_context_release(mme_ue, enb_ue);
        //d_assert(rv == CORE_OK,,
        //        "mme_send_delete_session_or_ue_context_release() failed");
    }
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_pdu_session_resource_setup_response(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;

    char buf[CORE_ADDRSTRLEN];
                              
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_PDUSessionResourceSetupResponse_t *PDUSessionResourceSetupResponse = NULL;

    NGAP_PDUSessionResourceSetupResponseIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceSetupListSURes_t	*PDUSessionResourceSetupListSURes = NULL;
    d_assert(ran, return,);
    d_assert(message, return,);

    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    PDUSessionResourceSetupResponse =
        &successfulOutcome->value.choice.PDUSessionResourceSetupResponse;
    d_assert(PDUSessionResourceSetupResponse, return,);

    for (i = 0; i < PDUSessionResourceSetupResponse->protocolIEs.list.count; i++)
    {
        ie = PDUSessionResourceSetupResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID =
                    &ie->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupListSURes:
                PDUSessionResourceSetupListSURes =
                    &ie->value.choice.PDUSessionResourceSetupListSURes;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] gnb_ID[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->gnb_id);

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return, );

    ran_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    d_trace(5, "    AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d]\n",
            ran_ue->amf_ue_ngap_id, ran_ue->ran_ue_ngap_id);

    
    d_assert(PDUSessionResourceSetupListSURes, return, );
    for (i = 0 ; i < PDUSessionResourceSetupListSURes->list.count; i++)
    {   
        NGAP_PDUSessionResourceSetupItemCxtReq_t *ie2 = NULL;
        // NGAP_PDUSessionResourceSetupResponseTransfer_t *PDUSessionResourceSetupResponseTransfer = NULL;
        ie2 = (NGAP_PDUSessionResourceSetupItemCxtReq_t *) 
            PDUSessionResourceSetupListSURes->list.array[i];
        d_assert(ie2, return,);

        amf_ue->psi = ie2->pDUSessionID;
        //ie2->pDUSessionResourceSetupRequestTransfer;
       
    }
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_ue_context_modification_response(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];
                              
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_UEContextModificationResponse_t *UEContextModificationResponse = NULL;

    NGAP_UEContextModificationResponseIEs_t *ie = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		// NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    UEContextModificationResponse =
        &successfulOutcome->value.choice.UEContextModificationResponse;
    d_assert(UEContextModificationResponse, return,);

    ran_ue_t *ran_ue = NULL;

    for (i = 0; i < UEContextModificationResponse->protocolIEs.list.count; i++)
    {
        ie = UEContextModificationResponse->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID =
                    &ie->value.choice.RAN_UE_NGAP_ID;
                break;
            // case NGAP_ProtocolIE_ID_id_CriticalityDiagnostics:
            //     NGAP_CriticalityDiagnostics =
            //         &ie->value.choice.CriticalityDiagnostics;
            default:
                break;
        }
    }

    d_trace(5, "    IP[%s] gnb_ID[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->gnb_id);

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    
    ran_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    d_trace(5, "    AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d]\n",
            ran_ue->amf_ue_ngap_id, ran_ue->ran_ue_ngap_id);
    if(!ran_ue)
    {
        //TODO: send ngap error indication
        return;
    }
    d_trace(3, "UE Context modification. AMF_UE_NGAP_ID[%lu], RAN_UE_NGAP_ID[%lu]\n", *AMF_UE_NGAP_ID, *RAN_UE_NGAP_ID);
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_ue_context_modification_failure(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];

    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    NGAP_UEContextModificationFailure_t *UEContextModificationFailure = NULL;

    NGAP_UEContextModificationFailureIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_Cause_t *Cause = NULL;
			// NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;

    d_assert(ran, return,);
    d_assert(message, return,);

    // amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(unsuccessfulOutcome, return,);
    UEContextModificationFailure =
        &unsuccessfulOutcome->value.choice.UEContextModificationFailure;
    d_assert(UEContextModificationFailure, return,);

    for (i = 0; i < UEContextModificationFailure->protocolIEs.list.count; i++)
    {
        ie = UEContextModificationFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
            default:
                break;
        }
    }
    d_trace(5, "    IP[%s] gnb_ID[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->gnb_id);

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    
    ran_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    d_trace(5, "    AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d]\n",
            ran_ue->amf_ue_ngap_id, ran_ue->ran_ue_ngap_id);
    
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
            Cause->present, Cause->choice.radioNetwork);

    if(!ran_ue)
    {
        //TODO: send ngap error indication
        return;
    }
    d_trace(3, "UE Context modification. AMF_UE_NGAP_ID[%lu], RAN_UE_NGAP_ID[%lu]\n", *AMF_UE_NGAP_ID, *RAN_UE_NGAP_ID);
}

/**
 * Direction: NG-RAN node -> AMF and AMF -> NG-RAN node
 */ 
void ngap_handle_ng_reset(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    status_t rv = 0;
    char buf[CORE_ADDRSTRLEN];
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_NGReset_t *NGReset = NULL;

    NGAP_NGResetIEs_t *ie = NULL;
        NGAP_Cause_t *Cause = NULL;
		NGAP_ResetType_t *ResetType = NULL;
           NGAP_UE_associatedLogicalNG_ConnectionListRes_t *partOfNG_Interface = NULL;
    
    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    NGReset = &initiatingMessage->value.choice.NGReset;
    d_assert(NGReset, return,);

    d_trace(3, "[NG] Reset\n");

    for(i = 0; i < NGReset->protocolIEs.list.count; i++)
    {
        ie = NGReset->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            case S1AP_ProtocolIE_ID_id_ResetType:
                ResetType = &ie->value.choice.ResetType;
                break;
            default:
                break;
        }
    }
    d_trace(5, "    IP[%s] gnb_ID[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->gnb_id);

    d_assert(Cause, return, );
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
        Cause->present, Cause->choice.radioNetwork);

    switch(Cause->present)
    {
        case NGAP_Cause_PR_radioNetwork:
        case NGAP_Cause_PR_transport:
        case NGAP_Cause_PR_protocol:
        case NGAP_Cause_PR_misc:
            break;
        case NGAP_Cause_PR_nas:
            d_warn("NAS-Cause[%d]", Cause->choice.nas);
        default:
            d_warn("Invalid cause group[%d]", Cause->present);
            break;
    }
    d_assert(ResetType, return,);
    switch(ResetType->present)
    {
        case NGAP_ResetType_PR_nG_Interface:
        {
            d_trace(5, "    NGAP_ResetType_PR_nG_Interface\n");
                 
            rv = ran_ue_remove_in_ran(ran);
            d_assert(rv == CORE_OK,,);
            break;
        }
        case NGAP_ResetType_PR_partOfNG_Interface:
        {
            d_trace(5, "    S1AP_ResetType_PR_partOfS1_Interface\n");

            partOfNG_Interface = ResetType->choice.partOfNG_Interface;
            d_assert(partOfNG_Interface, return,);
            for (i = 0; i < partOfNG_Interface->list.count; i++)
            {
                
                NGAP_UE_associatedLogicalNG_ConnectionItemResIEs_t *ie2 = NULL;
                NGAP_UE_associatedLogicalNG_ConnectionItem_t *item = NULL;

                ran_ue_t *ran_ue = NULL;

                ie2 = (NGAP_UE_associatedLogicalNG_ConnectionItemResIEs_t *)
                    partOfNG_Interface->list.array[i];
                d_assert(ie2, return,);

                item = &ie2->value.choice.UE_associatedLogicalNG_ConnectionItem;
                d_assert(item, return,);
                
                d_trace(5, "    AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d]\n",
                        item->aMF_UE_NGAP_ID ? *item->aMF_UE_NGAP_ID : -1,
                        item->rAN_UE_NGAP_ID ? *item->rAN_UE_NGAP_ID : -1);

                if (item->aMF_UE_NGAP_ID)
                    ran_ue = ran_ue_find_by_amf_ue_ngap_id(
                            *item->aMF_UE_NGAP_ID);
                else if (item->rAN_UE_NGAP_ID)
                    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran,
                            *item->rAN_UE_NGAP_ID);

                if (ran_ue == NULL)
                {
                    d_warn("Cannot find S1 Context "
                            "(AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d])\n",
                            item->aMF_UE_NGAP_ID ? *item->aMF_UE_NGAP_ID : -1,
                            item->rAN_UE_NGAP_ID ? *item->rAN_UE_NGAP_ID : -1);
                    continue;
                }

                rv = ran_ue_remove(ran_ue);
                d_assert(rv == CORE_OK,,);
            }
            break;
        }
        default:
            d_warn("Invalid ResetType[%d]", ResetType->present);
            break;
    }
    // TODO: add ngap send ng reset ack api
    // rv = ngap_send_ng_reset_ack(ran, partOfNG_Interface);
    d_assert(rv == CORE_OK,,);

}