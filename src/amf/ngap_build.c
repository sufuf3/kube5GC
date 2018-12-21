#define TRACE_MODULE _ngap_build

#include "core_debug.h"

#include "3gpp_types.h"
#include "ngap_conv.h"
#include "ngap_build.h"

status_t ngap_build_setup_rsp(pkbuf_t **pkbuf)
{
    status_t rv;
    int i = 0;
    int j = 0;

    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_NGSetupResponse_t *NGSetupResponse = NULL;
        NGAP_NGSetupResponseIEs_t *ie = NULL;
        NGAP_AMFName_t *AMFName = NULL;
        NGAP_ServedGUAMIList_t *ServedGUAMIList = NULL;
        NGAP_RelativeAMFCapacity_t *RelativeAMFCapacity = NULL;
        NGAP_PLMNSupportList_t *PLMNSupportList = NULL;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = NGAP_ProcedureCode_id_NGSetup;
    successfulOutcome->criticality = NGAP_Criticality_reject;
    successfulOutcome->value.present =
        NGAP_SuccessfulOutcome__value_PR_NGSetupResponse;

    NGSetupResponse = &successfulOutcome->value.choice.NGSetupResponse;
    d_trace(1, "NGSetupResponse=%d",NGSetupResponse);

    ie = core_calloc(1, sizeof(NGAP_NGSetupResponseIEs_t));
    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMFName;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_AMFName;
    AMFName = &ie->value.choice.AMFName;
#if 1 //TODO fix it 
    char tmp3[3] = {0x41, 0x42, 0x43};
    ngap_buffer_to_OCTET_STRING(tmp3, 3, AMFName);

#endif

    ie = core_calloc(1, sizeof(NGAP_NGSetupResponseIEs_t));
    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_ServedGUAMIList;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_ServedGUAMIList;
    ServedGUAMIList = &ie->value.choice.ServedGUAMIList;

    ie = core_calloc(1, sizeof(NGAP_NGSetupResponseIEs_t));
    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RelativeAMFCapacity;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_RelativeAMFCapacity;
    RelativeAMFCapacity = &ie->value.choice.RelativeAMFCapacity;
    *RelativeAMFCapacity = amf4g_self()->relative_capacity;

    ie = core_calloc(1, sizeof(NGAP_NGSetupResponseIEs_t));
    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PLMNSupportList;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_PLMNSupportList;
    PLMNSupportList = &ie->value.choice.PLMNSupportList;

    //TODO: fix it
    for (i = 0; i < 1/*amf4g_self()->max_num_of_served_guami*/; i++)
    {   
        NGAP_ServedGUAMIItem_t *ServedGUAMIItem = NULL;
        ServedGUAMIItem = core_calloc(1, sizeof(NGAP_ServedGUAMIItem_t));
         //TODO: fix it
        // served_guami_t *served_guami = &amf4g_self()->served_guami[i];
        //for (j = 0; j < served_guami->num_of_plmn_id, j++)
        //{
       	
            //TODO: fix it, use gUAMI pLMNIdentity          
            ngap_buffer_to_OCTET_STRING(tmp3, 3, &ServedGUAMIItem->gUAMI.pLMNIdentity);
            
            //TODO: fix it, use gUAMI aMFRegionID   
            char tmp2[2] = {0x01, 0x02};
            ngap_buffer_to_OCTET_STRING(tmp2, 2, &ServedGUAMIItem->gUAMI.aMFRegionID);
            
            // //TODO: fix it,  use gUAMI aMFSetID   
            char tmp4[4] = {0x01,0x02,0x03,0x04};
            ServedGUAMIItem->gUAMI.aMFSetID.size = 1;
            ServedGUAMIItem->gUAMI.aMFSetID.bits_unused = 4;
            ServedGUAMIItem->gUAMI.aMFSetID.buf = core_calloc(1, sizeof(c_uint8_t));
            memcpy(ServedGUAMIItem->gUAMI.aMFSetID.buf, tmp4, 1);
            

            // //TODO: fix it,  use gUAMI aMFPointer   
            ServedGUAMIItem->gUAMI.aMFPointer.size = 1;
            ServedGUAMIItem->gUAMI.aMFPointer.bits_unused = 4;
            ServedGUAMIItem->gUAMI.aMFPointer.buf = core_calloc(1, sizeof(c_uint8_t));
            memcpy(ServedGUAMIItem->gUAMI.aMFPointer.buf, tmp4, 1);

        ASN_SEQUENCE_ADD(&ServedGUAMIList->list, ServedGUAMIItem);
    }


    //TODO fix it : use real counter
    for (i = 0; i < 1/*amf4g_self()->max_num_of_plmn_support*/; i++)
    {
        NGAP_PLMNSupportItem_t *PLMNSupportItem = NULL;
        PLMNSupportItem = (NGAP_PLMNSupportItem_t *) core_calloc(1, sizeof(NGAP_PLMNSupportItem_t));

        // plmn_support_t *plmn_support = &amf4g_self()->plmn_support[i];
        plmn_id_t plmn_id;
        plmn_id_build(&plmn_id, 1, 1, 2);
        ngap_buffer_to_OCTET_STRING(&plmn_id, 3, &PLMNSupportItem->pLMNIdentity);
        //TODO fix it: use real counter
        for (j = 0; j < 1/*amf4g_self()->plmn_support->num_of_s_nssai*/; j++)
        {         
            NGAP_SliceSupportItem_t *SliceSupportItem = NULL;
            SliceSupportItem = (NGAP_SliceSupportItem_t *) core_calloc(1, sizeof(NGAP_SliceSupportItem_t));
            //TODO: fix it,  use s_NSSAI 
            char tmp1[1] = {0x12};
            ngap_buffer_to_OCTET_STRING(tmp1, 1, &SliceSupportItem->s_NSSAI.sST);
            ngap_buffer_to_OCTET_STRING(tmp3, 3, &SliceSupportItem->s_NSSAI.sD);
            // ngap_buffer_to_OCTET_STRING(&plmn_support->s_nssai[j].sst, SST_LEN, &SliceSupportItem->s_NSSAI.sST);
           
           ASN_SEQUENCE_ADD(&PLMNSupportItem->sliceSupportList.list, SliceSupportItem);
        }
        ASN_SEQUENCE_ADD(&PLMNSupportList->list, PLMNSupportItem);
    }

    rv = ngap_encode_pdu(pkbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}


status_t ngap_build_setup_failure(pkbuf_t **pkbuf, NGAP_Cause_PR group, long cause, long time_to_wait)
{
    return CORE_OK;
}

/** 
 *  Direction AMF -> NG-RAN Node
 **/
status_t ngap_build_initial_context_setup_request(
            pkbuf_t **ngapbuf, amf_ue_t *amf_ue, pkbuf_t *naspdubuf)
{
    int i = 0;
    status_t rv;
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_InitialContextSetupRequest_t *InitialContextSetupRequest = NULL;

    NGAP_InitialContextSetupRequestIEs_t *ie = NULL;

    NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
	NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
    NGAP_GUAMI_t *GUAMI = NULL;
    NGAP_PDUSessionResourceSetupListCxtReq_t *PDUSessionResourceSetupListCxtReq = NULL;
    NGAP_AllowedNSSAI_t	*AllowedNSSAI = NULL;
    NGAP_UESecurityCapabilities_t *UESecurityCapabilities = NULL;
	NGAP_SecurityKey_t *SecurityKey = NULL;

    ran_ue_t *ran_ue = NULL;

    d_assert(amf_ue, return CORE_ERROR, "Null param");
    ran_ue = amf_ue->ran_ue;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));
    
    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        NGAP_ProcedureCode_id_InitialContextSetup;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present =
        NGAP_InitiatingMessage__value_PR_InitialContextSetupRequest;

    InitialContextSetupRequest = 
        &initiatingMessage->value.choice.InitialContextSetupRequest;
    
    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_InitialContextSetupRequestIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_InitialContextSetupRequestIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_GUAMI;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_InitialContextSetupRequestIEs__value_PR_GUAMI;
    GUAMI = &ie->value.choice.GUAMI;
    /* which guami plmn/rid/ should be selete ?*/
    served_guami_t *served_guami = &amf4g_self()->served_guami[0];
    ngap_buffer_to_OCTET_STRING( &served_guami->plmn_id[0], 3, &GUAMI->pLMNIdentity);
    ngap_buffer_to_OCTET_STRING( &served_guami->amf_rid[0], 3, &GUAMI->aMFRegionID);
	//ngap_buffer_to_OCTET_STRING( &served_guami->amf_sid[0], 3, &GUAMI->aMFSetID);  //BIT_STRING_t
	// ngap_buffer_to_OCTET_STRING( &served_guami->amf_ptr[0], 3, &GUAMI->aMFPointer); //BIT_STRING_t

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupListCxtReq;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_InitialContextSetupRequestIEs__value_PR_PDUSessionResourceSetupListCxtReq;
    PDUSessionResourceSetupListCxtReq = &ie->value.choice.PDUSessionResourceSetupListCxtReq;
    /* TODO: max number of PDU Sesssions*/
    for (i = 0; i <  1 ; i++)
    {
        NGAP_PDUSessionResourceSetupItemCxtReqIEs_t *PDUSessionResourceSetupItemCxtReqIEs = NULL;
        PDUSessionResourceSetupItemCxtReqIEs = (NGAP_PDUSessionResourceSetupItemCxtReqIEs_t *) core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupItemCxtReqIEs_t));
        NGAP_PDUSessionResourceSetupItemCxtReq_t *PDUSessionResourceSetupItemCxtReq =
                &PDUSessionResourceSetupItemCxtReqIEs->value.choice.PDUSessionResourceSetupItemCxtReq;
        PDUSessionResourceSetupItemCxtReqIEs->criticality = NGAP_Criticality_reject;
        PDUSessionResourceSetupItemCxtReq->pDUSessionID = ran_ue->amf_ue->psi;
        /* TODO: add sst value, slice service Type*/
        // ngap_buffer_to_OCTET_STRING( 1, 1, &PDUSessionResourceSetupItemCxtReq->s_NSSAI.sST);
        // NGAP_PDUSessionResourceSetupRequestTransfer_t *PDUSessionResourceSetupRequestTransfer = NULL;
        // PDUSessionResourceSetupRequestTransfer = &PDUSessionResourceSetupItemCxtReq->pDUSessionResourceSetupRequestTransfer;
            // asn_uint642INTEGER((INTEGER_t *)&PDUSessionResourceSetupRequestTransfer->pDUSessionAggregateMaximumBitRate,
            //                     amf_ue->pdn->qos->mbr);
            // NGAP_BitRate_t	 pDUSessionAggregateMaximumBitRate;
            // NGAP_UPTransportLayerInformation_t	 uL_NGU_UP_TNLInformation;
            // NGAP_PDUSessionType_t	 pDUSessionType;
            // NGAP_QosFlowSetupRequestList_t	 qosFlowSetupRequestList;
        ASN_SEQUENCE_ADD(&PDUSessionResourceSetupListCxtReq->list, PDUSessionResourceSetupItemCxtReqIEs);
    }
    
    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AllowedNSSAI;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_InitialContextSetupRequestIEs__value_PR_AllowedNSSAI;
    AllowedNSSAI = &ie->value.choice.AllowedNSSAI;
    NGAP_AllowedNSSAI_Item_t *AllowedNSSAI_Item = NULL;
    for (i = 0 ; i < amf4g_self()->plmn_support->num_of_s_nssai; i++)
    {
        plmn_support_t *plmn_support = &amf4g_self()->plmn_support[i];
        AllowedNSSAI_Item = (NGAP_AllowedNSSAI_Item_t *) core_calloc(1, sizeof(NGAP_AllowedNSSAI_Item_t));
        ngap_buffer_to_OCTET_STRING(&plmn_support->s_nssai[i].sst, SST_LEN, &AllowedNSSAI_Item->s_NSSAI.sST);
        ASN_SEQUENCE_ADD(&AllowedNSSAI->list, AllowedNSSAI_Item);
    }       

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_UESecurityCapabilities;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_InitialContextSetupRequestIEs__value_PR_UESecurityCapabilities;
    UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;
	
    //nas_ue_network_capability_t ue_network_capability
    memcpy(&UESecurityCapabilities->nRencryptionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
    memcpy(&UESecurityCapabilities->nRintegrityProtectionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
    memcpy(&UESecurityCapabilities->eUTRAencryptionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
    memcpy(&UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
	
    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_SecurityKey;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_InitialContextSetupRequestIEs__value_PR_SecurityKey;
    SecurityKey = &ie->value.choice.SecurityKey;
  
    SecurityKey->size = SHA256_DIGEST_SIZE;
    SecurityKey->buf = 
        core_calloc(SecurityKey->size, sizeof(c_uint8_t));
    SecurityKey->bits_unused = 0;
    memcpy(SecurityKey->buf, "\xff\xff\xff\xff\xff\xff\xff\xff", SecurityKey->size); //NG-RAN key

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }


    return rv;
}

/**
 * AMF -> NG-RAN node
 **/
status_t ngap_build_pdu_session_resource_setup_request(pkbuf_t **ngapbuf, amf_ue_t *amf_ue)
{
    int i = 0;
    status_t rv = 0;

    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PDUSessionResourceSetupRequest_t *PDUSessionResourceSetupRequest = NULL;

    NGAP_PDUSessionResourceSetupRequestIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceSetupListSUReq_t *PDUSessionResourceSetupListSUReq = NULL;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_PDUSessionResourceSetup;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_PDUSessionResourceSetupRequest;
    PDUSessionResourceSetupRequest = &initiatingMessage->value.choice.PDUSessionResourceSetupRequest;
    
    ie = core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_PDUSessionResourceSetupRequestIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = amf_ue->ran_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_PDUSessionResourceSetupRequestIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = amf_ue->ran_ue->ran_ue_ngap_id;
    
    ie = core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_PDUSessionResourceSetupRequestIEs__value_PR_PDUSessionResourceSetupListSUReq;
    PDUSessionResourceSetupListSUReq = &ie->value.choice.PDUSessionResourceSetupListSUReq;

    for (i = 0 ; i < 1 ; i++)
    {
        NGAP_PDUSessionResourceSetupItemCxtReqIEs_t *PDUSessionResourceSetupItemCxtReqIEs = NULL;
        PDUSessionResourceSetupItemCxtReqIEs = (NGAP_PDUSessionResourceSetupItemCxtReqIEs_t *) core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupItemCxtReqIEs_t));
        NGAP_PDUSessionResourceSetupItemCxtReq_t *PDUSessionResourceSetupItemCxtReq =
                &PDUSessionResourceSetupItemCxtReqIEs->value.choice.PDUSessionResourceSetupItemCxtReq;
        
        PDUSessionResourceSetupItemCxtReqIEs->criticality = NGAP_Criticality_reject;
            // PDUSessionResourceSetupItemCxtReq->pDUSessionID = ran_ue->amf_ue->psi;
            // NGAP_PDUSessionID_t	 pDUSessionID;
	    ngap_buffer_to_OCTET_STRING(&amf4g_self()->plmn_support->s_nssai[0].sst, SST_LEN, &PDUSessionResourceSetupItemCxtReq->s_NSSAI.sST);
	        // OCTET_STRING_t	 pDUSessionResourceSetupRequestTransfer;
        
        ASN_SEQUENCE_ADD(&PDUSessionResourceSetupListSUReq->list, PDUSessionResourceSetupItemCxtReq);
    }

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngapbuf() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

/**
 * AMF -> NG-RAN node
 **/
status_t ngap_build_ue_context_modification_request(pkbuf_t **ngapbuf, ran_ue_t *ran_ue)
{
    status_t rv = 0;

    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UEContextModificationRequest_t *UEContextModificationRequest = NULL;

    NGAP_UEContextModificationRequestIEs_t *ie = NULL;
			NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
			NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
			// NGAP_RANPagingPriority_t	 RANPagingPriority;
			// NGAP_SecurityKey_t	 SecurityKey;
			// NGAP_IndexToRFSP_t	 IndexToRFSP;
			// NGAP_UEAggregateMaximumBitRate_t	 UEAggregateMaximumBitRate;
			// NGAP_UESecurityCapabilities_t	 UESecurityCapabilities;
			// NGAP_RRCInactiveAssistanceInformation_t	 RRCInactiveAssistanceInformation;
			// NGAP_EmergencyFallbackIndicator_t	 EmergencyFallbackIndicator;
    
    d_assert(ran_ue, return CORE_ERROR, "Null param");
    if (ran_ue->amf_ue_ngap_id == 0)
    {
        d_error("invalid amf ue ngap id (idx: %d)", ran_ue->index);
        return CORE_ERROR;
    }

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        NGAP_ProcedureCode_id_UEContextModification;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present =
        NGAP_InitiatingMessage__value_PR_UEContextModificationRequest;
    
    UEContextModificationRequest = 
        &initiatingMessage->value.choice.UEContextModificationRequest;
 
    ie = core_calloc(1, sizeof(NGAP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_UEContextModificationRequestIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_UEContextModificationRequestIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

/**
 * Direction: NG-RAN node -> AMF and AMF -> NG-RAN node
 */ 
status_t ngap_build_ng_reset(
    pkbuf_t **ngapbuf,
    NGAP_Cause_PR group, long cause,
    NGAP_UE_associatedLogicalNG_ConnectionListRes_t *partOfNG_Interface)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_NGReset_t *NGReset = NULL;

    NGAP_NGResetIEs_t *ie = NULL;
        NGAP_Cause_t *Cause = NULL;
		NGAP_ResetType_t *ResetType = NULL;
        //    NGAP_UE_associatedLogicalNG_ConnectionListRes_t *partOfNG_Interface = NULL;
    
    d_trace(3, "[AMF] Reset\n");
    
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        NGAP_ProcedureCode_id_NGReset;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present =
        NGAP_InitiatingMessage__value_PR_NGReset;
    
    NGReset = &initiatingMessage->value.choice.NGReset;

    ie = core_calloc(1, sizeof(NGAP_NGResetIEs_t));
    ASN_SEQUENCE_ADD(&NGReset->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_Cause;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_NGResetIEs__value_PR_Cause;
    Cause = &ie->value.choice.Cause;
    
    ie = core_calloc(1, sizeof(NGAP_NGResetIEs_t));
    ASN_SEQUENCE_ADD(&NGReset->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_ResetType;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_NGResetIEs__value_PR_ResetType;
    ResetType = &ie->value.choice.ResetType;

    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    d_trace(5, "    Group[%d] Cause[%d] partOfNG_Interface[%p]\n",
        Cause->present, Cause->choice.radioNetwork, partOfNG_Interface);

    if (partOfNG_Interface)
    {
        ResetType->present = NGAP_ResetType_PR_partOfNG_Interface;
        ResetType->choice.partOfNG_Interface = partOfNG_Interface;
    }
    else
    {
        ResetType->present = NGAP_ResetType_PR_nG_Interface;
        ResetType->choice.nG_Interface = NGAP_ResetAll_reset_all;
    }

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

/**
 * Direction: NG-RAN node -> AMF and AMF -> NG-RAN node
 */ 
status_t ngap_build_ng_reset_acknowledge(
    pkbuf_t **ngapbuf,
    NGAP_Cause_PR group, long cause,
    NGAP_UE_associatedLogicalNG_ConnectionListRes_t *partOfNG_Interface)
{
    status_t rv;
    int i = 0 ;

    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_NGResetAcknowledge_t *NGResetAcknowledge = NULL;

    NGAP_NGResetAcknowledgeIEs_t *ie = NULL;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = NGAP_ProcedureCode_id_NGReset;
    successfulOutcome->criticality = NGAP_Criticality_reject;
    successfulOutcome->value.present =
        NGAP_SuccessfulOutcome__value_PR_NGResetAcknowledge;
    NGResetAcknowledge = &successfulOutcome->value.choice.NGResetAcknowledge;

    if (partOfNG_Interface && partOfNG_Interface->list.count)
    {
        i = 0;
        NGAP_UE_associatedLogicalNG_ConnectionListResAck_t *list = NULL;
        ie = core_calloc(1, sizeof(NGAP_NGResetAcknowledgeIEs_t));
        ASN_SEQUENCE_ADD(&NGResetAcknowledge->protocolIEs, ie);
        
        ie->id = NGAP_ProtocolIE_ID_id_UE_associatedLogicalNG_ConnectionListResAck;
        ie->criticality = NGAP_Criticality_ignore;
        ie->value.present = NGAP_NGResetAcknowledgeIEs__value_PR_UE_associatedLogicalNG_ConnectionListResAck;
        list = &ie->value.choice.UE_associatedLogicalNG_ConnectionListResAck;

        for(i = 0 ; i < partOfNG_Interface->list.count ; i++)
        {
            NGAP_UE_associatedLogicalNG_ConnectionItemResIEs_t *ie1 = NULL;
            NGAP_UE_associatedLogicalNG_ConnectionItem_t *item1 = NULL;

            NGAP_UE_associatedLogicalNG_ConnectionItemResAck_t *ie2 = NULL;
            NGAP_UE_associatedLogicalNG_ConnectionItem_t *item2 = NULL;
            
            ie1 = (NGAP_UE_associatedLogicalNG_ConnectionItemResIEs_t *) 
                partOfNG_Interface->list.array[i];
            d_assert(ie1, return CORE_ERROR, );
            item1 = &ie1->value.choice.UE_associatedLogicalNG_ConnectionItem;
            d_assert(item1, return CORE_ERROR, );

            if (item1->aMF_UE_NGAP_ID == NULL && item1->rAN_UE_NGAP_ID == NULL)
            {
                d_warn("No AMF_UE_NGAP_ID & RAN_UE_NGAP_ID");
                continue;
            }

            ie2 = core_calloc(1, sizeof(NGAP_UE_associatedLogicalNG_ConnectionItemResAck_t));
            d_assert(ie2, return CORE_ERROR, );
            ASN_SEQUENCE_ADD(&list->list, ie2);

            ie2->id = NGAP_ProtocolIE_ID_id_UE_associatedLogicalNG_ConnectionItem;
            ie2->criticality = NGAP_Criticality_ignore;
            ie2->value.present = NGAP_UE_associatedLogicalNG_ConnectionItemResAck__value_PR_UE_associatedLogicalNG_ConnectionItem;
            item2 = &ie2->value.choice.UE_associatedLogicalNG_ConnectionItem;
            d_assert(item2, return CORE_ERROR, );

            if (item1->aMF_UE_NGAP_ID)
            {
                item2->aMF_UE_NGAP_ID = core_calloc(1, sizeof(NGAP_AMF_UE_NGAP_ID_t));
                d_assert(item2->aMF_UE_NGAP_ID, return CORE_ERROR, );
                *item2->aMF_UE_NGAP_ID = *item1->aMF_UE_NGAP_ID;
            }
            if (item1->rAN_UE_NGAP_ID)
            {
                item2->rAN_UE_NGAP_ID = core_calloc(1, sizeof(NGAP_RAN_UE_NGAP_ID_t));
                d_assert(item2->rAN_UE_NGAP_ID, return CORE_ERROR, );
                *item2->rAN_UE_NGAP_ID = *item1->rAN_UE_NGAP_ID;
            }
            d_trace(5, "    AMF_UE_NGAP_ID[%d] RAN_UE_NGAP_ID[%d]\n",
                item2->aMF_UE_NGAP_ID ? *item2->aMF_UE_NGAP_ID : -1,
                item2->rAN_UE_NGAP_ID ? *item2->rAN_UE_NGAP_ID : -1);
        }
    }
    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t ngap_build_handover_cancel_acknowledge(pkbuf_t **ngapbuf, ran_ue_t *source_ue)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_HandoverCancelAcknowledge_t *HandoverCancel = NULL;

    NGAP_HandoverCancelIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        // NGAP_Cause_t	 Cause;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));
    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = NGAP_ProcedureCode_id_HandoverCancel;
    successfulOutcome->criticality = NGAP_Criticality_reject;
    successfulOutcome->value.present =
        NGAP_SuccessfulOutcome__value_PR_HandoverCancelAcknowledge;
    HandoverCancel = &successfulOutcome->value.choice.HandoverCancelAcknowledge;

    ie = core_calloc(1, sizeof(NGAP_HandoverCancelIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCancel->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCancelAcknowledgeIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_HandoverCancelIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCancel->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCancelAcknowledgeIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = source_ue->ran_ue_ngap_id;
    
    d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            source_ue->ran_ue_ngap_id, source_ue->amf_ue_ngap_id);
  
    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

/**
 * Direction: AMF -> NG-RAN node
 **/

status_t ngap_build_ue_context_release_command(pkbuf_t **ngapbuf, ran_ue_t *ran_ue, NGAP_Cause_PR group, long cause)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UEContextReleaseCommand_t *UEContextReleaseCommand = NULL;

    NGAP_UEContextReleaseCommand_IEs_t *ie = NULL;
        NGAP_UE_NGAP_IDs_t *UE_NGAP_IDs = NULL;
        // NGAP_RANPagingPriority_t	 RANPagingPriority;
        NGAP_Cause_t *Cause = NULL;
    
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_UEContextRelease;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_UEContextReleaseCommand;
    UEContextReleaseCommand = &initiatingMessage->value.choice.UEContextReleaseCommand;
    
    ie = core_calloc(1, sizeof(NGAP_UEContextReleaseCommand_IEs_t));
    ASN_SEQUENCE_ADD(&UEContextReleaseCommand->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_UE_NGAP_IDs;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_UEContextReleaseCommand_IEs__value_PR_UE_NGAP_IDs;
    UE_NGAP_IDs = &ie->value.choice.UE_NGAP_IDs;

    if (ran_ue->ran_ue_ngap_id == INVALID_UE_NGAP_ID)
    {
        UE_NGAP_IDs->present = NGAP_UE_NGAP_IDs_PR_aMF_UE_NGAP_ID;
        UE_NGAP_IDs->choice.aMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;
    }
    else
    {
        UE_NGAP_IDs->present = NGAP_UE_NGAP_IDs_PR_uE_NGAP_ID_pair;
        UE_NGAP_IDs->choice.uE_NGAP_ID_pair = core_calloc(1, sizeof(NGAP_UE_NGAP_ID_pair_t));
        UE_NGAP_IDs->choice.uE_NGAP_ID_pair->aMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;
        UE_NGAP_IDs->choice.uE_NGAP_ID_pair->rAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;
        UE_NGAP_IDs->choice.aMF_UE_NGAP_ID =ran_ue->amf_ue_ngap_id;
    }

    ie = core_calloc(1, sizeof(NGAP_UEContextReleaseCommand_IEs_t));
    ASN_SEQUENCE_ADD(&UEContextReleaseCommand->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_Cause;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_UEContextReleaseCommand_IEs__value_PR_Cause;
    Cause = &ie->value.choice.Cause;

    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t ngap_build_handover_preparation_failure(pkbuf_t **ngapbuf, ran_ue_t *source_ue, NGAP_Cause_t *cause)
{
    status_t rv;

    NGAP_NGAP_PDU_t pdu;
    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    NGAP_HandoverPreparationFailure_t *HandoverPreparationFailure = NULL;

    NGAP_HandoverPreparationFailureIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_Cause_t *Cause = NULL;
#if 0 // optional
        NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    
    d_assert(ngapbuf, return CORE_ERROR,);
    d_assert(source_ue, return CORE_ERROR,);
    d_assert(cause, return CORE_ERROR,);

    d_trace(3, "[AMF] Handover preparation failure\n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = 
        core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));
    unsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    unsuccessfulOutcome->procedureCode = NGAP_ProcedureCode_id_HandoverPreparation;
    unsuccessfulOutcome->criticality = NGAP_Criticality_reject;
    unsuccessfulOutcome->value.present =
        NGAP_UnsuccessfulOutcome__value_PR_HandoverPreparationFailure;
    HandoverPreparationFailure = &unsuccessfulOutcome->value.choice.HandoverPreparationFailure;

    ie = core_calloc(1, sizeof(NGAP_HandoverPreparationFailureIEs_t));
    ASN_SEQUENCE_ADD(&HandoverPreparationFailure->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_HandoverCancelAcknowledgeIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_HandoverPreparationFailureIEs_t));
    ASN_SEQUENCE_ADD(&HandoverPreparationFailure->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_HandoverCancelAcknowledgeIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = source_ue->ran_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_HandoverPreparationFailureIEs_t));
    ASN_SEQUENCE_ADD(&HandoverPreparationFailure->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_Cause;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_HandoverCancelAcknowledgeIEs__value_PR_CriticalityDiagnostics;
    Cause = &ie->value.choice.Cause;

    Cause->present = cause->present;
    Cause->choice.radioNetwork = cause->choice.radioNetwork;

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ng1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;  
}

/**
 * Direction: AMF -> NG-RAN node
 **/
CORE_DECLARE(status_t) ngap_build_handover_request(
            pkbuf_t **ngapbuf, amf_ue_t *amf_ue, ran_ue_t *target_ue,
            NGAP_RAN_UE_NGAP_ID_t *ran_ue_ngap_id, NGAP_AMF_UE_NGAP_ID_t *amf_ue_ngap_id,
            NGAP_HandoverType_t *handovertype, NGAP_Cause_t *cause,
            NGAP_SourceToTarget_TransparentContainer_t *sourceToTarget_TransparentContainer)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_HandoverRequest_t *HandoverRequest = NULL;

    NGAP_HandoverRequestIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_HandoverType_t *HandoverType = NULL;
		NGAP_Cause_t *Cause = NULL;
		NGAP_UEAggregateMaximumBitRate_t *UEAggregateMaximumBitRate = NULL;
		NGAP_UESecurityCapabilities_t *UESecurityCapabilities = NULL;
		NGAP_SecurityContext_t *SecurityContext = NULL;
        NGAP_PDUSessionResourceSetupListHOReq_t *PDUSessionResourceSetupListHOReq = NULL;
		NGAP_SourceToTarget_TransparentContainer_t *SourceToTarget_TransparentContainer = NULL;
#if 0 
        NGAP_RRCInactiveAssistanceInformation_t	 RRCInactiveAssistanceInformation;
        NGAP_KamfChangeInd_t	 KamfChangeInd;
        NGAP_NAS_PDU_t	 NAS_PDU;
        NGAP_TraceActivation_t	 TraceActivation;
        NGAP_MaskedIMEISV_t	 MaskedIMEISV;
        NGAP_MobilityRestrictionList_t	 MobilityRestrictionList;
        NGAP_LocationReportingRequestType_t	 LocationReportingRequestType;
#endif
    d_assert(handovertype, return CORE_ERROR,);
    d_assert(cause, return CORE_ERROR,);
    d_assert(sourceToTarget_TransparentContainer, return CORE_ERROR,);
    d_assert(target_ue, return CORE_ERROR, "Null param");
    d_assert(amf_ue, return CORE_ERROR, "Null param");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_HandoverResourceAllocation;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_HandoverRequest;
    HandoverRequest = &initiatingMessage->value.choice.HandoverRequest;
        
    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = target_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_HandoverType;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_HandoverType;
    HandoverType = &ie->value.choice.HandoverType;
    *HandoverType = *handovertype;

    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_Cause;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_Cause;
    Cause = &ie->value.choice.Cause;
    Cause->present = cause->present;
    Cause->choice.radioNetwork = cause->choice.radioNetwork;

    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_UEAggregateMaximumBitRate;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_UEAggregateMaximumBitRate;
    UEAggregateMaximumBitRate = &ie->value.choice.UEAggregateMaximumBitRate;
    //TODO: fix it    
    asn_uint642INTEGER(&UEAggregateMaximumBitRate->uEAggregateMaximumBitRateUL, 0x1234);
    asn_uint642INTEGER(&UEAggregateMaximumBitRate->uEAggregateMaximumBitRateDL, 0x1234);

    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_UESecurityCapabilities;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_UESecurityCapabilities;
    UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;
        //TODO: fix it    
        UESecurityCapabilities->eUTRAencryptionAlgorithms.size = 2;
        UESecurityCapabilities->eUTRAencryptionAlgorithms.bits_unused = 0;
        UESecurityCapabilities->eUTRAencryptionAlgorithms.buf = 
            core_calloc(UESecurityCapabilities->eUTRAencryptionAlgorithms.size, sizeof(c_uint8_t));
        UESecurityCapabilities->eUTRAencryptionAlgorithms.buf[0] = 0;
            //(amf->ue_network_capability.eea << 1);
        UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms.size = 2;
        UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms.bits_unused = 0;
        UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms.buf = 
            core_calloc(UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms.size, sizeof(c_uint8_t));
        UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms.buf[0] = 0;

        UESecurityCapabilities->nRencryptionAlgorithms.size = 2;
        UESecurityCapabilities->nRencryptionAlgorithms.bits_unused = 0;
        UESecurityCapabilities->nRencryptionAlgorithms.buf = 
            core_calloc(UESecurityCapabilities->nRencryptionAlgorithms.size, sizeof(c_uint8_t));
        UESecurityCapabilities->nRencryptionAlgorithms.buf[0] = 0;

        UESecurityCapabilities->nRintegrityProtectionAlgorithms.size = 2;
        UESecurityCapabilities->nRintegrityProtectionAlgorithms.bits_unused = 0;
        UESecurityCapabilities->nRintegrityProtectionAlgorithms.buf = 
            core_calloc(UESecurityCapabilities->nRintegrityProtectionAlgorithms.size, sizeof(c_uint8_t));
        UESecurityCapabilities->nRintegrityProtectionAlgorithms.buf[0] = 0;

    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_SecurityContext;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_SecurityContext;
    SecurityContext = &ie->value.choice.SecurityContext;
        //TODO: fix it  
        SecurityContext->nextHopChainingCount = 1;//amf_ue->nhcc;
        SecurityContext->nextHopNH.size = SHA256_DIGEST_SIZE;
        SecurityContext->nextHopNH.buf = 
            core_calloc(SecurityContext->nextHopNH.size,
            sizeof(c_uint8_t));
        SecurityContext->nextHopNH.bits_unused = 0;
        memcpy(SecurityContext->nextHopNH.buf,
             "\x00", SecurityContext->nextHopNH.size);

    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupListHOReq;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_PDUSessionResourceSetupListHOReq;
    PDUSessionResourceSetupListHOReq = &ie->value.choice.PDUSessionResourceSetupListHOReq;
        NGAP_PDUSessionResourceSetupItemHOReqIEs_t *PDUSessionResourceSetupItemHOReqIEs = NULL;
        PDUSessionResourceSetupItemHOReqIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupItemHOReqIEs_t));
        ASN_SEQUENCE_ADD(&PDUSessionResourceSetupListHOReq->list, PDUSessionResourceSetupItemHOReqIEs);
        PDUSessionResourceSetupItemHOReqIEs->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupItemHOReq;
        PDUSessionResourceSetupItemHOReqIEs->criticality = NGAP_Criticality_reject;
        PDUSessionResourceSetupItemHOReqIEs->value.present = NGAP_PDUSessionResourceSetupItemHOReqIEs__value_PR_PDUSessionResourceSetupItemHOReq;
        PDUSessionResourceSetupItemHOReqIEs->value.choice.PDUSessionResourceSetupItemHOReq.pDUSessionID = amf_ue->psi;
#if 0 // need to fix
        PDUSessionResourceSetupItemHOReqIEs->value.choice.PDUSessionResourceSetupItemHOReq.s_NSSAI.sD = amf_ue->ran_ue->ran->supported_ta_list->s_nssai->sd;
        PDUSessionResourceSetupItemHOReqIEs->value.choice.PDUSessionResourceSetupItemHOReq.s_NSSAI.sST = amf_ue->ran_ue->ran->supported_ta_list->s_nssai->sst;
        PDUSessionResourceSetupItemHOReqIEs->value.choice.PDUSessionResourceSetupItemHOReq.handoverRequestTransfer;
#endif      
        

    ie = core_calloc(1, sizeof(NGAP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_SourceToTarget_TransparentContainer;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverRequestIEs__value_PR_SourceToTarget_TransparentContainer;
    SourceToTarget_TransparentContainer = &ie->value.choice.SourceToTarget_TransparentContainer;
    ngap_buffer_to_OCTET_STRING(
            sourceToTarget_TransparentContainer->buf, 
            sourceToTarget_TransparentContainer->size, 
            SourceToTarget_TransparentContainer);
    
    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;

}

status_t ngap_build_handover_command(pkbuf_t **ngapbuf, ran_ue_t *source_ue)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_HandoverCommand_t *HandoverCommand = NULL;

    NGAP_HandoverCommandIEs_t *ie = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_HandoverType_t	*HandoverType = NULL;
		NGAP_PDUSessionResourceSubjectToForwardingList_t *PDUSessionResourceSubjectToForwardingList = NULL;	
		NGAP_TargetToSource_TransparentContainer_t	*TargetToSource_TransparentContainer = NULL;
#if 0
    NGAP_PDUSessionList_t	 PDUSessionList;
    NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));
    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = NGAP_ProcedureCode_id_HandoverPreparation;
    successfulOutcome->criticality = NGAP_Criticality_reject;
    successfulOutcome->value.present =
        NGAP_SuccessfulOutcome__value_PR_HandoverCommand;
    HandoverCommand = &successfulOutcome->value.choice.HandoverCommand;

    ie = core_calloc(1, sizeof(NGAP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCommandIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCommandIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = source_ue->ran_ue_ngap_id;
    
	ie = core_calloc(1, sizeof(NGAP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_HandoverType;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCommandIEs__value_PR_HandoverType;
    HandoverType = &ie->value.choice.HandoverType;
    *HandoverType = source_ue->handover_type;

	ie = core_calloc(1, sizeof(NGAP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceSubjectToForwardingList;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCommandIEs__value_PR_PDUSessionResourceSubjectToForwardingList;
    PDUSessionResourceSubjectToForwardingList = &ie->value.choice.PDUSessionResourceSubjectToForwardingList;
        NGAP_PDUSessionResourceSubjectToForwardingItemIEs_t *PDUSessionResourceSubjectToForwardingItemIEs = NULL;
        PDUSessionResourceSubjectToForwardingItemIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceSubjectToForwardingItemIEs_t));
        ASN_SEQUENCE_ADD(&PDUSessionResourceSubjectToForwardingList->list, PDUSessionResourceSubjectToForwardingItemIEs);
        PDUSessionResourceSubjectToForwardingItemIEs->criticality = NGAP_Criticality_ignore;
        PDUSessionResourceSubjectToForwardingItemIEs->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceSubjectToForwardingItem;
        PDUSessionResourceSubjectToForwardingItemIEs->value.present 
            = NGAP_PDUSessionResourceSubjectToForwardingItemIEs__value_PR_PDUSessionResourceSubjectToForwardingItem;
        //PDUSessionResourceSubjectToForwardingItemIEs->value.choice.PDUSessionResourceSubjectToForwardingItem.handoverCommandTransfer'
        PDUSessionResourceSubjectToForwardingItemIEs->value.choice.PDUSessionResourceSubjectToForwardingItem.pDUSessionID 
            = source_ue->amf_ue->psi;
        
	ie = core_calloc(1, sizeof(NGAP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_TargetToSource_TransparentContainer;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCommandIEs__value_PR_TargetToSource_TransparentContainer;
    TargetToSource_TransparentContainer = &ie->value.choice.TargetToSource_TransparentContainer;
    ngap_buffer_to_OCTET_STRING(source_ue->amf_ue->container.buf, source_ue->amf_ue->container.size, TargetToSource_TransparentContainer);

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t ngap_build_path_switch_ack(pkbuf_t **ngapbuf, amf_ue_t *amf_ue)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_PathSwitchRequestAcknowledge_t *PathSwitchRequestAcknowledge = NULL;

    NGAP_PathSwitchRequestAcknowledgeIEs_t *ie = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;	
		//NGAP_SecurityContext_t *SecurityContext = NULL;	
		NGAP_PDUSessionResourceToBeSwitchedULList_t *PDUSessionResourceToBeSwitchedULList = NULL;
			
#if 0
    NGAP_UESecurityCapabilities_t	 UESecurityCapabilities;
    NGAP_KamfChangeInd_t	 KamfChangeInd;
    NGAP_PDUSessionList_t	 PDUSessionList;
	NGAP_RRCInactiveAssistanceInformation_t	 RRCInactiveAssistanceInformation;
	NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = NGAP_ProcedureCode_id_PathSwitchRequest;
    successfulOutcome->criticality = NGAP_Criticality_reject;
    successfulOutcome->value.present =
        NGAP_SuccessfulOutcome__value_PR_PathSwitchRequestAcknowledge;
    PathSwitchRequestAcknowledge = &successfulOutcome->value.choice.PathSwitchRequestAcknowledge;

    ie = core_calloc(1, sizeof(NGAP_PathSwitchRequestAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestAcknowledge->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_PathSwitchRequestAcknowledgeIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = amf_ue->ran_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_PathSwitchRequestAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestAcknowledge->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_PathSwitchRequestAcknowledgeIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = amf_ue->ran_ue->ran_ue_ngap_id;
#if 0 
    ie = core_calloc(1, sizeof(NGAP_PathSwitchRequestAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestAcknowledge->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_SecurityContext;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_PathSwitchRequestAcknowledgeIEs__value_PR_SecurityContext;
    SecurityContext = &ie->value.choice.SecurityContext;
   
    SecurityContext->nextHopChainingCount = amf4g_ue->nhcc;
    SecurityContext->nextHopParameter.size = SHA256_DIGEST_SIZE;
    SecurityContext->nextHopParameter.buf = 
        core_calloc(SecurityContext->nextHopParameter.size,
        sizeof(c_uint8_t));
    SecurityContext->nextHopParameter.bits_unused = 0;
    memcpy(SecurityContext->nextHopParameter.buf,
            amf4g_ue->nh, SecurityContext->nextHopParameter.size);
#endif

    ie = core_calloc(1, sizeof(NGAP_PathSwitchRequestAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestAcknowledge->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceToBeSwitchedULList;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_PathSwitchRequestAcknowledgeIEs__value_PR_PDUSessionResourceToBeSwitchedULList;
    PDUSessionResourceToBeSwitchedULList = &ie->value.choice.PDUSessionResourceToBeSwitchedULList;

    NGAP_PDUSessionResourceToBeSwitchedULItemIEs_t *PDUSessionResourceToBeSwitchedULItemIEs = NULL;
    PDUSessionResourceToBeSwitchedULItemIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceToBeSwitchedULItemIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceToBeSwitchedULList->list, PDUSessionResourceToBeSwitchedULItemIEs);
            NGAP_PDUSessionResourceToBeSwitchedULItem_t	*PDUSessionResourceToBeSwitchedULItem = NULL;
            PDUSessionResourceToBeSwitchedULItemIEs->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceToBeSwitchedDLItem;
            PDUSessionResourceToBeSwitchedULItemIEs->criticality = NGAP_Criticality_ignore;
            PDUSessionResourceToBeSwitchedULItemIEs->value.present = NGAP_PDUSessionResourceToBeSwitchedULItemIEs__value_PR_PDUSessionResourceToBeSwitchedULItem;
            PDUSessionResourceToBeSwitchedULItem = &PDUSessionResourceToBeSwitchedULItemIEs->value.choice.PDUSessionResourceToBeSwitchedULItem;
            //TODO: fix it
            //PDUSessionResourceToBeSwitchedULItem->pathSwitchRequestTransfer
            PDUSessionResourceToBeSwitchedULItem->pDUSessionID = amf_ue->psi;
    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}


status_t ngap_build_path_switch_failure(pkbuf_t **ngapbuf,
    c_uint32_t ran_ue_ngap_id, c_uint32_t amf_ue_ngap_id,
    NGAP_Cause_PR group, long cause)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    NGAP_PathSwitchRequestFailure_t *PathSwitchRequestFailure = NULL;

    NGAP_PathSwitchRequestFailureIEs_t *ie = NULL;
    	NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_Cause_t *Cause = NULL;
#if 0
		NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    d_assert(ngapbuf, return CORE_ERROR,);
    
    d_trace(3, "[AMF] Path Switch failure\n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = 
        core_calloc(1, sizeof(NGAP_UnsuccessfulOutcome_t));
    unsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    unsuccessfulOutcome->procedureCode = NGAP_ProcedureCode_id_PathSwitchRequest;
    unsuccessfulOutcome->criticality = NGAP_Criticality_reject;
    unsuccessfulOutcome->value.present = NGAP_UnsuccessfulOutcome__value_PR_HandoverFailure;
    PathSwitchRequestFailure = &unsuccessfulOutcome->value.choice.PathSwitchRequestFailure;

    ie = core_calloc(1, sizeof(NGAP_PathSwitchRequestFailureIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestFailure->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_PathSwitchRequestFailureIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_PathSwitchRequestFailureIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestFailure->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_PathSwitchRequestFailureIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_PathSwitchRequestFailureIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestFailure->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_Cause;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_PathSwitchRequestFailureIEs__value_PR_Cause;;
    Cause = &ie->value.choice.Cause;

    Cause->present = group;
    Cause->choice.radioNetwork = cause;


    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}


CORE_DECLARE(status_t) ngap_build_amf_status_indication(pkbuf_t **ngapbuf, ran_ue_t *ran_ue)
{
    status_t rv = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_AMFStatusIndication_t *AMFStatusIndication = NULL;

    NGAP_AMFStatusIndicationIEs_t *ie = NULL;
    	NGAP_UnavailableGUAMIList_t	 *UnavailableGUAMIList = NULL;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_AMFStatusIndication;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_HandoverRequest;
    AMFStatusIndication = &initiatingMessage->value.choice.AMFStatusIndication;
        
    ie = core_calloc(1, sizeof(NGAP_AMFStatusIndicationIEs_t));
    ASN_SEQUENCE_ADD(&AMFStatusIndication->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_AMFStatusIndicationIEs__value_PR_UnavailableGUAMIList;
    UnavailableGUAMIList = &ie->value.choice.UnavailableGUAMIList;
        NGAP_UnavailableGUAMIItem_t *NGAP_UnavailableGUAMIItem = NULL;
        NGAP_UnavailableGUAMIItem = core_calloc(1, sizeof(NGAP_AMFStatusIndicationIEs_t));
         ASN_SEQUENCE_ADD(&UnavailableGUAMIList->list, NGAP_UnavailableGUAMIItem);

   ngap_buffer_to_OCTET_STRING(&ran_ue->amf_ue->guti_5g.plmn_id, 3, &NGAP_UnavailableGUAMIItem->gUAMI.pLMNIdentity);
#if 0 // Fix it, use bit string.
   ngap_buffer_to_OCTET_STRING(&ran_ue->amf_ue->guti_5g.amf_ptr, 4, &NGAP_UnavailableGUAMIItem->gUAMI.aMFPointer);
   ngap_buffer_to_OCTET_STRING(&ran_ue->amf_ue->guti_5g.amf_rid, 4, &NGAP_UnavailableGUAMIItem->gUAMI.aMFRegionID);
   ngap_buffer_to_OCTET_STRING(&ran_ue->amf_ue->guti_5g.amf_sid, 2, &NGAP_UnavailableGUAMIItem->gUAMI.aMFSetID);
#endif
       
    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}


/**
 * Direction: AMF -> NG-RAN node
 **/
status_t ngap_build_amf_configuration_update(pkbuf_t **pkbuf)
{
    int i = 0;
    int j = 0;
    int k = 0;
    status_t rv;
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_AMFConfigurationUpdate_t *AMFConfigurationUpdate = NULL;

    NGAP_AMFConfigurationUpdateIEs_t *ie = NULL;
        NGAP_ServedGUAMIList_t *ServedGUAMIList = NULL;
        NGAP_PLMNSupportList_t *PLMNSupportList = NULL;      
        NGAP_AMF_TNLAssociationToAddList_t *AMF_TNLAssociationToAddList = NULL;
        NGAP_AMF_TNLAssociationToRemoveList_t *AMF_TNLAssociationToRemoveList = NULL;
        NGAP_AMF_TNLAssociationToUpdateList_t *AMF_TNLAssociationToUpdateList = NULL;
#if 0
    NGAP_AMFName_t	 AMFName;
    NGAP_RelativeAMFCapacity_t	 RelativeAMFCapacity;
#endif

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));
    
    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        NGAP_ProcedureCode_id_AMFConfigurationUpdate;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present =
        NGAP_InitiatingMessage__value_PR_AMFConfigurationUpdate;
    AMFConfigurationUpdate = 
        &initiatingMessage->value.choice.AMFConfigurationUpdate;
    
    ie = core_calloc(1, sizeof(NGAP_AMFConfigurationUpdateIEs_t));
    ASN_SEQUENCE_ADD(&AMFConfigurationUpdate->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_ServedGUAMIList;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_AMFConfigurationUpdateIEs__value_PR_ServedGUAMIList;
    ServedGUAMIList = &ie->value.choice.ServedGUAMIList;
    
        for (int i = 0 ; i < amf4g_self()->max_num_of_served_guami ; i ++)
        {
            //TODO: fix it, use NGAP_ServedGUAMIItem_IEs instead 
            NGAP_ServedGUAMIItem_t *ServedGUAMIItem = NULL;
            ServedGUAMIItem =(NGAP_ServedGUAMIItem_t *) core_calloc(1, sizeof(NGAP_ServedGUAMIItem_t));
            
            served_guami_t *served_guami =&amf4g_self()->served_guami[i];
            for (j = 0; j < 1/*served_guami->num_of_plmn_id*/; j++)
            {
                ngap_buffer_to_OCTET_STRING(&served_guami->plmn_id[j], PLMN_ID_LEN, &ServedGUAMIItem->gUAMI.pLMNIdentity);
                d_trace(5, "    PLMN_ID[MCC:%d MNC:%d]\n",
                    plmn_id_mcc(&served_guami->plmn_id[j]),
                    plmn_id_mnc(&served_guami->plmn_id[j]));
            }
            ASN_SEQUENCE_ADD(&ServedGUAMIList->list, ServedGUAMIItem);
        }

    ie = core_calloc(1, sizeof(NGAP_AMFConfigurationUpdateIEs_t));
    ASN_SEQUENCE_ADD(&AMFConfigurationUpdate->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PLMNSupportList;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_AMFConfigurationUpdateIEs__value_PR_PLMNSupportList;
    PLMNSupportList = &ie->value.choice.PLMNSupportList;
        for (i = 0 ; i < amf4g_self()->max_num_of_plmn_support ; i ++)
        {
            //TODO: fix it, NGAP_PLMNSupportItem_IEs_t instead
            NGAP_PLMNSupportItem_t *PLMNSupportItem = NULL;
            PLMNSupportItem =(NGAP_PLMNSupportItem_t *) core_calloc(1, sizeof(NGAP_PLMNSupportItem_t));
            
            served_guami_t *served_guami =&amf4g_self()->served_guami[i];
            for (j = 0; j < 1/*served_guami->num_of_plmn_id*/; j++)
            {
                ngap_buffer_to_OCTET_STRING(&served_guami->plmn_id[j], PLMN_ID_LEN, &PLMNSupportItem->pLMNIdentity);
                d_trace(5, "    PLMN_ID[MCC:%d MNC:%d]\n",
                    plmn_id_mcc(&served_guami->plmn_id[j]),
                    plmn_id_mnc(&served_guami->plmn_id[j]));
            }
                NGAP_SliceSupportItem_t *NGAP_SliceSupportItem = NULL;
                NGAP_SliceSupportItem =(NGAP_SliceSupportItem_t *) core_calloc(1, sizeof(NGAP_SliceSupportItem_t));
                ASN_SEQUENCE_ADD(&PLMNSupportItem->sliceSupportList.list, NGAP_SliceSupportItem);
                for (j = 0; j < amf4g_self()->max_num_of_plmn_support ; j++)
                {
                    for (k = 0; k < amf4g_self()->plmn_support[j].num_of_s_nssai; k++)
                    {
                        memcpy(&NGAP_SliceSupportItem->s_NSSAI, &amf4g_self()->plmn_support[j].s_nssai[k],  sizeof(NGAP_S_NSSAI_t));
                    }
                }
            ASN_SEQUENCE_ADD(&PLMNSupportList->list, PLMNSupportItem);
        }

    ie = core_calloc(1, sizeof(NGAP_AMFConfigurationUpdateIEs_t));
    ASN_SEQUENCE_ADD(&AMFConfigurationUpdate->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_TNLAssociationToAddList;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_AMFConfigurationUpdateIEs__value_PR_AMF_TNLAssociationToAddList;
    AMF_TNLAssociationToAddList = &ie->value.choice.AMF_TNLAssociationToAddList;
        NGAP_AMF_TNLAssociationToAddItemIEs_t *AMF_TNLAssociationToAddItemIEs = NULL;
        AMF_TNLAssociationToAddItemIEs = (NGAP_AMF_TNLAssociationToAddItemIEs_t *) core_calloc(1, sizeof(NGAP_AMF_TNLAssociationToAddItemIEs_t));
        AMF_TNLAssociationToAddItemIEs->id = NGAP_ProtocolIE_ID_id_AMF_TNLAssociationToAddItem;
        AMF_TNLAssociationToAddItemIEs->criticality = NGAP_Criticality_ignore;
        AMF_TNLAssociationToAddItemIEs->value.present = NGAP_AMF_TNLAssociationToAddItemIEs__value_PR_AMF_TNLAssociationToAddItem;
            NGAP_AMF_TNLAssociationToAddItem_t *AMF_TNLAssociationToAddItem = NULL;
            AMF_TNLAssociationToAddItem = &AMF_TNLAssociationToAddItemIEs->value.choice.AMF_TNLAssociationToAddItem;
                AMF_TNLAssociationToAddItem->aMF_TNLAssociationAddress.present = NGAP_CPTransportLayerInformation_PR_endpointIPAddress;
                //TODO: put bit string
                //AMF_TNLAssociationToAddItem->aMF_TNLAssociationAddress.choice.endpointIPAddress;
                AMF_TNLAssociationToAddItem->tNLAssociationWeightFactor = 0x1234;                     
        ASN_SEQUENCE_ADD(&AMF_TNLAssociationToAddList->list, AMF_TNLAssociationToAddItemIEs);

    ie = core_calloc(1, sizeof(NGAP_AMFConfigurationUpdateIEs_t));
    ASN_SEQUENCE_ADD(&AMFConfigurationUpdate->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_TNLAssociationToRemoveList;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_AMFConfigurationUpdateIEs__value_PR_AMF_TNLAssociationToRemoveList;
    AMF_TNLAssociationToRemoveList = &ie->value.choice.AMF_TNLAssociationToRemoveList;
        NGAP_AMF_TNLAssociationToRemoveItemIEs_t *AMF_TNLAssociationToRemoveItemIEs = NULL;
        AMF_TNLAssociationToRemoveItemIEs = (NGAP_AMF_TNLAssociationToRemoveItemIEs_t *) core_calloc(1, sizeof(NGAP_AMF_TNLAssociationToRemoveItemIEs_t));
        AMF_TNLAssociationToRemoveItemIEs->id = NGAP_ProtocolIE_ID_id_AMF_TNLAssociationToRemoveItem;
        AMF_TNLAssociationToRemoveItemIEs->criticality = NGAP_Criticality_ignore;
        AMF_TNLAssociationToRemoveItemIEs->value.present = NGAP_AMF_TNLAssociationToRemoveItemIEs__value_PR_AMF_TNLAssociationToRemoveItem;
            NGAP_AMF_TNLAssociationToRemoveItem_t *AMF_TNLAssociationToRemoveItem = NULL;
            AMF_TNLAssociationToRemoveItem = &AMF_TNLAssociationToRemoveItemIEs->value.choice.AMF_TNLAssociationToRemoveItem;
                AMF_TNLAssociationToRemoveItem->aMF_TNLAssociationAddress.present = NGAP_CPTransportLayerInformation_PR_endpointIPAddress;
                //TODO: put bit string
                //AMF_TNLAssociationToRemoveItem->aMF_TNLAssociationAddress.choice.endpointIPAddress;
    d_assert(AMF_TNLAssociationToRemoveList, return CORE_ERROR, );

    ie = core_calloc(1, sizeof(NGAP_AMFConfigurationUpdateIEs_t));
    ASN_SEQUENCE_ADD(&AMFConfigurationUpdate->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_TNLAssociationToUpdateList;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_AMFConfigurationUpdateIEs__value_PR_AMF_TNLAssociationToUpdateList;
    AMF_TNLAssociationToUpdateList = &ie->value.choice.AMF_TNLAssociationToUpdateList;
        NGAP_AMF_TNLAssociationToUpdateItemIEs_t *AMF_TNLAssociationToUpdateItemIEs = NULL;
        AMF_TNLAssociationToUpdateItemIEs = (NGAP_AMF_TNLAssociationToUpdateItemIEs_t *) core_calloc(1, sizeof(NGAP_AMF_TNLAssociationToUpdateItemIEs_t));
        AMF_TNLAssociationToUpdateItemIEs->id = NGAP_AMF_TNLAssociationToUpdateItemIEs__value_PR_AMF_TNLAssociationToUpdateItem;
        AMF_TNLAssociationToUpdateItemIEs->criticality = NGAP_Criticality_ignore;
        AMF_TNLAssociationToUpdateItemIEs->value.present = NGAP_AMF_TNLAssociationToUpdateItemIEs__value_PR_AMF_TNLAssociationToUpdateItem;
            NGAP_AMF_TNLAssociationToUpdateItem_t *AMF_TNLAssociationToUpdateItem = NULL;
            AMF_TNLAssociationToUpdateItem = &AMF_TNLAssociationToUpdateItemIEs->value.choice.AMF_TNLAssociationToUpdateItem;
                AMF_TNLAssociationToUpdateItem->aMF_TNLAssociationAddress.present = NGAP_CPTransportLayerInformation_PR_endpointIPAddress;
                //TODO: put bit string
                //AMF_TNLAssociationToRemoveItem->aMF_TNLAssociationAddress.choice.endpointIPAddress;
    d_assert(AMF_TNLAssociationToUpdateList, return CORE_ERROR, );


    rv = ngap_encode_pdu(pkbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t ngap_build_ran_configuration_update_acknowledge(pkbuf_t **pkbuf)
{
    status_t rv;

    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = NGAP_ProcedureCode_id_RANConfigurationUpdate;
    successfulOutcome->criticality = NGAP_Criticality_reject;
    successfulOutcome->value.present = NGAP_SuccessfulOutcome__value_PR_RANConfigurationUpdateAcknowledge;

    rv = ngap_encode_pdu(pkbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t ngap_build_ran_configuration_update_failure(pkbuf_t **pkbuf, NGAP_Cause_PR group, long cause, long time_to_wait)
{
    status_t rv;

    NGAP_NGAP_PDU_t pdu;
    NGAP_UnsuccessfulOutcome_t *UnsuccessfulOutcome = NULL;
    NGAP_RANConfigurationUpdateFailure_t *RANConfigurationUpdateFailure = NULL;

    NGAP_RANConfigurationUpdateFailureIEs_t *ie = NULL;
        NGAP_Cause_t *Cause = NULL;
        NGAP_TimeToWait_t *TimeToWait = NULL;
#if 0
        NGAP_CriticalityDiagnostics_t *CriticalityDiagnostics = NULL;
#endif

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = core_calloc(1, sizeof(NGAP_UnsuccessfulOutcome_t));
    UnsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    UnsuccessfulOutcome->procedureCode = NGAP_ProcedureCode_id_AMFConfigurationUpdate;
    UnsuccessfulOutcome->criticality = NGAP_Criticality_reject;
    UnsuccessfulOutcome->value.present = NGAP_UnsuccessfulOutcome__value_PR_RANConfigurationUpdateFailure;
    RANConfigurationUpdateFailure = &UnsuccessfulOutcome->value.choice.RANConfigurationUpdateFailure;

    ie = core_calloc(1, sizeof(NGAP_AMFConfigurationUpdateFailureIEs_t));
    ASN_SEQUENCE_ADD(&RANConfigurationUpdateFailure->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_Cause;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_RANConfigurationUpdateFailureIEs__value_PR_Cause;
    Cause = &ie->value.choice.Cause;
    
    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    if (time_to_wait > -1)
    {
        ie = core_calloc(1, sizeof(NGAP_RANConfigurationUpdateFailureIEs_t));
        ASN_SEQUENCE_ADD(&RANConfigurationUpdateFailure->protocolIEs, ie);

        ie->id = NGAP_ProtocolIE_ID_id_TimeToWait;
        ie->criticality = NGAP_Criticality_ignore;
        ie->value.present =  NGAP_RANConfigurationUpdateFailureIEs__value_PR_TimeToWait;

        TimeToWait = &ie->value.choice.TimeToWait;
    }

    if (TimeToWait)
        *TimeToWait = time_to_wait;

    rv = ngap_encode_pdu(pkbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}


/**
 * Direction: AMF -> NG-RAN node
 **/
status_t ngap_build_pdu_session_resource_modify_request(pkbuf_t **ngapbuf, amf_ue_t *amf_ue, ngap_message_t *message)
{
    status_t rv;

    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PDUSessionResourceModifyRequest_t *PDUSessionResourceModifyRequest = NULL;

    NGAP_PDUSessionResourceModifyRequestIEs_t *PDUSessionResourceModifyRequestIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceModifyListModReq_t *PDUSessionResourceModifyListModReq = NULL;
#if 0
    NGAP_RANPagingPriority_t *RANPagingPriority = NULL;
#endif 
    ran_ue_t *ran_ue = NULL;
    d_assert(amf_ue, return CORE_ERROR, "Null param");
    ran_ue = amf_ue->ran_ue;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));
    
    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
    NGAP_ProcedureCode_id_PDUSessionResourceModify;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_PDUSessionResourceModifyRequest;
    PDUSessionResourceModifyRequest =  &initiatingMessage->value.choice.PDUSessionResourceModifyRequest;
    
    PDUSessionResourceModifyRequestIEs = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyRequest->protocolIEs, PDUSessionResourceModifyRequestIEs);
    PDUSessionResourceModifyRequestIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    PDUSessionResourceModifyRequestIEs->criticality = NGAP_Criticality_reject;
    PDUSessionResourceModifyRequestIEs->value.present = NGAP_PDUSessionResourceModifyRequestIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &PDUSessionResourceModifyRequestIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    PDUSessionResourceModifyRequestIEs = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyRequest->protocolIEs, PDUSessionResourceModifyRequestIEs);
    PDUSessionResourceModifyRequestIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    PDUSessionResourceModifyRequestIEs->criticality = NGAP_Criticality_reject;
    PDUSessionResourceModifyRequestIEs->value.present = NGAP_PDUSessionResourceModifyRequestIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &PDUSessionResourceModifyRequestIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;

    PDUSessionResourceModifyRequestIEs = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyRequest->protocolIEs, PDUSessionResourceModifyRequestIEs);
    PDUSessionResourceModifyRequestIEs->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceModifyListModReq;
    PDUSessionResourceModifyRequestIEs->criticality = NGAP_Criticality_reject;
    PDUSessionResourceModifyRequestIEs->value.present = NGAP_PDUSessionResourceModifyRequestIEs__value_PR_PDUSessionResourceModifyListModReq;
    PDUSessionResourceModifyListModReq = &PDUSessionResourceModifyRequestIEs->value.choice.PDUSessionResourceModifyListModReq;
        
        NGAP_PDUSessionResourceModifyItemModReqIEs_t *PDUSessionResourceModifyItemModReqIEs = NULL;
        PDUSessionResourceModifyItemModReqIEs = (NGAP_PDUSessionResourceModifyItemModReqIEs_t *) core_calloc(1, sizeof(NGAP_PDUSessionResourceModifyItemModReqIEs_t));
        PDUSessionResourceModifyItemModReqIEs->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceModifyItemModReq;
        PDUSessionResourceModifyItemModReqIEs->criticality = NGAP_Criticality_reject;
        PDUSessionResourceModifyItemModReqIEs->value.present = NGAP_PDUSessionResourceModifyItemModReqIEs__value_PR_PDUSessionResourceModifyItemModReq;
            NGAP_PDUSessionResourceModifyItemModReq_t *PDUSessionResourceModifyItemModReq = NULL;
            PDUSessionResourceModifyItemModReq = (NGAP_PDUSessionResourceModifyItemModReq_t *)core_calloc(1, sizeof(NGAP_PDUSessionResourceModifyItemModReq_t));
            PDUSessionResourceModifyItemModReq->pDUSessionID = amf_ue->psi;
            //TODO: need to add IE:PDU Session Resource Modify Request Transfer
            //PDUSessionResourceModifyItemModReq->pDUSessionResourceModifyRequestTransfer
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyListModReq->list, PDUSessionResourceModifyItemModReq);

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }


    return rv;

}

/**
 * Direction: AMF -> NG-RAN node
 **/
status_t ngap_build_pdu_session_resource_modify_confirm(pkbuf_t **ngapbuf, ran_ue_t *source_ue)
{
    status_t rv;
    
    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_PDUSessionResourceModifyConfirm_t *PDUSessionResourceModifyConfirm = NULL;

    NGAP_PDUSessionResourceModifyConfirmIEs_t *PDUSessionResourceModifyConfirmIEs = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_PDUSessionResourceModifyListModCfm_t *PDUSessionResourceModifyListModCfm = NULL;
#if 0		
        NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = NGAP_ProcedureCode_id_PDUSessionResourceModifyIndication;
    successfulOutcome->criticality = NGAP_Criticality_reject;
    successfulOutcome->value.present =
        NGAP_SuccessfulOutcome__value_PR_PDUSessionResourceModifyConfirm;
    PDUSessionResourceModifyConfirm = &successfulOutcome->value.choice.PDUSessionResourceModifyConfirm;

    PDUSessionResourceModifyConfirmIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceModifyConfirmIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyConfirm->protocolIEs, PDUSessionResourceModifyConfirmIEs);
    PDUSessionResourceModifyConfirmIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    PDUSessionResourceModifyConfirmIEs->criticality = NGAP_Criticality_ignore;
    PDUSessionResourceModifyConfirmIEs->value.present = NGAP_PDUSessionResourceModifyConfirmIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &PDUSessionResourceModifyConfirmIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    PDUSessionResourceModifyConfirmIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceModifyConfirmIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyConfirm->protocolIEs, PDUSessionResourceModifyConfirmIEs);
    PDUSessionResourceModifyConfirmIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    PDUSessionResourceModifyConfirmIEs->criticality = NGAP_Criticality_ignore;
    PDUSessionResourceModifyConfirmIEs->value.present = NGAP_PDUSessionResourceModifyConfirmIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &PDUSessionResourceModifyConfirmIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = source_ue->ran_ue_ngap_id;

    PDUSessionResourceModifyConfirmIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceModifyConfirmIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyConfirm->protocolIEs, PDUSessionResourceModifyConfirmIEs);
    PDUSessionResourceModifyConfirmIEs->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceModifyListModCfm;
    PDUSessionResourceModifyConfirmIEs->criticality = NGAP_Criticality_ignore;
    PDUSessionResourceModifyConfirmIEs->value.present = NGAP_PDUSessionResourceModifyConfirmIEs__value_PR_PDUSessionResourceModifyListModCfm;
    PDUSessionResourceModifyListModCfm = &PDUSessionResourceModifyConfirmIEs->value.choice.PDUSessionResourceModifyListModCfm;
        NGAP_PDUSessionResourceModifyItemModCfm_t *PDUSessionResourceModifyItemModCfm = NULL;
        PDUSessionResourceModifyItemModCfm = (NGAP_PDUSessionResourceModifyItemModCfm_t *)core_calloc(1, sizeof(NGAP_PDUSessionResourceModifyItemModCfm_t));
                 PDUSessionResourceModifyItemModCfm->pDUSessionID = source_ue->amf_ue->psi;
                //TODO: need to add IE:PDU Session Resource Modify Request Transfer
                //PDUSessionResourceModifyItemModCfm->pDUSessionResourceModifyRequestTransfer
    ASN_SEQUENCE_ADD(&PDUSessionResourceModifyListModCfm->list, PDUSessionResourceModifyItemModCfm);

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }


    return rv;
}

/**
 * Direction: AMF -> NG-RAN node
 **/
status_t ngap_build_pdu_session_resource_release_command(pkbuf_t **ngapbuf, ran_ue_t *source_ue, NGAP_Cause_PR group, long cause)
{
    status_t rv;
    int i = 0;
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PDUSessionResourceReleaseCommand_t *PDUSessionResourceReleaseCommand = NULL;

    NGAP_PDUSessionResourceReleaseCommandIEs_t *PDUSessionResourceReleaseCommandIEs = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;		
		NGAP_PDUSessionList_t *PDUSessionList = NULL;
#if 0		
		NGAP_RANPagingPriority_t	 RANPagingPriority;
		NGAP_NAS_PDU_t	 NAS_PDU;
        NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_PDUSessionResourceRelease;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_PDUSessionResourceReleaseCommand;
    PDUSessionResourceReleaseCommand = &initiatingMessage->value.choice.PDUSessionResourceReleaseCommand;

    PDUSessionResourceReleaseCommandIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceReleaseCommand->protocolIEs, PDUSessionResourceReleaseCommandIEs);
    PDUSessionResourceReleaseCommandIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    PDUSessionResourceReleaseCommandIEs->criticality = NGAP_Criticality_reject;
    PDUSessionResourceReleaseCommandIEs->value.present = NGAP_PDUSessionResourceReleaseCommandIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &PDUSessionResourceReleaseCommandIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    PDUSessionResourceReleaseCommandIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceReleaseCommand->protocolIEs, PDUSessionResourceReleaseCommandIEs);
    PDUSessionResourceReleaseCommandIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    PDUSessionResourceReleaseCommandIEs->criticality = NGAP_Criticality_reject;
    PDUSessionResourceReleaseCommandIEs->value.present = NGAP_PDUSessionResourceReleaseCommandIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &PDUSessionResourceReleaseCommandIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    PDUSessionResourceReleaseCommandIEs = core_calloc(1, sizeof(NGAP_PDUSessionResourceReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceReleaseCommand->protocolIEs, PDUSessionResourceReleaseCommandIEs);
    PDUSessionResourceReleaseCommandIEs->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceReleasedList;
    PDUSessionResourceReleaseCommandIEs->criticality = NGAP_Criticality_ignore;
    PDUSessionResourceReleaseCommandIEs->value.present = NGAP_PDUSessionResourceReleaseCommandIEs__value_PR_PDUSessionList;
    PDUSessionList = &PDUSessionResourceReleaseCommandIEs->value.choice.PDUSessionList;
        NGAP_PDUSessionItem_t *PDUSessionItem = NULL;
        for(i = 0 ; i < PDUSessionList->list.count ; i++) 
        {
            PDUSessionItem = (NGAP_PDUSessionItem_t *) core_calloc(1, sizeof(NGAP_PDUSessionItem_t));
            PDUSessionItem->pDUSessionID = source_ue->amf_ue->psi;
            PDUSessionItem->cause.present = group;
            PDUSessionItem->cause.choice.radioNetwork =cause;
            ASN_SEQUENCE_ADD(&PDUSessionList->list, PDUSessionItem);
        }

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK; 
}

status_t ngap_build_ue_radio_capability_check_request(pkbuf_t **ngapbuf, ran_ue_t *source_ue)
{
    status_t rv;
    
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UERadioCapabilityCheckRequest_t *UERadioCapabilityCheckRequest = NULL;

    NGAP_UERadioCapabilityCheckRequestIEs_t *UERadioCapabilityCheckRequestIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
#if 0
        NGAP_UERadioCapability_t	 UERadioCapability;
#endif
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_UERadioCapabilityCheck;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_UERadioCapabilityCheckRequest;
    UERadioCapabilityCheckRequest = &initiatingMessage->value.choice.UERadioCapabilityCheckRequest;

    UERadioCapabilityCheckRequestIEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
    ASN_SEQUENCE_ADD(&UERadioCapabilityCheckRequest->protocolIEs, UERadioCapabilityCheckRequestIEs);
    UERadioCapabilityCheckRequestIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    UERadioCapabilityCheckRequestIEs->criticality = NGAP_Criticality_reject;
    UERadioCapabilityCheckRequestIEs->value.present = NGAP_UERadioCapabilityCheckRequestIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &UERadioCapabilityCheckRequestIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    UERadioCapabilityCheckRequestIEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
    ASN_SEQUENCE_ADD(&UERadioCapabilityCheckRequest->protocolIEs, UERadioCapabilityCheckRequestIEs);
    UERadioCapabilityCheckRequestIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    UERadioCapabilityCheckRequestIEs->criticality = NGAP_Criticality_reject;
    UERadioCapabilityCheckRequestIEs->value.present = NGAP_UERadioCapabilityCheckRequestIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &UERadioCapabilityCheckRequestIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = source_ue->amf_ue_ngap_id;

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK; 
}

status_t ngap_build_downlink_nas_transport(pkbuf_t **ngapbuf, ran_ue_t *ran_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_DownlinkNASTransport_t *DownlinkNASTransport = NULL;

    NGAP_DownlinkNASTransport_IEs_t *DownlinkNASTransport_IEs = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_NAS_PDU_t *NAS_PDU = NULL;
#if 0
    NGAP_AMFName_t	 AMFName;
    NGAP_RANPagingPriority_t	 RANPagingPriority;
    NGAP_MobilityRestrictionList_t	 MobilityRestrictionList;
    NGAP_IndexToRFSP_t	 IndexToRFSP;
    NGAP_UEAggregateMaximumBitRate_t	 UEAggregateMaximumBitRate;
#endif
    d_trace(3, "[AMF] Downlink NAS transport\n");
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_DownlinkNASTransport;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_DownlinkNASTransport;
    DownlinkNASTransport = &initiatingMessage->value.choice.DownlinkNASTransport;

    DownlinkNASTransport_IEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNASTransport->protocolIEs, DownlinkNASTransport_IEs);
    DownlinkNASTransport_IEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    DownlinkNASTransport_IEs->criticality = NGAP_Criticality_reject;
    DownlinkNASTransport_IEs->value.present = NGAP_DownlinkNASTransport_IEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &DownlinkNASTransport_IEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    DownlinkNASTransport_IEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNASTransport->protocolIEs, DownlinkNASTransport_IEs);
    DownlinkNASTransport_IEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    DownlinkNASTransport_IEs->criticality = NGAP_Criticality_reject;
    DownlinkNASTransport_IEs->value.present = NGAP_DownlinkNASTransport_IEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &DownlinkNASTransport_IEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;

    DownlinkNASTransport_IEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNASTransport->protocolIEs, DownlinkNASTransport_IEs);
    DownlinkNASTransport_IEs->id = NGAP_ProtocolIE_ID_id_NAS_PDU;
    DownlinkNASTransport_IEs->criticality = NGAP_Criticality_reject;
    DownlinkNASTransport_IEs->value.present = NGAP_DownlinkNASTransport_IEs__value_PR_NAS_PDU;
    NAS_PDU = &DownlinkNASTransport_IEs->value.choice.NAS_PDU;
#if 0  
    NAS_PDU->size = emmbuf->len;
    NAS_PDU->buf = core_calloc(NAS_PDU->size, sizeof(c_uint8_t));
    memcpy(NAS_PDU->buf, emmbuf->payload, NAS_PDU->size);
    pkbuf_free(emmbuf)
#endif
    d_assert(NAS_PDU, return CORE_ERROR, );

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK; 
}

status_t ngap_build_error_indication(pkbuf_t **ngapbuf, NGAP_AMF_UE_NGAP_ID_t *amf_ue_ngap_id, NGAP_RAN_UE_NGAP_ID_t *ran_ue_nagp_id, NGAP_Cause_PR group, long cause)
{
    status_t rv;
    
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_ErrorIndication_t *ErrorIndication = NULL;

    NGAP_ErrorIndicationIEs_t *ErrorIndicationIEs = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_Cause_t *Cause = NULL;
#if 0
	NGAP_CriticalityDiagnostics_t	 CriticalityDiagnostics;
#endif
    d_trace(3, "[AMF] Error indication\n");
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_ErrorIndication;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_ErrorIndication;
    ErrorIndication = &initiatingMessage->value.choice.ErrorIndication;

    if (amf_ue_ngap_id) 
    {
        ErrorIndicationIEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
        ASN_SEQUENCE_ADD(&ErrorIndication->protocolIEs, ErrorIndicationIEs);
        ErrorIndicationIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
        ErrorIndicationIEs->criticality = NGAP_Criticality_ignore;
        ErrorIndicationIEs->value.present = NGAP_ErrorIndicationIEs__value_PR_AMF_UE_NGAP_ID;
        AMF_UE_NGAP_ID = &ErrorIndicationIEs->value.choice.AMF_UE_NGAP_ID;
        *AMF_UE_NGAP_ID = *amf_ue_ngap_id;
         d_trace(5, "    AMF_UE_NGAP_ID[%d]\n", amf_ue_ngap_id);
    }

    if (ran_ue_nagp_id) 
    {
        ErrorIndicationIEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
        ASN_SEQUENCE_ADD(&ErrorIndication->protocolIEs, ErrorIndicationIEs);
        ErrorIndicationIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
        ErrorIndicationIEs->criticality = NGAP_Criticality_ignore;
        ErrorIndicationIEs->value.present = NGAP_ErrorIndicationIEs__value_PR_RAN_UE_NGAP_ID;
        RAN_UE_NGAP_ID = &ErrorIndicationIEs->value.choice.RAN_UE_NGAP_ID;
        *RAN_UE_NGAP_ID = *ran_ue_nagp_id;
        d_trace(5, "    RAN_UE_NGAP_ID[%d]\n", ran_ue_nagp_id);
    }

    ErrorIndicationIEs = core_calloc(1, sizeof(NGAP_UERadioCapabilityCheckRequestIEs_t));
    ASN_SEQUENCE_ADD(&ErrorIndication->protocolIEs, ErrorIndicationIEs);
    ErrorIndicationIEs->id = NGAP_ProtocolIE_ID_id_Cause;
    ErrorIndicationIEs->criticality = NGAP_Criticality_ignore;
    ErrorIndicationIEs->value.present = NGAP_ErrorIndicationIEs__value_PR_Cause;
    Cause = &ErrorIndicationIEs->value.choice.Cause;
    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    d_trace(5, "    Group[%d] Cause[%d]\n",
            Cause->present, Cause->choice.radioNetwork);


    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_downlink_ran_configuration_transfer(pkbuf_t **ngapbuf, ran_ue_t *ran_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_DownlinkRANConfigurationTransfer_t *DownlinkRANConfigurationTransfer = NULL;
#if 0
    NGAP_DownlinkRANConfigurationTransferIEs_t *DownlinkRANConfigurationTransferIEs = NULL;
	    NGAP_SONConfigurationTransfer_t	*SONConfigurationTransfer = NULL;
#endif
    d_trace(3, "[AMF] Downlink Ran Configuration Transfer\n");
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_DownlinkRANConfigurationTransfer;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_DownlinkRANConfigurationTransfer;
    DownlinkRANConfigurationTransfer = &initiatingMessage->value.choice.DownlinkRANConfigurationTransfer;
    d_assert(DownlinkRANConfigurationTransfer, return CORE_ERROR,);
#if 0
    DownlinkRANConfigurationTransferIEs = core_calloc(1, sizeof(NGAP_DownlinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkRANConfigurationTransfer->protocolIEs, DownlinkRANConfigurationTransferIEs);
    DownlinkRANConfigurationTransferIEs->id = NGAP_ProtocolIE_ID_id_SONConfigurationTransferDL;
    DownlinkRANConfigurationTransferIEs->criticality = NGAP_Criticality_ignore;
    DownlinkRANConfigurationTransferIEs->value.present = NGAP_DownlinkRANConfigurationTransferIEs__value_PR_SONConfigurationTransfer;
    SONConfigurationTransfer = &DownlinkRANConfigurationTransferIEs->value.choice.SONConfigurationTransfer;
#endif

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_downlink_ran_status_transfer(pkbuf_t **ngapbuf, ran_ue_t *ran_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_DownlinkRANStatusTransfer_t *DownlinkRANStatusTransfer = NULL;

    NGAP_DownlinkRANStatusTransferIEs_t *DownlinkRANStatusTransferIEs = NULL;
	  	NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
	    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_RANStatusTransfer_TransparentContainer_t *RANStatusTransfer_TransparentContainer = NULL;

    d_trace(3, "[AMF] Downlink Ran Status Transfer\n");
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_DownlinkRANStatusTransfer;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_DownlinkRANStatusTransfer;
    DownlinkRANStatusTransfer = &initiatingMessage->value.choice.DownlinkRANStatusTransfer;
    d_assert(DownlinkRANStatusTransfer, return CORE_ERROR,);

    DownlinkRANStatusTransferIEs = core_calloc(1, sizeof(NGAP_DownlinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkRANStatusTransfer->protocolIEs, DownlinkRANStatusTransferIEs);
    DownlinkRANStatusTransferIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    DownlinkRANStatusTransferIEs->criticality = NGAP_Criticality_reject;
    DownlinkRANStatusTransferIEs->value.present = NGAP_DownlinkRANStatusTransferIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &DownlinkRANStatusTransferIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    DownlinkRANStatusTransferIEs = core_calloc(1, sizeof(NGAP_DownlinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkRANStatusTransfer->protocolIEs, DownlinkRANStatusTransferIEs);
    DownlinkRANStatusTransferIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    DownlinkRANStatusTransferIEs->criticality = NGAP_Criticality_reject;
    DownlinkRANStatusTransferIEs->value.present = NGAP_DownlinkRANStatusTransferIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &DownlinkRANStatusTransferIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;
    //This IE may need to be refined. ref: 3841-9.2.3.14
    DownlinkRANStatusTransferIEs = core_calloc(1, sizeof(NGAP_DownlinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkRANStatusTransfer->protocolIEs, DownlinkRANStatusTransferIEs);
    DownlinkRANStatusTransferIEs->id = NGAP_ProtocolIE_ID_id_RANStatusTransfer_TransparentContainer;
    DownlinkRANStatusTransferIEs->criticality = NGAP_Criticality_reject;
    DownlinkRANStatusTransferIEs->value.present = NGAP_DownlinkRANStatusTransferIEs__value_PR_RANStatusTransfer_TransparentContainer;
    RANStatusTransfer_TransparentContainer = &DownlinkRANStatusTransferIEs->value.choice.RANStatusTransfer_TransparentContainer;
    d_assert(RANStatusTransfer_TransparentContainer, return CORE_ERROR,);

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_uplink_ran_configuration_transfer(pkbuf_t **ngapbuf, ran_ue_t *ran_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UplinkRANConfigurationTransfer_t *UplinkRANConfigurationTransfer = NULL;
#if 0
    NGAP_UplinkRANConfigurationTransferIEs_t *UplinkRANConfigurationTransferIEs = NULL;
	    NGAP_SONConfigurationTransfer_t	*SONConfigurationTransfer = NULL;
#endif
    d_trace(3, "[AMF] Uplink Ran Configuration Transfer\n");
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_DownlinkRANConfigurationTransfer;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_UplinkRANConfigurationTransfer;
    UplinkRANConfigurationTransfer = &initiatingMessage->value.choice.UplinkRANConfigurationTransfer;
    d_assert(UplinkRANConfigurationTransfer, return CORE_ERROR,);
#if 0
    UplinkRANConfigurationTransferIEs = core_calloc(1, sizeof(NGAP_UplinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&UplinkRANConfigurationTransfer->protocolIEs, UplinkRANConfigurationTransferIEs);
    UplinkRANConfigurationTransferIEs->id = NGAP_ProtocolIE_ID_id_SONConfigurationTransferDL;
    UplinkRANConfigurationTransferIEs->criticality = NGAP_Criticality_ignore;
    UplinkRANConfigurationTransferIEs->value.present = NGAP_UplinkRANConfigurationTransferIEs__value_PR_SONConfigurationTransfer;
    SONConfigurationTransfer = &UplinkRANConfigurationTransferIEs->value.choice.SONConfigurationTransfer;
#endif

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_upwnlink_ran_status_transfer(pkbuf_t **ngapbuf, ran_ue_t *ran_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UplinkRANStatusTransfer_t *UplinkRANStatusTransfer = NULL;

    NGAP_UplinkRANStatusTransferIEs_t *UplinkRANStatusTransferIEs = NULL;
	  	NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
	    NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_RANStatusTransfer_TransparentContainer_t *RANStatusTransfer_TransparentContainer = NULL;

    d_trace(3, "[AMF] Uplink Ran Status Transfer\n");
    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_UplinkRANStatusTransfer;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_UplinkRANStatusTransfer;
    UplinkRANStatusTransfer = &initiatingMessage->value.choice.UplinkRANStatusTransfer;
    d_assert(UplinkRANStatusTransfer, return CORE_ERROR,);

    UplinkRANStatusTransferIEs = core_calloc(1, sizeof(NGAP_UplinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&UplinkRANStatusTransfer->protocolIEs, UplinkRANStatusTransferIEs);
    UplinkRANStatusTransferIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    UplinkRANStatusTransferIEs->criticality = NGAP_Criticality_reject;
    UplinkRANStatusTransferIEs->value.present = NGAP_UplinkRANStatusTransferIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &UplinkRANStatusTransferIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    UplinkRANStatusTransferIEs = core_calloc(1, sizeof(NGAP_UplinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&UplinkRANStatusTransfer->protocolIEs, UplinkRANStatusTransferIEs);
    UplinkRANStatusTransferIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    UplinkRANStatusTransferIEs->criticality = NGAP_Criticality_reject;
    UplinkRANStatusTransferIEs->value.present = NGAP_UplinkRANStatusTransferIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &UplinkRANStatusTransferIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;
    //This IE may need to be refined. ref: 3841-9.2.3.14
    UplinkRANStatusTransferIEs = core_calloc(1, sizeof(NGAP_UplinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&UplinkRANStatusTransfer->protocolIEs, UplinkRANStatusTransferIEs);
    UplinkRANStatusTransferIEs->id = NGAP_ProtocolIE_ID_id_RANStatusTransfer_TransparentContainer;
    UplinkRANStatusTransferIEs->criticality = NGAP_Criticality_reject;
    UplinkRANStatusTransferIEs->value.present = NGAP_UplinkRANStatusTransferIEs__value_PR_RANStatusTransfer_TransparentContainer;
    RANStatusTransfer_TransparentContainer = &UplinkRANStatusTransferIEs->value.choice.RANStatusTransfer_TransparentContainer;
    d_assert(RANStatusTransfer_TransparentContainer, return CORE_ERROR,);

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_paging(pkbuf_t **ngapbuf, amf_ue_t *amf_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_Paging_t *Paging = NULL;

    NGAP_PagingIEs_t *PagingIEs = NULL;
        NGAP_UEIdentityIndexValue_t	*UEIdentityIndexValue = NULL;
        NGAP_UEPagingIdentity_t	*UEPagingIdentity = NULL;
        NGAP_TAIList_t *TAIList = NULL;
#if 0
NGAP_PagingDRX_t	 PagingDRX;
NGAP_PagingPriority_t	 PagingPriority;
NGAP_UERadioCapabilityForPaging_t	 UERadioCapabilityForPaging;
NGAP_PagingOrigin_t	 PagingOrigin;
NGAP_AssistanceDataForPaging_t	 AssistanceDataForPaging;    
#endif
    d_assert(amf_ue, return CORE_ERROR, "Null param");
    d_trace(3, "[AMF] Paging\n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_Paging;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_Paging;
    Paging = &initiatingMessage->value.choice.Paging;
    d_assert(Paging, return CORE_ERROR,);

     //This IE may need to be refined. ref: 3841-9.2.4.1
    PagingIEs = core_calloc(1, sizeof(NGAP_UplinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&Paging->protocolIEs, PagingIEs);
    PagingIEs->id = NGAP_ProtocolIE_ID_id_UEIdentityIndexValue;
    PagingIEs->criticality = NGAP_Criticality_ignore;
    PagingIEs->value.present = NGAP_PagingIEs__value_PR_UEIdentityIndexValue;
    UEIdentityIndexValue = &PagingIEs->value.choice.UEIdentityIndexValue;
    d_assert(UEIdentityIndexValue, return CORE_ERROR, );

    PagingIEs = core_calloc(1, sizeof(NGAP_UplinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&Paging->protocolIEs, PagingIEs);
    PagingIEs->id = NGAP_ProtocolIE_ID_id_UEPagingIdentity;
    PagingIEs->criticality = NGAP_Criticality_ignore;
    PagingIEs->value.present = NGAP_PagingIEs__value_PR_UEPagingIdentity;
    UEPagingIdentity = &PagingIEs->value.choice.UEPagingIdentity;
        UEPagingIdentity->present = NGAP_UEPagingIdentity_PR_fiveG_S_TMSI;
        NGAP_FiveG_S_TMSI_t *fiveG_S_TMSI = NULL;
        fiveG_S_TMSI = (NGAP_FiveG_S_TMSI_t *)core_calloc(1, sizeof(NGAP_FiveG_S_TMSI_t));
        fiveG_S_TMSI = UEPagingIdentity->choice.fiveG_S_TMSI;
        d_assert(fiveG_S_TMSI, return CORE_ERROR, );
        //  BIT String
        // fiveG_S_TMSI->aMFPointer = amf_ue->guti_5g.amf_ptr;
        //  BIT String
        // fiveG_S_TMSI->aMFSetID = amf_ue->guti_5g.amf_sid;
        // fiveG_S_TMSI->fiveG_TMSI = amf_ue->tmsi_5g;

    PagingIEs = core_calloc(1, sizeof(NGAP_UplinkRANConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&Paging->protocolIEs, PagingIEs);
    PagingIEs->id = NGAP_ProtocolIE_ID_id_TAIList;
    PagingIEs->criticality = NGAP_Criticality_ignore;
    PagingIEs->value.present = NGAP_PagingIEs__value_PR_TAIList;
    TAIList = &PagingIEs->value.choice.TAIList;
        NGAP_TAIItemIEs_t *TAIItemIEs = NULL;
        TAIItemIEs = (NGAP_TAIItemIEs_t *)core_calloc(1, sizeof(NGAP_TAIItemIEs_t));
        ASN_SEQUENCE_ADD(&TAIList->list, TAIItemIEs);
        TAIItemIEs->id = NGAP_ProtocolIE_ID_id_TAIItem;
        TAIItemIEs->criticality = NGAP_Criticality_ignore;
        TAIItemIEs->value.present = NGAP_TAIItemIEs__value_PR_TAIItem;
        NGAP_TAIItem_t *TAIItem = NULL;
        TAIItem = &TAIItemIEs->value.choice.TAIItem;
            ngap_buffer_to_OCTET_STRING(&amf_ue->tai.plmn_id, sizeof(plmn_id_t), &TAIItem->tAI.pLMNIdentity);
            ngap_uint16_to_OCTET_STRING(amf_ue->tai.tac, &TAIItem->tAI.tAC);
    
    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;        
}

status_t ngap_build_reroute_nas_request(pkbuf_t **ngapbuf, amf_ue_t *amf_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_RerouteNASRequest_t *RerouteNASRequest = NULL;

    NGAP_RerouteNASRequest_IEs_t *RerouteNASRequest_IEs = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        OCTET_STRING_t *OCTET_STRING = NULL;
        NGAP_AMFSetID_t	*AMFSetID = NULL;
#if 0
			NGAP_AMF_UE_NGAP_ID_t	 AMF_UE_NGAP_ID;
            NGAP_AllowedNSSAI_t	 AllowedNSSAI;
#endif		


    d_assert(amf_ue, return CORE_ERROR, "Null param");
    d_trace(3, "[AMF] Reroute NAS Request\n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_RerouteNASRequest;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_RerouteNASRequest;
    RerouteNASRequest = &initiatingMessage->value.choice.RerouteNASRequest;
    d_assert(RerouteNASRequest, return CORE_ERROR,);

    RerouteNASRequest_IEs = core_calloc(1, sizeof(NGAP_RerouteNASRequest_IEs_t));
    ASN_SEQUENCE_ADD(&RerouteNASRequest->protocolIEs, RerouteNASRequest_IEs);
    RerouteNASRequest_IEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    RerouteNASRequest_IEs->criticality = NGAP_Criticality_reject;
    RerouteNASRequest_IEs->value.present = NGAP_RerouteNASRequest_IEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &RerouteNASRequest_IEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = amf_ue->ran_ue->ran_ue_ngap_id;

    RerouteNASRequest_IEs = core_calloc(1, sizeof(NGAP_RerouteNASRequest_IEs_t));
    ASN_SEQUENCE_ADD(&RerouteNASRequest->protocolIEs, RerouteNASRequest_IEs);
    RerouteNASRequest_IEs->id = NGAP_ProtocolIE_ID_id_NGAP_Message;
    RerouteNASRequest_IEs->criticality = NGAP_Criticality_reject;
    RerouteNASRequest_IEs->value.present = NGAP_RerouteNASRequest_IEs__value_PR_OCTET_STRING;
    OCTET_STRING = &RerouteNASRequest_IEs->value.choice.OCTET_STRING;
    d_assert(OCTET_STRING, return CORE_ERROR,);

    RerouteNASRequest_IEs = core_calloc(1, sizeof(NGAP_RerouteNASRequest_IEs_t));
    ASN_SEQUENCE_ADD(&RerouteNASRequest->protocolIEs, RerouteNASRequest_IEs);
    RerouteNASRequest_IEs->id = NGAP_ProtocolIE_ID_id_AMFSetID;
    RerouteNASRequest_IEs->criticality = NGAP_Criticality_reject;
    RerouteNASRequest_IEs->value.present = NGAP_RerouteNASRequest_IEs__value_PR_AMFSetID;
    AMFSetID = &RerouteNASRequest_IEs->value.choice.AMFSetID;
    d_assert(AMFSetID, return CORE_ERROR,);
    // TODO: Bit String
    //*AMFSetID = amf_ue->guti_5g.amf_sid;

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_downlink_ue_associated_nrppa_transport(pkbuf_t **ngapbuf, amf_ue_t *amf_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_DownlinkUEAssociatedNRPPaTransport_t *DownlinkUEAssociatedNRPPaTransport = NULL;

    NGAP_DownlinkUEAssociatedNRPPaTransportIEs_t *DownlinkUEAssociatedNRPPaTransportIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_RoutingID_t *RoutingID = NULL;
        NGAP_NRPPa_PDU_t *NRPPa_PDU = NULL;

    d_assert(amf_ue, return CORE_ERROR, "Null param");
    d_trace(3, "[AMF] Downlink Ue Associated NRPPa Transport\n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_DownlinkUEAssociatedNRPPaTransport;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_DownlinkUEAssociatedNRPPaTransport;
    DownlinkUEAssociatedNRPPaTransport = &initiatingMessage->value.choice.DownlinkUEAssociatedNRPPaTransport;
    d_assert(DownlinkUEAssociatedNRPPaTransport, return CORE_ERROR,);

    DownlinkUEAssociatedNRPPaTransportIEs = core_calloc(1, sizeof(NGAP_DownlinkUEAssociatedNRPPaTransportIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkUEAssociatedNRPPaTransport->protocolIEs, DownlinkUEAssociatedNRPPaTransportIEs);
    DownlinkUEAssociatedNRPPaTransportIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    DownlinkUEAssociatedNRPPaTransportIEs->criticality = NGAP_Criticality_reject;
    DownlinkUEAssociatedNRPPaTransportIEs->value.present = NGAP_DownlinkUEAssociatedNRPPaTransportIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &DownlinkUEAssociatedNRPPaTransportIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = amf_ue->ran_ue->amf_ue_ngap_id;

    DownlinkUEAssociatedNRPPaTransportIEs = core_calloc(1, sizeof(NGAP_DownlinkUEAssociatedNRPPaTransportIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkUEAssociatedNRPPaTransport->protocolIEs, DownlinkUEAssociatedNRPPaTransportIEs);
    DownlinkUEAssociatedNRPPaTransportIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    DownlinkUEAssociatedNRPPaTransportIEs->criticality = NGAP_Criticality_reject;
    DownlinkUEAssociatedNRPPaTransportIEs->value.present = NGAP_DownlinkUEAssociatedNRPPaTransportIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &DownlinkUEAssociatedNRPPaTransportIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = amf_ue->ran_ue->ran_ue_ngap_id;

    DownlinkUEAssociatedNRPPaTransportIEs = core_calloc(1, sizeof(NGAP_DownlinkUEAssociatedNRPPaTransportIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkUEAssociatedNRPPaTransport->protocolIEs, DownlinkUEAssociatedNRPPaTransportIEs);
    DownlinkUEAssociatedNRPPaTransportIEs->id = NGAP_ProtocolIE_ID_id_RoutingID;
    DownlinkUEAssociatedNRPPaTransportIEs->criticality = NGAP_Criticality_reject;
    DownlinkUEAssociatedNRPPaTransportIEs->value.present = NGAP_DownlinkUEAssociatedNRPPaTransportIEs__value_PR_RoutingID;
    RoutingID = &DownlinkUEAssociatedNRPPaTransportIEs->value.choice.RoutingID;
    ngap_buffer_to_OCTET_STRING(&amf_ue->guti_5g.amf_rid, 2, RoutingID);
    
    DownlinkUEAssociatedNRPPaTransportIEs = core_calloc(1, sizeof(NGAP_DownlinkUEAssociatedNRPPaTransportIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkUEAssociatedNRPPaTransport->protocolIEs, DownlinkUEAssociatedNRPPaTransportIEs);
    DownlinkUEAssociatedNRPPaTransportIEs->id = NGAP_ProtocolIE_ID_id_NRPPa_PDU;
    DownlinkUEAssociatedNRPPaTransportIEs->criticality = NGAP_Criticality_reject;
    DownlinkUEAssociatedNRPPaTransportIEs->value.present = NGAP_DownlinkUEAssociatedNRPPaTransportIEs__value_PR_NRPPa_PDU;
    NRPPa_PDU = &DownlinkUEAssociatedNRPPaTransportIEs->value.choice.NRPPa_PDU;
    d_assert(NRPPa_PDU, return CORE_ERROR, );

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_downlink_non_ue_associated_nrppa_transport(pkbuf_t **ngapbuf, amf_ue_t *amf_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_DownlinkNonUEAssociatedNRPPaTransport_t *DownlinkNonUEAssociatedNRPPaTransport = NULL;

    NGAP_DownlinkNonUEAssociatedNRPPaTransportIEs_t *DownlinkNonUEAssociatedNRPPaTransportIEs = NULL;
		NGAP_RoutingID_t *RoutingID = NULL;
		NGAP_NRPPa_PDU_t *NRPPa_PDU = NULL;

    d_assert(amf_ue, return CORE_ERROR, "Null param");
    d_trace(3, "[AMF] Downlink Non Ue Associated NRPPa Transport\n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_DownlinkNonUEAssociatedNRPPaTransport;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_DownlinkNonUEAssociatedNRPPaTransport;
    DownlinkNonUEAssociatedNRPPaTransport = &initiatingMessage->value.choice.DownlinkNonUEAssociatedNRPPaTransport;
    d_assert(DownlinkNonUEAssociatedNRPPaTransport, return CORE_ERROR,);

    DownlinkNonUEAssociatedNRPPaTransportIEs = core_calloc(1, sizeof(NGAP_DownlinkNonUEAssociatedNRPPaTransportIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNonUEAssociatedNRPPaTransport->protocolIEs, DownlinkNonUEAssociatedNRPPaTransportIEs);
    DownlinkNonUEAssociatedNRPPaTransportIEs->id = NGAP_ProtocolIE_ID_id_RoutingID;
    DownlinkNonUEAssociatedNRPPaTransportIEs->criticality = NGAP_Criticality_reject;
    DownlinkNonUEAssociatedNRPPaTransportIEs->value.present = NGAP_DownlinkNonUEAssociatedNRPPaTransportIEs__value_PR_RoutingID;
    RoutingID = &DownlinkNonUEAssociatedNRPPaTransportIEs->value.choice.RoutingID;
    ngap_buffer_to_OCTET_STRING(&amf_ue->guti_5g.amf_rid, 2, RoutingID);

    DownlinkNonUEAssociatedNRPPaTransportIEs = core_calloc(1, sizeof(NGAP_DownlinkNonUEAssociatedNRPPaTransportIEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNonUEAssociatedNRPPaTransport->protocolIEs, DownlinkNonUEAssociatedNRPPaTransportIEs);
    DownlinkNonUEAssociatedNRPPaTransportIEs->id = NGAP_ProtocolIE_ID_id_NRPPa_PDU;
    DownlinkNonUEAssociatedNRPPaTransportIEs->criticality = NGAP_Criticality_reject;
    DownlinkNonUEAssociatedNRPPaTransportIEs->value.present = NGAP_DownlinkNonUEAssociatedNRPPaTransportIEs__value_PR_NRPPa_PDU;
    NRPPa_PDU = &DownlinkNonUEAssociatedNRPPaTransportIEs->value.choice.NRPPa_PDU;
    d_assert(NRPPa_PDU, return CORE_ERROR, );

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

status_t ngap_build_location_reporting_control(pkbuf_t **ngapbuf, amf_ue_t *amf_ue, NGAP_EventType_t enentType, NGAP_ReportArea_t reportArea)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_LocationReportingControl_t *LocationReportingControl = NULL;

    NGAP_LocationReportingControlIEs_t *LocationReportingControlIEs = NULL;
		NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
		NGAP_LocationReportingRequestType_t	*LocationReportingRequestType = NULL;

    d_assert(amf_ue, return CORE_ERROR, "Null param");
    d_trace(3, "[AMF] Location Reporting Control \n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_LocationReportingControl;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_LocationReportingControl;
    LocationReportingControl = &initiatingMessage->value.choice.LocationReportingControl;
    d_assert(LocationReportingControl, return CORE_ERROR,);

    LocationReportingControlIEs = core_calloc(1, sizeof(NGAP_LocationReportingControl_t));
    ASN_SEQUENCE_ADD(&LocationReportingControl->protocolIEs, LocationReportingControlIEs);
    LocationReportingControlIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    LocationReportingControlIEs->criticality = NGAP_Criticality_reject;
    LocationReportingControlIEs->value.present = NGAP_LocationReportingControlIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &LocationReportingControlIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = amf_ue->ran_ue->amf_ue_ngap_id;

    LocationReportingControlIEs = core_calloc(1, sizeof(NGAP_LocationReportingControl_t));
    ASN_SEQUENCE_ADD(&LocationReportingControl->protocolIEs, LocationReportingControlIEs);
    LocationReportingControlIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    LocationReportingControlIEs->criticality = NGAP_Criticality_reject;
    LocationReportingControlIEs->value.present = NGAP_LocationReportingControlIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &LocationReportingControlIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = amf_ue->ran_ue->ran_ue_ngap_id;

    LocationReportingControlIEs = core_calloc(1, sizeof(NGAP_LocationReportingControl_t));
    ASN_SEQUENCE_ADD(&LocationReportingControl->protocolIEs, LocationReportingControlIEs);
    LocationReportingControlIEs->id = NGAP_ProtocolIE_ID_id_LocationReportingRequestType;
    LocationReportingControlIEs->criticality = NGAP_Criticality_ignore;
    LocationReportingControlIEs->value.present = NGAP_LocationReportingControlIEs__value_PR_LocationReportingRequestType;
    LocationReportingRequestType = &LocationReportingControlIEs->value.choice.LocationReportingRequestType;
    LocationReportingRequestType->eventType = enentType;
    LocationReportingRequestType->reportArea = reportArea;
    NGAP_AreaOfInterestItem_t *AreaOfInterestItem = NULL;
    AreaOfInterestItem = (NGAP_AreaOfInterestItem_t *) core_calloc(1, sizeof(NGAP_AreaOfInterestItem_t));
    ASN_SEQUENCE_ADD(&LocationReportingRequestType->areaOfInterestList->list, AreaOfInterestItem);
    NGAP_AreaOfInterest_t *AreaOfInterest = NULL;
    AreaOfInterest = &AreaOfInterestItem->areaOfInterest;
        NGAP_AreaOfInterestTAIItem_t *AreaOfInterestTAIItem = NULL;
        AreaOfInterestTAIItem = (NGAP_AreaOfInterestTAIItem_t *) core_calloc(1, sizeof(NGAP_AreaOfInterestTAIItem_t));
        ASN_SEQUENCE_ADD(&AreaOfInterest->areaOfInterestTAIList.list, AreaOfInterestTAIItem);
        ngap_buffer_to_OCTET_STRING(&amf_ue->tai.plmn_id, PLMN_ID_LEN, &AreaOfInterestTAIItem->tAI.pLMNIdentity);
        ngap_buffer_to_OCTET_STRING(&amf_ue->tai.tac, 2, &AreaOfInterestTAIItem->tAI.tAC);
    
    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}

/**
 * AMF -> NG-RAN node
 **/
status_t ngap_build_ue_tnla_binding_release_request(pkbuf_t **ngapbuf, amf_ue_t *amf_ue)
{
    status_t rv;
  
    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_UETNLABindingReleaseRequest_t *UETNLABindingReleaseRequest = NULL;
    
    NGAP_UETNLABindingReleaseRequestIEs_t *UETNLABindingReleaseRequestIEs = NULL;
        NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
        NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;

    d_assert(amf_ue, return CORE_ERROR, "Null param");
    d_trace(3, "[AMF] Downlink Non Ue Associated NRPPa Transport\n");

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = NGAP_ProcedureCode_id_UETNLABindingRelease;
    initiatingMessage->criticality = NGAP_Criticality_ignore;
    initiatingMessage->value.present = NGAP_InitiatingMessage__value_PR_UETNLABindingReleaseRequest;
    UETNLABindingReleaseRequest = &initiatingMessage->value.choice.UETNLABindingReleaseRequest;
    d_assert(UETNLABindingReleaseRequest, return CORE_ERROR,);

    UETNLABindingReleaseRequestIEs = core_calloc(1, sizeof(NGAP_UETNLABindingReleaseRequestIEs_t));
    ASN_SEQUENCE_ADD(&UETNLABindingReleaseRequest->protocolIEs, UETNLABindingReleaseRequestIEs);
    UETNLABindingReleaseRequestIEs->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    UETNLABindingReleaseRequestIEs->criticality = NGAP_Criticality_reject;
    UETNLABindingReleaseRequestIEs->value.present = NGAP_UETNLABindingReleaseRequestIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &UETNLABindingReleaseRequestIEs->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = amf_ue->ran_ue->amf_ue_ngap_id;

    UETNLABindingReleaseRequestIEs = core_calloc(1, sizeof(NGAP_UETNLABindingReleaseRequestIEs_t));
    ASN_SEQUENCE_ADD(&UETNLABindingReleaseRequest->protocolIEs, UETNLABindingReleaseRequestIEs);
    UETNLABindingReleaseRequestIEs->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    UETNLABindingReleaseRequestIEs->criticality = NGAP_Criticality_reject;
    UETNLABindingReleaseRequestIEs->value.present = NGAP_UETNLABindingReleaseRequestIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &UETNLABindingReleaseRequestIEs->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = amf_ue->ran_ue->ran_ue_ngap_id;

    rv = ngap_encode_pdu(ngapbuf, &pdu);
    ngap_free_pdu(&pdu);
    
    if (rv != CORE_OK)
    {
        d_error("ngap_encode_pdu() failed");
        return CORE_ERROR;
    }
    return CORE_OK;
}