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
    c_uint32_t gnb_id = 0;
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
                d_trace(3, "NGAP_ProtocolIE_ID_id_GlobalRANNodeID :%d", __LINE__);
                d_assert(Global_RAN_Node_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_SupportedTAList:
                SupportedTAList = &ie->value.choice.SupportedTAList;
                d_trace(3, "NGAP_ProtocolIE_ID_id_SupportedTAList :%d", __LINE__);
                d_assert(SupportedTAList, return,);
                break;
            case NGAP_ProtocolIE_ID_id_DefaultPagingDRX:
                d_trace(3, "NGAP_ProtocolIE_ID_id_DefaultPagingDRX :%d", __LINE__);
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
    
    ngap_GNB_ID_to_uint32(&Global_RAN_Node_ID->choice.globalGNB_ID->gNB_ID, &gnb_id);
    d_trace(3," NGAP_ProtocolIE_ID_id_GlobalRANNodeID :%d gnb_id: %x", __LINE__, gnb_id);
    amf_ran_set_ran_id(ran, gnb_id, 0, 0, RAN_PR_GNB_ID);

    if (PagingDRX) {
        d_trace(5, "    PagingDRX[%ld]\n", *PagingDRX);
    }

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
            d_warn("NG-Setup failure:");
            d_warn("    Cannot find Served TAI. Check 'amf.tai' configuration");
            group = NGAP_Cause_PR_misc;
            cause = NGAP_CauseMisc_unknown_PLMN;
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
        d_trace(3, "[AMF] NG-Setup failure\n");
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

     switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    
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

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

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
        //d_assert(amf4g_ue,,);
        //rv = amf4g_send_delete_session_or_ue_context_release(amf4g_ue, enb_ue);
        //d_assert(rv == CORE_OK,,
        //        "amf4g_send_delete_session_or_ue_context_release() failed");
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
    
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    
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

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    
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
    
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    
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
            case NGAP_ProtocolIE_ID_id_ResetType:
                ResetType = &ie->value.choice.ResetType;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

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


/**
 * Direction: NG-RAN node -> AMF 
 */ 
void ngap_handle_handover_cancel(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    status_t rv = 0;
    char buf[CORE_ADDRSTRLEN];
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_HandoverCancel_t *HandoverCancel = NULL;

    NGAP_HandoverCancelIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_Cause_t *Cause = NULL;
    
    ran_ue_t *source_ue = NULL;
    ran_ue_t *target_ue = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    HandoverCancel = &initiatingMessage->value.choice.HandoverCancel;
    d_assert(HandoverCancel, return,);

    d_trace(3, "[NG] HandoverCancel\n");

    for(i = 0; i < HandoverCancel->protocolIEs.list.count; i++)
    {
        ie = HandoverCancel->protocolIEs.list.array[i];
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
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(AMF_UE_NGAP_ID, return,);
    d_assert(RAN_UE_NGAP_ID, return,);
    d_assert(Cause, return,);

    source_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(source_ue, return, "Cannot find UE for RAN_UE_NGAP_ID%d]", *RAN_UE_NGAP_ID);
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(source_ue->amf_ue_ngap_id == *AMF_UE_NGAP_ID, return,
            "Conflict MME-UE-S1AP-ID : %d != %d\n",
            source_ue->amf_ue_ngap_id, *AMF_UE_NGAP_ID);
    
    target_ue = source_ue->target_ue;
    d_assert(target_ue, return,);

    d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            source_ue->ran_ue_ngap_id, source_ue->amf_ue_ngap_id);
    d_trace(5, "    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            target_ue->ran_ue_ngap_id, target_ue->amf_ue_ngap_id);

    rv = ngap_send_handover_cancel_acknowledge(source_ue);
    d_assert(rv == CORE_OK,, "ngap send error");

    rv = ngap_send_ue_context_release_command(
        target_ue, NGAP_Cause_PR_radioNetwork,
        NGAP_CauseRadioNetwork_handover_cancelled,
        4, 3000);
    
    d_assert(rv == CORE_OK, return, "ngap send error");

    d_trace(3, "[AMF] Handover Cancel : "
            "UE[RAN-UE-NGAP-ID(%d)] --> ", source_ue->ran_ue_ngap_id);

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_ue_context_release_complete(amf_ran_t *ran, ngap_message_t *message)
{
    status_t rv = 0;
    int i = 0;
    char buf[CORE_ADDRSTRLEN];
                              
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_UEContextReleaseComplete_t *UEContextReleaseComplete = NULL;

    NGAP_UEContextReleaseComplete_IEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        // NGAP_UserLocationInformation_t	 UserLocationInformation;
        // NGAP_InfoOnRecommendedCellsAndRANNodesForPaging_t	 InfoOnRecommendedCellsAndRANNodesForPaging;
        // NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
    
    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    
    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    UEContextReleaseComplete =
        &successfulOutcome->value.choice.UEContextReleaseComplete;
    d_assert(UEContextReleaseComplete, return,);
    d_trace(3, "[AMF] UE Context release complete\n");

    for (i = 0; i < UEContextReleaseComplete->protocolIEs.list.count; i++)
    {
        ie = UEContextReleaseComplete->protocolIEs.list.array[i];
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
    
    d_trace(5, "    ran[%s] \n",
            CORE_ADDR(ran->addr, buf));
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    
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
    
    amf_ue = ran_ue->amf_ue;
    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]",
        ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);

    switch (ran_ue->ue_ctx_rel_action)
    {
        case NGAP_UE_CTX_REL_NO_ACTION:
        {
            d_trace(5, "    No Action\n");
            rv = ran_ue_remove(ran_ue);
            d_assert(rv == CORE_OK,, "ran_ue_remove() failed");
            break;
        }
        case NGAP_UE_CTX_REL_S1_NORMAL_RELEASE:
        {
            d_trace(5, "    Action: NG normal release\n");
            rv = ran_ue_remove(ran_ue);
            d_assert(rv == CORE_OK,, "ran_ue_remove() failed");

            d_assert(amf_ue,,);
            rv = amf_ue_deassociate(amf_ue);
            d_assert(rv == CORE_OK,, "amf_ue_deassociate() failed");
            break;
        }
        case NGAP_UE_CTX_REL_UE_CONTEXT_REMOVE:
        {
            d_trace(5, "    Action: UE context remove()\n");
            rv = ran_ue_remove(ran_ue);
            d_assert(rv == CORE_OK,, "ran_ue_remove() failed");

            d_assert(amf_ue,,);
            rv = amf_ue_remove(amf_ue);
            d_assert(rv == CORE_OK,, "amf_ue_remove() failed");
            break;
        }
        case NGAP_UE_CTX_REL_DELETE_INDIRECT_TUNNEL:
        {
            d_trace(5, "    Action: Delete indirect tunnel\n");

            rv = source_ue_deassociate_target_ue_5g(ran_ue);
            d_assert(rv == CORE_OK,,
                    "source_ue_deassociate_target_ue_5g() failed");

            rv = ran_ue_remove(ran_ue);
            d_assert(rv == CORE_OK,, "ran_ue_remove() failed");

            //TODO: fix it.
#if 0
            d_assert(amf,,);
            if (amf_ue_have_indirect_tunnel(amf_ue))
            {
                //TODO:
                // rv = amf4g_gtp_send_delete_indirect_data_forwarding_tunnel_request(amf4g_ue);
                // d_assert(rv == CORE_OK,, "amf4g_gtp_send_delete_indirect_data_"
                //     "forwarding_tunnel_request() failed");
            }
            else
            {
                d_warn("Check your eNodeB");
                d_warn("  There is no INDIRECT TUNNEL");
                d_warn("  Packet could be dropped during S1-Handover");
                // rv = amf_ue_clear_indirect_tunnel(amf_ue);
                d_assert(rv == CORE_OK,,
                        "amf_ue_clear_indirect_tunnel() failed");
            }
#endif
            break;
        }
        default:
        {
            d_assert(0,, "Invalid Action[%d]", ran_ue->ue_ctx_rel_action);
            break;
        }
    }
}

/**
 * Direction NG-RAN -> AMF 
 **/
void ngap_handle_handover_required(amf_ran_t *ran, ngap_message_t *message)
{
    status_t rv = 0;
    char buf[CORE_ADDRSTRLEN];
    int i;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_HandoverRequired_t *HandoverRequired = NULL;

    NGAP_HandoverRequiredIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_HandoverType_t *HandoverType = NULL;
        NGAP_Cause_t *Cause = NULL;
        NGAP_TargetID_t *TargetID = NULL;
#if 0 // not implement yet
		 NGAP_DirectForwardingPathAvailability_t	 DirectForwardingPathAvailability;

        NGAP_PDUSessionResourceListHORqd_t *PDUSessionResourceListHORqd = NULL;
#endif 
        NGAP_SourceToTarget_TransparentContainer_t *SourceToTarget_TransparentContainer = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    HandoverRequired = &initiatingMessage->value.choice.HandoverRequired;
    d_assert(HandoverRequired, return,);

    d_trace(3, "[AMF] handover required \n");

    for (i = 0; i < HandoverRequired->protocolIEs.list.count; i++)
    {
        ie = HandoverRequired->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                d_assert(AMF_UE_NGAP_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_HandoverType:
                HandoverType = &ie->value.choice.HandoverType;
                break;
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            case NGAP_ProtocolIE_ID_id_TargetID:
                TargetID = &ie->value.choice.TargetID;
                break;
            case NGAP_ProtocolIE_ID_id_PDUSessionResourceListHORqd:
#if 0 // not implement yet
                PDUSessionResourceListHORqd = &ie->value.choice.PDUSessionResourceListHORqd;
#endif
                break;
            case NGAP_ProtocolIE_ID_id_SourceToTarget_TransparentContainer:
                SourceToTarget_TransparentContainer = &ie->value.choice.SourceToTarget_TransparentContainer;
                break;
            default:
                break;
        }
    }
    
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(TargetID, return,);
    c_uint32_t target_ran_id = 0;
    switch(TargetID->present)
    {
        case NGAP_TargetID_PR_targetRANNodeID:
        {
           ngap_GNB_ID_to_uint32(&TargetID->choice.targetRANNodeID->globalRANNodeID.choice.globalGNB_ID->gNB_ID, &target_ran_id);
            break;
        }
        
#if 0 // not implement yet
        case NGAP_TargetID_PR_targeteNB_ID:
        ngap_GNB_ID_to_uint32(&TargetID->choice.targeteNB_ID->globalENB_ID.ngENB_ID, c_uint32_t *uint32)
            break;
#endif     
        default:
        {
            d_error("Not implemented(%d)", TargetID->present);
            return;
        }
    }
    
    amf_ran_t *target_ran = NULL;

    target_ran = amf_ran_find_by_ran_id(target_ran_id,0 ,0 ,ran->ran_id.ran_present);
    if (target_ran == NULL)
    {
        d_warn("Handover required : cannot find target gNB_ID[%d]",
                target_ran_id);
        return;
    }

    d_assert(RAN_UE_NGAP_ID, return,);
    d_assert(AMF_UE_NGAP_ID, return,);
    ran_ue_t *source_ue = NULL;
    source_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(source_ue, return,
            "Cannot find UE for RAN_UE_NGAP_ID[%d] ", *RAN_UE_NGAP_ID);
    d_assert(source_ue->amf_ue_ngap_id == *AMF_UE_NGAP_ID, return,
            "Conflict AMF_UE_NGAP_ID: %d != %d\n",
            source_ue->amf_ue_ngap_id, *AMF_UE_NGAP_ID);
    d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            source_ue->ran_ue_ngap_id, source_ue->amf_ue_ngap_id);
    
    amf_ue_t *amf_ue = NULL;
    amf_ue = source_ue->amf_ue;
    d_assert(amf_ue, return,);
#if 0 // TODO: fix it   
    if (SECURITY_CONTEXT_IS_VALID(amf4g_ue))
#else 
    if (0)
#endif
    {
#if 0 // TODO: fix it
        amf4g_ue->nhcc++;
        amf4g_kdf_nh(amf4g_ue->kasme, amf4g_ue->nh, amf4g_ue->nh);
#endif 
    }
    else
    {
        d_assert(Cause, return,);
        rv = ngap_send_handover_preparation_failure(source_ue, Cause);
        d_assert(rv == CORE_OK, return, "s1ap send error");

        return;
    }
    d_assert(HandoverType, return, );
    source_ue->handover_type = *HandoverType;
    rv = ngap_send_handover_request(amf_ue, target_ran,
            AMF_UE_NGAP_ID, RAN_UE_NGAP_ID,
            HandoverType, Cause,
            SourceToTarget_TransparentContainer);
    d_assert(rv == CORE_OK,, "ngap send error");    
}

void ngap_handle_handover_request_acknowledge(amf_ran_t *ran, ngap_message_t *message)
{
    status_t rv = 0;
    int i = 0;
    char buf[CORE_ADDRSTRLEN];
                              
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_HandoverRequestAcknowledge_t *HandoverRequestAcknowledge = NULL;

    NGAP_HandoverRequestAcknowledgeIEs_t *ie = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_PDUSessionResourceAdmittedList_t *PDUSessionResourceAdmittedList = NULL;
		NGAP_TargetToSource_TransparentContainer_t *TargetToSource_TransparentContainer = NULL;
#if 0
        NGAP_PDUSessionList_t	 PDUSessionList;
	    NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif

    ran_ue_t *source_ue = NULL;
    ran_ue_t *target_ue = NULL;
    amf_ue_t *amf_ue = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);

    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    HandoverRequestAcknowledge = &successfulOutcome->value.choice.HandoverRequestAcknowledge;
    d_assert(HandoverRequestAcknowledge, return,);
    d_trace(3, "[AMF] Handover request acknowledge\n");

    for (i = 0; i < HandoverRequestAcknowledge->protocolIEs.list.count; i++)
    {
        ie = HandoverRequestAcknowledge->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID =
                    &ie->value.choice.RAN_UE_NGAP_ID;
                break;
             case NGAP_ProtocolIE_ID_id_PDUSessionResourceAdmittedList:
                PDUSessionResourceAdmittedList = &ie->value.choice.PDUSessionResourceAdmittedList;
                break;
            case NGAP_ProtocolIE_ID_id_TargetToSource_TransparentContainer:
                TargetToSource_TransparentContainer =  &ie->value.choice.TargetToSource_TransparentContainer;
            default:
                break;
        }
    }

    d_trace(5, "    ran[%s] \n",
            CORE_ADDR(ran->addr, buf));
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(AMF_UE_NGAP_ID, return,);
    d_assert(RAN_UE_NGAP_ID, return,);
    d_assert(PDUSessionResourceAdmittedList, return,);
    d_assert(TargetToSource_TransparentContainer, return,);

    target_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    d_assert(target_ue, return, "Cannot find UE for AMF_UE_NGAP_ID[%d] ", *AMF_UE_NGAP_ID);

    target_ue->ran_ue_ngap_id = *RAN_UE_NGAP_ID;
    source_ue = target_ue->source_ue;
    d_assert(source_ue, return,);
    amf_ue = source_ue->amf_ue;
    d_assert(amf_ue, return,);

    d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            source_ue->ran_ue_ngap_id, source_ue->amf_ue_ngap_id);
    d_trace(5, "    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            target_ue->ran_ue_ngap_id, target_ue->amf_ue_ngap_id);

    for (i = 0 ; i < PDUSessionResourceAdmittedList->list.count ; i ++)
    {
#if 0   
        //TODO : 
        NGAP_PDUSessionResourceAdmittedItemIEs_t *ie2 = NULL;
        NGAP_PDUSessionResourceAdmittedItem_t *PDUSession = NULL;

        ie2 = (NGAP_PDUSessionResourceAdmittedItemIEs_t *)PDUSessionResourceAdmittedList->list.array[i];
        d_assert(ie2, return, );
        PDUSession = &ie2->value.choice.PDUSessionResourceAdmittedItem;
        PDUSession->pDUSessionID;
#endif  
    }


    NGAP_STORE_DATA(&amf_ue->container, TargetToSource_TransparentContainer);

#if 0
    if (amf4g_ue_have_indirect_tunnel(amf4g_ue) == 1)
    {
        rv = amf4g_gtp_send_create_indirect_data_forwarding_tunnel_request(
                amf4g_ue);
        d_assert(rv == CORE_OK, return, "gtp send failed");
    }
    else
#endif
    {
        rv = ngap_send_handover_command(source_ue);
        d_assert(rv == CORE_OK, return, "gtp send failed");
    }


}

