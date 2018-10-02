#include "core_debug.h"

/* TODO: remove 3gpp_type.h head file, until ngap_conv.c done*/
#include "3gpp_types.h"

#include "ngap_conv.h"
#include "ngap_handler.h"

void ngap_handle_ng_setup_request(amf_ran_t *ran, ngap_message_t *message)
{
    int i = 0;
                              
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_NGSetupRequest_t *NGSetupRequest = NULL;

    NGAP_NGSetupRequestIEs_t *ie = NULL;
    NGAP_GlobalRANNodeID_t *Global_RAN_Node_ID = NULL;
    NGAP_SupportedTAList_t *SupportedTAList = NULL;
	// NGAP_RANNodeName_t	 *RANNodeName = NULL; //Optional
	NGAP_PagingDRX_t *PagingDRX = NULL;

    d_assert(message, return,);
    initiatingMessage = message->choice.initiatingMessage;
    d_assert(initiatingMessage, return,);

    NGSetupRequest = &initiatingMessage->value.choice.NGSetupRequest;
    d_assert(NGSetupRequest, return,);

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
    
}