#define TRACE_MODULE _amf_sbi_path

#include "core_network.h"

#include "amf_sbi_path.h"
#include "amf4g_event.h"
#include "amf4g_sm.h"

#include <unistd.h>
#include <signal.h>
#include <error.h>

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

static int _amf_sbi_message_amf_smContextCreate(sock_id sock, void *data)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;
    event_t e;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    event_set(&e, AMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_SM_CONTEXT_CREATE);
    d_trace(-1, "[AMF] Recv SM Context Create\n");
    d_trace(-1, "Payload: %s\n", pkbuf->payload);
    rv = amf4g_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("amf4g_event_send error");
	pkbuf_free(pkbuf);
	return 0;
    }

    return CORE_OK;
}

static int _amf_sbi_message_amf_smContextUpdate(sock_id sock, void *data)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;
    event_t e;
    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    event_set(&e, AMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_SM_CONTEXT_UPDATE);
    d_trace(-1, "[AMF] Recv SM Context Update\n");
    d_trace(-1, "Payload: %s\n", pkbuf->payload);
    rv = amf4g_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("amf4g_event_send error");
	pkbuf_free(pkbuf);
	return 0;
    }

    return CORE_OK;
}

static int _amf_sbi_message_amf_smContextRelease(sock_id sock, void *data)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;
    event_t e;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;

    event_set(&e, AMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_SM_CONTEXT_RELEASE);
    d_trace(-1, "[AMF] Recv SM Context Release\n");
    d_trace(-1, "Payload: %s\n", pkbuf->payload);
    rv = amf4g_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("amf4g_event_send error");
	pkbuf_free(pkbuf);
	return 0;
    }

    return CORE_OK;
}

static int _amf_sbi_message_amf_smContextRetrieve(sock_id sock, void *data)
{
    status_t rv;
    pkbuf_t *pkbuf = NULL;
    c_sockaddr_t from;
    event_t e;

    rv = unixgram_recvfrom(sock, &pkbuf, &from);
    if (rv != CORE_OK) return errno == EAGAIN ? 0 : -1;
    d_info("Recv: %s, from: %s", pkbuf->payload, from.sun_path);

    event_set(&e, AMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_SM_CONTEXT_RETRIEVE);
    rv = amf4g_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("amf4g_event_send error");
	pkbuf_free(pkbuf);
	return 0;
    }
    
    return CORE_OK;
}

void amf_start_server()
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
            rv = execl("./http_server/amf_http_server", "amf_http_server", NULL);
            d_assert(rv != -1, return, "exec server error: %s", strerror(errno));
        }
    } else if (rv > 0)
    {
        amf4g_self()->server_pid = rv;
    }
}

status_t amf_sbi_server_open()
{
    status_t rv;

    // Create csock for smContextCreate
    memcpy(smContextCreateLocalAddr.sun_path, "/tmp/amf_smContextCreate.csock", sizeof(smContextCreateLocalAddr.sun_path));
    smContextCreateLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextCreateSock, &smContextCreateLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextCreateSock, _amf_sbi_message_amf_smContextCreate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextCreateRemoteAddr.sun_path, "/tmp/amf_smContextCreate.gsock", sizeof(smContextCreateRemoteAddr.sun_path));
    smContextCreateRemoteAddr.sun.sun_family = AF_UNIX;

    // Create csock for smContextUpdate
    memcpy(smContextUpdateLocalAddr.sun_path, "/tmp/amf_smContextUpdate.csock", sizeof(smContextUpdateLocalAddr.sun_path));
    smContextUpdateLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextUpdateSock, &smContextUpdateLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextUpdateSock, _amf_sbi_message_amf_smContextUpdate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextUpdateRemoteAddr.sun_path, "/tmp/amf_smContextUpdate.gsock", sizeof(smContextUpdateRemoteAddr.sun_path));
    smContextUpdateRemoteAddr.sun.sun_family = AF_UNIX;

    // Create csock for smContextRelease
    memcpy(smContextReleaseLocalAddr.sun_path, "/tmp/amf_smContextRelease.csock", sizeof(smContextReleaseLocalAddr.sun_path));
    smContextReleaseLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextReleaseSock, &smContextReleaseLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextReleaseSock, _amf_sbi_message_amf_smContextRelease, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextReleaseRemoteAddr.sun_path, "/tmp/amf_smContextRelease.gsock", sizeof(smContextReleaseRemoteAddr.sun_path));
    smContextReleaseRemoteAddr.sun.sun_family = AF_UNIX;

    // Create csock for smContextRetrieve
    memcpy(smContextRetrieveLocalAddr.sun_path, "/tmp/amf_smContextRetrieve.csock", sizeof(smContextRetrieveLocalAddr.sun_path));
    smContextRetrieveLocalAddr.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&smContextRetrieveSock, &smContextRetrieveLocalAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(smContextRetrieveSock, _amf_sbi_message_amf_smContextRetrieve, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);
    memcpy(smContextRetrieveRemoteAddr.sun_path, "/tmp/amf_smContextRetrieve.gsock", sizeof(smContextRetrieveRemoteAddr.sun_path));
    smContextRetrieveRemoteAddr.sun.sun_family = AF_UNIX;

    amf_start_server();
    return CORE_OK;
}

status_t amf_sbi_send_sm_context_create(pkbuf_t *pkbuf)
{
    d_info("AMF SMContextCreate send");
    status_t rv;
    rv = unixgram_sendto(smContextCreateSock, pkbuf, &smContextCreateRemoteAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Create Send Failed!");
    return CORE_OK;
}

status_t amf_sbi_send_sm_context_update(pkbuf_t *pkbuf)
{
    status_t rv;
    rv = unixgram_sendto(smContextUpdateSock, pkbuf, &smContextUpdateRemoteAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Update Send Failed!");
    return CORE_OK;
}

status_t amf_sbi_send_sm_context_release(pkbuf_t *pkbuf)
{
    status_t rv;
    rv = unixgram_sendto(smContextReleaseSock, pkbuf, &smContextReleaseRemoteAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Release Send Failed!");
    return CORE_OK;
}

status_t amf_sbi_send_sm_context_retrieve(pkbuf_t *pkbuf)
{
    status_t rv;
    rv = unixgram_sendto(smContextRetrieveSock, pkbuf, &smContextRetrieveRemoteAddr);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SM Context Retrieve Send Failed!");
    return CORE_OK;
}

status_t amf_sbi_server_close()
{
    kill(amf4g_self()->server_pid, SIGINT);
    return CORE_OK;
}
