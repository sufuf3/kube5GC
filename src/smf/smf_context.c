#define TRACE_MODULE _smf_context

#include "core_debug.h"

#include "smf_context.h"

static smf_context_t self;
static int context_initialized = 0;

status_t smf_context_init()
{
    d_assert(context_initialized == 0, return CORE_ERROR,
            "SMF context already has been initialized");

    /* Initialize SMF context */
    memset(&self, 0, sizeof(smf_context_t));

    context_initialized = 1;

    return CORE_OK;
}

status_t smf_context_final()
{
    d_assert(context_initialized == 1, return CORE_ERROR,
            "SMF context already has been finalized");

    context_initialized = 0;
    
    return CORE_OK;
}

smf_context_t* smf_self()
{
    return &self;
}

status_t smf_context_parse_config()
{
    return CORE_OK;
}

status_t smf_context_setup_trace_module()
{
    return CORE_OK;
}

status_t smf_ue_pool_generate()
{
    return CORE_OK;
}