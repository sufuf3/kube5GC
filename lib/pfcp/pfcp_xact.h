#ifndef __PFCP_XACT_H__
#define __PFCP_XACT_H__

#include "core_pkbuf.h"
#include "core_list.h"
#include "core_index.h"
#include "core_timer.h"

#include "pfcp_message.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _pfcp_node_t pfcp_node_t;

/**
 * Transaction context
 */
typedef struct _pfcp_xact_t {
    lnode_t         node;           /**< A node of list */
    index_t         index;
    
#define PFCP_LOCAL_ORIGINATOR  0
#define PFCP_REMOTE_ORIGINATOR 1
    c_uint8_t       org;            /**< Transaction' originator. 
                                         local or remote */

    c_uint32_t      xid;            /**< Transaction ID */
    pfcp_node_t      *gnode;         /**< Relevant PFCP node context */

    int             step;           /**< Current step in the sequence.
                                         1 : Initial 
                                         2 : Triggered 
                                         3 : Triggered-Reply */
    struct {
        c_uint8_t   type;           /**< Message type history */
        pkbuf_t     *pkbuf;         /**< Packet history */
    } seq[3];                       /**< history for the each step */

    tm_block_id     tm_response;    /**< Timer waiting for next message */
    c_uint8_t       response_rcount;
    tm_block_id     tm_holding;  /**< Timer waiting for holding message */
    c_uint8_t       holding_rcount;

    struct _pfcp_xact_t *assoc_xact; /**< Associated transaction */

#define PFCP_XACT_STORE_SESSION(xact, session) \
    do { \
        d_assert((xact), break, "Null param"); \
        d_assert((session), break, "Null param"); \
        ((xact)->sess) = (session); \
    } while(0)

#define PFCP_XACT_RETRIEVE_SESSION(xact) ((xact) ? ((xact)->sess) : NULL)
    void            *sess;          /**< Session Store */
    void            *gtp_xact;          //$ gtp_xact_t
    pkbuf_t         *gtpbuf;
} pfcp_xact_t;

CORE_DECLARE(status_t) pfcp_xact_init(tm_service_t *tm_service,
        c_uintptr_t response_event, c_uintptr_t holding_event);
CORE_DECLARE(status_t) pfcp_xact_final(void);

CORE_DECLARE(pfcp_xact_t *) pfcp_xact_local_create(
        pfcp_node_t *gnode, pfcp_header_t *hdesc, pkbuf_t *pkbuf);
CORE_DECLARE(pfcp_xact_t *) pfcp_xact_remote_create(
        pfcp_node_t *gnode, c_uint32_t sqn);
CORE_DECLARE(void) pfcp_xact_delete_all(pfcp_node_t *gnode);

CORE_DECLARE(status_t) pfcp_xact_update_tx(pfcp_xact_t *xact,
        pfcp_header_t *hdesc, pkbuf_t *pkbuf);
CORE_DECLARE(status_t) pfcp_xact_update_rx(pfcp_xact_t *xact, c_uint8_t type);

CORE_DECLARE(status_t) pfcp_xact_commit(pfcp_xact_t *xact);
CORE_DECLARE(status_t) pfcp_xact_timeout(index_t index, c_uintptr_t event, c_uint8_t *type);

CORE_DECLARE(status_t) pfcp_xact_receive(
        pfcp_node_t *gnode, pfcp_header_t *h, pfcp_xact_t **xact);

CORE_DECLARE(pfcp_xact_t *) pfcp_xact_find(index_t index);
CORE_DECLARE(pfcp_xact_t *)pfcp_xact_find_by_xid(
        pfcp_node_t *gnode, c_uint8_t type, c_uint32_t xid);
//CORE_DECLARE(void) pfcp_xact_associate(pfcp_xact_t *xact1, pfcp_xact_t *xact2);
//CORE_DECLARE(void) pfcp_xact_deassociate(pfcp_xact_t *xact1, pfcp_xact_t *xact2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PFCP_XACT_H__ */
