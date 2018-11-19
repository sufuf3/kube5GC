#include "core_debug.h"
#include "core_pkbuf.h"

#include "3gpp_types.h"


// #include "mme/s1ap_build.h"
// #include "mme/s1ap_conv.h"

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
    
    printf("\n test tests1ap_enb_connect \n");
    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = testngap_ran_connect(&sock[i]);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
    }

    printf("\n test testngap_build_setup_req \n");
    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = testngap_build_setup_req(
                &sendbuf, NGAP_GNB_ID_PR_gNB_ID, 0xfffff);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);

        printf("\n test testngap_ran_send \n");
        sleep(5);
        rv = testngap_ran_send(sock[i], sendbuf);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
        

#if 1
        printf("\n test testngap_ran_read start\n");
        rv = testngap_ran_read(sock[i], recvbuf);
        printf("\n test testngap_ran_read end\n");
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
       
        printf("\n test ngap_decode_pdu \n");
        rv = ngap_decode_pdu(&message, recvbuf);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);

        ngap_free_pdu(&message);
#endif
    }

    printf("\n test testngap_ran_close \n");
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
    printf("\n test ngsetup \n");
    abts_run_test(suite, ngsetup_test1, NULL);
    // abts_run_test(suite, ngsetup_test2, NULL);
    // abts_run_test(suite, ngsetup_test3, NULL);

    return suite;

}