void ngap_handle_handover_failure(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];
    status_t rv;

    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    NGAP_HandoverFailure_t *HandoverFailure = NULL;

    NGAP_HandoverFailureIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_Cause_t *Cause = NULL;
#if 0 
			NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    ran_ue_t *source_ue = NULL;
    ran_ue_t *target_ue = NULL;
    
    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    unsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(unsuccessfulOutcome, return,);
    HandoverFailure = &unsuccessfulOutcome->value.choice.HandoverFailure;
    d_assert(HandoverFailure, return,);

    d_trace(3, "[AMF] Handover failure\n");

    for (i = 0; i < HandoverFailure->protocolIEs.list.count; i++)
    {
        ie = HandoverFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(AMF_UE_NGAP_ID, return,);
    d_assert(Cause, return,);

    target_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    d_assert(target_ue, return,
        "Cannot find UE for AMF_UE_NGAP_ID[%d]", *AMF_UE_NGAP_ID);
    source_ue = target_ue->source_ue;
    d_assert(source_ue, return,);

    d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            source_ue->ran_ue_ngap_id, source_ue->amf_ue_ngap_id);
    d_trace(5, "    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            target_ue->ran_ue_ngap_id, target_ue->amf_ue_ngap_id);

    rv = ngap_send_handover_preparation_failure(source_ue, Cause);
    d_assert(rv ==CORE_OK, return, "ngap send error");

    rv = ngap_send_ue_context_release_command(
        target_ue, NGAP_Cause_PR_radioNetwork,
        NGAP_CauseRadioNetwork_ho_failure_in_target_5GC_ngran_node_or_target_system,
        NGAP_UE_CTX_REL_DELETE_INDIRECT_TUNNEL, 0);
    d_assert(rv == CORE_OK, return, "npap send error");
}

