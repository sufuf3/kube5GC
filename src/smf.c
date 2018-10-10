#define TRACE_MODULE _smf_main

#include "core_debug.h"
#include "core_signal.h"
#include "core_semaphore.h"

#include "app/context.h"
#include "app/app.h"

const char *app_name = "smf";

status_t app_initialize(const char *config_path, const char *log_path)
{
    status_t rv;
    int app = 0;

    rv = app_will_initialize(config_path, log_path);
    if (rv != CORE_OK) return rv;

    app = context_self()->logger.trace.app;
    if (app)
    {
        d_trace_level(&_smf_main, app);
    }

    d_trace(1, "SMF try to initialize\n");
    rv = smf_initialize();
    if (rv != CORE_OK)
    {
        d_error("Failed to intialize SMF");
        return rv;
    }
    d_trace(1, "SMF initialize...done\n");

    rv = app_did_initialize();
    if (rv != CORE_OK) return rv;

    return CORE_OK;
}

void app_terminate(void)
{
    app_will_terminate();

    d_trace(1, "SMF try to terminate\n");
    smf_terminate();
    d_trace(1, "SMF terminate...done\n");

    app_did_terminate();
}
