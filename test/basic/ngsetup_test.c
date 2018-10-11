#include "core_debug.h"
#include "core_pkbuf.h"

#include "3gpp_types.h"


// #include "mme/s1ap_build.h"
// #include "mme/s1ap_conv.h"

#include "amf/ngap_build.h"
#include "amf/ngap_conv.h"

#include "testutil.h"
#include "testpacket.h"

#define NUM_OF_TEST_DUPLICATED_RAN 4

static void ngsetup_test1(abts_case *tc, void *data)
{
    status_t rv;
    sock_id sock[NUM_OF_TEST_DUPLICATED_RAN];
    pkbuf_t *sendbuf;
    pkbuf_t *recvbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    s1ap_message_t message;
    int i;
    
    printf("\n test tests1ap_enb_connect \n");
    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = tests1ap_enb_connect(&sock[i]);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
    }

    printf("\n test testngap_build_setup_req \n");
    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = testngap_build_setup_req(
                &sendbuf, NGAP_NgENB_ID_PR_macroNgENB_ID, 0xfffff);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);

        printf("\n test tests1ap_enb_send \n");
        rv = tests1ap_enb_send(sock[i], sendbuf);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);

        printf("\n test tests1ap_enb_read \n");
        rv = tests1ap_enb_read(sock[i], recvbuf);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
       
        printf("\n test s1ap_decode_pdu \n");
        rv = s1ap_decode_pdu(&message, recvbuf);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);

        s1ap_free_pdu(&message);
    }

    printf("\n test tests1ap_enb_close \n");
    for (i = 0; i < NUM_OF_TEST_DUPLICATED_RAN; i++)
    {
        rv = tests1ap_enb_close(sock[i]);
        ABTS_INT_EQUAL(tc, CORE_OK, rv);
    }

    pkbuf_free(recvbuf);

    core_sleep(time_from_msec(300));
}

abts_suite *test_ngsetup(abts_suite *suite)
{
    suite = ADD_SUITE(suite)
    printf("\n test ngsetup \n");
    abts_run_test(suite, ngsetup_test1, NULL);

    return suite;

}