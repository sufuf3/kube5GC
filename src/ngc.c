#define TRACE_MODULE _ngc_main

#include "core_general.h"
#include "core_debug.h"
#include "core_semaphore.h"

#include "app/context.h"
#include "app/app.h"

static semaphore_id pcrf_sem1 = 0;
static semaphore_id pcrf_sem2 = 0;

static semaphore_id upf_sem1 = 0;
static semaphore_id upf_sem2 = 0;

static semaphore_id smf_sem1 = 0;
static semaphore_id smf_sem2 = 0;

static semaphore_id hss_sem1 = 0;
static semaphore_id hss_sem2 = 0;

const char *app_name = "ngc";

status_t app_initialize(const char *config_path, const char *log_path)
{
    pid_t pid;
    status_t rv;
    int app = 0;

    rv = app_will_initialize(config_path, log_path);
    if (rv != CORE_OK) return rv;

    app = context_self()->logger.trace.app;
    if (app)
    {
        d_trace_level(&_ngc_main, app);
    }


    /************************* PCRF Process **********************/
    semaphore_create(&pcrf_sem1, 0); /* copied to PCRF/UPF/HSS process */
    semaphore_create(&pcrf_sem2, 0); /* copied to PCRF/UPF/HSS process */

    if (context_self()->parameter.no_pcrf == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            d_trace(1, "PCRF try to initialize\n");
            rv = pcrf_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize PCRF");
            d_trace(1, "PCRF initialize...done\n");

            if (pcrf_sem1) semaphore_post(pcrf_sem1);
            if (pcrf_sem2) semaphore_wait(pcrf_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "PCRF try to terminate\n");
                pcrf_terminate();
                d_trace(1, "PCRF terminate...done\n");
            }

            if (pcrf_sem1) semaphore_post(pcrf_sem1);

            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (pcrf_sem1) semaphore_wait(pcrf_sem1);
    }


    /************************* UPF Process **********************/

    semaphore_create(&upf_sem1, 0); /* copied to UPF/HSS process */
    semaphore_create(&upf_sem2, 0); /* copied to UPF/HSS process */

    if (context_self()->parameter.no_upf == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);

            d_trace(1, "UPF try to initialize\n");
            rv = upf_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize UPF");
            d_trace(1, "UPF initialize...done\n");

            if (upf_sem1) semaphore_post(upf_sem1);
            if (upf_sem2) semaphore_wait(upf_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "UPF try to terminate\n");
                upf_terminate();
                d_trace(1, "UPF terminate...done\n");
            }

            if (upf_sem1) semaphore_post(upf_sem1);

            /* allocated from parent process */
            if (upf_sem1) semaphore_delete(upf_sem1);
            if (upf_sem2) semaphore_delete(upf_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (upf_sem1) semaphore_wait(upf_sem1);
    }


    /************************* SMF Process **********************/

    semaphore_create(&smf_sem1, 0); /* copied to SMF/HSS process */
    semaphore_create(&smf_sem2, 0); /* copied to SMF/HSS process */

    if (context_self()->parameter.no_upf == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);
            if (upf_sem1) semaphore_delete(upf_sem1);
            if (upf_sem2) semaphore_delete(upf_sem2);

            d_trace(1, "SMF try to initialize\n");
            rv = smf_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize SMF");
            d_trace(1, "SMF initialize...done\n");

            if (smf_sem1) semaphore_post(smf_sem1);
            if (smf_sem2) semaphore_wait(smf_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "SMF try to terminate\n");
                smf_terminate();
                d_trace(1, "SMF terminate...done\n");
            }

            if (smf_sem1) semaphore_post(smf_sem1);

            /* allocated from parent process */
            if (smf_sem1) semaphore_delete(smf_sem1);
            if (smf_sem2) semaphore_delete(smf_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (smf_sem1) semaphore_wait(smf_sem1);
    }


    /************************* HSS Process **********************/

    semaphore_create(&hss_sem1, 0); /* copied to HSS process */
    semaphore_create(&hss_sem2, 0); /* copied to HSS process */

    if (context_self()->parameter.no_hss == 0)
    {
        pid = fork();
        d_assert(pid >= 0, _exit(EXIT_FAILURE), "fork() failed");

        if (pid == 0)
        {
            /* allocated from parent process */
            if (pcrf_sem1) semaphore_delete(pcrf_sem1);
            if (pcrf_sem2) semaphore_delete(pcrf_sem2);
            if (upf_sem1) semaphore_delete(upf_sem1);
            if (upf_sem2) semaphore_delete(upf_sem2);
            if (smf_sem1) semaphore_delete(smf_sem1);
            if (smf_sem2) semaphore_delete(smf_sem2);

            d_trace(1, "HSS try to initialize\n");
            rv = hss_initialize();
            d_assert(rv == CORE_OK,, "Failed to intialize HSS");
            d_trace(1, "HSS initialize...done\n");

            if (hss_sem1) semaphore_post(hss_sem1);
            if (hss_sem2) semaphore_wait(hss_sem2);

            if (rv == CORE_OK)
            {
                d_trace(1, "HSS try to terminate\n");
                hss_terminate();
                d_trace(1, "HSS terminate...done\n");
            }

            if (hss_sem1) semaphore_post(hss_sem1);

            if (hss_sem1) semaphore_delete(hss_sem1);
            if (hss_sem2) semaphore_delete(hss_sem2);

            app_did_terminate();

            core_terminate();

            _exit(EXIT_SUCCESS);
        }

        if (hss_sem1) semaphore_wait(hss_sem1);
    }

    rv = app_did_initialize();
    if (rv != CORE_OK) return rv;

    d_trace(1, "AMF try to initialize\n");
    rv = amf4g_initialize();
    d_assert(rv == CORE_OK, return rv, "Failed to intialize AMF");
    d_trace(1, "AMF initialize...done\n");

    return CORE_OK;;
}

void app_terminate(void)
{
    app_will_terminate();

    d_trace(1, "AMF try to terminate\n");
    amf4g_terminate();
    d_trace(1, "AMF terminate...done\n");

    if (context_self()->parameter.no_hss == 0)
    {
        if (hss_sem2) semaphore_post(hss_sem2);
        if (hss_sem1) semaphore_wait(hss_sem1);
    }
    if (hss_sem1) semaphore_delete(hss_sem1);
    if (hss_sem2) semaphore_delete(hss_sem2);

    if (context_self()->parameter.no_smf == 0)
    {
        if (smf_sem2) semaphore_post(smf_sem2);
        if (smf_sem1) semaphore_wait(smf_sem1);
    }
    if (smf_sem1) semaphore_delete(smf_sem1);
    if (smf_sem2) semaphore_delete(smf_sem2);

    if (context_self()->parameter.no_upf == 0)
    {
        if (upf_sem2) semaphore_post(upf_sem2);
        if (upf_sem1) semaphore_wait(upf_sem1);
    }
    if (upf_sem1) semaphore_delete(upf_sem1);
    if (upf_sem2) semaphore_delete(upf_sem2);

    if (context_self()->parameter.no_pcrf == 0)
    {
        if (pcrf_sem2) semaphore_post(pcrf_sem2);
        if (pcrf_sem1) semaphore_wait(pcrf_sem1);
    }
    if (pcrf_sem1) semaphore_delete(pcrf_sem1);
    if (pcrf_sem2) semaphore_delete(pcrf_sem2);

    app_did_terminate();
}
