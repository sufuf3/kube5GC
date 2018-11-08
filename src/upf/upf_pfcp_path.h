#ifndef __UPF_PFCP_PATH_H__
#define __UPF_PFCP_PATH_H__

#include "pfcp/pfcp_xact.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(status_t) upf_pfcp_open();
CORE_DECLARE(status_t) upf_pfcp_close();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __UPF_PFCP_PATH_H__ */
