#define TRACE_MODULE _amf_sbi_path

#include "core_network.h"

#include "amf_sbi_path.h"
#include "mme_event.h"
#include "mme_sm.h"

sock_id gAmf_smContextCreateSock;
sock_id gAmf_smContextUpdateSock;
sock_id gAmf_smContextReleaseSock;
sock_id gAmf_smContextRetrieveSock;
#include <unistd.h>
#include <signal.h>

static int _amf_sbi_message_amf_smContextCreate(sock_id sock, void *data)
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
    event_set_param2(&e, N11_SM_CONTEXT_CREATE);
    rv = mme_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("mme_event_send error");
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
    d_info("Recv: %s, from: %s", pkbuf->payload, from.sun_path);

    event_set(&e, AMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_SM_CONTEXT_UPDATE);
    rv = mme_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("mme_event_send error");
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
    d_info("Recv: %s, from: %s", pkbuf->payload, from.sun_path);

    event_set(&e, AMF_EVT_N11_MESSAGE);
    event_set_param1(&e, (c_uintptr_t)pkbuf);
    event_set_param2(&e, N11_SM_CONTEXT_RELEASE);
    rv = mme_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("mme_event_send error");
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
    rv = mme_event_send(&e);
    if (rv != CORE_OK)
    {
        d_error("mme_event_send error");
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
        mme_self()->server_pid = rv;
    }
}

status_t amf_sbi_server_open()
{
    status_t rv;
    c_sockaddr_t addr_smContextCreate, addr_smContextUpdate, addr_smContextRelease, addr_smContextRetrieve;

    // Create csock for smContextCreate
    memcpy(addr_smContextCreate.sun_path, "/tmp/amf_smContextCreate.csock", sizeof(addr_smContextCreate.sun_path));
    addr_smContextCreate.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&gAmf_smContextCreateSock, &addr_smContextCreate);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(gAmf_smContextCreateSock, _amf_sbi_message_amf_smContextCreate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    // Create csock for smContextUpdate
    memcpy(addr_smContextUpdate.sun_path, "/tmp/amf_smContextUpdate.csock", sizeof(addr_smContextUpdate.sun_path));
    addr_smContextUpdate.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&gAmf_smContextUpdateSock, &addr_smContextUpdate);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(gAmf_smContextUpdateSock, _amf_sbi_message_amf_smContextUpdate, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    // Create csock for smContextRelease
    memcpy(addr_smContextRelease.sun_path, "/tmp/amf_smContextRelease.csock", sizeof(addr_smContextRelease.sun_path));
    addr_smContextRelease.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&gAmf_smContextReleaseSock, &addr_smContextRelease);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(gAmf_smContextReleaseSock, _amf_sbi_message_amf_smContextRelease, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    // Create csock for smContextRetrieve
    memcpy(addr_smContextRetrieve.sun_path, "/tmp/amf_smContextRetrieve.csock", sizeof(addr_smContextRetrieve.sun_path));
    addr_smContextRetrieve.sun.sun_family = AF_UNIX;
    rv = unixgram_server(&gAmf_smContextRetrieveSock, &addr_smContextRetrieve);
    d_assert(rv == CORE_OK, return CORE_ERROR, "SBI failed");
    rv = sock_register(gAmf_smContextRetrieveSock, _amf_sbi_message_amf_smContextRetrieve, NULL);
    d_assert(rv == CORE_OK, return CORE_ERROR,);

    amf_start_server();
    return CORE_OK;
}

status_t amf_sbi_server_close()
{
    kill(mme_self()->server_pid, SIGINT);
    return CORE_OK;
}
