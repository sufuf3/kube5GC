#define TRACE_MODULE _s1ap_build

#include "core_debug.h"

#include "fd/s6a/s6a_message.h"

#include "amf4g_context.h"

#include "amf4g_kdf.h"
#include "s1ap_build.h"
#include "s1ap_conv.h"

status_t s1ap_build_ue_radio_capability_match_request(
    pkbuf_t **s1apbuf, enb_ue_t *enb_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UERadioCapabilityMatchRequest_t *UERadioCapabilityMatchRequest = NULL;

    S1AP_UERadioCapabilityMatchRequestIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    // S1AP_UERadioCapability_t *UERadioCapability = NULL;//Optional
    

    d_assert(enb_ue, return CORE_ERROR, "Null param");

    d_trace(1, "[AMF] UE Radio Capability Match Request\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));

    //Message Type: reject
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));
    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_UERadioCapabilityMatch;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_UERadioCapabilityMatchRequest;

    UERadioCapabilityMatchRequest =
        &initiatingMessage->value.choice.UERadioCapabilityMatchRequest;

    //MME UE S1AP ID: reject
    ie = core_calloc(1, sizeof(S1AP_UERadioCapabilityMatchRequestIEs_t));
    ASN_SEQUENCE_ADD(&UERadioCapabilityMatchRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UERadioCapabilityMatchRequestIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    //ENB UE S1AP ID: reject
    ie = core_calloc(1, sizeof(S1AP_UERadioCapabilityMatchRequestIEs_t));
    ASN_SEQUENCE_ADD(&UERadioCapabilityMatchRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UERadioCapabilityMatchRequestIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    //UE Radio Capability: ignore
    /*ie = core_calloc(1, sizeof(S1AP_UERadioCapabilityMatchRequestIEs_t));
    ASN_SEQUENCE_ADD(&UERadioCapabilityMatchRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_UERadioCapability;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_UERadioCapabilityMatchRequestIEs__value_PR_UERadioCapability;

    UERadioCapability = &ie->value.choice.UERadioCapability;*/

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    //s1ap_buffer_to_OCTET_STRING(enb_ue,sizeof(enb_ue_t),OCTET_STRING);
 
    //s1ap_uint32_to_OCTET_STRING(1, UERadioCapability);//0 for test, failed

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_e_rab_modification_confirm(
            pkbuf_t **s1apbuf, enb_ue_t *enb_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_E_RABModificationConfirm_t *E_RABModificationConfirm = NULL;

    S1AP_E_RABModificationConfirmIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    d_trace(1, "[AMF] E-RAB Modification Confirm\n");
    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    //Message Type: reject
    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_E_RABModificationIndication;//same as modification indication
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_E_RABModificationConfirm;

    E_RABModificationConfirm = &successfulOutcome->value.choice.E_RABModificationConfirm;

    //MME UE S1AP ID: ignore
    ie = core_calloc(1, sizeof(S1AP_E_RABModificationConfirmIEs_t));
    ASN_SEQUENCE_ADD(&E_RABModificationConfirm->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_E_RABModificationConfirmIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    //eNB UE S1AP ID: ignore
    ie = core_calloc(1, sizeof(S1AP_E_RABModificationConfirmIEs_t));
    ASN_SEQUENCE_ADD(&E_RABModificationConfirm->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_E_RABModificationConfirmIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    //debug place
    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_mme_cp_relocation_indication(
        pkbuf_t **s1apbuf, enb_ue_t *source_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
	S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_MMECPRelocationIndication_t *MMECPRelocationIndication = NULL;

    S1AP_MMECPRelocationIndicationIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    d_assert(s1apbuf, return CORE_ERROR,);
    d_assert(source_ue, return CORE_ERROR,);

    d_trace(3, "[AMF] AMF4G CP relocation indication\n");


	memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_MMECPRelocationIndication;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_MMECPRelocationIndication;

    MMECPRelocationIndication =
        &initiatingMessage->value.choice.MMECPRelocationIndication;


    ie = core_calloc(1, sizeof(S1AP_MMECPRelocationIndicationIEs_t));
    ASN_SEQUENCE_ADD(&MMECPRelocationIndication->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_MMECPRelocationIndicationIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_MMECPRelocationIndicationIEs_t));
    ASN_SEQUENCE_ADD(&MMECPRelocationIndication->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_MMECPRelocationIndicationIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = source_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = source_ue->enb_ue_s1ap_id;

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_reroute_nas_request(
            pkbuf_t **s1apbuf, enb_ue_t *enb_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_RerouteNASRequest_t *RerouteNASRequest = NULL;

    S1AP_RerouteNASRequest_IEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    OCTET_STRING_t* OCTET_STRING = NULL;
    S1AP_MME_Group_ID_t *MME_Group_ID = NULL;

    d_assert(enb_ue, return CORE_ERROR, "Null param");

    d_trace(1, "[AMF] Reroute NAS request\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));

    //Message Type: reject
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));
    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_RerouteNASRequest;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_RerouteNASRequest;

    RerouteNASRequest =
        &initiatingMessage->value.choice.RerouteNASRequest;

    //MME UE S1AP ID: reject
    ie = core_calloc(1, sizeof(S1AP_RerouteNASRequest_IEs_t));
    ASN_SEQUENCE_ADD(&RerouteNASRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_RerouteNASRequest_IEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    //ENB UE S1AP ID: ignore
    ie = core_calloc(1, sizeof(S1AP_RerouteNASRequest_IEs_t));
    ASN_SEQUENCE_ADD(&RerouteNASRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_RerouteNASRequest_IEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    //S1 Message: reject
    ie = core_calloc(1, sizeof(S1AP_RerouteNASRequest_IEs_t) + sizeof(enb_ue_t));
    ASN_SEQUENCE_ADD(&RerouteNASRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_S1_Message;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_RerouteNASRequest_IEs__value_PR_OCTET_STRING;

    OCTET_STRING = &ie->value.choice.OCTET_STRING;
    
    //MME Group ID: reject
    ie = core_calloc(1, sizeof(S1AP_RerouteNASRequest_IEs_t));
    ASN_SEQUENCE_ADD(&RerouteNASRequest->protocolIEs, ie);
    
    ie->id = S1AP_ProtocolIE_ID_id_MME_Group_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_RerouteNASRequest_IEs__value_PR_MME_Group_ID;

    MME_Group_ID = &ie->value.choice.MME_Group_ID;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    s1ap_buffer_to_OCTET_STRING(enb_ue,sizeof(enb_ue_t),OCTET_STRING);
 
    s1ap_uint16_to_OCTET_STRING(100, MME_Group_ID);//100 for test

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_enb_configuration_update_acknowledge(pkbuf_t **pkbuf){

	status_t rv;

	S1AP_S1AP_PDU_t pdu;
	S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;

	memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));


	successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_ENBConfigurationUpdate;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_ENBConfigurationUpdateAcknowledge;

	rv = s1ap_encode_pdu(pkbuf, &pdu);
	s1ap_free_pdu(&pdu);

	if (rv != CORE_OK)
	{
		d_error("s1ap_encode_pdu() failed");
		return CORE_ERROR;
	}

	return CORE_OK;

}

status_t s1ap_build_enb_configuration_update_failure(
        pkbuf_t **pkbuf, S1AP_Cause_PR group, long cause, long time_to_wait)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_ENBConfigurationUpdateFailure_t *ENBConfigurationUpdateFailure = NULL;

    S1AP_ENBConfigurationUpdateFailureIEs_t *ie = NULL;
    S1AP_Cause_t *Cause = NULL;
    S1AP_TimeToWait_t *TimeToWait = NULL;
    
    d_trace(5, "    Group[%d] Cause[%d] TimeToWait[%ld]\n",
            group, cause, time_to_wait);

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = 
        core_calloc(1, sizeof(S1AP_UnsuccessfulOutcome_t));

    unsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    unsuccessfulOutcome->procedureCode = S1AP_ProcedureCode_id_ENBConfigurationUpdate;
    unsuccessfulOutcome->criticality = S1AP_Criticality_reject;
    unsuccessfulOutcome->value.present =
        S1AP_UnsuccessfulOutcome__value_PR_ENBConfigurationUpdateFailure;

    ENBConfigurationUpdateFailure = &unsuccessfulOutcome->value.choice.ENBConfigurationUpdateFailure;

    ie = core_calloc(1, sizeof(S1AP_ENBConfigurationUpdateFailureIEs_t));
    ASN_SEQUENCE_ADD(&ENBConfigurationUpdateFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_ENBConfigurationUpdateFailureIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    if (time_to_wait > -1)
    {
        ie = core_calloc(1, sizeof(S1AP_ENBConfigurationUpdateFailureIEs_t));
        ASN_SEQUENCE_ADD(&ENBConfigurationUpdateFailure->protocolIEs, ie);

        ie->id = S1AP_ProtocolIE_ID_id_TimeToWait;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present = S1AP_ENBConfigurationUpdateFailureIEs__value_PR_Cause;

        TimeToWait = &ie->value.choice.TimeToWait;
    }

    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    if (TimeToWait)
        *TimeToWait = time_to_wait;

    rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_ue_context_suspend_response(
        pkbuf_t **pkbuf,
	S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_UEContextSuspendResponse_t *UEContextSuspendResponse = NULL;

    S1AP_UEContextSuspendResponseIEs_t *ie = NULL;
	S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
	S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_UEContextSuspend;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_S1SetupResponse;

    UEContextSuspendResponse = &successfulOutcome->value.choice.UEContextSuspendResponse;

    ie = core_calloc(1, sizeof(S1AP_UEContextSuspendResponseIEs_t));
    ASN_SEQUENCE_ADD(&UEContextSuspendResponse->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_UEContextSuspendResponseIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_UEContextSuspendResponseIEs_t));
    ASN_SEQUENCE_ADD(&UEContextSuspendResponse->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_UEContextSuspendResponseIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    *ENB_UE_S1AP_ID = *enb_ue_s1ap_id;
    *MME_UE_S1AP_ID = *mme_ue_s1ap_id;


    rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;

}
status_t s1ap_build_ERAB_release_command(pkbuf_t **s1apbuf,
    c_uint32_t enb_ue_s1ap_id, c_uint32_t mme_ue_s1ap_id)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_E_RABReleaseCommand_t *E_RABReleaseCommand = NULL;

    S1AP_E_RABReleaseCommandIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    d_trace(3, "[AMF] ERAB release command failure\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_E_RABRelease;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_E_RABReleaseCommand;

    E_RABReleaseCommand =
        &initiatingMessage->value.choice.E_RABReleaseCommand;

    ie = core_calloc(1, sizeof(S1AP_E_RABReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&E_RABReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_E_RABReleaseCommandIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_E_RABReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&E_RABReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_E_RABReleaseCommandIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    *MME_UE_S1AP_ID = mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue_s1ap_id;
	
    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

// MME Configuration Update
status_t s1ap_build_mme_configuration_update(pkbuf_t **pkbuf)
{
	status_t rv;
	int i,j;

	S1AP_S1AP_PDU_t pdu;


	// MMEConfigurationUpdate is a part of initiatingMessage
	S1AP_InitiatingMessage_t *initiatingMessage;
	S1AP_MMEConfigurationUpdate_t *MMEConfigurationUpdate = NULL;
	S1AP_MMEConfigurationUpdateIEs_t *ie = NULL;

	/********************ie include*********************/
	// Optional
	// S1AP_MMEname_t *MMEname = NULL;
	// S1AP_RelativeMMECapacity_t *RelativeMMECapacity = NULL;
	 

	// Mandatory
	S1AP_ServedGUMMEIs_t *ServedGUMMEIs = NULL;
	//S1AP_ServedDCNs_t *ServedDCNs = NULL;
	/***************************************************/
	/*S1AP_ServedPLMNs_t *ServedPLMNs = NULL;
	S1AP_ServedGroupIDs_t *ServedGroupIDs = NULL;
	S1AP_ServedMMECs_t *ServedMMECs = NULL;
	S1AP_MME_Code_t *MME_Code = NULL;*/

	memset(&pdu,0,sizeof(S1AP_S1AP_PDU_t));
	pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
	pdu.choice.initiatingMessage = core_calloc(1,sizeof(S1AP_InitiatingMessage_t));
	initiatingMessage = pdu.choice.initiatingMessage;//pointer point to it
	initiatingMessage->procedureCode = S1AP_ProcedureCode_id_MMEConfigurationUpdate;
	initiatingMessage->criticality = S1AP_Criticality_reject;
	initiatingMessage->value.present = S1AP_InitiatingMessage__value_PR_MMEConfigurationUpdate;

	MMEConfigurationUpdate = &initiatingMessage->value.choice.MMEConfigurationUpdate;

	// allocate each IE
	//S1AP_ProtocolIE_ID_id_ServedDCNs;
	
	// Gummeis
	ie = core_calloc(1, sizeof(S1AP_MMEConfigurationUpdateIEs_t));
    ASN_SEQUENCE_ADD(&MMEConfigurationUpdate->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_ServedGUMMEIs;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_S1SetupResponseIEs__value_PR_ServedGUMMEIs;
	ServedGUMMEIs = &ie->value.choice.ServedGUMMEIs;

	
	for (i = 0; i < amf4g_self()->max_num_of_served_gummei; i++)
    {
        S1AP_ServedGUMMEIsItem_t *ServedGUMMEIsItem = NULL;
        ServedGUMMEIsItem = (S1AP_ServedGUMMEIsItem_t *)
            core_calloc(1, sizeof(S1AP_ServedGUMMEIsItem_t));

        served_gummei_t *served_gummei = &amf4g_self()->served_gummei[i];
        for (j = 0; j < served_gummei->num_of_plmn_id; j++)
        {
            S1AP_PLMNidentity_t *PLMNidentity = NULL;
            PLMNidentity = (S1AP_PLMNidentity_t *)
                core_calloc(1, sizeof(S1AP_PLMNidentity_t));
            s1ap_buffer_to_OCTET_STRING(
                    &served_gummei->plmn_id[j], PLMN_ID_LEN, PLMNidentity);
            ASN_SEQUENCE_ADD(
                    &ServedGUMMEIsItem->servedPLMNs.list, PLMNidentity);
            d_trace(5, "    PLMN_ID[MCC:%d MNC:%d]\n",
                plmn_id_mcc(&served_gummei->plmn_id[j]),
                plmn_id_mnc(&served_gummei->plmn_id[j]));
        }

        for (j = 0; j < served_gummei->num_of_amf4g_gid; j++)
        {
            S1AP_MME_Group_ID_t *MME_Group_ID = NULL;
            MME_Group_ID = (S1AP_MME_Group_ID_t *)
                core_calloc(1, sizeof(S1AP_MME_Group_ID_t));
            s1ap_uint16_to_OCTET_STRING(
                    served_gummei->amf4g_gid[j], MME_Group_ID);
            ASN_SEQUENCE_ADD(
                    &ServedGUMMEIsItem->servedGroupIDs.list, MME_Group_ID);
            d_trace(5, "    AMF4G Group[%d]\n", served_gummei->amf4g_gid[j]);
        }

        for (j = 0; j < served_gummei->num_of_amf4g_code; j++)
        {
            S1AP_MME_Code_t *MME_Code = NULL ;
            MME_Code = (S1AP_MME_Code_t *)
                core_calloc(1, sizeof(S1AP_MME_Code_t));
            s1ap_uint8_to_OCTET_STRING(served_gummei->amf4g_code[j], MME_Code);
            ASN_SEQUENCE_ADD(&ServedGUMMEIsItem->servedMMECs.list, MME_Code);
            d_trace(5, "    AMF4G Code[%d]\n", served_gummei->amf4g_code[j]);
        }
        ASN_SEQUENCE_ADD(&ServedGUMMEIs->list, ServedGUMMEIsItem);
	}
	rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
	
}

status_t s1ap_build_setup_rsp(pkbuf_t **pkbuf)
{
    
    status_t rv;
    int i, j;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_S1SetupResponse_t *S1SetupResponse = NULL;

    S1AP_S1SetupResponseIEs_t *ie = NULL;
    S1AP_ServedGUMMEIs_t *ServedGUMMEIs = NULL;
    S1AP_RelativeMMECapacity_t *RelativeMMECapacity = NULL;

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_S1Setup;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_S1SetupResponse;

    S1SetupResponse = &successfulOutcome->value.choice.S1SetupResponse;

    ie = core_calloc(1, sizeof(S1AP_S1SetupResponseIEs_t));
    ASN_SEQUENCE_ADD(&S1SetupResponse->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_ServedGUMMEIs;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_S1SetupResponseIEs__value_PR_ServedGUMMEIs;

    ServedGUMMEIs = &ie->value.choice.ServedGUMMEIs;

    ie = core_calloc(1, sizeof(S1AP_S1SetupResponseIEs_t));
    ASN_SEQUENCE_ADD(&S1SetupResponse->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_RelativeMMECapacity;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_S1SetupResponseIEs__value_PR_RelativeMMECapacity;

    RelativeMMECapacity = &ie->value.choice.RelativeMMECapacity;

    for (i = 0; i < amf4g_self()->max_num_of_served_gummei; i++)
    {
        S1AP_ServedGUMMEIsItem_t *ServedGUMMEIsItem = NULL;
        ServedGUMMEIsItem = (S1AP_ServedGUMMEIsItem_t *)
            core_calloc(1, sizeof(S1AP_ServedGUMMEIsItem_t));

        served_gummei_t *served_gummei = &amf4g_self()->served_gummei[i];
        for (j = 0; j < served_gummei->num_of_plmn_id; j++)
        {
            S1AP_PLMNidentity_t *PLMNidentity = NULL;
            PLMNidentity = (S1AP_PLMNidentity_t *)
                core_calloc(1, sizeof(S1AP_PLMNidentity_t));
            s1ap_buffer_to_OCTET_STRING(
                    &served_gummei->plmn_id[j], PLMN_ID_LEN, PLMNidentity);
            ASN_SEQUENCE_ADD(
                    &ServedGUMMEIsItem->servedPLMNs.list, PLMNidentity);
            d_trace(5, "    PLMN_ID[MCC:%d MNC:%d]\n",
                plmn_id_mcc(&served_gummei->plmn_id[j]),
                plmn_id_mnc(&served_gummei->plmn_id[j]));
        }

        for (j = 0; j < served_gummei->num_of_amf4g_gid; j++)
        {
            S1AP_MME_Group_ID_t *MME_Group_ID = NULL;
            MME_Group_ID = (S1AP_MME_Group_ID_t *)
                core_calloc(1, sizeof(S1AP_MME_Group_ID_t));
            s1ap_uint16_to_OCTET_STRING(
                    served_gummei->amf4g_gid[j], MME_Group_ID);
            ASN_SEQUENCE_ADD(
                    &ServedGUMMEIsItem->servedGroupIDs.list, MME_Group_ID);
            d_trace(5, "    AMF4G Group[%d]\n", served_gummei->amf4g_gid[j]);
        }

        for (j = 0; j < served_gummei->num_of_amf4g_code; j++)
        {
            S1AP_MME_Code_t *MME_Code = NULL ;
            MME_Code = (S1AP_MME_Code_t *)
                core_calloc(1, sizeof(S1AP_MME_Code_t));
            s1ap_uint8_to_OCTET_STRING(served_gummei->amf4g_code[j], MME_Code);
            ASN_SEQUENCE_ADD(&ServedGUMMEIsItem->servedMMECs.list, MME_Code);
            d_trace(5, "    AMF4G Code[%d]\n", served_gummei->amf4g_code[j]);
        }
        ASN_SEQUENCE_ADD(&ServedGUMMEIs->list, ServedGUMMEIsItem);
    }

    *RelativeMMECapacity = amf4g_self()->relative_capacity;

    rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);
    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_overload_start(pkbuf_t **pkbuf)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_OverloadStart_t *OverloadStart = NULL;
    S1AP_OverloadStartIEs_t *ie = NULL;

    // Mandatory IEs
    S1AP_OverloadResponse_t *OverloadResponse;

    // Optional IEs
    // S1AP_GUMMEIList_t *GUMMEIList;
    // S1AP_TrafficLoadReductionIndication_t *TrafficLoadReductionIndication;
    
    // PDU
    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    // IE: Message Type
    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_OverloadStart;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_OverloadStart;
    OverloadStart = &initiatingMessage->value.choice.OverloadStart;

    // IE: Overload Response
    ie = core_calloc(1, sizeof(S1AP_OverloadStartIEs_t));
    ASN_SEQUENCE_ADD(&OverloadStart->protocolIEs, ie);
    
    ie->id = S1AP_ProtocolIE_ID_id_OverloadResponse;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_OverloadStartIEs__value_PR_OverloadResponse;

    // IE: Overload Response > Overload Action (TS23.401-f40: Ch4.3.7.4.1)
    OverloadResponse = &ie->value.choice.OverloadResponse;
    OverloadResponse->present = S1AP_OverloadResponse_PR_overloadAction;
    OverloadResponse->choice.overloadAction = S1AP_OverloadAction_reject_non_emergency_mo_dt;

    rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_overload_stop(pkbuf_t **pkbuf){

    status_t rv;
    S1AP_S1AP_PDU_t pdu;

    /************ InitiatingMessage ***************/
	S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    // S1AP_OverloadStop_t *OverloadStop = NULL;
    //S1AP_OverloadStopIEs_t *ie = NULL;

    /******************** IE *********************/
    //S1AP_GUMMEIList_t *GUMMEIList = NULL;
    

    /**************** Message type: reject *******************/
    memset(&pdu,0,sizeof(S1AP_S1AP_PDU_t));
	pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
	pdu.choice.initiatingMessage = core_calloc(1,sizeof(S1AP_InitiatingMessage_t));
	initiatingMessage = pdu.choice.initiatingMessage;//pointer point to it
	initiatingMessage->procedureCode = S1AP_ProcedureCode_id_OverloadStop;
	initiatingMessage->criticality = S1AP_Criticality_reject;
	initiatingMessage->value.present = S1AP_InitiatingMessage__value_PR_OverloadStop;
	// OverloadStop = &initiatingMessage->value.choice.OverloadStop;

    /********* GUMMEIList: ignore (optional) *********/
    /*ie = core_calloc(1, sizeof(S1AP_OverloadStopIEs_t));
    ASN_SEQUENCE_ADD(&OverloadStop->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_GUMMEIList;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_OverloadStopIEs__value_PR_GUMMEIList;
	GUMMEIList = &ie->value.choice.GUMMEIList;*/

    /****************** Encode **********************/
    rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_write_replace_warning_request(pkbuf_t **pkbuf)
{
	status_t rv;
	S1AP_S1AP_PDU_t pdu;

	/************ InitiatingMessage ***************/
	S1AP_InitiatingMessage_t *initiatingMessage;
	S1AP_WriteReplaceWarningRequest_t *WriteReplaceWarningRequest = NULL;
	S1AP_WriteReplaceWarningRequestIEs_t *ie = NULL;

	/******************** IE *********************/ 
	/* Mandatory */
	S1AP_MessageIdentifier_t *MessageIdentifier = NULL;
	S1AP_SerialNumber_t *SerialNumber = NULL;
	S1AP_RepetitionPeriod_t	 *RepetitionPeriod = NULL;
	S1AP_NumberofBroadcastRequest_t	 *NumberofBroadcastRequest = NULL;

	/* Optional */
	//S1AP_WarningAreaList_t *WarningAreaList = NULL ;
	//S1AP_ExtendedRepetitionPeriod_t *ExtendedRepetitionPeriod = NULL ;
	//S1AP_WarningType_t *WarningType = NULL ;
	//S1AP_WarningSecurityInfo_t *WarningSecurityInfo = NULL ;
	//S1AP_DataCodingScheme_t *DataCodingScheme = NULL ;
	//S1AP_WarningMessageContents_t *WarningMessageContents = NULL ;
	//S1AP_ConcurrentWarningMessageIndicator_t *ConcurrentWarningMessageIndicator = NULL ;

	/**************** Message type *******************/
	memset(&pdu,0,sizeof(S1AP_S1AP_PDU_t));
	pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
	pdu.choice.initiatingMessage = core_calloc(1,sizeof(S1AP_InitiatingMessage_t));
	initiatingMessage = pdu.choice.initiatingMessage;//pointer point to it
	initiatingMessage->procedureCode = S1AP_ProcedureCode_id_WriteReplaceWarning;
	initiatingMessage->criticality = S1AP_Criticality_reject;
	initiatingMessage->value.present = S1AP_InitiatingMessage__value_PR_WriteReplaceWarningRequest;
	WriteReplaceWarningRequest = &initiatingMessage->value.choice.WriteReplaceWarningRequest;

	/********* Message Identifier *********/
	ie = core_calloc(1, sizeof(S1AP_WriteReplaceWarningRequestIEs_t));
    ASN_SEQUENCE_ADD(&WriteReplaceWarningRequest->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_MessageIdentifier;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_WriteReplaceWarningRequestIEs__value_PR_MessageIdentifier;
	MessageIdentifier = &ie->value.choice.MessageIdentifier;
	MessageIdentifier->size = 2;
    MessageIdentifier->buf = core_calloc(MessageIdentifier->size, sizeof(c_uint8_t));
    MessageIdentifier->bits_unused = 0;
	unsigned short identifier_value = 4352;
    memcpy(MessageIdentifier->buf, &identifier_value, MessageIdentifier->size);

	/************ Serial Number *************/
	ie = core_calloc(1, sizeof(S1AP_WriteReplaceWarningRequestIEs_t));
    ASN_SEQUENCE_ADD(&WriteReplaceWarningRequest->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_SerialNumber;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_WriteReplaceWarningRequestIEs__value_PR_SerialNumber;
	SerialNumber = &ie->value.choice.SerialNumber;
	SerialNumber->size = 2;
    SerialNumber->buf = core_calloc(SerialNumber->size, sizeof(c_uint8_t));
    SerialNumber->bits_unused = 0;	
	unsigned short SerialNumber_value = 0;
    memcpy(SerialNumber->buf, &SerialNumber_value, SerialNumber->size);

	/************ Repetition Period *************/
	ie = core_calloc(1, sizeof(S1AP_WriteReplaceWarningRequestIEs_t));
    ASN_SEQUENCE_ADD(&WriteReplaceWarningRequest->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_RepetitionPeriod;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_WriteReplaceWarningRequestIEs__value_PR_RepetitionPeriod;
	RepetitionPeriod = &ie->value.choice.RepetitionPeriod;
	long RepetitionPeriod_value = 10;
    memcpy(RepetitionPeriod, &RepetitionPeriod_value, sizeof(long));

	/************ Number of Broadcast Request *************/
	ie = core_calloc(1, sizeof(S1AP_WriteReplaceWarningRequestIEs_t));
    ASN_SEQUENCE_ADD(&WriteReplaceWarningRequest->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_NumberofBroadcastRequest;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_WriteReplaceWarningRequestIEs__value_PR_NumberofBroadcastRequest;
	NumberofBroadcastRequest = &ie->value.choice.NumberofBroadcastRequest;
	long NumberofBroadcastRequest_value = 10;
    memcpy(NumberofBroadcastRequest, &NumberofBroadcastRequest_value, sizeof(long));
	
	/****************** Encode **********************/
	rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
	
}

status_t s1ap_build_kill_request(pkbuf_t **pkbuf)
{
	status_t rv;
	S1AP_S1AP_PDU_t pdu;

	/************ InitiatingMessage ***************/
	S1AP_InitiatingMessage_t *initiatingMessage;
	S1AP_KillRequest_t	 *KillRequest = NULL;
	S1AP_KillRequestIEs_t *ie = NULL;

	/******************** IE *********************/ 
	/* Mandatory */
	S1AP_MessageIdentifier_t *MessageIdentifier = NULL;
	S1AP_SerialNumber_t *SerialNumber = NULL;

	/* Optional */
	//S1AP_WarningAreaList_t *WarningAreaList = NULL;
	//S1AP_KillAllWarningMessages_t *KillAllWarningMessages = NULL;

	/**************** Message type *******************/
	memset(&pdu,0,sizeof(S1AP_S1AP_PDU_t));
	pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
	pdu.choice.initiatingMessage = core_calloc(1,sizeof(S1AP_InitiatingMessage_t));
	initiatingMessage = pdu.choice.initiatingMessage;//pointer point to it
	initiatingMessage->procedureCode = S1AP_ProcedureCode_id_Kill;
	initiatingMessage->criticality = S1AP_Criticality_reject;
	initiatingMessage->value.present = S1AP_InitiatingMessage__value_PR_KillRequest;
	KillRequest = &initiatingMessage->value.choice.KillRequest;

	/********* Message Identifier *********/
	ie = core_calloc(1, sizeof(S1AP_KillRequestIEs_t));
    ASN_SEQUENCE_ADD(&KillRequest->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_MessageIdentifier;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_KillRequestIEs__value_PR_MessageIdentifier;
	MessageIdentifier = &ie->value.choice.MessageIdentifier;
	MessageIdentifier->size = 2;
    MessageIdentifier->buf = core_calloc(MessageIdentifier->size, sizeof(c_uint8_t));
    MessageIdentifier->bits_unused = 0;
	unsigned short identifier_value = 4352;
    memcpy(MessageIdentifier->buf, &identifier_value, MessageIdentifier->size);

	/************ Serial Number *************/
	ie = core_calloc(1, sizeof(S1AP_KillRequestIEs_t));
    ASN_SEQUENCE_ADD(&KillRequest->protocolIEs, ie);
	ie->id = S1AP_ProtocolIE_ID_id_SerialNumber;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_KillRequestIEs__value_PR_SerialNumber;
	SerialNumber = &ie->value.choice.SerialNumber;
	SerialNumber->size = 2;
    SerialNumber->buf = core_calloc(SerialNumber->size, sizeof(c_uint8_t));
    SerialNumber->bits_unused = 0;	
	unsigned short SerialNumber_value = 0;
    memcpy(SerialNumber->buf, &SerialNumber_value, SerialNumber->size);

	/****************** Encode **********************/
	rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
	
}	

status_t s1ap_build_setup_failure(
        pkbuf_t **pkbuf, S1AP_Cause_PR group, long cause, long time_to_wait)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_S1SetupFailure_t *S1SetupFailure = NULL;

    S1AP_S1SetupFailureIEs_t *ie = NULL;
    S1AP_Cause_t *Cause = NULL;
    S1AP_TimeToWait_t *TimeToWait = NULL;
    
    d_trace(5, "    Group[%d] Cause[%d] TimeToWait[%ld]\n",
            group, cause, time_to_wait);

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = 
        core_calloc(1, sizeof(S1AP_UnsuccessfulOutcome_t));

    unsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    unsuccessfulOutcome->procedureCode = S1AP_ProcedureCode_id_S1Setup;
    unsuccessfulOutcome->criticality = S1AP_Criticality_reject;
    unsuccessfulOutcome->value.present =
        S1AP_UnsuccessfulOutcome__value_PR_S1SetupFailure;

    S1SetupFailure = &unsuccessfulOutcome->value.choice.S1SetupFailure;

    ie = core_calloc(1, sizeof(S1AP_S1SetupFailureIEs_t));
    ASN_SEQUENCE_ADD(&S1SetupFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_S1SetupFailureIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    if (time_to_wait > -1)
    {
        ie = core_calloc(1, sizeof(S1AP_S1SetupFailureIEs_t));
        ASN_SEQUENCE_ADD(&S1SetupFailure->protocolIEs, ie);

        ie->id = S1AP_ProtocolIE_ID_id_TimeToWait;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present = S1AP_S1SetupFailureIEs__value_PR_TimeToWait;

        TimeToWait = &ie->value.choice.TimeToWait;
    }

    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    if (TimeToWait)
        *TimeToWait = time_to_wait;

    rv = s1ap_encode_pdu(pkbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_downlink_nas_transport(
            pkbuf_t **s1apbuf, enb_ue_t *enb_ue, pkbuf_t *emmbuf)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_DownlinkNASTransport_t *DownlinkNASTransport = NULL;

    S1AP_DownlinkNASTransport_IEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_NAS_PDU_t *NAS_PDU = NULL;
    // S1AP_NRUESecurityCapabilities_t	*NRUESecurityCapabilities = NULL;

    d_assert(emmbuf, return CORE_ERROR, "Null param");
    d_assert(enb_ue, return CORE_ERROR, "Null param");

    d_trace(3, "[AMF4G] Downlink NAS transport\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_downlinkNASTransport;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_DownlinkNASTransport;

    DownlinkNASTransport =
        &initiatingMessage->value.choice.DownlinkNASTransport;

    ie = core_calloc(1, sizeof(S1AP_DownlinkNASTransport_IEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNASTransport->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_DownlinkNASTransport_IEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_DownlinkNASTransport_IEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNASTransport->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_DownlinkNASTransport_IEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_DownlinkNASTransport_IEs_t));
    ASN_SEQUENCE_ADD(&DownlinkNASTransport->protocolIEs, ie);
    
    ie->id = S1AP_ProtocolIE_ID_id_NAS_PDU;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_DownlinkNASTransport_IEs__value_PR_NAS_PDU;

    NAS_PDU = &ie->value.choice.NAS_PDU;

    // ie->id = S1AP_ProtocolIE_ID_id_NRUESecurityCapabilities;
    // ie->criticality = S1AP_Criticality_ignore;
    // ie->value.present = S1AP_DownlinkNASTransport_IEs__value_PR_NRUESecurityCapabilities;
    // NRUESecurityCapabilities = &ie->value.choice.NRUESecurityCapabilities;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    NAS_PDU->size = emmbuf->len;
    NAS_PDU->buf = core_calloc(NAS_PDU->size, sizeof(c_uint8_t));
    memcpy(NAS_PDU->buf, emmbuf->payload, NAS_PDU->size);
    pkbuf_free(emmbuf);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_initial_context_setup_request(
            pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue, pkbuf_t *emmbuf)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_InitialContextSetupRequest_t *InitialContextSetupRequest = NULL;

    S1AP_InitialContextSetupRequestIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_UEAggregateMaximumBitrate_t *UEAggregateMaximumBitrate = NULL;
    S1AP_E_RABToBeSetupListCtxtSUReq_t *E_RABToBeSetupListCtxtSUReq = NULL;
    S1AP_UESecurityCapabilities_t *UESecurityCapabilities = NULL;
    S1AP_SecurityKey_t *SecurityKey = NULL;

    enb_ue_t *enb_ue = NULL;
    amf4g_sess_t *sess = NULL;
    amf4g_bearer_t *bearer = NULL;
    s6a_subscription_data_t *subscription_data = NULL;

    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    enb_ue = amf4g_ue->enb_ue;
    d_assert(enb_ue, return CORE_ERROR, "Null param");
    subscription_data = &amf4g_ue->subscription_data;
    d_assert(subscription_data, return CORE_ERROR, "Null param");

    d_trace(3, "[AMF4G] Initial context setup request\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_InitialContextSetup;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_InitialContextSetupRequest;

    InitialContextSetupRequest =
        &initiatingMessage->value.choice.InitialContextSetupRequest;

    ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_InitialContextSetupRequestIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_InitialContextSetupRequestIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_uEaggregateMaximumBitrate;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_InitialContextSetupRequestIEs__value_PR_UEAggregateMaximumBitrate;

    UEAggregateMaximumBitrate = &ie->value.choice.UEAggregateMaximumBitrate;

    ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_E_RABToBeSetupListCtxtSUReq;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
    S1AP_InitialContextSetupRequestIEs__value_PR_E_RABToBeSetupListCtxtSUReq;

    E_RABToBeSetupListCtxtSUReq = &ie->value.choice.E_RABToBeSetupListCtxtSUReq;

    ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_UESecurityCapabilities;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_InitialContextSetupRequestIEs__value_PR_UESecurityCapabilities;

    UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;

    ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_SecurityKey;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_InitialContextSetupRequestIEs__value_PR_SecurityKey;

    SecurityKey = &ie->value.choice.SecurityKey;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    asn_uint642INTEGER(
            &UEAggregateMaximumBitrate->uEaggregateMaximumBitRateUL, 
            subscription_data->ambr.uplink);
    asn_uint642INTEGER(
            &UEAggregateMaximumBitrate->uEaggregateMaximumBitRateDL, 
            subscription_data->ambr.downlink);

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            S1AP_E_RABToBeSetupItemCtxtSUReqIEs_t *item = NULL;
            S1AP_E_RABToBeSetupItemCtxtSUReq_t *e_rab = NULL;
            S1AP_GBR_QosInformation_t *gbrQosInformation = NULL;
            S1AP_NAS_PDU_t *nasPdu = NULL;

            item = core_calloc(
                    1, sizeof(S1AP_E_RABToBeSetupItemCtxtSUReqIEs_t));
            ASN_SEQUENCE_ADD(&E_RABToBeSetupListCtxtSUReq->list, item);

            item->id = S1AP_ProtocolIE_ID_id_E_RABToBeSetupItemCtxtSUReq;
            item->criticality = S1AP_Criticality_reject;
            item->value.present = S1AP_E_RABToBeSetupItemCtxtSUReqIEs__value_PR_E_RABToBeSetupItemCtxtSUReq;

            e_rab = &item->value.choice.E_RABToBeSetupItemCtxtSUReq;

            e_rab->e_RAB_ID = bearer->ebi;
            e_rab->e_RABlevelQoSParameters.qCI = bearer->qos.qci;

            d_trace(5, "    EBI[%d] QCI[%d] UPF-S1U-TEID[%d]\n",
                    bearer->ebi, bearer->qos.qci, bearer->upf_s1u_teid);

            e_rab->e_RABlevelQoSParameters.allocationRetentionPriority.
                priorityLevel = bearer->qos.arp.priority_level;
            e_rab->e_RABlevelQoSParameters.allocationRetentionPriority.
                pre_emptionCapability =
                    !(bearer->qos.arp.pre_emption_capability);
            e_rab->e_RABlevelQoSParameters.allocationRetentionPriority.
                pre_emptionVulnerability =
                    !(bearer->qos.arp.pre_emption_vulnerability);

            if (bearer->qos.mbr.downlink || bearer->qos.mbr.uplink ||
                bearer->qos.gbr.downlink || bearer->qos.gbr.uplink)
            {
                if (bearer->qos.mbr.downlink == 0)
                    bearer->qos.mbr.downlink = MAX_BIT_RATE;
                if (bearer->qos.mbr.uplink == 0)
                    bearer->qos.mbr.uplink = MAX_BIT_RATE;
                if (bearer->qos.gbr.downlink == 0)
                    bearer->qos.gbr.downlink = MAX_BIT_RATE;
                if (bearer->qos.gbr.uplink == 0)
                    bearer->qos.gbr.uplink = MAX_BIT_RATE;

                gbrQosInformation = 
                        core_calloc(1, sizeof(struct S1AP_GBR_QosInformation));
                asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateDL,
                        bearer->qos.mbr.downlink);
                asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateUL,
                        bearer->qos.mbr.uplink);
                asn_uint642INTEGER(&gbrQosInformation->
                        e_RAB_GuaranteedBitrateDL, bearer->qos.gbr.downlink);
                asn_uint642INTEGER(&gbrQosInformation->
                        e_RAB_GuaranteedBitrateUL, bearer->qos.gbr.uplink);
                e_rab->e_RABlevelQoSParameters.gbrQosInformation =
                        gbrQosInformation;
            }

            rv = s1ap_ip_to_BIT_STRING(
                    &bearer->upf_s1u_ip, &e_rab->transportLayerAddress);
            d_assert(rv == CORE_OK, return CORE_ERROR,);
            s1ap_uint32_to_OCTET_STRING(bearer->upf_s1u_teid, &e_rab->gTP_TEID);

            if (emmbuf && emmbuf->len)
            {
                nasPdu = (S1AP_NAS_PDU_t *)core_calloc(
                        1, sizeof(S1AP_NAS_PDU_t));
                nasPdu->size = emmbuf->len;
                nasPdu->buf = core_calloc(nasPdu->size, sizeof(c_uint8_t));
                memcpy(nasPdu->buf, emmbuf->payload, nasPdu->size);
                e_rab->nAS_PDU = nasPdu;
                pkbuf_free(emmbuf);
            }

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }

    UESecurityCapabilities->encryptionAlgorithms.size = 2;
    UESecurityCapabilities->encryptionAlgorithms.buf = 
        core_calloc(UESecurityCapabilities->encryptionAlgorithms.size, 
                    sizeof(c_uint8_t));
    UESecurityCapabilities->encryptionAlgorithms.bits_unused = 0;
    UESecurityCapabilities->encryptionAlgorithms.buf[0] = 
        (amf4g_ue->ue_network_capability.eea << 1);

    UESecurityCapabilities->integrityProtectionAlgorithms.size = 2;
    UESecurityCapabilities->integrityProtectionAlgorithms.buf =
        core_calloc(UESecurityCapabilities->
                        integrityProtectionAlgorithms.size, sizeof(c_uint8_t));
    UESecurityCapabilities->integrityProtectionAlgorithms.bits_unused = 0;
    UESecurityCapabilities->integrityProtectionAlgorithms.buf[0] =
        (amf4g_ue->ue_network_capability.eia << 1);

    SecurityKey->size = SHA256_DIGEST_SIZE;
    SecurityKey->buf = 
        core_calloc(SecurityKey->size, sizeof(c_uint8_t));
    SecurityKey->bits_unused = 0;
    memcpy(SecurityKey->buf, amf4g_ue->kenb, SecurityKey->size);

    /* Set UeRadioCapability if exists */
#if 0
    if (amf4g_ue->radio_capa)
    {
        S1AP_UERadioCapability_t *UERadioCapability = NULL;
        S1AP_UERadioCapability_t *radio_capa = 
            (S1AP_UERadioCapability_t *)amf4g_ue->radio_capa;

        ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
        ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

        ie->id = S1AP_ProtocolIE_ID_id_UERadioCapability;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present =
            S1AP_InitialContextSetupRequestIEs__value_PR_UERadioCapability;

        UERadioCapability = &ie->value.choice.UERadioCapability;

        UERadioCapability->size = radio_capa->size;
        UERadioCapability->buf = 
            core_calloc(UERadioCapability->size, sizeof(c_uint8_t));
        memcpy(UERadioCapability->buf, radio_capa->buf, radio_capa->size);
    }
#else
    if (amf4g_ue->ueRadioCapability.buf && amf4g_ue->ueRadioCapability.size)
    {
        S1AP_UERadioCapability_t *UERadioCapability = NULL;

        ie = core_calloc(1, sizeof(S1AP_InitialContextSetupRequestIEs_t));
        ASN_SEQUENCE_ADD(&InitialContextSetupRequest->protocolIEs, ie);

        ie->id = S1AP_ProtocolIE_ID_id_UERadioCapability;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present =
            S1AP_InitialContextSetupRequestIEs__value_PR_UERadioCapability;

        UERadioCapability = &ie->value.choice.UERadioCapability;

        d_assert(UERadioCapability, return CORE_ERROR,);
        s1ap_buffer_to_OCTET_STRING(
                amf4g_ue->ueRadioCapability.buf, amf4g_ue->ueRadioCapability.size,
                UERadioCapability);
    }
#endif

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_e_rab_setup_request(
            pkbuf_t **s1apbuf, amf4g_bearer_t *bearer, pkbuf_t *esmbuf)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_E_RABSetupRequest_t *E_RABSetupRequest = NULL;

    S1AP_E_RABSetupRequestIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_E_RABToBeSetupListBearerSUReq_t *E_RABToBeSetupListBearerSUReq = NULL;

    S1AP_E_RABToBeSetupItemBearerSUReqIEs_t *item = NULL;
    S1AP_E_RABToBeSetupItemBearerSUReq_t *e_rab = NULL;
    S1AP_GBR_QosInformation_t *gbrQosInformation = NULL;
    S1AP_NAS_PDU_t *nasPdu = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;

    d_assert(esmbuf, return CORE_ERROR, "Null param");
    d_assert(bearer, return CORE_ERROR, "Null param");

    amf4g_ue = bearer->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    enb_ue = amf4g_ue->enb_ue;
    d_assert(enb_ue, return CORE_ERROR, "Null param");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_E_RABSetup;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_E_RABSetupRequest;

    E_RABSetupRequest = &initiatingMessage->value.choice.E_RABSetupRequest;

    ie = core_calloc(1, sizeof(S1AP_E_RABSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&E_RABSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_E_RABSetupRequestIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_E_RABSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&E_RABSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_E_RABSetupRequestIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_E_RABSetupRequestIEs_t));
    ASN_SEQUENCE_ADD(&E_RABSetupRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_E_RABToBeSetupListBearerSUReq;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_E_RABSetupRequestIEs__value_PR_E_RABToBeSetupListBearerSUReq;

    E_RABToBeSetupListBearerSUReq =
        &ie->value.choice.E_RABToBeSetupListBearerSUReq;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    item = core_calloc(1, sizeof(S1AP_E_RABToBeSetupItemBearerSUReqIEs_t));
    ASN_SEQUENCE_ADD(&E_RABToBeSetupListBearerSUReq->list, item);

    item->id = S1AP_ProtocolIE_ID_id_E_RABToBeSetupItemBearerSUReq;
    item->criticality = S1AP_Criticality_reject;
    item->value.present = S1AP_E_RABToBeSetupItemBearerSUReqIEs__value_PR_E_RABToBeSetupItemBearerSUReq;

    e_rab = &item->value.choice.E_RABToBeSetupItemBearerSUReq;

    e_rab->e_RAB_ID = bearer->ebi;
    e_rab->e_RABlevelQoSParameters.qCI = bearer->qos.qci;

    d_trace(5, "    EBI[%d] QCI[%d]\n", bearer->ebi, bearer->qos.qci);

    e_rab->e_RABlevelQoSParameters.allocationRetentionPriority.
        priorityLevel = bearer->qos.arp.priority_level;
    e_rab->e_RABlevelQoSParameters.allocationRetentionPriority.
        pre_emptionCapability = !(bearer->qos.arp.pre_emption_capability);
    e_rab->e_RABlevelQoSParameters.allocationRetentionPriority.
        pre_emptionVulnerability = !(bearer->qos.arp.pre_emption_vulnerability);

    if (bearer->qos.mbr.downlink || bearer->qos.mbr.uplink ||
        bearer->qos.gbr.downlink || bearer->qos.gbr.uplink)
    {
        if (bearer->qos.mbr.downlink == 0)
            bearer->qos.mbr.downlink = MAX_BIT_RATE;
        if (bearer->qos.mbr.uplink == 0)
            bearer->qos.mbr.uplink = MAX_BIT_RATE;
        if (bearer->qos.gbr.downlink == 0)
            bearer->qos.gbr.downlink = MAX_BIT_RATE;
        if (bearer->qos.gbr.uplink == 0)
            bearer->qos.gbr.uplink = MAX_BIT_RATE;

        gbrQosInformation = core_calloc(1, sizeof(S1AP_GBR_QosInformation_t));
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateDL,
                bearer->qos.mbr.downlink);
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateUL,
                bearer->qos.mbr.uplink);
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_GuaranteedBitrateDL,
                bearer->qos.gbr.downlink);
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_GuaranteedBitrateUL,
                bearer->qos.gbr.uplink);
        e_rab->e_RABlevelQoSParameters.gbrQosInformation = gbrQosInformation;
    }

    rv = s1ap_ip_to_BIT_STRING(
            &bearer->upf_s1u_ip, &e_rab->transportLayerAddress);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    s1ap_uint32_to_OCTET_STRING(bearer->upf_s1u_teid, &e_rab->gTP_TEID);
    d_trace(5, "    UPF-S1U-TEID[%d]\n", bearer->upf_s1u_teid);

    nasPdu = &e_rab->nAS_PDU;
    nasPdu->size = esmbuf->len;
    nasPdu->buf = core_calloc(nasPdu->size, sizeof(c_uint8_t));
    memcpy(nasPdu->buf, esmbuf->payload, nasPdu->size);
    pkbuf_free(esmbuf);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_e_rab_modify_request(
            pkbuf_t **s1apbuf, amf4g_bearer_t *bearer, pkbuf_t *esmbuf)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_E_RABModifyRequest_t *E_RABModifyRequest = NULL;

    S1AP_E_RABModifyRequestIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_E_RABToBeModifiedListBearerModReq_t
        *E_RABToBeModifiedListBearerModReq = NULL;

    S1AP_E_RABToBeModifiedItemBearerModReqIEs_t *item = NULL;
    S1AP_E_RABToBeModifiedItemBearerModReq_t *e_rab = NULL;
    S1AP_GBR_QosInformation_t *gbrQosInformation = NULL;
    S1AP_NAS_PDU_t *nasPdu = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;

    d_assert(esmbuf, return CORE_ERROR, "Null param");
    d_assert(bearer, return CORE_ERROR, "Null param");

    amf4g_ue = bearer->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    enb_ue = amf4g_ue->enb_ue;
    d_assert(enb_ue, return CORE_ERROR, "Null param");

    d_trace(3, "[AMF4G] E-RAB modify request\n");
    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_E_RABModify;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_E_RABModifyRequest;

    E_RABModifyRequest = &initiatingMessage->value.choice.E_RABModifyRequest;

    ie = core_calloc(1, sizeof(S1AP_E_RABModifyRequestIEs_t));
    ASN_SEQUENCE_ADD(&E_RABModifyRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_E_RABModifyRequestIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_E_RABModifyRequestIEs_t));
    ASN_SEQUENCE_ADD(&E_RABModifyRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_E_RABModifyRequestIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_E_RABModifyRequestIEs_t));
    ASN_SEQUENCE_ADD(&E_RABModifyRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_E_RABToBeModifiedListBearerModReq;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_E_RABModifyRequestIEs__value_PR_E_RABToBeModifiedListBearerModReq;

    E_RABToBeModifiedListBearerModReq =
        &ie->value.choice.E_RABToBeModifiedListBearerModReq;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    item = core_calloc(1, sizeof(S1AP_E_RABToBeModifiedItemBearerModReqIEs_t));
    ASN_SEQUENCE_ADD(&E_RABToBeModifiedListBearerModReq->list, item);

    item->id = S1AP_ProtocolIE_ID_id_E_RABToBeModifiedItemBearerModReq;
    item->criticality = S1AP_Criticality_reject;
    item->value.present = S1AP_E_RABToBeModifiedItemBearerModReqIEs__value_PR_E_RABToBeModifiedItemBearerModReq;

    e_rab = &item->value.choice.E_RABToBeModifiedItemBearerModReq;

    e_rab->e_RAB_ID = bearer->ebi;
    e_rab->e_RABLevelQoSParameters.qCI = bearer->qos.qci;

    d_trace(5, "    EBI[%d] QCI[%d]\n", bearer->ebi, bearer->qos.qci);

    e_rab->e_RABLevelQoSParameters.allocationRetentionPriority.
        priorityLevel = bearer->qos.arp.priority_level;
    e_rab->e_RABLevelQoSParameters.allocationRetentionPriority.
        pre_emptionCapability = !(bearer->qos.arp.pre_emption_capability);
    e_rab->e_RABLevelQoSParameters.allocationRetentionPriority.
        pre_emptionVulnerability = !(bearer->qos.arp.pre_emption_vulnerability);

    if (bearer->qos.mbr.downlink || bearer->qos.mbr.uplink ||
        bearer->qos.gbr.downlink || bearer->qos.gbr.uplink)
    {
        if (bearer->qos.mbr.downlink == 0)
            bearer->qos.mbr.downlink = MAX_BIT_RATE;
        if (bearer->qos.mbr.uplink == 0)
            bearer->qos.mbr.uplink = MAX_BIT_RATE;
        if (bearer->qos.gbr.downlink == 0)
            bearer->qos.gbr.downlink = MAX_BIT_RATE;
        if (bearer->qos.gbr.uplink == 0)
            bearer->qos.gbr.uplink = MAX_BIT_RATE;

        gbrQosInformation = 
                core_calloc(1, sizeof(S1AP_GBR_QosInformation_t));
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateDL,
                bearer->qos.mbr.downlink);
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateUL,
                bearer->qos.mbr.uplink);
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_GuaranteedBitrateDL,
                bearer->qos.gbr.downlink);
        asn_uint642INTEGER(&gbrQosInformation->e_RAB_GuaranteedBitrateUL,
                bearer->qos.gbr.uplink);
        e_rab->e_RABLevelQoSParameters.gbrQosInformation = gbrQosInformation;
    }

    nasPdu = &e_rab->nAS_PDU;
    nasPdu->size = esmbuf->len;
    nasPdu->buf = core_calloc(nasPdu->size, sizeof(c_uint8_t));
    memcpy(nasPdu->buf, esmbuf->payload, nasPdu->size);
    pkbuf_free(esmbuf);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_e_rab_release_command(pkbuf_t **s1apbuf,
        amf4g_bearer_t *bearer, pkbuf_t *esmbuf, 
        S1AP_Cause_PR group, long cause)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_E_RABReleaseCommand_t *E_RABReleaseCommand = NULL;

    S1AP_E_RABReleaseCommandIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_UEAggregateMaximumBitrate_t *UEAggregateMaximumBitrate = NULL;
    S1AP_E_RABList_t *E_RABList = NULL;
    S1AP_NAS_PDU_t *nasPdu = NULL;

    S1AP_E_RABItemIEs_t *item = NULL;
    S1AP_E_RABItem_t *e_rab = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    enb_ue_t *enb_ue = NULL;
    s6a_subscription_data_t *subscription_data = NULL;

    d_assert(esmbuf, return CORE_ERROR, "Null param");
    d_assert(bearer, return CORE_ERROR, "Null param");

    amf4g_ue = bearer->amf4g_ue;
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    enb_ue = amf4g_ue->enb_ue;
    d_assert(enb_ue, return CORE_ERROR, "Null param");
    subscription_data = &amf4g_ue->subscription_data;
    d_assert(subscription_data, return CORE_ERROR, "Null param");

    d_trace(3, "[AMF4G] E-RAB release command\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_E_RABRelease;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_E_RABReleaseCommand;

    E_RABReleaseCommand = &initiatingMessage->value.choice.E_RABReleaseCommand;

    ie = core_calloc(1, sizeof(S1AP_E_RABReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&E_RABReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_E_RABReleaseCommandIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_E_RABReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&E_RABReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_E_RABReleaseCommandIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_E_RABReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&E_RABReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_uEaggregateMaximumBitrate;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_E_RABReleaseCommandIEs__value_PR_UEAggregateMaximumBitrate;

    UEAggregateMaximumBitrate = &ie->value.choice.UEAggregateMaximumBitrate;

    ie = core_calloc(1, sizeof(S1AP_E_RABReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&E_RABReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_E_RABToBeReleasedList;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_E_RABReleaseCommandIEs__value_PR_E_RABList;

    E_RABList = &ie->value.choice.E_RABList;

    ie = core_calloc(1, sizeof(S1AP_E_RABReleaseCommandIEs_t));
    ASN_SEQUENCE_ADD(&E_RABReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_NAS_PDU;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_E_RABReleaseCommandIEs__value_PR_NAS_PDU;

    nasPdu = &ie->value.choice.NAS_PDU;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    asn_uint642INTEGER(
            &UEAggregateMaximumBitrate->uEaggregateMaximumBitRateUL, 
            subscription_data->ambr.uplink);
    asn_uint642INTEGER(
            &UEAggregateMaximumBitrate->uEaggregateMaximumBitRateDL, 
            subscription_data->ambr.downlink);

    item = core_calloc(1, sizeof(S1AP_E_RABItemIEs_t));
    ASN_SEQUENCE_ADD(&E_RABList->list, item);

    item->id = S1AP_ProtocolIE_ID_id_E_RABItem;
    item->criticality = S1AP_Criticality_ignore;
    item->value.present = S1AP_E_RABItemIEs__value_PR_E_RABItem;

    e_rab = &item->value.choice.E_RABItem;

    e_rab->e_RAB_ID = bearer->ebi;
    e_rab->cause.present = group;
    e_rab->cause.choice.radioNetwork = cause;

    d_trace(5, "    EBI[%d] Gruop[%d] Cause[%d]\n", bearer->ebi, group, cause);

    nasPdu->size = esmbuf->len;
    nasPdu->buf = core_calloc(nasPdu->size, sizeof(c_uint8_t));
    memcpy(nasPdu->buf, esmbuf->payload, nasPdu->size);
    pkbuf_free(esmbuf);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_ue_context_release_command(
    pkbuf_t **s1apbuf, enb_ue_t *enb_ue, S1AP_Cause_PR group, long cause)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UEContextReleaseCommand_t *UEContextReleaseCommand = NULL;

    S1AP_UEContextReleaseCommand_IEs_t *ie = NULL;
    S1AP_UE_S1AP_IDs_t *UE_S1AP_IDs = NULL;
    S1AP_Cause_t *Cause = NULL;

    d_assert(enb_ue, return CORE_ERROR, "Null param");

    if (enb_ue->mme_ue_s1ap_id == 0)
    {
        d_error("invalid mme ue s1ap id (idx: %d)", enb_ue->index);
        return CORE_ERROR;
    }

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_UEContextRelease;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_UEContextReleaseCommand;

    UEContextReleaseCommand =
        &initiatingMessage->value.choice.UEContextReleaseCommand;

    ie = core_calloc(1, sizeof(S1AP_UEContextReleaseCommand_IEs_t));
    ASN_SEQUENCE_ADD(&UEContextReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_UE_S1AP_IDs;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UEContextReleaseCommand_IEs__value_PR_UE_S1AP_IDs;

    UE_S1AP_IDs = &ie->value.choice.UE_S1AP_IDs;

    ie = core_calloc(1, sizeof(S1AP_UEContextReleaseCommand_IEs_t));
    ASN_SEQUENCE_ADD(&UEContextReleaseCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_UEContextReleaseCommand_IEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    if (enb_ue->enb_ue_s1ap_id == INVALID_UE_S1AP_ID)
    {
        UE_S1AP_IDs->present = S1AP_UE_S1AP_IDs_PR_mME_UE_S1AP_ID;
        UE_S1AP_IDs->choice.mME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    }
    else
    {
        UE_S1AP_IDs->present = S1AP_UE_S1AP_IDs_PR_uE_S1AP_ID_pair;
        UE_S1AP_IDs->choice.uE_S1AP_ID_pair = 
            core_calloc(1, sizeof(S1AP_UE_S1AP_ID_pair_t));
        UE_S1AP_IDs->choice.uE_S1AP_ID_pair->mME_UE_S1AP_ID = 
            enb_ue->mme_ue_s1ap_id;
        UE_S1AP_IDs->choice.uE_S1AP_ID_pair->eNB_UE_S1AP_ID = 
            enb_ue->enb_ue_s1ap_id;
    }

    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_ue_context_resume_response(
        pkbuf_t **s1apbuf, 
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id)
{
	status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_UEContextResumeResponse_t *UEContextResumeResponse = NULL;

    S1AP_UEContextResumeResponseIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    // S1AP_E_RABFailedToResumeListResumeRes_t	 *E_RABFailedToResumeListResumeRes;
   
    d_trace(3, "[AMF4G] UE Context Resume Response\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_UEContextResume;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_UEContextResumeResponse;

    UEContextResumeResponse =
        &successfulOutcome->value.choice.UEContextResumeResponse;

    ie = core_calloc(1, sizeof(S1AP_UEContextResumeResponseIEs_t));
    ASN_SEQUENCE_ADD(&UEContextResumeResponse->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_UEContextResumeResponseIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_UEContextResumeResponseIEs_t));
    ASN_SEQUENCE_ADD(&UEContextResumeResponse->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_UEContextResumeResponseIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            *enb_ue_s1ap_id, *mme_ue_s1ap_id);
            
    *MME_UE_S1AP_ID = *mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = *enb_ue_s1ap_id;

    ie = core_calloc(1, sizeof(S1AP_UEContextResumeResponseIEs_t));
    ASN_SEQUENCE_ADD(&UEContextResumeResponse->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_E_RABFailedToResumeListResumeRes;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_UEContextResumeResponseIEs__value_PR_E_RABFailedToResumeListResumeRes;

    // E_RABFailedToResumeListResumeRes = &ie->value.choice.E_RABFailedToResumeListResumeRes;

    // E-RAB Failed To Resume List
    /* S1AP_E_RABFailedToResumeItemResumeResIEs_t *item = NULL;
    S1AP_E_RABFailedToResumeItemResumeRes_t *e_rab = NULL;
    S1AP_Cause_t *Cause = NULL;
	for ()
    {
        amf4g_bearer_t *bearer;
        S1AP_Cause_t *cause;
        
        item = core_calloc(1, sizeof(S1AP_E_RABFailedToResumeItemResumeResIEs_t));
        ASN_SEQUENCE_ADD(&E_RABFailedToResumeListResumeRes->list, item);

        item->id = S1AP_ProtocolIE_ID_id_E_RABFailedToResumeItemResumeRes;
        item->criticality = S1AP_Criticality_reject;
        item->value.present = S1AP_UEContextResumeResponseIEs__value_PR_E_RABFailedToResumeListResumeRes;

        e_rab = &item->value.choice.E_RABFailedToResumeItemResumeRes;
        e_rab->e_RAB_ID = bearer->ebi;

        item = core_calloc(1, sizeof(S1AP_E_RABFailedToResumeItemResumeResIEs_t));
        ASN_SEQUENCE_ADD(&E_RABFailedToResumeListResumeRes->list, item);

        item->id = S1AP_ProtocolIE_ID_id_E_RABFailedToResumeItemResumeRes;
        item->criticality = S1AP_Criticality_reject;
        item->value.present = S1AP_UEContextResumeResponseIEs__value_PR_E_RABFailedToResumeListResumeRes;

        Cause = &item->value.choice.E_RABFailedToResumeItemResumeRes.cause;
        Cause->present = cause->present;
        Cause->choice.radioNetwork = cause->choice.radioNetwork;
    } */

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_ue_context_resume_failure(
        pkbuf_t **s1apbuf, 
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_Cause_t *cause)
{
	status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_UEContextResumeFailure_t	 *UEContextResumeFailure = NULL;

    S1AP_UEContextResumeFailureIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    d_assert(s1apbuf, return CORE_ERROR,);
    d_assert(cause, return CORE_ERROR,);

    d_trace(3, "[AMF4G] UE Context Resume Failure\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = 
        core_calloc(1, sizeof(S1AP_UnsuccessfulOutcome_t));

    unsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    unsuccessfulOutcome->procedureCode =
        S1AP_ProcedureCode_id_UEContextResume;
    unsuccessfulOutcome->criticality = S1AP_Criticality_reject;
    unsuccessfulOutcome->value.present =
        S1AP_UnsuccessfulOutcome__value_PR_UEContextResumeFailure;

    UEContextResumeFailure =
        &unsuccessfulOutcome->value.choice.UEContextResumeFailure;

    ie = core_calloc(1, sizeof(S1AP_UEContextResumeFailureIEs_t));
    ASN_SEQUENCE_ADD(&UEContextResumeFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_UEContextResumeFailureIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_UEContextResumeFailureIEs_t));
    ASN_SEQUENCE_ADD(&UEContextResumeFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_UEContextResumeFailureIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_UEContextResumeFailureIEs_t));
    ASN_SEQUENCE_ADD(&UEContextResumeFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_UEContextResumeFailureIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue_s1ap_id, mme_ue_s1ap_id);
    d_trace(5, "    Group[%d] Cause[%d]\n",
            cause->present, cause->choice.radioNetwork);

    *MME_UE_S1AP_ID = *mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = *enb_ue_s1ap_id;
    Cause->present = cause->present;
    Cause->choice.radioNetwork = cause->choice.radioNetwork;

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}
/******************************************/

status_t s1ap_build_ue_context_modification_request(pkbuf_t **s1apbuf,
        enb_ue_t *enb_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UEContextModificationRequest_t *UEContextModificationRequest = NULL;

    S1AP_UEContextModificationRequestIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_SecurityKey_t *SecurityKey = NULL;

    d_assert(enb_ue, return CORE_ERROR, "Null param");

    if (enb_ue->mme_ue_s1ap_id == 0)
    {
        d_error("invalid mme ue s1ap id (idx: %d)", enb_ue->index);
        return CORE_ERROR;
    }

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));

    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));
    
    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_UEContextModification;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_UEContextModificationRequest;

    UEContextModificationRequest =
        &initiatingMessage->value.choice.UEContextModificationRequest;

    ie = core_calloc(1, sizeof(S1AP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UEContextModificationRequestIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UEContextModificationRequestIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_UEContextModificationRequestIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_SecurityKey;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UEContextModificationRequestIEs__value_PR_SecurityKey;

    SecurityKey = &ie->value.choice.SecurityKey;

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    SecurityKey->size = SHA256_DIGEST_SIZE;
    SecurityKey->buf = 
        core_calloc(SecurityKey->size, sizeof(c_uint8_t));
    SecurityKey->bits_unused = 0;
    memcpy(SecurityKey->buf, enb_ue->amf4g_ue->kenb, SecurityKey->size);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_ue_context_modification_confirm(
	pkbuf_t **s1apbuf, 
	enb_ue_t *enb_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_UEContextModificationConfirm_t *UEContextModificationConfirm = NULL;

    S1AP_UEContextModificationConfirmIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    d_assert(enb_ue, return CORE_ERROR, "Null param");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_UEContextModificationIndication;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_InitiatingMessage__value_PR_UEContextModificationIndication;

    UEContextModificationConfirm =
        &successfulOutcome->value.choice.UEContextModificationConfirm;

    ie = core_calloc(1, sizeof(S1AP_UEContextModificationConfirmIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationConfirm->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UEContextModificationIndicationIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_UEContextModificationIndicationIEs_t));
    ASN_SEQUENCE_ADD(&UEContextModificationConfirm->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UEContextModificationIndicationIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_paging(pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_Paging_t *Paging = NULL;

    S1AP_PagingIEs_t *ie = NULL;

    S1AP_UEIdentityIndexValue_t *UEIdentityIndexValue = NULL;
    S1AP_UEPagingID_t *UEPagingID = NULL;
    S1AP_CNDomain_t *CNDomain = NULL;
    S1AP_TAIList_t *TAIList = NULL;

    S1AP_TAIItemIEs_t *item = NULL;
    S1AP_TAIItem_t *tai_item = NULL;

    c_uint16_t index_value;
    c_uint64_t ue_imsi_value = 0;
    int i = 0;

    d_assert(amf4g_ue, return CORE_ERROR, "Null param");

    d_trace(3, "[AMF4G] Paging\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_Paging;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present = S1AP_InitiatingMessage__value_PR_Paging;

    Paging = &initiatingMessage->value.choice.Paging;

    ie = core_calloc(1, sizeof(S1AP_PagingIEs_t));
    ASN_SEQUENCE_ADD(&Paging->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_UEIdentityIndexValue;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_PagingIEs__value_PR_UEIdentityIndexValue;

    UEIdentityIndexValue = &ie->value.choice.UEIdentityIndexValue;

    ie = core_calloc(1, sizeof(S1AP_PagingIEs_t));
    ASN_SEQUENCE_ADD(&Paging->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_UEPagingID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_PagingIEs__value_PR_UEPagingID;

    UEPagingID = &ie->value.choice.UEPagingID;

    ie = core_calloc(1, sizeof(S1AP_PagingIEs_t));
    ASN_SEQUENCE_ADD(&Paging->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_CNDomain;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_PagingIEs__value_PR_CNDomain;

    CNDomain = &ie->value.choice.CNDomain;

    ie = core_calloc(1, sizeof(S1AP_PagingIEs_t));
    ASN_SEQUENCE_ADD(&Paging->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_TAIList;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_PagingIEs__value_PR_TAIList;

    TAIList = &ie->value.choice.TAIList;

    /* Set UE Identity Index value : IMSI mod 4096 */
    UEIdentityIndexValue->size = 2;
    UEIdentityIndexValue->buf = 
        core_calloc(UEIdentityIndexValue->size, sizeof(c_uint8_t));

    /* Conver string to value */
    for (i = 0; i < strlen(amf4g_ue->imsi_bcd); i++)
    {
        ue_imsi_value = ue_imsi_value*10 + (amf4g_ue->imsi_bcd[i] - '0');
    }

    /* index(10bit) = ue_imsi_value mod 1024 */
    index_value = ue_imsi_value % 1024;
    UEIdentityIndexValue->buf[0] = index_value >> 2;
    UEIdentityIndexValue->buf[1] = (index_value & 0x3f) << 6;
    UEIdentityIndexValue->bits_unused = 6;

    /* Set Paging Identity */
    UEPagingID->present = S1AP_UEPagingID_PR_s_TMSI;
    UEPagingID->choice.s_TMSI = 
        core_calloc(1, sizeof(S1AP_S_TMSI_t));
    s1ap_uint8_to_OCTET_STRING(amf4g_ue->guti.mme_code, 
            &UEPagingID->choice.s_TMSI->mMEC);

    s1ap_uint32_to_OCTET_STRING(amf4g_ue->guti.m_tmsi, 
            &UEPagingID->choice.s_TMSI->m_TMSI);

    d_trace(5, "    AMF4G_CODE[%d] M_TMSI[0x%x]\n",
            amf4g_ue->guti.mme_code, amf4g_ue->guti.m_tmsi);

    *CNDomain = S1AP_CNDomain_ps;

    item = core_calloc(1, sizeof(S1AP_TAIItemIEs_t));
    ASN_SEQUENCE_ADD(&TAIList->list, item);

    item->id = S1AP_ProtocolIE_ID_id_TAIItem;
    item->criticality = S1AP_Criticality_ignore;
    item->value.present = S1AP_TAIItemIEs__value_PR_TAIItem;

    tai_item = &item->value.choice.TAIItem;

    s1ap_buffer_to_OCTET_STRING(&amf4g_ue->tai.plmn_id, sizeof(plmn_id_t),
            &tai_item->tAI.pLMNidentity);
    s1ap_uint16_to_OCTET_STRING(amf4g_ue->tai.tac, &tai_item->tAI.tAC);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_mme_configuration_transfer(
        pkbuf_t **s1apbuf, 
        S1AP_SONConfigurationTransfer_t *son_configuration_transfer)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_MMEConfigurationTransfer_t *MMEConfigurationTransfer = NULL;

    S1AP_MMEConfigurationTransferIEs_t *ie = NULL;
    S1AP_SONConfigurationTransfer_t *SONConfigurationTransfer = NULL;

    d_assert(s1apbuf, return CORE_ERROR,);
    d_assert(son_configuration_transfer, return CORE_ERROR,);

    d_trace(3, "[AMF4G] MME Configuration Transfer\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_MMEConfigurationTransfer;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_MMEConfigurationTransfer;

    MMEConfigurationTransfer =
        &initiatingMessage->value.choice.MMEConfigurationTransfer;

    ie = core_calloc(1, sizeof(S1AP_MMEConfigurationTransferIEs_t));
    ASN_SEQUENCE_ADD(&MMEConfigurationTransfer->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_SONConfigurationTransferMCT;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_MMEConfigurationTransferIEs__value_PR_SONConfigurationTransfer;

    SONConfigurationTransfer = &ie->value.choice.SONConfigurationTransfer;

    rv = s1ap_copy_ie(&asn_DEF_S1AP_SONConfigurationTransfer,
            son_configuration_transfer, SONConfigurationTransfer);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}
status_t s1ap_build_mme_direct_information_transfer(
            pkbuf_t **s1apbuf)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
 //   S1AP_MMEDirectInformationTransfer_t *MMEDirectInformationTransfer = NULL;

//    S1AP_MMEDirectInformationTransferIEs_t *ie = NULL;
//	S1AP_Inter_SystemInformationTransferType_t *Inter_SystemInformationTransferType = NULL;

    d_trace(3, "[AMF4G] MME Direct Information Transfer\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_MMEDirectInformationTransfer;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_MMEDirectInformationTransfer;

   /* MMEDirectInformationTransfer =
        &initiatingMessage->value.choice.MMEDirectInformationTransfer;

	ie = core_calloc(1, sizeof(S1AP_MMEDirectInformationTransferIEs_t));
    ASN_SEQUENCE_ADD(&MMEDirectInformationTransfer->protocolIEs, ie);

	ie->id = S1AP_ProtocolIE_ID_id_Inter_SystemInformationTransferTypeMDT;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_MMEDirectInformationTransferIEs__value_PR_Inter_SystemInformationTransferType;

    Inter_SystemInformationTransferType = &ie->value.choice.Inter_SystemInformationTransferType;

	rv = s1ap_copy_ie(&asn_DEF_S1AP_Inter_SystemInformationTransferType,
            inter_systemInformationtransferType, Inter_SystemInformationTransferType);*/
    //d_assert(rv == CORE_OK, return CORE_ERROR,);

	rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
	
}
status_t s1ap_build_path_switch_ack(pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_PathSwitchRequestAcknowledge_t *PathSwitchRequestAcknowledge = NULL;

    S1AP_PathSwitchRequestAcknowledgeIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_SecurityContext_t *SecurityContext = NULL;

    enb_ue_t *enb_ue = NULL;

    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    enb_ue = amf4g_ue->enb_ue;
    d_assert(enb_ue, return CORE_ERROR, "Null param");

    d_trace(3, "[AMF4G] Path switch acknowledge\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_PathSwitchRequest;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_PathSwitchRequestAcknowledge;

    PathSwitchRequestAcknowledge =
        &successfulOutcome->value.choice.PathSwitchRequestAcknowledge;

    ie = core_calloc(1, sizeof(S1AP_PathSwitchRequestAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestAcknowledge->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_PathSwitchRequestAcknowledgeIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_PathSwitchRequestAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestAcknowledge->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_PathSwitchRequestAcknowledgeIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_PathSwitchRequestAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestAcknowledge->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_SecurityContext;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_PathSwitchRequestAcknowledgeIEs__value_PR_SecurityContext;

    SecurityContext = &ie->value.choice.SecurityContext;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue->enb_ue_s1ap_id, enb_ue->mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = enb_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue->enb_ue_s1ap_id;

    SecurityContext->nextHopChainingCount = amf4g_ue->nhcc;
    SecurityContext->nextHopParameter.size = SHA256_DIGEST_SIZE;
    SecurityContext->nextHopParameter.buf = 
        core_calloc(SecurityContext->nextHopParameter.size,
        sizeof(c_uint8_t));
    SecurityContext->nextHopParameter.bits_unused = 0;
    memcpy(SecurityContext->nextHopParameter.buf,
            amf4g_ue->nh, SecurityContext->nextHopParameter.size);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_path_switch_failure(pkbuf_t **s1apbuf,
    c_uint32_t enb_ue_s1ap_id, c_uint32_t mme_ue_s1ap_id,
    S1AP_Cause_PR group, long cause)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_PathSwitchRequestFailure_t *PathSwitchRequestFailure = NULL;

    S1AP_PathSwitchRequestFailureIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    d_trace(3, "[AMF4G] Path switch failure\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = 
        core_calloc(1, sizeof(S1AP_UnsuccessfulOutcome_t));

    unsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    unsuccessfulOutcome->procedureCode =
        S1AP_ProcedureCode_id_PathSwitchRequest;
    unsuccessfulOutcome->criticality = S1AP_Criticality_reject;
    unsuccessfulOutcome->value.present =
        S1AP_UnsuccessfulOutcome__value_PR_PathSwitchRequestFailure;

    PathSwitchRequestFailure =
        &unsuccessfulOutcome->value.choice.PathSwitchRequestFailure;

    ie = core_calloc(1, sizeof(S1AP_PathSwitchRequestFailureIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_PathSwitchRequestFailureIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_PathSwitchRequestFailureIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_PathSwitchRequestFailureIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_PathSwitchRequestFailureIEs_t));
    ASN_SEQUENCE_ADD(&PathSwitchRequestFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_PathSwitchRequestFailureIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue_s1ap_id, mme_ue_s1ap_id);
    d_trace(5, "    Group[%d] Cause[%d]\n", group, cause);

    *MME_UE_S1AP_ID = mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = enb_ue_s1ap_id;
    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_handover_command(pkbuf_t **s1apbuf, enb_ue_t *source_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_HandoverCommand_t *HandoverCommand = NULL;

    S1AP_HandoverCommandIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_HandoverType_t *HandoverType = NULL;
    S1AP_E_RABSubjecttoDataForwardingList_t
        *E_RABSubjecttoDataForwardingList = NULL;
    S1AP_Target_ToSource_TransparentContainer_t *Target_ToSource_TransparentContainer = NULL;

    amf4g_ue_t *amf4g_ue = NULL;
    amf4g_sess_t *sess = NULL;
    amf4g_bearer_t *bearer = NULL;

    d_assert(source_ue, return CORE_ERROR, "Null param");
    amf4g_ue = source_ue->amf4g_ue;

    d_trace(3, "[AMF4G] Handover command\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode =
        S1AP_ProcedureCode_id_HandoverPreparation;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_HandoverCommand;

    HandoverCommand = &successfulOutcome->value.choice.HandoverCommand;
    d_assert(HandoverCommand, return CORE_ERROR,);

    ie = core_calloc(1, sizeof(S1AP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_HandoverCommandIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_HandoverCommandIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_HandoverType;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_HandoverCommandIEs__value_PR_HandoverType;

    HandoverType = &ie->value.choice.HandoverType;

    *MME_UE_S1AP_ID = source_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = source_ue->enb_ue_s1ap_id;
    *HandoverType = source_ue->handover_type;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            S1AP_E_RABDataForwardingItem_t *e_rab = NULL;

            if (AMF4G_HAVE_UPF_DL_INDIRECT_TUNNEL(bearer) ||
                AMF4G_HAVE_UPF_UL_INDIRECT_TUNNEL(bearer))
            {
                S1AP_E_RABDataForwardingItemIEs_t *item = NULL;

                if (E_RABSubjecttoDataForwardingList == NULL)
                {
                    ie = core_calloc(1, sizeof(S1AP_HandoverCommandIEs_t));
                    d_assert(ie, return CORE_ERROR,);
                    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);

                    ie->id = S1AP_ProtocolIE_ID_id_E_RABSubjecttoDataForwardingList;
                    ie->criticality = S1AP_Criticality_ignore;
                    ie->value.present =
                        S1AP_HandoverCommandIEs__value_PR_E_RABSubjecttoDataForwardingList;

                    E_RABSubjecttoDataForwardingList =
                        &ie->value.choice.E_RABSubjecttoDataForwardingList;
                }
                d_assert(E_RABSubjecttoDataForwardingList, return CORE_ERROR,);

                item = core_calloc(
                        1, sizeof(S1AP_E_RABDataForwardingItemIEs_t));
                d_assert(item, return CORE_ERROR,);
                ASN_SEQUENCE_ADD(&E_RABSubjecttoDataForwardingList->list, item);

                item->id = S1AP_ProtocolIE_ID_id_E_RABDataForwardingItem;
                item->criticality = S1AP_Criticality_ignore;
                item->value.present =
                    S1AP_E_RABDataForwardingItemIEs__value_PR_E_RABDataForwardingItem;

                e_rab = &item->value.choice.E_RABDataForwardingItem;
                d_assert(e_rab, return CORE_ERROR,);

                e_rab->e_RAB_ID = bearer->ebi;
            }

            if (AMF4G_HAVE_UPF_DL_INDIRECT_TUNNEL(bearer))
            {
                d_assert(e_rab, return CORE_ERROR,);
                e_rab->dL_transportLayerAddress =
                    (S1AP_TransportLayerAddress_t *)
                    core_calloc(1, sizeof(S1AP_TransportLayerAddress_t));
                rv = s1ap_ip_to_BIT_STRING(
                        &bearer->upf_dl_ip, e_rab->dL_transportLayerAddress);
                d_assert(rv == CORE_OK, return CORE_ERROR,);

                e_rab->dL_gTP_TEID = (S1AP_GTP_TEID_t *)
                    core_calloc(1, sizeof(S1AP_GTP_TEID_t));
                s1ap_uint32_to_OCTET_STRING(
                        bearer->upf_dl_teid, e_rab->dL_gTP_TEID);
                d_trace(5, "    UPF-DL-TEID[%d]\n", bearer->upf_dl_teid);
            }

            if (AMF4G_HAVE_UPF_UL_INDIRECT_TUNNEL(bearer))
            {
                d_assert(e_rab, return CORE_ERROR,);
                e_rab->uL_TransportLayerAddress =
                    (S1AP_TransportLayerAddress_t *)
                    core_calloc(1, sizeof(S1AP_TransportLayerAddress_t));
                rv = s1ap_ip_to_BIT_STRING(
                    &bearer->upf_ul_ip, e_rab->uL_TransportLayerAddress);
                d_assert(rv == CORE_OK, return CORE_ERROR,);

                e_rab->uL_GTP_TEID = (S1AP_GTP_TEID_t *)
                    core_calloc(1, sizeof(S1AP_GTP_TEID_t));
                s1ap_uint32_to_OCTET_STRING(
                        bearer->upf_ul_teid, e_rab->uL_GTP_TEID);
                d_trace(5, "    UPF-UL-TEID[%d]\n", bearer->upf_dl_teid);
            }

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }

    ie = core_calloc(1, sizeof(S1AP_HandoverCommandIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCommand->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Target_ToSource_TransparentContainer;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_HandoverCommandIEs__value_PR_Target_ToSource_TransparentContainer;

    Target_ToSource_TransparentContainer =
        &ie->value.choice.Target_ToSource_TransparentContainer;

    s1ap_buffer_to_OCTET_STRING(amf4g_ue->container.buf, amf4g_ue->container.size, 
            Target_ToSource_TransparentContainer);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_handover_preparation_failure(
        pkbuf_t **s1apbuf, enb_ue_t *source_ue, S1AP_Cause_t *cause)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_UnsuccessfulOutcome_t *unsuccessfulOutcome = NULL;
    S1AP_HandoverPreparationFailure_t *HandoverPreparationFailure = NULL;

    S1AP_HandoverPreparationFailureIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    d_assert(s1apbuf, return CORE_ERROR,);
    d_assert(source_ue, return CORE_ERROR,);
    d_assert(cause, return CORE_ERROR,);

    d_trace(3, "[AMF4G] Handover preparation failure\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_unsuccessfulOutcome;
    pdu.choice.unsuccessfulOutcome = 
        core_calloc(1, sizeof(S1AP_UnsuccessfulOutcome_t));

    unsuccessfulOutcome = pdu.choice.unsuccessfulOutcome;
    unsuccessfulOutcome->procedureCode =
        S1AP_ProcedureCode_id_HandoverPreparation;
    unsuccessfulOutcome->criticality = S1AP_Criticality_reject;
    unsuccessfulOutcome->value.present =
        S1AP_UnsuccessfulOutcome__value_PR_HandoverPreparationFailure;

    HandoverPreparationFailure =
        &unsuccessfulOutcome->value.choice.HandoverPreparationFailure;

    ie = core_calloc(1, sizeof(S1AP_HandoverPreparationFailureIEs_t));
    ASN_SEQUENCE_ADD(&HandoverPreparationFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_HandoverPreparationFailureIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_HandoverPreparationFailureIEs_t));
    ASN_SEQUENCE_ADD(&HandoverPreparationFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_HandoverPreparationFailureIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_HandoverPreparationFailureIEs_t));
    ASN_SEQUENCE_ADD(&HandoverPreparationFailure->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_HandoverPreparationFailureIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);
    d_trace(5, "    Group[%d] Cause[%d]\n",
            cause->present, cause->choice.radioNetwork);

    *MME_UE_S1AP_ID = source_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = source_ue->enb_ue_s1ap_id;
    Cause->present = cause->present;
    Cause->choice.radioNetwork = cause->choice.radioNetwork;

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_handover_request(
        pkbuf_t **s1apbuf, amf4g_ue_t *amf4g_ue, enb_ue_t *target_ue,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_HandoverType_t *handovertype,
        S1AP_Cause_t *cause,
        S1AP_Source_ToTarget_TransparentContainer_t
            *source_totarget_transparentContainer)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_HandoverRequest_t *HandoverRequest = NULL;

    S1AP_HandoverRequestIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_HandoverType_t *HandoverType = NULL;
    S1AP_Cause_t *Cause = NULL;
    S1AP_UEAggregateMaximumBitrate_t *UEAggregateMaximumBitrate = NULL;
    S1AP_E_RABToBeSetupListHOReq_t *E_RABToBeSetupListHOReq = NULL;
    S1AP_Source_ToTarget_TransparentContainer_t
        *Source_ToTarget_TransparentContainer = NULL;
    S1AP_UESecurityCapabilities_t *UESecurityCapabilities = NULL;
    S1AP_SecurityContext_t *SecurityContext = NULL;

    amf4g_sess_t *sess = NULL;
    amf4g_bearer_t *bearer = NULL;
    s6a_subscription_data_t *subscription_data = NULL;

    d_assert(handovertype, return CORE_ERROR,);
    d_assert(cause, return CORE_ERROR,);
    d_assert(source_totarget_transparentContainer, return CORE_ERROR,);

    d_assert(target_ue, return CORE_ERROR, "Null param");
    d_assert(amf4g_ue, return CORE_ERROR, "Null param");
    subscription_data = &amf4g_ue->subscription_data;
    d_assert(subscription_data, return CORE_ERROR, "Null param");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode =
        S1AP_ProcedureCode_id_HandoverResourceAllocation;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_HandoverRequest;

    HandoverRequest = &initiatingMessage->value.choice.HandoverRequest;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_HandoverRequestIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_HandoverType;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_HandoverRequestIEs__value_PR_HandoverType;

    HandoverType = &ie->value.choice.HandoverType;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_HandoverRequestIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_uEaggregateMaximumBitrate;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_HandoverRequestIEs__value_PR_UEAggregateMaximumBitrate;

    UEAggregateMaximumBitrate = &ie->value.choice.UEAggregateMaximumBitrate;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_E_RABToBeSetupListHOReq;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_HandoverRequestIEs__value_PR_E_RABToBeSetupListHOReq;

    E_RABToBeSetupListHOReq = &ie->value.choice.E_RABToBeSetupListHOReq;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Source_ToTarget_TransparentContainer;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_HandoverRequestIEs__value_PR_Source_ToTarget_TransparentContainer;

    Source_ToTarget_TransparentContainer =
        &ie->value.choice.Source_ToTarget_TransparentContainer;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_UESecurityCapabilities;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_HandoverRequestIEs__value_PR_UESecurityCapabilities;

    UESecurityCapabilities = &ie->value.choice.UESecurityCapabilities;

    ie = core_calloc(1, sizeof(S1AP_HandoverRequestIEs_t));
    ASN_SEQUENCE_ADD(&HandoverRequest->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_SecurityContext;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
        S1AP_HandoverRequestIEs__value_PR_SecurityContext;

    SecurityContext = &ie->value.choice.SecurityContext;

    *MME_UE_S1AP_ID = target_ue->mme_ue_s1ap_id;
    *HandoverType = *handovertype;
    Cause->present = cause->present;
    Cause->choice.radioNetwork = cause->choice.radioNetwork;

    asn_uint642INTEGER(
            &UEAggregateMaximumBitrate->uEaggregateMaximumBitRateUL, 
            subscription_data->ambr.uplink);
    asn_uint642INTEGER(
            &UEAggregateMaximumBitrate->uEaggregateMaximumBitRateDL, 
            subscription_data->ambr.downlink);

    sess = amf4g_sess_first(amf4g_ue);
    while(sess)
    {
        bearer = amf4g_bearer_first(sess);
        while(bearer)
        {
            S1AP_E_RABToBeSetupItemHOReqIEs_t *item = NULL;
            S1AP_E_RABToBeSetupItemHOReq_t *e_rab = NULL;
            S1AP_GBR_QosInformation_t *gbrQosInformation = NULL;

            item = core_calloc(1, sizeof(S1AP_E_RABToBeSetupItemHOReqIEs_t));
            ASN_SEQUENCE_ADD(&E_RABToBeSetupListHOReq->list, item);

            item->id = S1AP_ProtocolIE_ID_id_E_RABToBeSetupItemHOReq;
            item->criticality = S1AP_Criticality_reject;
            item->value.present =
            S1AP_E_RABToBeSetupItemHOReqIEs__value_PR_E_RABToBeSetupItemHOReq;

            e_rab = &item->value.choice.E_RABToBeSetupItemHOReq;

            e_rab->e_RAB_ID = bearer->ebi;
            e_rab->e_RABlevelQosParameters.qCI = bearer->qos.qci;

            e_rab->e_RABlevelQosParameters.allocationRetentionPriority.
                priorityLevel = bearer->qos.arp.priority_level;
            e_rab->e_RABlevelQosParameters.allocationRetentionPriority.
                pre_emptionCapability =
                    !(bearer->qos.arp.pre_emption_capability);
            e_rab->e_RABlevelQosParameters.allocationRetentionPriority.
                pre_emptionVulnerability =
                    !(bearer->qos.arp.pre_emption_vulnerability);

            if (bearer->qos.mbr.downlink || bearer->qos.mbr.uplink ||
                bearer->qos.gbr.downlink || bearer->qos.gbr.uplink)
            {
                if (bearer->qos.mbr.downlink == 0)
                    bearer->qos.mbr.downlink = MAX_BIT_RATE;
                if (bearer->qos.mbr.uplink == 0)
                    bearer->qos.mbr.uplink = MAX_BIT_RATE;
                if (bearer->qos.gbr.downlink == 0)
                    bearer->qos.gbr.downlink = MAX_BIT_RATE;
                if (bearer->qos.gbr.uplink == 0)
                    bearer->qos.gbr.uplink = MAX_BIT_RATE;

                gbrQosInformation = 
                        core_calloc(1, sizeof(struct S1AP_GBR_QosInformation));
                asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateDL,
                        bearer->qos.mbr.downlink);
                asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateUL,
                        bearer->qos.mbr.uplink);
                asn_uint642INTEGER(&gbrQosInformation->
                        e_RAB_GuaranteedBitrateDL, bearer->qos.gbr.downlink);
                asn_uint642INTEGER(&gbrQosInformation->
                        e_RAB_GuaranteedBitrateUL, bearer->qos.gbr.uplink);
                e_rab->e_RABlevelQosParameters.gbrQosInformation =
                        gbrQosInformation;
            }
            rv = s1ap_ip_to_BIT_STRING(
                    &bearer->upf_s1u_ip, &e_rab->transportLayerAddress);
            d_assert(rv == CORE_OK, return CORE_ERROR,);
            s1ap_uint32_to_OCTET_STRING(bearer->upf_s1u_teid, &e_rab->gTP_TEID);
            d_trace(5, "    UPF-S1U-TEID[%d]\n", bearer->upf_s1u_teid);

            bearer = amf4g_bearer_next(bearer);
        }
        sess = amf4g_sess_next(sess);
    }

    s1ap_buffer_to_OCTET_STRING(
            source_totarget_transparentContainer->buf, 
            source_totarget_transparentContainer->size, 
            Source_ToTarget_TransparentContainer);

    UESecurityCapabilities->encryptionAlgorithms.size = 2;
    UESecurityCapabilities->encryptionAlgorithms.buf = 
        core_calloc(UESecurityCapabilities->encryptionAlgorithms.size, 
                    sizeof(c_uint8_t));
    UESecurityCapabilities->encryptionAlgorithms.bits_unused = 0;
    UESecurityCapabilities->encryptionAlgorithms.buf[0] = 
        (amf4g_ue->ue_network_capability.eea << 1);

    UESecurityCapabilities->integrityProtectionAlgorithms.size = 2;
    UESecurityCapabilities->integrityProtectionAlgorithms.buf =
        core_calloc(UESecurityCapabilities->
                        integrityProtectionAlgorithms.size, sizeof(c_uint8_t));
    UESecurityCapabilities->integrityProtectionAlgorithms.bits_unused = 0;
    UESecurityCapabilities->integrityProtectionAlgorithms.buf[0] =
        (amf4g_ue->ue_network_capability.eia << 1);

    SecurityContext->nextHopChainingCount = amf4g_ue->nhcc;
    SecurityContext->nextHopParameter.size = SHA256_DIGEST_SIZE;
    SecurityContext->nextHopParameter.buf = 
        core_calloc(SecurityContext->nextHopParameter.size,
        sizeof(c_uint8_t));
    SecurityContext->nextHopParameter.bits_unused = 0;
    memcpy(SecurityContext->nextHopParameter.buf,
            amf4g_ue->nh, SecurityContext->nextHopParameter.size);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_handover_cancel_ack(pkbuf_t **s1apbuf, enb_ue_t *source_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_HandoverCancelAcknowledge_t *HandoverCancelAcknowledge = NULL;

    S1AP_HandoverCancelAcknowledgeIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    d_assert(source_ue, return CORE_ERROR, "Null param");

    d_trace(3, "[AMF4G] Handover cancel acknowledge\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_HandoverCancel;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_HandoverCancelAcknowledge;

    HandoverCancelAcknowledge =
        &successfulOutcome->value.choice.HandoverCancelAcknowledge;

    ie = core_calloc(1, sizeof(S1AP_HandoverCancelAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCancelAcknowledge->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_HandoverCancelAcknowledgeIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_HandoverCancelAcknowledgeIEs_t));
    ASN_SEQUENCE_ADD(&HandoverCancelAcknowledge->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_HandoverCancelAcknowledgeIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    *MME_UE_S1AP_ID = source_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = source_ue->enb_ue_s1ap_id;

    d_trace(5, "    Source : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            source_ue->enb_ue_s1ap_id, source_ue->mme_ue_s1ap_id);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_mme_status_transfer(pkbuf_t **s1apbuf,
        enb_ue_t *target_ue,
        S1AP_ENB_StatusTransfer_TransparentContainer_t
            *enb_statustransfer_transparentContainer)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_MMEStatusTransfer_t *MMEStatusTransfer = NULL;

    S1AP_MMEStatusTransferIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_ENB_StatusTransfer_TransparentContainer_t
        *ENB_StatusTransfer_TransparentContainer = NULL;

    d_assert(target_ue, return CORE_ERROR,);
    d_assert(enb_statustransfer_transparentContainer, return CORE_ERROR,);
    
    d_trace(3, "[AMF4G] MME status transfer\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_MMEStatusTransfer;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_MMEStatusTransfer;

    MMEStatusTransfer = &initiatingMessage->value.choice.MMEStatusTransfer;

    ie = core_calloc(1, sizeof(S1AP_MMEStatusTransferIEs_t));
    ASN_SEQUENCE_ADD(&MMEStatusTransfer->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_MMEStatusTransferIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_MMEStatusTransferIEs_t));
    ASN_SEQUENCE_ADD(&MMEStatusTransfer->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_MMEStatusTransferIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    ie = core_calloc(1, sizeof(S1AP_MMEStatusTransferIEs_t));
    ASN_SEQUENCE_ADD(&MMEStatusTransfer->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present =
    S1AP_MMEStatusTransferIEs__value_PR_ENB_StatusTransfer_TransparentContainer;

    ENB_StatusTransfer_TransparentContainer =
        &ie->value.choice.ENB_StatusTransfer_TransparentContainer;

    *MME_UE_S1AP_ID = target_ue->mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = target_ue->enb_ue_s1ap_id;

    d_trace(5, "    Target : ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            target_ue->enb_ue_s1ap_id, target_ue->mme_ue_s1ap_id);

    rv = s1ap_copy_ie(
            &asn_DEF_S1AP_ENB_StatusTransfer_TransparentContainer,
            enb_statustransfer_transparentContainer,
            ENB_StatusTransfer_TransparentContainer);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_error_indication(
        pkbuf_t **s1apbuf,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
        S1AP_Cause_PR group, long cause)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_ErrorIndication_t *ErrorIndication = NULL;

    S1AP_ErrorIndicationIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;
    S1AP_Cause_t *Cause = NULL;

    d_trace(3, "[AMF4G] Error Indication\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_ErrorIndication;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_ErrorIndication;

    ErrorIndication = &initiatingMessage->value.choice.ErrorIndication;

    if (mme_ue_s1ap_id)
    {
        ie = core_calloc(1, sizeof(S1AP_ErrorIndicationIEs_t));
        ASN_SEQUENCE_ADD(&ErrorIndication->protocolIEs, ie);

        ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present = S1AP_ErrorIndicationIEs__value_PR_MME_UE_S1AP_ID;

        MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

        *MME_UE_S1AP_ID = *mme_ue_s1ap_id;
        d_trace(5, "    AMF4G_UE_S1AP_ID[%d]\n", mme_ue_s1ap_id);
    }

    if (enb_ue_s1ap_id)
    {
        ie = core_calloc(1, sizeof(S1AP_ErrorIndicationIEs_t));
        ASN_SEQUENCE_ADD(&ErrorIndication->protocolIEs, ie);

        ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present = S1AP_ErrorIndicationIEs__value_PR_ENB_UE_S1AP_ID;

        ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

        *ENB_UE_S1AP_ID = *enb_ue_s1ap_id;
        d_trace(5, "    ENB_UE_S1AP_ID[%d]\n", enb_ue_s1ap_id);
    }

    ie = core_calloc(1, sizeof(S1AP_ErrorIndicationIEs_t));
    ASN_SEQUENCE_ADD(&ErrorIndication->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_ErrorIndicationIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    d_trace(5, "    Group[%d] Cause[%d]\n",
            Cause->present, Cause->choice.radioNetwork);

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_s1_reset(
        pkbuf_t **s1apbuf,
        S1AP_Cause_PR group, long cause,
        S1AP_UE_associatedLogicalS1_ConnectionListRes_t *partOfS1_Interface)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_Reset_t *Reset = NULL;

    S1AP_ResetIEs_t *ie = NULL;
    S1AP_Cause_t *Cause = NULL;
    S1AP_ResetType_t *ResetType = NULL;

    d_trace(3, "[AMF4G] Reset\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_Reset;
    initiatingMessage->criticality = S1AP_Criticality_ignore;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_Reset;

    Reset = &initiatingMessage->value.choice.Reset;

    ie = core_calloc(1, sizeof(S1AP_ResetIEs_t));
    ASN_SEQUENCE_ADD(&Reset->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_Cause;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present = S1AP_ResetIEs__value_PR_Cause;

    Cause = &ie->value.choice.Cause;

    ie = core_calloc(1, sizeof(S1AP_ResetIEs_t));
    ASN_SEQUENCE_ADD(&Reset->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_ResetType;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_ResetIEs__value_PR_ResetType;

    ResetType = &ie->value.choice.ResetType;

    Cause->present = group;
    Cause->choice.radioNetwork = cause;

    d_trace(5, "    Group[%d] Cause[%d] partOfS1_Interface[%p]\n",
            Cause->present, Cause->choice.radioNetwork, partOfS1_Interface);

    if (partOfS1_Interface)
    {
        ResetType->present = S1AP_ResetType_PR_partOfS1_Interface;
        ResetType->choice.partOfS1_Interface = partOfS1_Interface;
    }
    else
    {
        ResetType->present = S1AP_ResetType_PR_s1_Interface;
        ResetType->choice.s1_Interface = S1AP_ResetAll_reset_all;
    }

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_s1_reset_partial(
        pkbuf_t **s1apbuf,
        S1AP_Cause_PR group, long cause,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id)
{
    S1AP_UE_associatedLogicalS1_ConnectionListRes_t *partOfS1_Interface = NULL;
    S1AP_UE_associatedLogicalS1_ConnectionItemRes_t *ie2 = NULL;
    S1AP_UE_associatedLogicalS1_ConnectionItem_t *item = NULL;

    partOfS1_Interface = core_calloc(1,
            sizeof(S1AP_UE_associatedLogicalS1_ConnectionListRes_t));
    d_assert(partOfS1_Interface, return CORE_ERROR,);

    ie2 = core_calloc(1,
            sizeof(S1AP_UE_associatedLogicalS1_ConnectionItemRes_t));
    ASN_SEQUENCE_ADD(&partOfS1_Interface->list, ie2);

    ie2->id = S1AP_ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem;
    ie2->criticality = S1AP_Criticality_reject;
    ie2->value.present = S1AP_UE_associatedLogicalS1_ConnectionItemRes__value_PR_UE_associatedLogicalS1_ConnectionItem;

    item = &ie2->value.choice.UE_associatedLogicalS1_ConnectionItem;
    item->mME_UE_S1AP_ID = mme_ue_s1ap_id;
    item->eNB_UE_S1AP_ID = enb_ue_s1ap_id;

    return s1ap_build_s1_reset(s1apbuf, group, cause, partOfS1_Interface);
}

status_t s1ap_build_s1_reset_ack(
        pkbuf_t **s1apbuf,
        S1AP_UE_associatedLogicalS1_ConnectionListRes_t *partOfS1_Interface)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_SuccessfulOutcome_t *successfulOutcome = NULL;
    S1AP_ResetAcknowledge_t *ResetAcknowledge = NULL;

    S1AP_ResetAcknowledgeIEs_t *ie = NULL;

    d_trace(3, "[AMF4G] ResetAcknowledge\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_successfulOutcome;
    pdu.choice.successfulOutcome = 
        core_calloc(1, sizeof(S1AP_SuccessfulOutcome_t));

    successfulOutcome = pdu.choice.successfulOutcome;
    successfulOutcome->procedureCode = S1AP_ProcedureCode_id_Reset;
    successfulOutcome->criticality = S1AP_Criticality_reject;
    successfulOutcome->value.present =
        S1AP_SuccessfulOutcome__value_PR_ResetAcknowledge;

    ResetAcknowledge = &successfulOutcome->value.choice.ResetAcknowledge;

    if (partOfS1_Interface && partOfS1_Interface->list.count)
    {
        int i = 0;
        S1AP_UE_associatedLogicalS1_ConnectionListResAck_t *list = NULL;

        ie = core_calloc(1, sizeof(S1AP_ResetAcknowledgeIEs_t));
        ASN_SEQUENCE_ADD(&ResetAcknowledge->protocolIEs, ie);

        ie->id =
            S1AP_ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionListResAck;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present = S1AP_ResetAcknowledgeIEs__value_PR_UE_associatedLogicalS1_ConnectionListResAck;

        list = &ie->value.choice.UE_associatedLogicalS1_ConnectionListResAck;

        for (i = 0; i < partOfS1_Interface->list.count; i++)
        {
            S1AP_UE_associatedLogicalS1_ConnectionItemRes_t *ie1 = NULL;
            S1AP_UE_associatedLogicalS1_ConnectionItem_t *item1 = NULL;

            S1AP_UE_associatedLogicalS1_ConnectionItemResAck_t *ie2 = NULL;
            S1AP_UE_associatedLogicalS1_ConnectionItem_t *item2 = NULL;

            ie1 = (S1AP_UE_associatedLogicalS1_ConnectionItemRes_t *)
                partOfS1_Interface->list.array[i];
            d_assert(ie1, return CORE_ERROR,);

            item1 = &ie1->value.choice.UE_associatedLogicalS1_ConnectionItem;
            d_assert(item1, return CORE_ERROR,);

            if (item1->mME_UE_S1AP_ID == NULL && item1->eNB_UE_S1AP_ID == NULL)
            {
                d_warn("No AMF4G_UE_S1AP_ID & ENB_UE_S1AP_ID");
                continue;
            }
            ie2 = core_calloc(1,
                    sizeof(S1AP_UE_associatedLogicalS1_ConnectionItemResAck_t));
            d_assert(ie2, return CORE_ERROR,);
            ASN_SEQUENCE_ADD(&list->list, ie2);

            ie2->id =
                S1AP_ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem;
            ie2->criticality = S1AP_Criticality_ignore;
            ie2->value.present = S1AP_UE_associatedLogicalS1_ConnectionItemResAck__value_PR_UE_associatedLogicalS1_ConnectionItem;

            item2 = &ie2->value.choice.UE_associatedLogicalS1_ConnectionItem;
            d_assert(item2, return CORE_ERROR,);

            if (item1->mME_UE_S1AP_ID)
            {
                item2->mME_UE_S1AP_ID = core_calloc(1,
                        sizeof(S1AP_MME_UE_S1AP_ID_t));
                d_assert(item2->mME_UE_S1AP_ID, return CORE_ERROR,);
                *item2->mME_UE_S1AP_ID = *item1->mME_UE_S1AP_ID;
            }

            if (item1->eNB_UE_S1AP_ID)
            {
                item2->eNB_UE_S1AP_ID = core_calloc(1,
                        sizeof(S1AP_ENB_UE_S1AP_ID_t));
                d_assert(item2->eNB_UE_S1AP_ID, return CORE_ERROR,);
                *item2->eNB_UE_S1AP_ID = *item1->eNB_UE_S1AP_ID;
            }

            d_trace(5, "    AMF4G_UE_S1AP_ID[%d] ENB_UE_S1AP_ID[%d]\n",
                    item2->mME_UE_S1AP_ID ? *item2->mME_UE_S1AP_ID : -1,
                    item2->eNB_UE_S1AP_ID ? *item2->eNB_UE_S1AP_ID : -1);
        }
    }

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_connection_establishment_indication(pkbuf_t **s1apbuf,
        S1AP_MME_UE_S1AP_ID_t *mme_ue_s1ap_id,
        S1AP_ENB_UE_S1AP_ID_t *enb_ue_s1ap_id,
	amf4g_ue_t *amf4g_ue)
{
    status_t rv;

    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_ConnectionEstablishmentIndication_t *ConnectionEstablishmentIndication = NULL;

    S1AP_ConnectionEstablishmentIndicationIEs_t *ie = NULL;
    S1AP_MME_UE_S1AP_ID_t *MME_UE_S1AP_ID = NULL;
    S1AP_ENB_UE_S1AP_ID_t *ENB_UE_S1AP_ID = NULL;

    d_trace(3, "[AMF4G] Connection Establishment Indication\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = 
        core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_ConnectionEstablishmentIndication;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present =
        S1AP_InitiatingMessage__value_PR_ConnectionEstablishmentIndication;

    ConnectionEstablishmentIndication = &initiatingMessage->value.choice.ConnectionEstablishmentIndication;

    /* AMF4G_UE_S1AP_ID */
    ie = core_calloc(1, sizeof(S1AP_ConnectionEstablishmentIndicationIEs_t));
    ASN_SEQUENCE_ADD(&ConnectionEstablishmentIndication->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_MME_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_ConnectionEstablishmentIndicationIEs__value_PR_MME_UE_S1AP_ID;

    MME_UE_S1AP_ID = &ie->value.choice.MME_UE_S1AP_ID;

    /* ENB_UE_S1AP_ID */
    ie = core_calloc(1, sizeof(S1AP_ConnectionEstablishmentIndicationIEs_t));
    ASN_SEQUENCE_ADD(&ConnectionEstablishmentIndication->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_eNB_UE_S1AP_ID;
    ie->criticality = S1AP_Criticality_ignore;
    ie->value.present =
        S1AP_ConnectionEstablishmentIndicationIEs__value_PR_ENB_UE_S1AP_ID;

    ENB_UE_S1AP_ID = &ie->value.choice.ENB_UE_S1AP_ID;

    d_trace(5, "    ENB_UE_S1AP_ID[%d] AMF4G_UE_S1AP_ID[%d]\n",
            enb_ue_s1ap_id, mme_ue_s1ap_id);

    *MME_UE_S1AP_ID = *mme_ue_s1ap_id;
    *ENB_UE_S1AP_ID = *enb_ue_s1ap_id;

    /* UE RADIO CAPABILITY */
    if (amf4g_ue->ueRadioCapability.buf && amf4g_ue->ueRadioCapability.size)
    {
        S1AP_UERadioCapability_t *UERadioCapability = NULL;

        ie = core_calloc(1, sizeof(S1AP_ConnectionEstablishmentIndicationIEs_t));
        ASN_SEQUENCE_ADD(&ConnectionEstablishmentIndication->protocolIEs, ie);

        ie->id = S1AP_ProtocolIE_ID_id_UERadioCapability;
        ie->criticality = S1AP_Criticality_ignore;
        ie->value.present =
            S1AP_ConnectionEstablishmentIndicationIEs__value_PR_ENB_UE_S1AP_ID;

        UERadioCapability = &ie->value.choice.UERadioCapability;

        d_assert(UERadioCapability, return CORE_ERROR,);
        s1ap_buffer_to_OCTET_STRING(
                amf4g_ue->ueRadioCapability.buf, amf4g_ue->ueRadioCapability.size,
                UERadioCapability);
    }

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
}

status_t s1ap_build_ue_information_transfer(
	pkbuf_t **s1apbuf,
	amf4g_ue_t *amf4g_ue,
	S1AP_S_TMSI_t *S_TMSI)
{
    status_t rv;
				
    S1AP_S1AP_PDU_t pdu;
    S1AP_InitiatingMessage_t *initiatingMessage = NULL;
    S1AP_UEInformationTransfer_t *UEInformationTransfer = NULL;

    S1AP_UEInformationTransferIEs_t *ie = NULL;

    d_trace(3, "[AMF4G] UE information transfer\n");

    memset(&pdu, 0, sizeof (S1AP_S1AP_PDU_t));
    pdu.present = S1AP_S1AP_PDU_PR_initiatingMessage;
    pdu.choice.initiatingMessage = core_calloc(1, sizeof(S1AP_InitiatingMessage_t));

    initiatingMessage = pdu.choice.initiatingMessage;
    initiatingMessage->procedureCode = S1AP_ProcedureCode_id_UEInformationTransfer;
    initiatingMessage->criticality = S1AP_Criticality_reject;
    initiatingMessage->value.present = S1AP_InitiatingMessage__value_PR_UEInformationTransfer;

    UEInformationTransfer = &initiatingMessage->value.choice.UEInformationTransfer;

    /* S-TMSI field */
    ie = core_calloc(1, sizeof(S1AP_UEInformationTransferIEs_t));
    ASN_SEQUENCE_ADD(&UEInformationTransfer->protocolIEs, ie);

    ie->id = S1AP_ProtocolIE_ID_id_S_TMSI;
    ie->criticality = S1AP_Criticality_reject;
    ie->value.present = S1AP_UEInformationTransferIEs__value_PR_S_TMSI;

    memcpy(&ie->value.choice.S_TMSI, S_TMSI, sizeof(S1AP_S_TMSI_t));
	
    if (!amf4g_ue)
    {
	return CORE_ERROR;
    }
    else
    {
    	/* UE Level QoS Parameters field */
	amf4g_sess_t *sess = NULL;
	amf4g_bearer_t *bearer = NULL;
	c_uint8_t lowest_priority = 99;
	c_uint8_t ebi = -1;
		
	sess = amf4g_sess_first(amf4g_ue);
	while(sess)
	{
	    bearer = amf4g_bearer_first(sess);
	    while(bearer)
	    {
	        /* Non-GBR */
		if( (bearer->qos.qci >= PDN_QCI_5 && bearer->qos.qci <= PDN_QCI_9) || bearer->qos.qci == PDN_QCI_69 || bearer->qos.qci == PDN_QCI_70)
		{
		    if (bearer->qos.arp.priority_level < lowest_priority)
		    {
			lowest_priority = bearer->qos.arp.priority_level;
			ebi = bearer->ebi;
		    }
		}  

	        bearer = amf4g_bearer_next(bearer);
	    }
	    sess = amf4g_sess_next(sess);
	}
		
	/* Find bearer */
	if (lowest_priority == 99 || ebi == -1)
	{
	    sess = amf4g_sess_first(amf4g_ue);
	    bearer = amf4g_bearer_first(sess);
	}
	else  bearer = amf4g_bearer_find_by_ue_ebi(amf4g_ue, ebi);

	if (bearer)
	{
	    S1AP_E_RABLevelQoSParameters_t *E_RABLevelQoSParameters = NULL;
	    S1AP_GBR_QosInformation_t *gbrQosInformation = NULL;
		
	    ie = core_calloc(1, sizeof(S1AP_UEInformationTransferIEs_t));
	    ASN_SEQUENCE_ADD(&UEInformationTransfer->protocolIEs, ie);

	    ie->id = S1AP_ProtocolIE_ID_id_UE_Level_QoS_Parameters;
	    ie->criticality = S1AP_Criticality_ignore;
	    ie->value.present = S1AP_UEInformationTransferIEs__value_PR_E_RABLevelQoSParameters;

	    E_RABLevelQoSParameters = &ie->value.choice.E_RABLevelQoSParameters;
	    E_RABLevelQoSParameters->qCI = bearer->qos.qci;
	    E_RABLevelQoSParameters->allocationRetentionPriority.priorityLevel = bearer->qos.arp.priority_level;
	    E_RABLevelQoSParameters->allocationRetentionPriority.pre_emptionCapability = bearer->qos.arp.pre_emption_capability;
	    E_RABLevelQoSParameters->allocationRetentionPriority.pre_emptionVulnerability = bearer->qos.arp.pre_emption_vulnerability;
			
	    gbrQosInformation = core_calloc(1, sizeof(struct S1AP_GBR_QosInformation));
            asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateDL, bearer->qos.mbr.downlink);
            asn_uint642INTEGER(&gbrQosInformation->e_RAB_MaximumBitrateUL, bearer->qos.mbr.uplink);
            asn_uint642INTEGER(&gbrQosInformation->e_RAB_GuaranteedBitrateDL, bearer->qos.gbr.downlink);
            asn_uint642INTEGER(&gbrQosInformation->e_RAB_GuaranteedBitrateUL, bearer->qos.gbr.uplink);
            E_RABLevelQoSParameters->gbrQosInformation = gbrQosInformation;
			
	}
	

	/* UE Radio Capability field */
	if (amf4g_ue->ueRadioCapability.buf && amf4g_ue->ueRadioCapability.size)
	{
	    S1AP_UERadioCapability_t *UERadioCapability = NULL;

	    UERadioCapability = &ie->value.choice.UERadioCapability;

	    ie = core_calloc(1, sizeof(S1AP_UEInformationTransferIEs_t));
	    ASN_SEQUENCE_ADD(&UEInformationTransfer->protocolIEs, ie);

	    ie->id = S1AP_ProtocolIE_ID_id_UERadioCapability;
	    ie->criticality = S1AP_Criticality_ignore;
	    ie->value.present = S1AP_UEInformationTransferIEs__value_PR_UERadioCapability;

	    UERadioCapability = &ie->value.choice.UERadioCapability;

	    d_assert(UERadioCapability, return CORE_ERROR,);
	    s1ap_buffer_to_OCTET_STRING(amf4g_ue->ueRadioCapability.buf, amf4g_ue->ueRadioCapability.size, UERadioCapability);
	}

    }

    rv = s1ap_encode_pdu(s1apbuf, &pdu);
    s1ap_free_pdu(&pdu);

    if (rv != CORE_OK)
    {
        d_error("s1ap_encode_pdu() failed");
        return CORE_ERROR;
    }

    return CORE_OK;
	
}
