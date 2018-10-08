#include "testutil.h"

static void ngsetup_test1(abts_case *tc, void *data)
{

}

abts_suite *test_ngsetup(abts_suite *suite)
{
    suite = ADD_SUITE(suite)

    abts_run_test(suite, ngsetup_test1, NULL);

    return suite;

}