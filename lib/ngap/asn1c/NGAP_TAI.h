/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NGAP-IEs"
 * 	found in "../support/r14.4.0/38413-e40.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps`
 */

#ifndef	_NGAP_TAI_H_
#define	_NGAP_TAI_H_


#include <asn_application.h>

/* Including external dependencies */
#include "NGAP_PLMNIdentity.h"
#include "NGAP_TAC.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct NGAP_ProtocolExtensionContainer;

/* NGAP_TAI */
typedef struct NGAP_TAI {
	NGAP_PLMNIdentity_t	 pLMNIdentity;
	NGAP_TAC_t	 tAC;
	struct NGAP_ProtocolExtensionContainer	*iE_Extensions;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} NGAP_TAI_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_NGAP_TAI;
extern asn_SEQUENCE_specifics_t asn_SPC_NGAP_TAI_specs_1;
extern asn_TYPE_member_t asn_MBR_NGAP_TAI_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _NGAP_TAI_H_ */
#include <asn_internal.h>