void ngap_handle_handover_notification(amf_ran_t *ran, ngap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i;
    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_HandoverNotify_t *HandoverNotify = NULL;

    NGAP_HandoverNotifyIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_UserLocationInformation_t *UserLocationInformation = NULL;
            NGAP_TAC_t *tAC = NULL;
            NGAP_PLMNIdentity_t *PLMNIdentity = NULL;
            NGAP_EUTRACellIdentity_t *eUTRACellIdentity = NULL;
    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    HandoverNotify = &initiatingMessage->value.choice.HandoverNotify;
    d_assert(HandoverNotify, return,);

    d_trace(3, "[AMF] Handover notification \n");
    
    ran_ue_t *source_ue = NULL;
    ran_ue_t *target_ue = NULL;
    amf_ue_t *amf_ue = NULL;

    for (i = 0; i < HandoverNotify->protocolIEs.list.count; i++)
    {
        ie = HandoverNotify->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                d_assert(AMF_UE_NGAP_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                d_assert(RAN_UE_NGAP_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_UserLocationInformation:
                UserLocationInformation = &ie->value.choice.UserLocationInformation;
                d_assert(UserLocationInformation, return,);
                break;
            default:
                break;
        }
    }
    
    d_trace(5, "    ran[%s] \n",
            CORE_ADDR(ran->addr, buf));
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    switch(UserLocationInformation->present)
    {
        case NGAP_UserLocationInformation_PR_userLocationInformationEUTRA:
        {
            NGAP_UserLocationInformationEUTRA_t *userLocationInformationEUTRA = NULL;
            userLocationInformationEUTRA = UserLocationInformation->choice.userLocationInformationEUTRA;
            d_assert(userLocationInformationEUTRA , return, );
            PLMNIdentity = &userLocationInformationEUTRA->tAI.pLMNIdentity;
            d_assert(PLMNIdentity, return,);
            tAC = &userLocationInformationEUTRA->tAI.tAC;
            d_assert(tAC, return,);
            PLMNIdentity = &userLocationInformationEUTRA->eUTRA_CGI.pLMNIdentity;
            d_assert(PLMNIdentity, return,);
            eUTRACellIdentity = &userLocationInformationEUTRA->eUTRA_CGI.eUTRACellIdentity;
            d_assert(eUTRACellIdentity, return,);

            d_assert(AMF_UE_NGAP_ID, return, );
            d_assert(RAN_UE_NGAP_ID, return, );
            target_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
            d_assert(target_ue, return, "Cannot find UE for RAN_UE_NGAP_ID[%d] ", *RAN_UE_NGAP_ID);
            d_assert(target_ue->amf_ue_ngap_id == *AMF_UE_NGAP_ID, return,
                     "Conflict RAN_UE_NGAP_ID[%d] : != %d \n",
                     target_ue->amf_ue_ngap_id, *AMF_UE_NGAP_ID);

            source_ue = target_ue-> source_ue;
            d_assert(source_ue, return, );
            amf_ue = source_ue->amf_ue;
            d_assert(amf_ue, return, );
            d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
                    source_ue->ran_ue_ngap_id, source_ue->amf_ue_ngap_id);
            d_trace(5, "    Target : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
                    target_ue->ran_ue_ngap_id, target_ue->amf_ue_ngap_id);
            amf_ue_associate_ran_ue(amf_ue, target_ue);
#if 0
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

            rv = amf4g_gtp_send_modify_bearer_request(bearer, 1);
            d_assert(rv == CORE_OK, return, "gtp send failed");

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }
#endif
            break;
        }
        case NGAP_UserLocationInformation_PR_userLocationInformationNR:
        {
            NGAP_UserLocationInformationNR_t *userLocationInformationNR = NULL;
            userLocationInformationNR = UserLocationInformation->choice.userLocationInformationNR;
            d_assert(userLocationInformationNR , return, );
            break;
        }
        case NGAP_UserLocationInformation_PR_userLocationInformationN3IWF:
        {
            NGAP_UserLocationInformationN3IWF_t *userLocationInformationN3IWF = NULL;
            userLocationInformationN3IWF = UserLocationInformation->choice.userLocationInformationN3IWF;
            d_assert(userLocationInformationN3IWF , return, );
            break;
        }
        case NGAP_UserLocationInformation_PR_NOTHING:
        default:
        {
            break;
        }

    }	
}

void ngap_handle_initial_ue_message(amf_ran_t *ran, ngap_message_t *message)
{
    int i;
    char buf[CORE_ADDRSTRLEN];

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_InitialUEMessage_t	*InitialUEMessage = NULL;

    NGAP_InitialUEMessage_IEs_t	*ie = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_NAS_PDU_t *NAS_PDU = NULL;
        NGAP_UserLocationInformation_t *UserLocationInformation = NULL;
            NGAP_TAC_t *tAC = NULL;
            NGAP_PLMNIdentity_t *PLMNIdentity = NULL;
            NGAP_EUTRACellIdentity_t *eUTRACellIdentity = NULL;
#if 0 
        NGAP_RRCEstablishmentCause_t	 RRCEstablishmentCause;
        NGAP_FiveG_S_TMSI_t	 FiveG_S_TMSI;
        NGAP_GUAMI_t	 GUAMI;
        NGAP_AMFSetID_t	 AMFSetID;
        NGAP_UEContextRequest_t	 UEContextRequest;
#endif
    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    InitialUEMessage = &initiatingMessage->value.choice.InitialUEMessage;
    d_assert(InitialUEMessage, return,);

    d_trace(3, "[AMF] Initial UE message \n");
    
    ran_ue_t *ran_ue = NULL;

    for (i = 0; i < InitialUEMessage->protocolIEs.list.count; i++)
    {
        ie = InitialUEMessage->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                d_assert(RAN_UE_NGAP_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_NAS_PDU:
                NAS_PDU = &ie->value.choice.NAS_PDU;
                d_assert(NAS_PDU, return,);
                break;
            case NGAP_ProtocolIE_ID_id_UserLocationInformation:
                UserLocationInformation = &ie->value.choice.UserLocationInformation;
                d_assert(NAS_PDU, return,);
                break;
            default:
                break;
        }
    }

    d_trace(5, "    ran[%s] \n",
            CORE_ADDR(ran->addr, buf));
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(RAN_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    if(!ran_ue)
    {
        ran_ue = ran_ue_add(ran);
        d_assert(ran_ue, return, );

        ran_ue->ran_ue_ngap_id = *RAN_UE_NGAP_ID;
        switch(UserLocationInformation->present)
        {
            case NGAP_UserLocationInformation_PR_userLocationInformationEUTRA:
            {
                NGAP_UserLocationInformationEUTRA_t *userLocationInformationEUTRA = NULL;
                userLocationInformationEUTRA = UserLocationInformation->choice.userLocationInformationEUTRA;
                d_assert(userLocationInformationEUTRA , return, );
                PLMNIdentity = &userLocationInformationEUTRA->tAI.pLMNIdentity;
                d_assert(PLMNIdentity, return,);
                tAC = &userLocationInformationEUTRA->tAI.tAC;
                d_assert(tAC, return,);
                PLMNIdentity = &userLocationInformationEUTRA->eUTRA_CGI.pLMNIdentity;
                d_assert(PLMNIdentity, return,);
                eUTRACellIdentity = &userLocationInformationEUTRA->eUTRA_CGI.eUTRACellIdentity;
                d_assert(eUTRACellIdentity, return,);

                d_assert(RAN_UE_NGAP_ID, return, );
                
#if 0
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

    d_trace(5, "    ENB_UE_S1AP_ID[%d] MME_UE_S1AP_ID[%d] TAC[%d]\n",
        enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id, enb_ue->nas.tai.tac);

    /*if (enb_ue){
        pkbuf_t *s1apbuf = NULL;
        s1ap_build_reroute_nas_request(&s1apbuf,enb_ue);
        d_assert(s1ap_send_to_enb(enb, s1apbuf, S1AP_NON_UE_SIGNALLING) == CORE_OK,,"s1ap_send_to_enb() failed");
    }*/

    d_assert(s1ap_send_to_nas(enb_ue,
        S1AP_ProcedureCode_id_initialUEMessage, NAS_PDU) == CORE_OK,,
        "s1ap_send_to_nas failed");
    #endif
                break;
            }
            case NGAP_UserLocationInformation_PR_userLocationInformationNR:
            {
                NGAP_UserLocationInformationNR_t *userLocationInformationNR = NULL;
                userLocationInformationNR = UserLocationInformation->choice.userLocationInformationNR;
                d_assert(userLocationInformationNR , return, );
                break;
            }
            case NGAP_UserLocationInformation_PR_userLocationInformationN3IWF:
            {
                NGAP_UserLocationInformationN3IWF_t *userLocationInformationN3IWF = NULL;
                userLocationInformationN3IWF = UserLocationInformation->choice.userLocationInformationN3IWF;
                d_assert(userLocationInformationN3IWF , return, );
                break;
            }
            case NGAP_UserLocationInformation_PR_NOTHING:
            default:
            {
                break;
            }

        }	
    }
    
}

void ngap_handle_ue_context_release_request(amf_ran_t *ran, ngap_message_t *message)
{
    status_t rv;
    int i;
    char buf[CORE_ADDRSTRLEN];

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UEContextReleaseRequest_t	*UEContextReleaseRequest = NULL;
    
    NGAP_UEContextReleaseRequest_IEs_t	*ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_Cause_t *Cause = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    UEContextReleaseRequest = &initiatingMessage->value.choice.UEContextReleaseRequest;
    d_assert(UEContextReleaseRequest, return,);

    ran_ue_t *ran_ue = NULL;
    amf_ue_t *amf_ue = NULL;

    d_trace(3, "[AMF] UE Context release request \n");

    for (i = 0; i < UEContextReleaseRequest->protocolIEs.list.count; i++)
    {
        ie = UEContextReleaseRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                d_assert(AMF_UE_NGAP_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                d_assert(RAN_UE_NGAP_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &ie->value.choice.Cause;
                d_assert(Cause, return,);
                break;
            default:
                break;
        }
    }

    d_trace(5, "    ran[%s] \n",
            CORE_ADDR(ran->addr, buf));
    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    
    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);
    if(!ran_ue) 
    {
        d_warn("No UE Context : AMF_UE_NGAP_ID[%d]", *AMF_UE_NGAP_ID);
#if 0   //TODO: add error indication
        rv = s1ap_send_error_indication(enb, 
                AMF_UE_NGAP_ID, ENB_UE_S1AP_ID,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK, return, "s1ap send error");
#endif
        return;
    }
    
    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);

    d_assert(Cause, return,);
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

    amf_ue = ran_ue->amf_ue;
    if (amf_ue)
    {
#if 0
        if (FSM_CHECK(&amf_ue->sm, emm_state_registered))
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
#endif
    }
    else
    {
        d_trace(5, "    NG Context Not Associated\n");
        rv = ngap_send_ue_context_release_command(ran_ue,
            NGAP_Cause_PR_nas, NGAP_CauseNas_normal_release ,
            NGAP_UE_CTX_REL_NO_ACTION, 0);
        d_assert(rv == CORE_OK,, "ngap send error");
    }
}

