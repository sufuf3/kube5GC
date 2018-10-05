#define TRACE_MODULE _ngap_build

#include "core_debug.h"

#include "3gpp_types.h"
#include "s1ap_conv.h"
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
        NGAP_PLMNIdentity_t *PLMNidentity = NULL;
        PLMNidentity = (NGAP_PLMNIdentity_t *) core_calloc(1, sizeof(NGAP_PLMNIdentity_t));
        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        s1ap_buffer_to_OCTET_STRING(&served_guami->plmn_id, PLMN_ID_LEN, PLMNidentity);
        //TODO: need to check 
        ASN_SEQUENCE_ADD(&ServedGUAMIItem->gUAMI.pLMNIdentity, PLMNidentity);
        //}
	    NGAP_AMFRegionID_t	*aMFRegionID = NULL;
        aMFRegionID = (NGAP_AMFRegionID_t *) core_calloc(1, sizeof(NGAP_AMFRegionID_t));
        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        ASN_SEQUENCE_ADD(&ServedGUAMIItem->gUAMI.aMFRegionID, aMFRegionID);

        NGAP_AMFSetID_t	 *aMFSetID = NULL;
        aMFSetID = (NGAP_AMFSetID_t *) core_calloc(1, sizeof(NGAP_AMFSetID_t));
        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        ASN_SEQUENCE_ADD(&ServedGUAMIItem->gUAMI.aMFSetID, aMFSetID);
            
	    NGAP_AMFPointer_t *aMFPointer = NULL;
        aMFPointer = (NGAP_AMFPointer_t *) core_calloc(1, sizeof(NGAP_AMFPointer_t));
        //TODO: fix it, modify the api to ngap_buffer to OCTET String
        ASN_SEQUENCE_ADD(&ServedGUAMIItem->gUAMI.aMFPointer, aMFPointer);

        ASN_SEQUENCE_ADD(&ServedGUAMIList->list, ServedGUAMIItem);
    }
    
    for (i = 0; i < mme_self()->max_num_of_plmn_support; i++)
    {
        NGAP_PLMNSupportItem_t *PLMNSupportItem = NULL;
        PLMNSupportItem = (NGAP_PLMNSupportItem_t *) core_calloc(1, sizeof(NGAP_PLMNSupportItem_t));

        plmn_support_t *plmn_support = &mme_self()->plmn_support[i];

	    NGAP_PLMNIdentity_t	*pLMNIdentity = NULL;
        pLMNIdentity = (NGAP_PLMNIdentity_t *) core_calloc(1, sizeof(NGAP_PLMNIdentity_t));
        s1ap_buffer_to_OCTET_STRING(&plmn_support->plmn_id[i], PLMN_ID_LEN, pLMNIdentity);
        //TODO: need to check 
        ASN_SEQUENCE_ADD(&PLMNSupportItem->pLMNIdentity, pLMNIdentity);

        NGAP_SliceSupportList_t	*sliceSupportList = NULL;
        sliceSupportList = (NGAP_SliceSupportList_t *) core_calloc(1, sizeof(NGAP_SliceSupportList_t));
        for (j = 0; j < mme_self()->plmn_support->num_of_s_nssai; j++)
        {         
             NGAP_SliceSupportItem_t *SliceSupportItem = NULL;
            // SliceSupportItem = (NGAP_SliceSupportItem_t *) core_calloc(1, sizeof(NGAP_SliceSupportItem_t));
            // memcpy( SliceSupportItem->s_NSSAI.sST, &plmn_support->s_nssai[j].sst,SST_LEN );
            s1ap_buffer_to_OCTET_STRING(&plmn_support->s_nssai[j].sst, SST_LEN, &SliceSupportItem->s_NSSAI.sST);
           
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
