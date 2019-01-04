#ifndef __PFCP_TYPES_H__
#define __PFCP_TYPES_H__

#include "core_tlv_msg.h"
#include "3gpp_types.h"

typedef struct c_sockaddr_t c_sockaddr_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define PFCP_VERSION    1

//$ CAUSE
#define PFCP_CAUSE_SUCCESS                              1
#define PFCP_CAUSE_REQUEST_REJECTED                     64
#define PFCP_CAUSE_SESSION_CONTEXT_NOT_FOUND            65
#define PFCP_CAUSE_MANDATORY_IE_MISSING                 66
#define PFCP_CAUSE_CONDITIONAL_IE_MISSING               67
#define PFCP_CAUSE_INVALID_LENGTH                       68
#define PFCP_CAUSE_MANDATORY_IE_INCORRECT               69
#define PFCP_CAUSE_INVALID_FORWARDING_POLICY            70
#define PFCP_CAUSE_INVALID_F_TEID_ALLOCATION_OPTION     71
#define PFCP_CAUSE_NO_ESTABLISHED_PFCP_ASSOCIATION      72
#define PFCP_CAUSE_RULE_CREATION_MODIFICATION_FAILURE   73
#define PFCP_CAUSE_PFCP_ENTITY_IN_CONGESTION            74
#define PFCP_CAUSE_NO_RESOURCES_AVAILABLE               75
#define PFCP_CAUSE_SERVICE_NOT_SUPPORTED                76
#define PFCP_CAUSE_SYSTEM_FAILURE                       77
char* pfcp_cause_get_name(c_uint8_t cause);


//$ Apply Action
#define PFCP_FAR_APPLY_ACTION_DROP      1
#define PFCP_FAR_APPLY_ACTION_FORW      2
#define PFCP_FAR_APPLY_ACTION_BUFF      4
#define PFCP_FAR_APPLY_ACTION_NOCP      8
#define PFCP_FAR_APPLY_ACTION_DUPL     16

//$ destination interface
#define PFCP_FAR_DEST_INTF_ACCESS       0   //$ DL traffic
#define PFCP_FAR_DEST_INTF_CORE         1   //$ UL traffic
#define PFCP_FAR_DEST_INTF_SGILAN       2   //$ SGi-LAN
#define PFCP_FAR_DEST_INTF_CPF          3   //$ CP-Function
#define PFCP_FAR_DEST_INTF_LIF          4   //$ LI Function

//$ Precedence
#define PGWC_PRECEDENCE_BASE  31

//$ outer header remove
#define PFCP_OUTER_HDR_RMV_DESC_GTPU_IP4    0
#define PFCP_OUTER_HDR_RMV_DESC_GTPU_IP6    1
#define PFCP_OUTER_HDR_RMV_DESC_UDP_IP4     2
#define PFCP_OUTER_HDR_RMV_DESC_UDP_IP6     3
#define PFCP_OUTER_HDR_RMV_DESC_NULL        0xFF  

//$ source interface
#define PFCP_SRC_INTF_ACCESS    0  //$ UL traffic
#define PFCP_SRC_INTF_CORE      1  //$ DL traffic
#define PFCP_SRC_INTF_SGILAN    2  //$ SGi-LAN
#define PFCP_SRC_INTF_CP_F      3  //$ CP-function

//$ PDN type
#define PFCP_PDN_TYPE_IPV4      1
#define PFCP_PDN_TYPE_IPV6      2
#define PFCP_PDN_TYPE_IPV4V6    3
#define PFCP_PDN_TYPE_NONIP     4


#define PFCP_UE_IP_ADDR_HDR_LEN                      1
#define PFCP_UE_IP_ADDR_IPV4_LEN                     IPV4_LEN+PFCP_UE_IP_ADDR_HDR_LEN
#define PFCP_UE_IP_ADDR_IPV6_LEN                     IPV6_LEN+PFCP_UE_IP_ADDR_HDR_LEN
#define PFCP_UE_IP_ADDR_IPV4V6_LEN                   IPV4V6_LEN+PFCP_UE_IP_ADDR_HDR_LEN

#define PFCP_UE_IP_ADDR_SOURCE                  0
#define PFCP_UE_IP_ADDR_DESITINATION            1
typedef struct _pfcp_ue_ip_addr_t {
ED4(c_uint8_t       spare:5;,
    c_uint8_t       sd:1;,            /* source or destination*/
    c_uint8_t       ipv4:1;,
    c_uint8_t       ipv6:1;)
    union {
        /* GTP_F_TEID_IPV4 */
        c_uint32_t addr;
    
        /* GTP_F_TEID_IPV6 */
        c_uint8_t addr6[IPV6_LEN];
    
        /* GTP_F_TEID_BOTH */
        struct {
            c_uint32_t addr;
            c_uint8_t addr6[IPV6_LEN];
        } both;
    };
} __attribute__ ((packed)) pfcp_ue_ip_addr_t;


