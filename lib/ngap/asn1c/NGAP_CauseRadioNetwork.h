/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "NGAP-IEs"
 * 	found in "../support/r14.4.0/38413-e40.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps`
 */

#ifndef	_NGAP_CauseRadioNetwork_H_
#define	_NGAP_CauseRadioNetwork_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum NGAP_CauseRadioNetwork {
	NGAP_CauseRadioNetwork_unspecified	= 0,
	NGAP_CauseRadioNetwork_txnrelocoverall_expiry	= 1,
	NGAP_CauseRadioNetwork_successful_handover	= 2,
	NGAP_CauseRadioNetwork_release_due_to_ngran_generated_reason	= 3,
	NGAP_CauseRadioNetwork_release_due_to_5gc_generated_reason	= 4,
	NGAP_CauseRadioNetwork_handover_cancelled	= 5,
	NGAP_CauseRadioNetwork_partial_handover	= 6,
	NGAP_CauseRadioNetwork_ho_failure_in_target_5GC_ngran_node_or_target_system	= 7,
	NGAP_CauseRadioNetwork_ho_target_not_allowed	= 8,
	NGAP_CauseRadioNetwork_tngrelocoverall_expiry	= 9,
	NGAP_CauseRadioNetwork_tngrelocprep_expiry	= 10,
	NGAP_CauseRadioNetwork_cell_not_available	= 11,
	NGAP_CauseRadioNetwork_unknown_targetID	= 12,
	NGAP_CauseRadioNetwork_no_radio_resources_available_in_target_cell	= 13,
	NGAP_CauseRadioNetwork_unknown_local_UE_NGAP_ID	= 14,
	NGAP_CauseRadioNetwork_inconsistent_remote_UE_NGAP_ID	= 15,
	NGAP_CauseRadioNetwork_handover_desirable_for_radio_reason	= 16,
	NGAP_CauseRadioNetwork_time_critical_handover	= 17,
	NGAP_CauseRadioNetwork_resource_optimisation_handover	= 18,
	NGAP_CauseRadioNetwork_reduce_load_in_serving_cell	= 19,
	NGAP_CauseRadioNetwork_user_inactivity	= 20,
	NGAP_CauseRadioNetwork_radio_connection_with_ue_lost	= 21,
	NGAP_CauseRadioNetwork_load_balancing_tau_required	= 22,
	NGAP_CauseRadioNetwork_radio_resources_not_available	= 23,
	NGAP_CauseRadioNetwork_invalid_qos_combination	= 24,
	NGAP_CauseRadioNetwork_failure_in_radio_interface_procedure	= 25,
	NGAP_CauseRadioNetwork_interaction_with_other_procedure	= 26,
	NGAP_CauseRadioNetwork_unknown_PDU_session_ID	= 27,
	NGAP_CauseRadioNetwork_unkown_qos_flow_ID	= 28,
	NGAP_CauseRadioNetwork_multiple_PDU_session_ID_instances	= 29,
	NGAP_CauseRadioNetwork_multiple_qos_flow_ID_instances	= 30,
	NGAP_CauseRadioNetwork_encryption_and_or_integrity_protection_algorithms_not_supported	= 31,
	NGAP_CauseRadioNetwork_ng_intra_system_handover_triggered	= 32,
	NGAP_CauseRadioNetwork_ng_inter_system_handover_triggered	= 33,
	NGAP_CauseRadioNetwork_xn_handover_triggered	= 34,
	NGAP_CauseRadioNetwork_not_supported_5QI_value	= 35,
	NGAP_CauseRadioNetwork_ue_context_transfer	= 36,
	NGAP_CauseRadioNetwork_ims_voice_eps_fallback_or_rat_fallback_triggered	= 37,
	NGAP_CauseRadioNetwork_up_integrity_protection_not_possible	= 38,
	NGAP_CauseRadioNetwork_up_confidentiality_protection_not_possible	= 39,
	NGAP_CauseRadioNetwork_slice_not_supported	= 40,
	NGAP_CauseRadioNetwork_ue_in_rrc_inactive_state_not_reachable	= 41
	/*
	 * Enumeration is extensible
	 */
} e_NGAP_CauseRadioNetwork;

/* NGAP_CauseRadioNetwork */
typedef long	 NGAP_CauseRadioNetwork_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_NGAP_CauseRadioNetwork_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_NGAP_CauseRadioNetwork;
extern const asn_INTEGER_specifics_t asn_SPC_CauseRadioNetwork_specs_1;
asn_struct_free_f CauseRadioNetwork_free;
asn_struct_print_f CauseRadioNetwork_print;
asn_constr_check_f CauseRadioNetwork_constraint;
ber_type_decoder_f CauseRadioNetwork_decode_ber;
der_type_encoder_f CauseRadioNetwork_encode_der;
xer_type_decoder_f CauseRadioNetwork_decode_xer;
xer_type_encoder_f CauseRadioNetwork_encode_xer;
oer_type_decoder_f CauseRadioNetwork_decode_oer;
oer_type_encoder_f CauseRadioNetwork_encode_oer;
per_type_decoder_f CauseRadioNetwork_decode_uper;
per_type_encoder_f CauseRadioNetwork_encode_uper;
per_type_decoder_f CauseRadioNetwork_decode_aper;
per_type_encoder_f CauseRadioNetwork_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _NGAP_CauseRadioNetwork_H_ */
#include <asn_internal.h>