void ngap_handle_path_switch_request(amf_ran_t *ran, ngap_message_t *message)
{
    char buf[CORE_ADDRSTRLEN];
    int i;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PathSwitchRequest_t *PathSwitchRequest = NULL;

    NGAP_PathSwitchRequestIEs_t *ie = NULL;
    	NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;;
		NGAP_UserLocationInformation_t *UserLocationInformation = NULL;;
            NGAP_TAC_t *tAC = NULL;
            NGAP_PLMNIdentity_t *PLMNIdentity = NULL;
            NGAP_EUTRACellIdentity_t *eUTRACellIdentity = NULL;
		NGAP_UESecurityCapabilities_t *UESecurityCapabilities = NULL;;
		//NGAP_PDUSessionResourceToBeSwitchedDLList_t	*PDUSessionResourceToBeSwitchedDLList = NULL;;
#if 0	    
        NGAP_PDUSessionList_t	 PDUSessionList;
#endif
    ran_ue_t *ran_ue = NULL;
    amf_ue_t *amf_ue = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    PathSwitchRequest = &initiatingMessage->value.choice.PathSwitchRequest;
    d_assert(PathSwitchRequest, return,);

    d_trace(3, "[AMF] Path switch request \n");

    for (i = 0; i < PathSwitchRequest->protocolIEs.list.count; i++)
    {
        ie = PathSwitchRequest->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
                d_assert(AMF_UE_NGAP_ID, return,);
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_UserLocationInformation:
                UserLocationInformation = &ie->value.choice.UserLocationInformation;
                break;
            case NGAP_ProtocolIE_ID_id_UESecurityCapabilities:
                UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;
                break;
            // case NGAP_ProtocolIE_ID_id_PDUSessionResourceToBeSwitchedDLList:
            //     PDUSessionResourceToBeSwitchedDLList = &ie->value.choice.PDUSessionResourceToBeSwitchedDLList;
            //     break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    switch(UserLocationInformation->present)
    {
        case NGAP_UserLocationInformation_PR_userLocationInformationEUTRA:
        {
            NGAP_UserLocationInformationEUTRA_t *userLocationInformationEUTRA = NULL;
            userLocationInformationEUTRA = UserLocationInformation->choice.userLocationInformationEUTRA;
            d_assert(userLocationInformationEUTRA , return, );
            PLMNIdentity = &userLocationInformationEUTRA->tAI.pLMNIdentity;
            d_assert(PLMNIdentity, return,);
            tAC = &userLocationInformationEUTRA->tAI.tAC;
            d_assert(tAC, return,);
            PLMNIdentity = &userLocationInformationEUTRA->eUTRA_CGI.pLMNIdentity;
            d_assert(PLMNIdentity, return,);
            eUTRACellIdentity = &userLocationInformationEUTRA->eUTRA_CGI.eUTRACellIdentity;
            d_assert(eUTRACellIdentity, return,);
            d_assert(RAN_UE_NGAP_ID, return, );
            
            break;
        }
        case NGAP_UserLocationInformation_PR_userLocationInformationNR:
        {
            NGAP_UserLocationInformationNR_t *userLocationInformationNR = NULL;
            userLocationInformationNR = UserLocationInformation->choice.userLocationInformationNR;
            d_assert(userLocationInformationNR , return, );
            break;
        }
        case NGAP_UserLocationInformation_PR_userLocationInformationN3IWF:
        {
            NGAP_UserLocationInformationN3IWF_t *userLocationInformationN3IWF = NULL;
            userLocationInformationN3IWF = UserLocationInformation->choice.userLocationInformationN3IWF;
            d_assert(userLocationInformationN3IWF , return, );
            break;
        }
        case NGAP_UserLocationInformation_PR_NOTHING:
        default:
        {
            break;
        }

    }
    NGAP_NrencryptionAlgorithms_t *nRencryptionAlgorithms = NULL; 
	NGAP_NrintegrityProtectionAlgorithms_t *nRintegrityProtectionAlgorithms = NULL;
	NGAP_EUTRAencryptionAlgorithms_t *eUTRAencryptionAlgorithms = NULL;
	NGAP_EUTRAintegrityProtectionAlgorithms_t *eUTRAintegrityProtectionAlgorithms = NULL;
    d_assert(UESecurityCapabilities, return,);
    nRencryptionAlgorithms = &UESecurityCapabilities->nRencryptionAlgorithms;
    d_assert(nRencryptionAlgorithms, return,);
    nRintegrityProtectionAlgorithms = &UESecurityCapabilities->nRintegrityProtectionAlgorithms;
    d_assert(nRintegrityProtectionAlgorithms, return,);
    eUTRAencryptionAlgorithms = &UESecurityCapabilities->eUTRAencryptionAlgorithms;
    d_assert(eUTRAencryptionAlgorithms, return,);
    eUTRAintegrityProtectionAlgorithms = &UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms;
    d_assert(eUTRAintegrityProtectionAlgorithms, return,);

    d_assert(AMF_UE_NGAP_ID, return,);
    d_assert(RAN_UE_NGAP_ID, return,);
    ran_ue = ran_ue_find_by_amf_ue_ngap_id(*AMF_UE_NGAP_ID);

    if (!ran_ue)
    {
#if 0
        d_error("Cannot find UE from sourceMME-UE-S1AP-ID[%d] and eNB[%s:%d]",
                *MME_UE_S1AP_ID, CORE_ADDR(enb->addr, buf), enb->enb_id);
        // TODO : add path switch failure message
        rv = s1ap_build_path_switch_failure(&s1apbuf,
                *ENB_UE_S1AP_ID, *MME_UE_S1AP_ID,
                S1AP_Cause_PR_radioNetwork,
                S1AP_CauseRadioNetwork_unknown_mme_ue_s1ap_id);
        d_assert(rv == CORE_OK && s1apbuf, return, "s1ap build error");

        rv = ngap_send_to_ran(ran, ngapbuf, NGAP_NON_UE_SIGNALLING);
        d_assert(rv == CORE_OK,, "ngap send error");
        return;
#endif
    }

    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);

    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return, "NULL param");
#if 0
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

        rv = ngap_send_to_ran_ue(ran_ue, ngapbuf);
        d_assert(rv == CORE_OK,, "s1ap send error");
        return;
    }
#endif
    
    ran_ue->ran_ue_ngap_id = *RAN_UE_NGAP_ID;
#if 0
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
#endif
}

/**
 * Direction: NG-RAN node -> AMF
 **/
