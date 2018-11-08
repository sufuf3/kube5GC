#ifndef __UPF_IPFW_H__
#define __UPF_IPFW_H__

#include "core_debug.h"

#include "upf_context.h"

CORE_DECLARE(status_t) upf_compile_packet_filter(
        upf_rule_t *pf, c_int8_t *description);

CORE_DECLARE(upf_pdr_t*) upf_pdr_find_by_packet(pkbuf_t *pkt);

#endif /* __UPF_IPFW_H__ */
