/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NGAP-IEs"
 * 	found in "../support/r14.4.0/38413-e40.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps`
 */

#ifndef	_NGAP_DLForwarding_H_
#define	_NGAP_DLForwarding_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum NGAP_DLForwarding {
	NGAP_DLForwarding_dl_forwarding_proposed	= 0
	/*
	 * Enumeration is extensible
	 */
} e_NGAP_DLForwarding;

/* NGAP_DLForwarding */
typedef long	 NGAP_DLForwarding_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_NGAP_DLForwarding_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_NGAP_DLForwarding;
extern const asn_INTEGER_specifics_t asn_SPC_DLForwarding_specs_1;
asn_struct_free_f DLForwarding_free;
asn_struct_print_f DLForwarding_print;
asn_constr_check_f DLForwarding_constraint;
ber_type_decoder_f DLForwarding_decode_ber;
der_type_encoder_f DLForwarding_encode_der;
xer_type_decoder_f DLForwarding_decode_xer;
xer_type_encoder_f DLForwarding_encode_xer;
oer_type_decoder_f DLForwarding_decode_oer;
oer_type_encoder_f DLForwarding_encode_oer;
per_type_decoder_f DLForwarding_decode_uper;
per_type_encoder_f DLForwarding_encode_uper;
per_type_decoder_f DLForwarding_decode_aper;
per_type_encoder_f DLForwarding_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _NGAP_DLForwarding_H_ */
#include <asn_internal.h>