void ngap_handle_amf_configuration_update_acknowledge(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    int j = 0;
    NGAP_SuccessfulOutcome_t *SuccessfulOutcome = NULL;
    NGAP_AMFConfigurationUpdateAcknowledge_t *AMFConfigurationUpdateAcknowledge = NULL;

    NGAP_AMFConfigurationUpdateAcknowledgeIEs_t *AMFConfigurationUpdateAcknowledgeIEs = NULL;
        NGAP_AMF_TNLAssociationSetupList_t *AMF_TNLAssociationSetupList = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    SuccessfulOutcome = message->choice.successfulOutcome;
    AMFConfigurationUpdateAcknowledge = &SuccessfulOutcome->value.choice.AMFConfigurationUpdateAcknowledge;

    d_assert(AMFConfigurationUpdateAcknowledgeIEs, return, );
    d_trace(3, "[AMF] AMF configuration update acknowledge\n");


    for (i = 0; i < AMFConfigurationUpdateAcknowledge->protocolIEs.list.count; i++)
    {
        AMFConfigurationUpdateAcknowledgeIEs = AMFConfigurationUpdateAcknowledge->protocolIEs.list.array[i];
        switch(AMFConfigurationUpdateAcknowledgeIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_TNLAssociationSetupList = &AMFConfigurationUpdateAcknowledgeIEs->value.choice.AMF_TNLAssociationSetupList;
                d_assert(AMF_TNLAssociationSetupList, return,);
                break;

            default:
                break;
        }
    }
    for(j = 0 ; j < AMF_TNLAssociationSetupList->list.count; j++)
    {
        NGAP_AMF_TNLAssociationSetupItem_t *AMF_TNLAssociationSetupItem = NULL;
        AMF_TNLAssociationSetupItem = (NGAP_AMF_TNLAssociationSetupItem_t *) AMF_TNLAssociationSetupList->list.array[j];
        // AMF_TNLAssociationSetupItem->aMF_TNLAssociationAddress.choice.endpointIPAddress;
          //Previously received AMF Transport Layer information for the TNL association.
        d_assert(AMF_TNLAssociationSetupItem, return, );
    }
 
}

/**
 * Direction: NG-RAN node -> AMF
 **/
void ngap_handle_amf_configuration_update_failure(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    NGAP_UnsuccessfulOutcome_t *UnsuccessfulOutcome = NULL;
    NGAP_AMFConfigurationUpdateFailure_t *AMFConfigurationUpdateFailure = NULL;

    NGAP_AMFConfigurationUpdateFailureIEs_t *ie = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);
    d_assert(message, return,);
    UnsuccessfulOutcome = message->choice.unsuccessfulOutcome;
    d_assert(UnsuccessfulOutcome, return,);
    AMFConfigurationUpdateFailure = &UnsuccessfulOutcome->value.choice.AMFConfigurationUpdateFailure;

    d_assert(AMFConfigurationUpdateFailure, return, );
    d_trace(3, "[AMF] AMF configuration update failure\n");

    for(i = 0; i < AMFConfigurationUpdateFailure->protocolIEs.list.count; i++)
    {
        ie = AMFConfigurationUpdateFailure->protocolIEs.list.array[i];
        switch(ie->id)
        {
            case NGAP_ProtocolIE_ID_id_Cause:
                break;
            default:
                break;
        }
    }
}

void ngap_handle_ran_configuration_update(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    int j = 0;
    int k = 0;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_RANConfigurationUpdate_t *RANConfigurationUpdate = NULL;

    NGAP_RANConfigurationUpdateIEs_t *RANConfigurationUpdateIEs = NULL;
        NGAP_SupportedTAList_t *SupportedTAList = NULL;
        NGAP_PagingDRX_t *PagingDRX = NULL;
#if 0
        NGAP_RANNodeName_t *RANNodeName = NULL;
#endif

    pkbuf_t *ngapbuf = NULL;
    NGAP_Cause_PR group = NGAP_Cause_PR_NOTHING;
    long cause = 0;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    RANConfigurationUpdate = &initiatingMessage->value.choice.RANConfigurationUpdate;
    d_assert(RANConfigurationUpdate, return,);

    d_trace(3, "[AMF] RAN Configuration Update \n");
    
    for (i = 0; i < RANConfigurationUpdate->protocolIEs.list.count; i++)
    {
        RANConfigurationUpdateIEs = RANConfigurationUpdate->protocolIEs.list.array[i];
        switch(RANConfigurationUpdateIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_SupportedTAList:
                SupportedTAList = &RANConfigurationUpdateIEs->value.choice.SupportedTAList;
                break;
            case NGAP_ProtocolIE_ID_id_DefaultPagingDRX:
                PagingDRX = &RANConfigurationUpdateIEs->value.choice.PagingDRX;
                break;
            default:
                break;
        }
    }

    if (PagingDRX)
    {
        d_trace(5, "    PagingDRX[%ld]\n", *PagingDRX);
    }
        
    ran->num_of_supported_ta_list = 0;
    for (i = 0; SupportedTAList->list.count; i ++)
    {
        NGAP_SupportedTAItem_t *NGAP_SupportedTAItem = NULL;
        NGAP_SupportedTAItem = SupportedTAList->list.array[i];
        d_assert(NGAP_SupportedTAItem, return,);

        for (j = 0; NGAP_SupportedTAItem->broadcastPLMNList.list.count;j++)
        {
            NGAP_BroadcastPLMNItem_t *BroadcastPLMNItem = NULL;
            BroadcastPLMNItem = NGAP_SupportedTAItem->broadcastPLMNList.list.array[j];
            memcpy(&ran->supported_ta_list[ran->num_of_supported_ta_list].tai.tac, 
                &NGAP_SupportedTAItem->tAC.buf, sizeof(c_uint16_t));
                ran->supported_ta_list[ran->num_of_supported_ta_list].tai.tac =
                    ntohs(ran->supported_ta_list[ran->num_of_supported_ta_list].tai.tac);
            memcpy(&ran->supported_ta_list[ran->num_of_supported_ta_list].tai.plmn_id,
                &BroadcastPLMNItem->pLMNIdentity.buf, sizeof(plmn_id_t));
                c_uint16_t tmp_num_of_s_nssai = ran->supported_ta_list[ran->num_of_supported_ta_list].num_of_s_nssai;
                for(k = 0; BroadcastPLMNItem->tAISliceSupportList.list.count; k++)
                {
                    NGAP_SliceSupportItem_t *SliceSupportItem = NULL;
                    SliceSupportItem = BroadcastPLMNItem->tAISliceSupportList.list.array[k];
                    memcpy(&ran->supported_ta_list[ran->num_of_supported_ta_list].s_nssai[tmp_num_of_s_nssai].sst,
                    &SliceSupportItem->s_NSSAI.sST, sizeof(NGAP_SST_t));
                    tmp_num_of_s_nssai++;
                }
                ran->supported_ta_list[ran->num_of_supported_ta_list].num_of_s_nssai = tmp_num_of_s_nssai;
        }
        ran->num_of_supported_ta_list++;
    }

    if (ran->num_of_supported_ta_list == 0)
    {
        d_warn("RAN Configuration update failure:");
        d_warn("    No supported TA exist in RAN Configuration update");
        group = NGAP_Cause_PR_misc;
        cause = NGAP_CauseMisc_unspecified;
    }
    else
    {
        int served_tai_index = -1;
        for (i = 0; i < ran->num_of_supported_ta_list; i++)
        {
            served_tai_index = 
                amf_find_served_tai(&ran->supported_ta_list[i].tai);
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
            group = NGAP_Cause_PR_misc;
            cause = NGAP_CauseMisc_unknown_PLMN;
        }
    }

        if (group == NGAP_Cause_PR_NOTHING)
    {
        d_trace(3, "[AMF]ran Configuration update ACKNOWLEDGE\n");
        d_assert(ngap_build_ran_configuration_update_acknowledge(&ngapbuf) == CORE_OK, 
                return, "ngap_build_ran_configuration_update_acknowledge() failed");
    }
    else
    {
        d_trace(3, "[AMF] ran Configuration update failure\n");
        d_assert(ngap_build_ran_configuration_update_failure(
                &ngapbuf, group, cause, NGAP_TimeToWait_v10s) == CORE_OK, 
                return, "ngap_build_ran_configuration_update_failure() failed");
    }

    d_assert(ngap_send_to_ran(ran, ngapbuf, NGAP_NON_UE_SIGNALLING) == CORE_OK,,
            "ngap_send_to_ran() failed");

}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_pdu_session_resource_modify_response(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];

    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_PDUSessionResourceModifyResponse_t *PDUSessionResourceModifyResponse = NULL;

    NGAP_PDUSessionResourceModifyResponseIEs_t *PDUSessionResourceModifyResponseIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceModifyListModRes_t *PDUSessionResourceModifyListModRes = NULL;
