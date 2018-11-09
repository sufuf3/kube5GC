/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NGAP-IEs"
 * 	found in "../support/r14.4.0/38413-e40.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps`
 */

#ifndef	_NGAP_EmergencyAreaIDBroadcastNR_Item_H_
#define	_NGAP_EmergencyAreaIDBroadcastNR_Item_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NGAP_EmergencyAreaID.h"
#include "NGAP_CompletedCellsInEAI-NR.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct NGAP_ProtocolExtensionContainer;

/* NGAP_EmergencyAreaIDBroadcastNR-Item */
typedef struct NGAP_EmergencyAreaIDBroadcastNR_Item {
	NGAP_EmergencyAreaID_t	 emergencyAreaID;
	NGAP_CompletedCellsInEAI_NR_t	 completedCellsInEAI_NR;
	struct NGAP_ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NGAP_EmergencyAreaIDBroadcastNR_Item_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NGAP_EmergencyAreaIDBroadcastNR_Item;
extern asn_SEQUENCE_specifics_t asn_SPC_NGAP_EmergencyAreaIDBroadcastNR_Item_specs_1;
extern asn_TYPE_member_t asn_MBR_NGAP_EmergencyAreaIDBroadcastNR_Item_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _NGAP_EmergencyAreaIDBroadcastNR_Item_H_ */
#include <asn_internal.h>
