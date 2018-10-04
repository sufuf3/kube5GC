#define TRACE_MODULE _ngap_build

#include "core_debug.h"

#include "ngap_build.h"

status_t ngap_build_setup_rsp(pkbuf_t **pkbuf)
{
//  status_t rv;
//     int i, j;

    // NGAP_NGAP_PDU_t pdu;
    // NGAP_SuccessfulOutcome_t *successfulOutcome = NULL;
    // NGAP_NGSetupResponse_t *NGSetupResponse = NULL;
    
    // NGAP_NGSetupResponseIEs_t *ie = NULL;
    // // NGAP_ServedGUAMIItem_t *ServedGUAMIItemIEs = NULL;
    // NGAP_AMFName_t *AMFName = NULL;
    // NGAP_ServedGUAMIList_t *ServedGUAMIList = NULL;
    // NGAP_RelativeAMFCapacity_t *RelativeAMFCapacity = NULL;
    // NGAP_PLMNSupportList_t *PLMNSupportList = NULL;
            
    // memset(&pdu, 0, sizeof (NGAP_NGAP_PDU_t));
    // pdu.present = NGAP_NGAP_PDU_PR_successfulOutcome;
    // pdu.choice.successfulOutcome = 
    //     core_calloc(1, sizeof(NGAP_SuccessfulOutcome_t));

    // successfulOutcome = pdu.choice.successfulOutcome;
    // successfulOutcome->procedureCode = NGAP_ProcedureCode_id_NGSetup;
    // successfulOutcome->criticality = NGAP_Criticality_reject;
    // successfulOutcome->value.present =
    //     NGAP_SuccessfulOutcome__value_PR_NGSetupResponse;

    // NGSetupResponse = &successfulOutcome->value.choice.NGSetupResponse;

    // ie = core_calloc(1, sizeof(NGAP_NGSetupResponseIEs_t));
    // ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    // ie->id = NGAP_ProtocolIE_ID_id_AMFName;
    // ie->criticality = NGAP_Criticality_reject;
    // ie->value.present = NGAP_NGSetupResponseIEs__value_PR_AMFName;
    // AMFName = &ie->value.choice.AMFName;

    // ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    // ie->id = NGAP_ProtocolIE_ID_id_ServedGUAMIList;
    // ie->criticality = NGAP_Criticality_reject;
    // ie->value.present = NGAP_NGSetupResponseIEs__value_PR_ServedGUAMIList;
    // ServedGUAMIList = &ie->value.choice.ServedGUAMIList;

    // ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    // ie->id = NGAP_ProtocolIE_ID_id_RelativeAMFCapacity;
    // ie->criticality = NGAP_Criticality_ignore;
    // ie->value.present = NGAP_NGSetupResponseIEs__value_PR_RelativeAMFCapacity;
    // RelativeAMFCapacity = &ie->value.choice.RelativeAMFCapacity;

    // ASN_SEQUENCE_ADD(&NGSetupResponse->protocolIEs, ie);
    // ie->id = NGAP_ProtocolIE_ID_id_PLMNSupportList;
    // ie->criticality = NGAP_Criticality_ignore;
    // ie->value.present = NGAP_NGSetupResponseIEs__value_PR_PLMNSupportList;
    // PLMNSupportList = &ie->value.choice.PLMNSupportList;

    return CORE_OK;
}


status_t ngap_build_setup_failure(pkbuf_t **pkbuf, NGAP_Cause_PR group, long cause, long time_to_wait)
{
    return CORE_OK;
}
