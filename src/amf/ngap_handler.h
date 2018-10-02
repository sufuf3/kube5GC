#ifndef __NGAP_HANDLER_H__
#define __NGAP_HANDLER_H__

#include "ngap/ngap_message.h"

/* TODP: need to modify the include head file to amf_context.h*/
#include "mme_context.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ngap_handle_ng_setup_request(amf_ran_t *ran, ngap_message_t *message);

CORE_DECLARE(void) ngap_handle_ng_setup_request(
        amf_ran_t *ran, ngap_message_t *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NGAP_HANDLER_H__ */
