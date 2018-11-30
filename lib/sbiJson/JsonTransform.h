#ifndef __JSONTRANSFORM__H
#define __JSONTRANSFORM__H

#include "core.h"
#include "core_errno.h"
#include "cJSON.h"
#include "commonJsonKey.h"

#ifdef __cplusplus
extern "C"
{
#endif

CORE_DECLARE(void) JSONTRANSFORM_S2J_create_session_request(void *sess, cJSON *pJson);

#ifdef __cplusplus
}
#endif 

#endif