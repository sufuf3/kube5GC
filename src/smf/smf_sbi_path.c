#define TRACE_MODULE _smf_sbi_path

#include "core_network.h"

#include "smf_sbi_path.h"
#include "smf_event.h"
#include "smf_sm.h"

#include <unistd.h>
#include <signal.h>

c_sockaddr_t smContextCreateLocalAddr,
        smContextCreateRemoteAddr,
        smContextUpdateLocalAddr,
        smContextUpdateRemoteAddr,
        smContextReleaseLocalAddr,
        smContextReleaseRemoteAddr,
        smContextRetrieveLocalAddr,
        smContextRetrieveRemoteAddr;

sock_id smContextCreateSock,
        smContextUpdateSock,
        smContextReleaseSock,
        smContextRetrieveSock;

static int _smf_sbi_message_smf_smContextCreate(sock_id sock, void *data)
{
    status_t rv;
    event_t e;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    event_set(&e, SMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_TYPE_SM_CONTEXT_CREATE);
    d_trace(-1, "[SMF] Recv SM Context Create\n");
    d_trace(-1, "Payload: %s\n", pkbuf->payload);
    rv = smf_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("smf_event_send error");
	    pkbuf_free(pkbuf);
	return 0;
    }

    return CORE_OK;
}

static int _smf_sbi_message_smf_smContextUpdate(sock_id sock, void *data)
{
    d_info("SMF Recieve Sm Context Update");
    status_t rv;
    event_t e;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    event_set(&e, SMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_TYPE_SM_CONTEXT_UPDATE);
    d_trace(-1, "[SMF] Recv SM Context Update\n");
    d_trace(-1, "Payload: %s\n", pkbuf->payload);
    rv = smf_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("smf_event_send error");
	pkbuf_free(pkbuf);
	return 0;
    }

    return CORE_OK;
}

static int _smf_sbi_message_smf_smContextRelease(sock_id sock, void *data)
{
    status_t rv;
    event_t e;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    event_set(&e, SMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_TYPE_SM_CONTEXT_RELEASE);
    d_trace(-1, "[SMF] Recv SM Context Release\n");
    d_trace(-1, "Payload: %s\n", pkbuf->payload);
    rv = smf_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("smf_event_send error");
	pkbuf_free(pkbuf);
	return 0;
    }

    return CORE_OK;
}

static int _smf_sbi_message_smf_smContextRetrieve(sock_id sock, void *data)
{
    status_t rv;
    event_t e;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    event_set(&e, SMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_TYPE_SM_CONTEXT_RETRIEVE);
    rv = smf_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("smf_event_send error");
	pkbuf_free(pkbuf);
	return 0;
    }

    return CORE_OK;
}

void smf_start_server()
{
    status_t rv;
    rv = fork();
    if (rv < 0)
    {
        d_fatal("Open server fail");
        return;
    }
    else if (rv == 0)
    {
        rv = setsid();
        if (rv == -1)
        {
            d_fatal("Server set session fail");
            exit(EXIT_FAILURE);
        } else
        {
            rv = execl("./http_server/smf_http_server", "smf_http_server", NULL);
            d_assert(rv != -1, return, "exec server error: %s", strerror(errno));
        }
    } else if (rv > 0)
    {
        smf_self()->server_pid = rv;
    }
}

status_t smf_sbi_server_open()
{
    status_t rv;

    // Create csock for smContextCreate
    memcpy(smContextCreateLocalAddr.sun_path, "/tmp/smf_smContextCreate.csock", sizeof(smContextCreateLocalAddr.sun_path));
    smContextCreateLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextCreateSock, &smContextCreateLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextCreateSock, _smf_sbi_message_smf_smContextCreate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextCreateRemoteAddr.sun_path, "/tmp/smf_smContextCreate.gsock", sizeof(smContextCreateRemoteAddr.sun_path));
    smContextCreateRemoteAddr.sun.sun_family = AF_UNIX;

    // Create csock for smContextUpdate
    memcpy(smContextUpdateLocalAddr.sun_path, "/tmp/smf_smContextUpdate.csock", sizeof(smContextUpdateLocalAddr.sun_path));
    smContextUpdateLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextUpdateSock, &smContextUpdateLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextUpdateSock, _smf_sbi_message_smf_smContextUpdate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextUpdateRemoteAddr.sun_path, "/tmp/smf_smContextUpdate.gsock", sizeof(smContextUpdateRemoteAddr.sun_path));
    smContextUpdateRemoteAddr.sun.sun_family = AF_UNIX;

    // Create csock for smContextRelease
    memcpy(smContextReleaseLocalAddr.sun_path, "/tmp/smf_smContextRelease.csock", sizeof(smContextReleaseLocalAddr.sun_path));
    smContextReleaseLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextReleaseSock, &smContextReleaseLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextReleaseSock, _smf_sbi_message_smf_smContextRelease, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextReleaseRemoteAddr.sun_path, "/tmp/smf_smContextRelease.gsock", sizeof(smContextReleaseRemoteAddr.sun_path));
    smContextReleaseRemoteAddr.sun.sun_family = AF_UNIX;

    // Create csock for smContextRetrieve
    memcpy(smContextRetrieveLocalAddr.sun_path, "/tmp/smf_smContextRetrieve.csock", sizeof(smContextRetrieveLocalAddr.sun_path));
    smContextRetrieveLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextRetrieveSock, &smContextRetrieveLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextRetrieveSock, _smf_sbi_message_smf_smContextRetrieve, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextRetrieveRemoteAddr.sun_path, "/tmp/smf_smContextRetrieve.gsock", sizeof(smContextRetrieveRemoteAddr.sun_path));
    smContextRetrieveRemoteAddr.sun.sun_family = AF_UNIX;

    smf_start_server();
    return CORE_OK;
}

status_t smf_sbi_send_sm_context_create(pkbuf_t *pkbuf)
{
    status_t rv;
    rv = unixgram_sendto(smContextCreateSock, pkbuf, &smContextCreateRemoteAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Create Send Failed!");
    return CORE_OK;
}

status_t smf_sbi_send_sm_context_update(pkbuf_t *pkbuf)
{
    d_info("smf_sbi_send_sm_context_update");
    status_t rv;
    rv = unixgram_sendto(smContextUpdateSock, pkbuf, &smContextUpdateRemoteAddr);
    d_info("smf_sbi_send_sm_context_update end");
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Update Send Failed!");
    return CORE_OK;
}

status_t smf_sbi_send_sm_context_release(pkbuf_t *pkbuf)
{
    status_t rv;
    rv = unixgram_sendto(smContextReleaseSock, pkbuf, &smContextReleaseRemoteAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Release Send Failed!");
    return CORE_OK;
}

status_t smf_sbi_send_sm_context_retrieve(pkbuf_t *pkbuf)
{
    status_t rv;
    rv = unixgram_sendto(smContextRetrieveSock, pkbuf, &smContextRetrieveRemoteAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Retrieve Send Failed!");
    return CORE_OK;
}

status_t smf_sbi_server_close()
{
    kill(smf_self()->server_pid, SIGINT);
    return CORE_OK;
}
