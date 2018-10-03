#define TRACE_MODULE _ngap_handler
#include "core_debug.h"

/* TODO: remove 3gpp_type.h head file, until ngap_conv.c done*/
#include "3gpp_types.h"

#include "ngap_conv.h"
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

    //c_uint32_t gnb_id = 0;

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



}