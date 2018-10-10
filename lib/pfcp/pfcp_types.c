/*
 * Copyright (c) 2017, NextEPC Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define TRACE_MODULE _pfcptypes

#include "core_debug.h"
#include "core_lib.h"
#include "core_network.h"

#include "pfcp_types.h"
#include "pfcp_message.h"

char* pfcp_cause_get_name(c_uint8_t cause)
{
    switch(cause)
    {
        case PFCP_CAUSE_SUCCESS:
            return "PFCP_CAUSE_SUCCESS";
            break;
        case PFCP_CAUSE_REQUEST_REJECTED:
            return "PFCP_CAUSE_REQUEST_REJECTED";
            break;
        case PFCP_CAUSE_SESSION_CONTEXT_NOT_FOUND:
            return "PFCP_CAUSE_SESSION_CONTEXT_NOT_FOUND";
            break;
        case PFCP_CAUSE_MANDATORY_IE_MISSING:
            return "PFCP_CAUSE_MANDATORY_IE_MISSING";
            break;
        case PFCP_CAUSE_CONDITIONAL_IE_MISSING:
            return "PFCP_CAUSE_CONDITIONAL_IE_MISSING";
            break;
        case PFCP_CAUSE_INVALID_LENGTH:
            return "PFCP_CAUSE_INVALID_LENGTH";
            break;
        case PFCP_CAUSE_MANDATORY_IE_INCORRECT:
            return "PFCP_CAUSE_MANDATORY_IE_INCORRECT";
            break;
        case PFCP_CAUSE_INVALID_FORWARDING_POLICY:
            return "PFCP_CAUSE_INVALID_FORWARDING_POLICY";
            break;
        case PFCP_CAUSE_INVALID_F_TEID_ALLOCATION_OPTION:
            return "PFCP_CAUSE_INVALID_F_TEID_ALLOCATION_OPTION";
            break;
        case PFCP_CAUSE_NO_ESTABLISHED_PFCP_ASSOCIATION:
            return "PFCP_CAUSE_NO_ESTABLISHED_PFCP_ASSOCIATION";
            break;
        case PFCP_CAUSE_RULE_CREATION_MODIFICATION_FAILURE:
            return "PFCP_CAUSE_RULE_CREATION_MODIFICATION_FAILURE";
            break;
        case PFCP_CAUSE_PFCP_ENTITY_IN_CONGESTION:
            return "PFCP_CAUSE_PFCP_ENTITY_IN_CONGESTION";
            break;
        case PFCP_CAUSE_NO_RESOURCES_AVAILABLE:
            return "PFCP_CAUSE_NO_RESOURCES_AVAILABLE";
            break;
        case PFCP_CAUSE_SERVICE_NOT_SUPPORTED:
            return "PFCP_CAUSE_SERVICE_NOT_SUPPORTED";
            break;
        case PFCP_CAUSE_SYSTEM_FAILURE:
            return "PFCP_CAUSE_SYSTEM_FAILURE";
            break;
    }
    return "PFCP_CAUSE_UNKNOWN";
}
