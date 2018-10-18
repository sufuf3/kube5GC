#define TRACE_MODULE _ngap_build

#include "core_debug.h"

#include "3gpp_types.h"
#include "ngap_conv.h"
#include "ngap_build.h"

status_t ngap_build_setup_rsp(pkbuf_t **pkbuf)
{
    //status_t rv;
    int i, j;

    NGAP_NGAP_PDU_t pdu;
    NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    NGAP_NGSetupResponse_t *NGSetupResponse = NULL;
    
    NGAP_NGSetupResponseIEs_t *ie = NULL;
    //NGAP_AMFName_t *AMFName = NULL;
    NGAP_ServedGUAMIList_t *ServedGUAMIList = NULL;
    //NGAP_RelativeAMFCapacity_t *RelativeAMFCapacity = NULL;
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

    ie = core_calloc(1, sizeof(NGAP_NGSetupResponseIEs_t));
    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMFName;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_AMFName;
    //AMFName = &ie->value.choice.AMFName;

    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_ServedGUAMIList;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_ServedGUAMIList;
    ServedGUAMIList = &ie->value.choice.ServedGUAMIList;

    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RelativeAMFCapacity;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_RelativeAMFCapacity;
    //RelativeAMFCapacity = &ie->value.choice.RelativeAMFCapacity;

    ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PLMNSupportList;
    ie->criticality = NGAP_Criticality_ignore;
    ie->value.present = NGAP_NGSetupResponseIEs__value_PR_PLMNSupportList;
    PLMNSupportList = &ie->value.choice.PLMNSupportList;

    for (i = 0; i < mme_self()->max_num_of_served_guami; i++)
    {   
        NGAP_ServedGUAMIItem_t *ServedGUAMIItem = NULL;
        ServedGUAMIItem = (NGAP_ServedGUAMIItem_t *) core_calloc(1, sizeof(NGAP_ServedGUAMIItem_t));

        served_guami_t *served_guami = &mme_self()->served_guami[i];
        //for (j = 0; j < served_guami->num_of_plmn_id, j++)
        //{
        
        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        ngap_buffer_to_OCTET_STRING(&served_guami->plmn_id, PLMN_ID_LEN, &ServedGUAMIItem->gUAMI.pLMNIdentity);
        
        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        // ASN_SEQUENCE_ADD(&ServedGUAMIItem->gUAMI.aMFRegionID, aMFRegionID);

        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        // ASN_SEQUENCE_ADD(&ServedGUAMIItem->gUAMI.aMFSetID, aMFSetID);
            
        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        // ASN_SEQUENCE_ADD(&ServedGUAMIItem->gUAMI.aMFPointer, aMFPointer);

        ASN_SEQUENCE_ADD(&ServedGUAMIList->list, ServedGUAMIItem);
    }
    
    for (i = 0; i < mme_self()->max_num_of_plmn_support; i++)
    {
        NGAP_PLMNSupportItem_t *PLMNSupportItem = NULL;
        PLMNSupportItem = (NGAP_PLMNSupportItem_t *) core_calloc(1, sizeof(NGAP_PLMNSupportItem_t));

        plmn_support_t *plmn_support = &mme_self()->plmn_support[i];

	    NGAP_PLMNIdentity_t	*pLMNIdentity = NULL;
        pLMNIdentity = (NGAP_PLMNIdentity_t *) core_calloc(1, sizeof(NGAP_PLMNIdentity_t));
        ngap_buffer_to_OCTET_STRING(&plmn_support->plmn_id[i], PLMN_ID_LEN, pLMNIdentity);
        //TODO: need to check 
        ASN_SEQUENCE_ADD(&PLMNSupportItem->pLMNIdentity, pLMNIdentity);

        NGAP_SliceSupportList_t	*sliceSupportList = NULL;
        sliceSupportList = (NGAP_SliceSupportList_t *) core_calloc(1, sizeof(NGAP_SliceSupportList_t));
        for (j = 0; j < mme_self()->plmn_support->num_of_s_nssai; j++)
        {         
             NGAP_SliceSupportItem_t *SliceSupportItem = NULL;
            // SliceSupportItem = (NGAP_SliceSupportItem_t *) core_calloc(1, sizeof(NGAP_SliceSupportItem_t));
            // memcpy( SliceSupportItem->s_NSSAI.sST, &plmn_support->s_nssai[j].sst,SST_LEN );
            ngap_buffer_to_OCTET_STRING(&plmn_support->s_nssai[j].sst, SST_LEN, &SliceSupportItem->s_NSSAI.sST);
           
           ASN_SEQUENCE_ADD(&sliceSupportList->list, SliceSupportItem);
        }


        ASN_SEQUENCE_ADD(&PLMNSupportList->list, PLMNSupportItem);
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
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = ran_ue->ran_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_GUAMI;
    GUAMI = &ie->value.choice.GUAMI;
    /* which guami plmn/rid/ should be selete ?*/
    served_guami_t *served_guami = &mme_self()->served_guami[0];
    ngap_buffer_to_OCTET_STRING( &served_guami->plmn_id[0], 3, &GUAMI->pLMNIdentity);
    ngap_buffer_to_OCTET_STRING( &served_guami->amf_rid[0], 3, &GUAMI->aMFRegionID);
	//ngap_buffer_to_OCTET_STRING( &served_guami->amf_sid[0], 3, &GUAMI->aMFSetID);  //BIT_STRING_t
	// ngap_buffer_to_OCTET_STRING( &served_guami->amf_ptr[0], 3, &GUAMI->aMFPointer); //BIT_STRING_t

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_PDUSessionResourceSetupListCxtReq;
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
    AllowedNSSAI = &ie->value.choice.AllowedNSSAI;
    NGAP_AllowedNSSAI_Item_t *AllowedNSSAI_Item = NULL;
    for (i = 0 ; i < mme_self()->plmn_support->num_of_s_nssai; i++)
    {
        plmn_support_t *plmn_support = &mme_self()->plmn_support[i];
        AllowedNSSAI_Item = (NGAP_AllowedNSSAI_Item_t *) core_calloc(1, sizeof(NGAP_AllowedNSSAI_Item_t));
        ngap_buffer_to_OCTET_STRING(&plmn_support->s_nssai[i].sst, SST_LEN, &AllowedNSSAI_Item->s_NSSAI.sST);
        ASN_SEQUENCE_ADD(&AllowedNSSAI->list, AllowedNSSAI_Item);
    }       

    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_UESecurityCapabilities;
    UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;
	
    //nas_ue_network_capability_t ue_network_capability
    memcpy(&UESecurityCapabilities->nRencryptionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
    memcpy(&UESecurityCapabilities->nRintegrityProtectionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
    memcpy(&UESecurityCapabilities->eUTRAencryptionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
    memcpy(&UESecurityCapabilities->eUTRAintegrityProtectionAlgorithms, "\xff\xff", 2);  //BIT_STRING_t
	
    ie = core_calloc(1, sizeof(NGAP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_SecurityKey;
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
//TODO: fix the api input
status_t ngap_build_pdu_session_resource_setup_request(pkbuf_t **ngapbuf)
{
    int i = 0;
    status_t rv = 0;

    NGAP_NGAP_PDU_t pdu;
    NGAP_InitiatingMessage_t *initiatingMessage = NULL;
    NGAP_PDUSessionResourceSetupRequest_t *PDUSessionResourceSetupRequest = NULL;

    NGAP_PDUSessionResourceSetupRequestIEs_t *ie = NULL;
        // NGAP_AMF_UE_NGAP_ID_t *AMF_UE_NGAP_ID = NULL;
		// NGAP_RAN_UE_NGAP_ID_t *RAN_UE_NGAP_ID = NULL;
        NGAP_PDUSessionResourceSetupListSUReq_t *PDUSessionResourceSetupListSUReq = NULL;

    memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    pdu.present = NGAP_NGAP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(NGAP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        NGAP_ProcedureCode_id_PDUSessionResourceSetup;
    initiatingMessage->criticality = NGAP_Criticality_reject;
    initiatingMessage->value.present =
        NGAP_InitiatingMessage__value_PR_PDUSessionResourceSetupRequest;
    
    PDUSessionResourceSetupRequest = 
        &initiatingMessage->value.choice.PDUSessionResourceSetupRequest;
    
    // ie = core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupRequestIEs_t));
    // ASN_SEQUENCE_ADD(&PDUSessionResourceSetupRequest->protocolIEs, ie);
    // ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    // AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    
    // ie = core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupRequestIEs_t));
    // ASN_SEQUENCE_ADD(&PDUSessionResourceSetupRequest->protocolIEs, ie);
    // ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    // RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    
    ie = core_calloc(1, sizeof(NGAP_PDUSessionResourceSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&PDUSessionResourceSetupRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
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
	    ngap_buffer_to_OCTET_STRING(&mme_self()->plmn_support->s_nssai[0].sst, SST_LEN, &PDUSessionResourceSetupItemCxtReq->s_NSSAI.sST);
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
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = ran_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationRequest->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
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
        d_error("s1ap_encode_pdu() failed");
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

status_t ngap_build_handover_cancel_acknowledge(pkbuf_t **ngapbuf, ran_ue_t *sourece_ue)
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

    ie = core_calloc(1, sizeof(NGAP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCancel->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_AMF_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCancelAcknowledgeIEs__value_PR_AMF_UE_NGAP_ID;
    AMF_UE_NGAP_ID = &ie->value.choice.AMF_UE_NGAP_ID;
    *AMF_UE_NGAP_ID = sourece_ue->amf_ue_ngap_id;

    ie = core_calloc(1, sizeof(NGAP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCancel->protocolIEs, ie);
    ie->id = NGAP_ProtocolIE_ID_id_RAN_UE_NGAP_ID;
    ie->criticality = NGAP_Criticality_reject;
    ie->value.present = NGAP_HandoverCancelAcknowledgeIEs__value_PR_RAN_UE_NGAP_ID;
    RAN_UE_NGAP_ID = &ie->value.choice.RAN_UE_NGAP_ID;
    *RAN_UE_NGAP_ID = sourece_ue->ran_ue_ngap_id;
    
    d_trace(5, "    Source : RAN_UE_NGAP_ID[%d] AMF_UE_NGAP_ID[%d]\n",
            sourece_ue->ran_ue_ngap_id, sourece_ue->amf_ue_ngap_id);
  
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