#if 0        
        NGAP_PDUSessionList_t	 PDUSessionList;
        NGAP_UserLocationInformation_t	 UserLocationInformation;
        NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    d_assert(ran, return,);
    d_assert(message, return,);
    
    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    PDUSessionResourceModifyResponse =
        &successfulOutcome->value.choice.PDUSessionResourceModifyResponse;
    d_assert(PDUSessionResourceModifyResponse, return,);

    for (i = 0; i < PDUSessionResourceModifyResponse->protocolIEs.list.count; i++)
    {
        PDUSessionResourceModifyResponseIEs = PDUSessionResourceModifyResponse->protocolIEs.list.array[i];
        switch(PDUSessionResourceModifyResponseIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &PDUSessionResourceModifyResponseIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID =
                    &PDUSessionResourceModifyResponseIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_PDUSessionResourceModifyListModRes:
                PDUSessionResourceModifyListModRes =
                    &PDUSessionResourceModifyResponseIEs->value.choice.PDUSessionResourceModifyListModRes;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return, );

    d_assert(PDUSessionResourceModifyListModRes, return, );
    for (i = 0 ; i < PDUSessionResourceModifyListModRes->list.count; i++)
    {   
        NGAP_PDUSessionResourceModifyItemModResIEs_t *PDUSessionResourceModifyItemModResIEs = NULL;
        // NGAP_PDUSessionResourceSetupResponseTransfer_t *PDUSessionResourceSetupResponseTransfer = NULL;
        PDUSessionResourceModifyItemModResIEs = (NGAP_PDUSessionResourceModifyItemModResIEs_t *) 
            PDUSessionResourceModifyListModRes->list.array[i];
            d_assert(PDUSessionResourceModifyItemModResIEs, return,);
            NGAP_PDUSessionResourceModifyItemModRes_t *PDUSessionResourceModifyItemModRes = NULL;
            PDUSessionResourceModifyItemModRes = &PDUSessionResourceModifyItemModResIEs->value.choice.PDUSessionResourceModifyItemModRes;
    
        amf_ue->psi = PDUSessionResourceModifyItemModRes->pDUSessionID;
        //PDUSessionResourceModifyItemModRes->pDUSessionResourceSetupRequestTransfer;
       
    }
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_pdu_session_resource_modify_indication(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PDUSessionResourceModifyIndication_t *PDUSessionResourceModifyIndication = NULL;


    NGAP_PDUSessionResourceModifyIndicationIEs_t *PDUSessionResourceModifyIndicationIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceModifyListModInd_t *PDUSessionResourceModifyListModInd = NULL;

    d_assert(ran, return,);
    d_assert(message, return,);
    
    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    PDUSessionResourceModifyIndication =
        &initiatingMessage->value.choice.PDUSessionResourceModifyIndication;
    d_assert(PDUSessionResourceModifyIndication, return,);

    for (i = 0; i < PDUSessionResourceModifyIndication->protocolIEs.list.count; i++)
    {
        PDUSessionResourceModifyIndicationIEs = PDUSessionResourceModifyIndication->protocolIEs.list.array[i];
        switch(PDUSessionResourceModifyIndicationIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &PDUSessionResourceModifyIndicationIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID =
                    &PDUSessionResourceModifyIndicationIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_PDUSessionResourceModifyListModInd:
                PDUSessionResourceModifyListModInd =
                    &PDUSessionResourceModifyIndicationIEs->value.choice.PDUSessionResourceModifyListModInd;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return, );

    d_assert(PDUSessionResourceModifyListModInd, return, );
    for (i = 0 ; i < PDUSessionResourceModifyListModInd->list.count; i++)
    {   
        NGAP_PDUSessionResourceModifyItemModIndIEs_t *PDUSessionResourceModifyItemModIndIEs = NULL;
        // NGAP_PDUSessionResourceSetupResponseTransfer_t *PDUSessionResourceSetupResponseTransfer = NULL;
        PDUSessionResourceModifyItemModIndIEs = (NGAP_PDUSessionResourceModifyItemModIndIEs_t *) 
            PDUSessionResourceModifyListModInd->list.array[i];
            d_assert(PDUSessionResourceModifyItemModIndIEs, return,);
            NGAP_PDUSessionResourceModifyItemModInd_t *PDUSessionResourceModifyItemModInd = NULL;
            PDUSessionResourceModifyItemModInd = &PDUSessionResourceModifyItemModIndIEs->value.choice.PDUSessionResourceModifyItemModInd;
    
        amf_ue->psi = PDUSessionResourceModifyItemModInd->pDUSessionID;
        //PDUSessionResourceModifyItemModRes->pDUSessionResourceSetupRequestTransfer;
       
    }
}


/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_pdu_session_resource_release_response(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];

    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_PDUSessionResourceReleaseResponse_t *PDUSessionResourceReleaseResponse = NULL;

    NGAP_PDUSessionResourceReleaseResponseIEs_t *PDUSessionResourceReleaseResponseIEs = NULL;
    	NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
	    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
#if 0
		NGAP_UserLocationInformation_t	 UserLocationInformation;
		NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    d_assert(ran, return,);
    d_assert(message, return,);
    
    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    PDUSessionResourceReleaseResponse = &successfulOutcome->value.choice.PDUSessionResourceReleaseResponse;
    d_assert(PDUSessionResourceReleaseResponse, return,);
    

    for (i = 0; i < PDUSessionResourceReleaseResponse->protocolIEs.list.count; i++)
    {
        PDUSessionResourceReleaseResponseIEs = PDUSessionResourceReleaseResponse->protocolIEs.list.array[i];
        switch(PDUSessionResourceReleaseResponseIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &PDUSessionResourceReleaseResponseIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID =
                    &PDUSessionResourceReleaseResponseIEs->value.choice.RAN_UE_NGAP_ID;
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return, );
    
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_ue_radio_capability_check_response(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];

    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_UERadioCapabilityCheckResponse_t *NGAP_UERadioCapabilityCheckResponse = NULL;

    NGAP_UERadioCapabilityCheckResponseIEs_t *NGAP_UERadioCapabilityCheckResponseIEs = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_IMSVoiceSupportIndicator_t	*IMSVoiceSupportIndicator = NULL;
#if 0
			NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    d_assert(ran, return,);
    d_assert(message, return,);
    
    amf_ue_t *amf_ue = NULL;
    ran_ue_t *ran_ue = NULL;

    successfulOutcome = message->choice.successfulOutcome;
    d_assert(successfulOutcome, return,);
    NGAP_UERadioCapabilityCheckResponse = &successfulOutcome->value.choice.UERadioCapabilityCheckResponse;
    d_assert(NGAP_UERadioCapabilityCheckResponse, return,);
    
    for (i = 0; i < NGAP_UERadioCapabilityCheckResponse->protocolIEs.list.count; i++)
    {
        NGAP_UERadioCapabilityCheckResponseIEs = NGAP_UERadioCapabilityCheckResponse->protocolIEs.list.array[i];
        switch(NGAP_UERadioCapabilityCheckResponseIEs->id)
        {
            case NGAP_UERadioCapabilityCheckResponseIEs__value_PR_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &NGAP_UERadioCapabilityCheckResponseIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_UERadioCapabilityCheckResponseIEs__value_PR_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &NGAP_UERadioCapabilityCheckResponseIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_UERadioCapabilityCheckResponseIEs__value_PR_IMSVoiceSupportIndicator:
                IMSVoiceSupportIndicator = &NGAP_UERadioCapabilityCheckResponseIEs->value.choice.IMSVoiceSupportIndicator;
                d_assert(IMSVoiceSupportIndicator, return, );
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }

    d_assert(AMF_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[%d]", *RAN_UE_NGAP_ID);
    amf_ue = ran_ue->amf_ue;
    d_assert(amf_ue, return, );

}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_nas_non_delivery_indication(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_NASNonDeliveryIndication_t *NASNonDeliveryIndication = NULL;

    NGAP_NASNonDeliveryIndication_IEs_t *NASNonDeliveryIndication_IEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_NAS_PDU_t *NAS_PDU = NULL;
        NGAP_Cause_t *Cause = NULL;
    ran_ue_t *ran_ue = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    NASNonDeliveryIndication = &initiatingMessage->value.choice.NASNonDeliveryIndication;
    d_assert(NASNonDeliveryIndication, return,);

    d_trace(3, "[AMF] NAS NON DELIVERY INDICATION\n");

    for(i = 0; i < NASNonDeliveryIndication->protocolIEs.list.count; i++)
    {
        NASNonDeliveryIndication_IEs = NASNonDeliveryIndication->protocolIEs.list.array[i];
        switch(NASNonDeliveryIndication_IEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &NASNonDeliveryIndication_IEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &NASNonDeliveryIndication_IEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_NAS_PDU:
                NAS_PDU = &NASNonDeliveryIndication_IEs->value.choice.NAS_PDU;
                break;
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &NASNonDeliveryIndication_IEs->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(AMF_UE_NGAP_ID, return, );
    d_assert(RAN_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[RAN_UE_NGAP_ID:%d]", *RAN_UE_NGAP_ID);

    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n", ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);
    d_assert(Cause, return,);
    d_trace(5, "    Cause[Group:%d Cause:%d]\n", Cause->present, Cause->choice.radioNetwork);      
#if 0            
    //TODO: NAS Handle
    d_assert(ngap_send_to_nas(ran_ue,
        NGAP_ProcedureCode_id_NASNonDeliveryIndication, NAS_PDU) == CORE_OK,,
        "ngap_send_to_nas failed"); 
#endif
   d_assert(NAS_PDU, return,);
}


/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_pdu_session_resource_notify(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PDUSessionResourceNotify_t *PDUSessionResourceNotify = NULL;

    NGAP_PDUSessionResourceNotifyIEs_t *PDUSessionResourceNotifyIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceNotifyList_t	*PDUSessionResourceNotifyList = NULL;
#if 0
    NGAP_PDUSessionList_t	 PDUSessionList;
    NGAP_UserLocationInformation_t	 UserLocationInformation;
#endif
    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    PDUSessionResourceNotify = &initiatingMessage->value.choice.PDUSessionResourceNotify;
    d_assert(PDUSessionResourceNotify, return,);

    d_trace(3, "[AMF] PDU Session Resource Notify \n");

    ran_ue_t *ran_ue = NULL;

    for(i = 0; i < PDUSessionResourceNotify->protocolIEs.list.count; i++)
    {
        PDUSessionResourceNotifyIEs = PDUSessionResourceNotify->protocolIEs.list.array[i];
        switch(PDUSessionResourceNotifyIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &PDUSessionResourceNotifyIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &PDUSessionResourceNotifyIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_PDUSessionResourceNotifyList:
                PDUSessionResourceNotifyList = &PDUSessionResourceNotifyIEs->value.choice.PDUSessionResourceNotifyList;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(AMF_UE_NGAP_ID, return, );
    d_assert(RAN_UE_NGAP_ID, return, );

    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);

    NGAP_PDUSessionResourceNotifyItemIEs_t *PDUSessionResourceNotifyItemIEs = NULL;
    for(i = 0; i < PDUSessionResourceNotifyList->list.count ; i++)
    {
        PDUSessionResourceNotifyItemIEs = (NGAP_PDUSessionResourceNotifyItemIEs_t *) PDUSessionResourceNotifyList->list.array[i];
        NGAP_PDUSessionResourceNotifyItem_t	*PDUSessionResourceNotifyItem = &PDUSessionResourceNotifyItemIEs->value.choice.PDUSessionResourceNotifyItem;
        ran_ue->amf_ue->psi = PDUSessionResourceNotifyItem->pDUSessionID;

        //TODO: 
        // ngap_send_amf_status_transfer(ran_ue,  PDUSessionResourceNotifyItem->pDUSessionResourceNotifyTransfer);
    }

}


/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_uplink_nas_transport(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UplinkNASTransport_t *UplinkNASTransport = NULL;

    NGAP_UplinkNASTransport_IEs_t *UplinkNASTransport_IEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_NAS_PDU_t *NAS_PDU = NULL;
        NGAP_UserLocationInformation_t *UserLocationInformation = NULL;

    ran_ue_t *ran_ue = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UplinkNASTransport = &initiatingMessage->value.choice.UplinkNASTransport;
    d_assert(UplinkNASTransport, return,);

    d_trace(3, "[AMF] Uplink Nas Transport\n");

    for(i = 0; i < UplinkNASTransport->protocolIEs.list.count; i++)
    {
        UplinkNASTransport_IEs = UplinkNASTransport->protocolIEs.list.array[i];
        switch(UplinkNASTransport_IEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &UplinkNASTransport_IEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &UplinkNASTransport_IEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_NAS_PDU:
                NAS_PDU = &UplinkNASTransport_IEs->value.choice.NAS_PDU;
                break;
            case NGAP_ProtocolIE_ID_id_UserLocationInformation:
                UserLocationInformation = &UplinkNASTransport_IEs->value.choice.UserLocationInformation;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(AMF_UE_NGAP_ID, return, );
    d_assert(RAN_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[RAN_UE_NGAP_ID:%d]", *RAN_UE_NGAP_ID);

    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n", ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);
#if 0            
    //TODO: NAS Handle
    d_assert(ngap_send_to_nas(ran_ue,
        NGAP_ProcedureCode_id_NASNonDeliveryIndication, NAS_PDU) == CORE_OK,,
        "ngap_send_to_nas failed"); 
#endif
    d_assert(NAS_PDU, return,);

    switch(UserLocationInformation->present)
    {
        case NGAP_UserLocationInformation_PR_userLocationInformationEUTRA:            
            d_assert(UserLocationInformation->choice.userLocationInformationEUTRA, return,);
            // UserLocationInformation->choice.userLocationInformationEUTRA;->tAI
            // UserLocationInformation->choice.userLocationInformationEUTRA;->eUTRA_CGI.eUTRACellIdentity
            // UserLocationInformation->choice.userLocationInformationEUTRA;->eUTRA_CGI.pLMNIdentity
            break;
	    case NGAP_UserLocationInformation_PR_userLocationInformationNR:            
            d_assert(UserLocationInformation->choice.userLocationInformationNR, return,);
            // UserLocationInformation->choice.userLocationInformationNR->tAI
            // UserLocationInformation->choice.userLocationInformationNR->nR_CGI.pLMNIdentity
            //  UserLocationInformation->choice.userLocationInformationNR->nR_CGI.nRCellIdentity.
            break;
        case NGAP_UserLocationInformation_PR_userLocationInformationN3IWF:
            d_assert(UserLocationInformation->choice.userLocationInformationN3IWF, return,);
            // userLocationInformationN3IWF = &UserLocationInformation->choice.userLocationInformationN3IWF->iPAddress.
            // userLocationInformationN3IWF = &UserLocationInformation->choice.userLocationInformationN3IWF->portNumber
            break;
        case NGAP_UserLocationInformation_PR_NOTHING:
            break;
    }
    d_assert(UserLocationInformation, return,);
   
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_uplink_ue_associated_nrppa_transport(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UplinkUEAssociatedNRPPaTransport_t *UplinkUEAssociatedNRPPaTransport = NULL;

    NGAP_UplinkUEAssociatedNRPPaTransportIEs_t *UplinkUEAssociatedNRPPaTransportIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_RoutingID_t *RoutingID = NULL;
        NGAP_NRPPa_PDU_t *NRPPa_PDU = NULL;
    
    ran_ue_t *ran_ue = NULL;
    
    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UplinkUEAssociatedNRPPaTransport = &initiatingMessage->value.choice.UplinkUEAssociatedNRPPaTransport;
    d_assert(UplinkUEAssociatedNRPPaTransport, return,);

    d_trace(3, "[AMF] Upline UE Associated NRPPa Transport \n");

    for(i = 0; i < UplinkUEAssociatedNRPPaTransport->protocolIEs.list.count; i++)
    {
        UplinkUEAssociatedNRPPaTransportIEs = UplinkUEAssociatedNRPPaTransport->protocolIEs.list.array[i];
        switch(UplinkUEAssociatedNRPPaTransportIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &UplinkUEAssociatedNRPPaTransportIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &UplinkUEAssociatedNRPPaTransportIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RoutingID:
                RoutingID = &UplinkUEAssociatedNRPPaTransportIEs->value.choice.RoutingID;
                break;
            case NGAP_ProtocolIE_ID_id_NRPPa_PDU:
                NRPPa_PDU = &UplinkUEAssociatedNRPPaTransportIEs->value.choice.NRPPa_PDU;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(AMF_UE_NGAP_ID, return, );
    d_assert(RAN_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[RAN_UE_NGAP_ID:%d]", *RAN_UE_NGAP_ID);

    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n", ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);
    
    d_assert(RoutingID, return, );
    memcpy(&ran_ue->amf_ue->guti_5g.amf_rid ,RoutingID->buf, RoutingID->size);
    d_assert(NRPPa_PDU, return, );
    
}


/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_uplink_non_ue_associated_nrppa_transport(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UplinkNonUEAssociatedNRPPaTransport_t *UplinkNonUEAssociatedNRPPaTransport = NULL;

    NGAP_UplinkNonUEAssociatedNRPPaTransportIEs_t *UplinkNonUEAssociatedNRPPaTransportIEs = NULL;
        NGAP_RoutingID_t *RoutingID = NULL;
        NGAP_NRPPa_PDU_t *NRPPa_PDU = NULL;
    
    ran_ue_t *ran_ue = NULL;
     d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UplinkNonUEAssociatedNRPPaTransport = &initiatingMessage->value.choice.UplinkNonUEAssociatedNRPPaTransport;
    d_assert(UplinkNonUEAssociatedNRPPaTransport, return,);

    d_trace(3, "[AMF] Uplink Non UE Associated NRPPa Transport \n");

    for(i = 0; i < UplinkNonUEAssociatedNRPPaTransport->protocolIEs.list.count; i++)
    {
        UplinkNonUEAssociatedNRPPaTransportIEs = UplinkNonUEAssociatedNRPPaTransport->protocolIEs.list.array[i];
        switch(UplinkNonUEAssociatedNRPPaTransportIEs->id)
        {            
            case NGAP_ProtocolIE_ID_id_RoutingID:
                RoutingID = &UplinkNonUEAssociatedNRPPaTransportIEs->value.choice.RoutingID;
                break;
            case NGAP_ProtocolIE_ID_id_NRPPa_PDU:
                NRPPa_PDU = &UplinkNonUEAssociatedNRPPaTransportIEs->value.choice.NRPPa_PDU;
                break;
            default:
                break;
        }
    }

    d_assert(RoutingID, return, );
    memcpy(&ran_ue->amf_ue->guti_5g.amf_rid ,RoutingID->buf, RoutingID->size);
    d_assert(NRPPa_PDU, return, );
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_location_reporting_failure_indication(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    char buf[CORE_ADDRSTRLEN];    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_LocationReportingFailureIndication_t *LocationReportingFailureIndication = NULL;

    NGAP_LocationReportingFailureIndicationIEs_t *LocationReportingFailureIndicationIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_Cause_t *Cause = NULL;

    ran_ue_t *ran_ue = NULL;
    
    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    LocationReportingFailureIndication = &initiatingMessage->value.choice.LocationReportingFailureIndication;
    d_assert(LocationReportingFailureIndication, return,);

    d_trace(3, "[AMF] Location Reporting Failure Indication \n");

    for(i = 0; i < LocationReportingFailureIndication->protocolIEs.list.count; i++)
    {
        LocationReportingFailureIndicationIEs = LocationReportingFailureIndication->protocolIEs.list.array[i];
        switch(LocationReportingFailureIndicationIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &LocationReportingFailureIndicationIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &LocationReportingFailureIndicationIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_Cause:
                Cause = &LocationReportingFailureIndicationIEs->value.choice.Cause;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(AMF_UE_NGAP_ID, return, );
    d_assert(RAN_UE_NGAP_ID, return, );
    ran_ue = ran_ue_find_by_ran_ue_ngap_id(ran, *RAN_UE_NGAP_ID);
    d_assert(ran_ue, return, "No UE Context[RAN_UE_NGAP_ID:%d]", *RAN_UE_NGAP_ID);

    d_trace(5, "    RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n", ran_ue->ran_ue_ngap_id, ran_ue->amf_ue_ngap_id);
    
    d_assert(Cause, return,);
    d_trace(5, "    Cause[Group:%d Cause:%d]\n",
            Cause->present, Cause->choice.radioNetwork);
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_location_report(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
    int j = 0;
    char buf[CORE_ADDRSTRLEN];    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_LocationReport_t *LocationReport = NULL;

    NGAP_LocationReportIEs_t *LocationReportIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_UserLocationInformation_t *UserLocationInformation = NULL;
        NGAP_LocationReportingRequestType_t	*LocationReportingRequestType = NULL;
#if 0 
    NGAP_TimeStamp_t	 TimeStamp;
    NGAP_UEPresenceInAreaOfInterestList_t	 UEPresenceInAreaOfInterestList;
#endif
    // ran_ue_t *ran_ue = NULL;
    
    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    LocationReport = &initiatingMessage->value.choice.LocationReport;
    d_assert(LocationReport, return,);

    d_trace(3, "[AMF] Location Report \n");

    for(i = 0; i < LocationReport->protocolIEs.list.count; i++)
    {
        LocationReportIEs = LocationReport->protocolIEs.list.array[i];
        switch(LocationReportIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &LocationReportIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &LocationReportIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_UserLocationInformation:
                UserLocationInformation = &LocationReportIEs->value.choice.UserLocationInformation;
                break;
            case NGAP_ProtocolIE_ID_id_LocationReportingRequestType:
                LocationReportingRequestType = &LocationReportIEs->value.choice.LocationReportingRequestType;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(AMF_UE_NGAP_ID, return, );
    d_assert(RAN_UE_NGAP_ID, return, );


    switch(UserLocationInformation->present)
    {
        case NGAP_UserLocationInformation_PR_userLocationInformationEUTRA:            
            d_assert(UserLocationInformation->choice.userLocationInformationEUTRA, return,);
            // UserLocationInformation->choice.userLocationInformationEUTRA;->tAI
            // UserLocationInformation->choice.userLocationInformationEUTRA;->eUTRA_CGI.eUTRACellIdentity
            // UserLocationInformation->choice.userLocationInformationEUTRA;->eUTRA_CGI.pLMNIdentity
            break;
	    case NGAP_UserLocationInformation_PR_userLocationInformationNR:            
            d_assert(UserLocationInformation->choice.userLocationInformationNR, return,);
            // UserLocationInformation->choice.userLocationInformationNR->tAI
            // UserLocationInformation->choice.userLocationInformationNR->nR_CGI.pLMNIdentity
            //  UserLocationInformation->choice.userLocationInformationNR->nR_CGI.nRCellIdentity.
            break;
        case NGAP_UserLocationInformation_PR_userLocationInformationN3IWF:
            d_assert(UserLocationInformation->choice.userLocationInformationN3IWF, return,);
            // userLocationInformationN3IWF = &UserLocationInformation->choice.userLocationInformationN3IWF->iPAddress.
            // userLocationInformationN3IWF = &UserLocationInformation->choice.userLocationInformationN3IWF->portNumber
            break;
        case NGAP_UserLocationInformation_PR_NOTHING:
            break;
    }
    d_assert(UserLocationInformation, return,);

    // LocationReportingRequestType->eventType
    //  LocationReportingRequestType->reportArea
    for (i = 0; i < LocationReportingRequestType->areaOfInterestList->list.count; i++)
    {
        NGAP_AreaOfInterestItem_t *AreaOfInterestItem = NULL;
        AreaOfInterestItem = LocationReportingRequestType->areaOfInterestList->list.array[i];
        NGAP_AreaOfInterest_t *NGAP_AreaOfInterest = NULL;
        NGAP_AreaOfInterest = &AreaOfInterestItem->areaOfInterest;
        for ( j = 0 ; j < NGAP_AreaOfInterest->areaOfInterestTAIList.list.count; j++)
        {
            NGAP_AreaOfInterestTAIItem_t *AreaOfInterestTAIItem = NULL;
            AreaOfInterestTAIItem = NGAP_AreaOfInterest->areaOfInterestTAIList.list.array[j];
            //TODO: 
            // AreaOfInterestTAIItem->tAI.pLMNIdentity
            // AreaOfInterestTAIItem->tAI.tAC
            d_assert(AreaOfInterestTAIItem, return, );
        }
    }
}


/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_ue_capability_info_indication(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;

    char buf[CORE_ADDRSTRLEN];    
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UECapabilityInfoIndication_t *UECapabilityInfoIndication = NULL;

    NGAP_UECapabilityInfoIndicationIEs_t *UECapabilityInfoIndicationIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_UERadioCapability_t *UERadioCapability = NULL;
#if 0
NGAP_UERadioCapabilityForPaging_t	 UERadioCapabilityForPaging;
#endif

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UECapabilityInfoIndication = &initiatingMessage->value.choice.UECapabilityInfoIndication;
    d_assert(UECapabilityInfoIndication, return,);

    d_trace(3, "[AMF] UE Capability Info Indication \n");

    for(i = 0; i < UECapabilityInfoIndication->protocolIEs.list.count; i++)
    {
        UECapabilityInfoIndicationIEs = UECapabilityInfoIndication->protocolIEs.list.array[i];
        switch(UECapabilityInfoIndicationIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID:
                AMF_UE_NGAP_ID = &UECapabilityInfoIndicationIEs->value.choice.AMF_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID:
                RAN_UE_NGAP_ID = &UECapabilityInfoIndicationIEs->value.choice.RAN_UE_NGAP_ID;
                break;
            case NGAP_ProtocolIE_ID_id_UERadioCapability:
                UERadioCapability = &UECapabilityInfoIndicationIEs->value.choice.UERadioCapability;
                break;
            default:
                break;
        }
    }

    switch(ran->ran_id.ran_present) 
    {
        case RAN_PR_GNB_ID:
        d_trace(5, "    IP[%s] gnb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.gnb_id);
            break;
        case RAN_PR_NgENB_ID:
        d_trace(5, "    IP[%s] ngenb_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.ngenb_id);
            break;
        case RAN_PR_N3IWF_ID:
        d_trace(5, "    IP[%s] n3iwf_id[%d]\n",
            CORE_ADDR(ran->addr, buf), ran->ran_id.n3iwf_id);
            break;
    }
    d_assert(AMF_UE_NGAP_ID, return, );
    d_assert(RAN_UE_NGAP_ID, return, );
    d_assert(UERadioCapability, return, );
    
}

/**
 * NG-RAN node -> AMF
 * */
void ngap_handle_uplink_ran_configuration_transfer(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;

    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UplinkRANConfigurationTransfer_t *UplinkRANConfigurationTransfer =  NULL;

    NGAP_UplinkRANConfigurationTransferIEs_t *UplinkRANConfigurationTransferIEs = NULL;
        NGAP_SONConfigurationTransfer_t	 *SONConfigurationTransfer = NULL;

    d_assert(ran, return,);
    d_assert(ran->sock, return,);

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);
    UplinkRANConfigurationTransfer = &initiatingMessage->value.choice.UplinkRANConfigurationTransfer;
    d_assert(UplinkRANConfigurationTransfer, return,);

    d_trace(3, "[AMF] Uplink RAN Configuration Transfer \n");

    for(i = 0; i < UplinkRANConfigurationTransfer->protocolIEs.list.count; i++)
    {
        UplinkRANConfigurationTransferIEs = UplinkRANConfigurationTransfer->protocolIEs.list.array[i];
        switch(UplinkRANConfigurationTransferIEs->id)
        {
            case NGAP_ProtocolIE_ID_id_SONConfigurationTransferUL:
                SONConfigurationTransfer = &UplinkRANConfigurationTransferIEs->value.choice.SONConfigurationTransfer;
                break;
            default:
                break;
        }
    }
     d_assert(SONConfigurationTransfer, return,);
}