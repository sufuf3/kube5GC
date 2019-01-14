#ifndef __SMF_IPFW_H__
#define __SMF_IPFW_H__

#include "core_debug.h"

#include "smf_context.h"

CORE_DECLARE(status_t) smf_compile_packet_filter(
        smf_rule_t *pf, c_int8_t *description);

CORE_DECLARE(smf_bearer_t*) smf_bearer_find_by_packet(pkbuf_t *pkt);

#endif /* __SMF_IPFW_H__ */
