#define TRACE_MODULE _ngsetup_test
#include "core_debug.h"
#include "core_pkbuf.h"

#include "3gpp_types.h"


#include "amf/ngap_build.h"
#include "amf/ngap_conv.h"

#include "testutil.h"
#include "testpacket.h"

#define NUM_OF_TEST_DUPLICATED_RAN 1

static void ngsetup_test1(abts_case *tc, void *data)
{
    status_t rv;
    sock_id sock[NUM_OF_TEST_DUPLICATED_RAN];
    pkbuf_t *sendbuf;
    pkbuf_t *recvbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    ngap_message_t message;
    int i;
    
    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = testngap_ran_connect(&sock[i]);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
    }

    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = testngap_build_setup_req(
                &sendbuf, NGAP_GNB_ID_PR_gNB_ID, 0xABCDE);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);

        d_trace(3, " test testngap_ran_send");
        sleep(5);
        rv = testngap_ran_send(sock[i], sendbuf);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
        

#if 1
        d_trace(3, "test testngap_ran_read start");
        rv = testngap_ran_read(sock[i], recvbuf);
        d_trace(3, "test testngap_ran_read end");
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
       
        d_trace(3, "test ngap_decode_pdu");
        rv = ngap_decode_pdu(&message, recvbuf);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);

        ngap_free_pdu(&message);
#endif
    }

    d_trace(3, "test testngap_ran_close");
    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = testngap_ran_close(sock[i]);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
    }

    pkbuf_free(recvbuf);

    core_sleep(time_from_msec(300));
}


static void ngsetup_test2(abts_case *tc, void *data)
{
    pkbuf_t *sendbuf;
    status_t rv;
    rv = ngap_build_initial_context_setup_failure(&sendbuf, 0x1234, 0x5678);
    ABTS_INT_EQUAL(tc, CORE_OK, rv);
}

static void ngsetup_test3(abts_case *tc, void *data)
{
    pkbuf_t *sendbuf;
    status_t rv;

    rv = testngap_build_initial_context_setup_response(&sendbuf, 0x1234, 0x5678);
    ABTS_INT_EQUAL(tc, CORE_OK, rv);
}

abts_suite *test_ngsetup(abts_suite *suite)
{
    suite = ADD_SUITE(suite)
    d_trace(3, "test ngsetup");
    abts_run_test(suite, ngsetup_test1, NULL);
    // abts_run_test(suite, ngsetup_test2, NULL);
    // abts_run_test(suite, ngsetup_test3, NULL);

    return suite;

}