#define PFCP_F_TEID_HDR_LEN                      5
#define PFCP_F_TEID_IPV4_LEN                     IPV4_LEN+PFCP_F_TEID_HDR_LEN
#define PFCP_F_TEID_IPV6_LEN                     IPV6_LEN+PFCP_F_TEID_HDR_LEN
#define PFCP_F_TEID_IPV4V6_LEN                   IPV4V6_LEN+PFCP_F_TEID_HDR_LEN
typedef struct _pfcp_f_teid_t {
ED5(c_uint8_t       spare:4;,
    c_uint8_t       chid:1;,
    c_uint8_t       ch:1;,
    c_uint8_t       ipv6:1;,
    c_uint8_t       ipv4:1;)
    c_uint32_t      teid;
    union {
        union {
            /* GTP_F_TEID_IPV4 */
            c_uint32_t addr;
    
            /* GTP_F_TEID_IPV6 */
            c_uint8_t addr6[IPV6_LEN];
    
            /* GTP_F_TEID_BOTH */
            struct {
                c_uint32_t addr;
                c_uint8_t addr6[IPV6_LEN];
            } both;
        };
        c_uint8_t choose_id;
    };
} __attribute__ ((packed)) pfcp_f_teid_t;


#define PFCP_F_SEID_HDR_LEN                      9
#define PFCP_F_SEID_IPV4_LEN                     IPV4_LEN+PFCP_F_SEID_HDR_LEN
#define PFCP_F_SEID_IPV6_LEN                     IPV6_LEN+PFCP_F_SEID_HDR_LEN
#define PFCP_F_SEID_IPV4V6_LEN                   IPV4V6_LEN+PFCP_F_SEID_HDR_LEN
typedef struct _pfcp_f_seid_t {
ED3(c_uint8_t       spare:6;,
    c_uint8_t       ipv4:1;,
    c_uint8_t       ipv6:1;)
    c_uint64_t      seid;
    union {
        /* PFCP_F_TEID_IPV4 */
        c_uint32_t addr;

        /* PFCP_F_TEID_IPV6 */
        c_uint8_t addr6[IPV6_LEN];

        /* PFCP_F_TEID_BOTH */
        struct {
            c_uint32_t addr;
            c_uint8_t addr6[IPV6_LEN];
        } both;
    };
} __attribute__ ((packed)) pfcp_f_seid_t;


typedef struct _pfcp_node_id_t {
ED2(c_uint8_t spare:4;,
    c_uint8_t type:4;)
#define PFCP_NODE_ID_IPV4   0
#define PFCP_NODE_ID_IPV6   1
#define PFCP_NODE_ID_FQDN   2
#define PFPC_NODE_ID_LEN(__nid) \
        (1 + ((__nid.type != PFCP_NODE_ID_IPV4) ? \
        (__nid.type != PFCP_NODE_ID_IPV6 ? -1 : 16 ) : 4))
    union {
        /* IPV4 */
        c_uint32_t addr;

        /* IPV6 */
        c_uint8_t addr6[IPV6_LEN];
    };
} __attribute__ ((packed)) pfcp_node_id_t;

//$ outer header creation in FAR
typedef struct _pfcp_outer_hdr_t {
ED5(c_uint8_t       spare:4;,
    c_uint8_t       udp_ipv6:1;,
    c_uint8_t       udp_ipv4:1;,
    c_uint8_t       gtpu_ipv6:1;,
    c_uint8_t       gtpu_ipv4:1;)
    c_uint8_t       void0;
    c_uint32_t      teid;
    union {
        /* PFCP_F_TEID_IPV4 */
        c_uint32_t addr;

        /* PFCP_F_TEID_IPV6 */
        c_uint8_t addr6[IPV6_LEN];

        /* PFCP_F_TEID_BOTH */
        struct {
            c_uint32_t addr;
            c_uint8_t addr6[IPV6_LEN];
        } both;
    };
} __attribute__ ((packed)) pfcp_outer_hdr_t;

typedef struct _pfcp_report_type_t {
ED5(c_uint8_t       spare0:4;,
    c_uint8_t       UPIR:1;,            /* User Plane Inactivity Report */
    c_uint8_t       ERIR:1;,            /* Error Indication Report */
    c_uint8_t       USAR:1;,            /* Usage Report */
    c_uint8_t       DLDR:1;)            /* Downlink Data Report */
} __attribute__ ((packed)) pfcp_report_type_t;

typedef struct _pfcp_downlink_data_service_information_t {
#define PFCP_DOWNLINK_DATA_SERVICE_INFORMATION_LEN(__data) \
    sizeof(struct _pfcp_downlink_data_service_information_t) - (__data).PPI - (__data).QFII
ED3(c_uint8_t       spare1:6;,
    c_uint8_t       PPI:1;,            /* Paging Policy Indication */
    c_uint8_t       QFII:1;)
ED2(
    c_uint8_t       spare2:2;,
    c_uint8_t       paging_policy_indication:6;
)
ED2(
    c_uint8_t       spare:2;,
    c_uint8_t       QFI:6;
)
} __attribute__ ((packed)) pfcp_downlink_data_service_information_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PFCP_TYPES_H__ */

