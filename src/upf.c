#define TRACE_MODULE _upf_main

#include "core_debug.h"
#include "core_signal.h"
#include "core_semaphore.h"

#include "app/context.h"
#include "app/app.h"

const char *app_name = "upf";

status_t app_initialize(const char *config_path, const char *log_path)
{
    status_t rv;
    int app = 0;

    rv = app_will_initialize(config_path, log_path);
    if (rv != CORE_OK) return rv;

    app = context_self()->logger.trace.app;
    if (app)
    {
        d_trace_level(&_upf_main, app);
    }

    d_trace(1, "UPF try to initialize\n");
    rv = upf_initialize();
    if (rv != CORE_OK)
    {
        d_error("Failed to intialize UPF");
        return rv;
    }
    d_trace(1, "UPF initialize...done\n");

    rv = app_did_initialize();
    if (rv != CORE_OK) return rv;

return CORE_OK;
}

void app_terminate(void)
{
    app_will_terminate();

    d_trace(1, "UPF try to terminate\n");
    upf_terminate();
    d_trace(1, "UPF terminate...done\n");

    app_did_terminate();
}
