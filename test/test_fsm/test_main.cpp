#include <Arduino.h>
#include <unity.h>
#include <errno.h>
#include <fsm.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

static void _mock_cb(int state){}

void test_fsm_start()
{        
    /* NO INIT */
    TEST_ASSERT_NOT_EQUAL(fsm_start(0), 0);    
    
    /* NO CALLBACK */
    TEST_ASSERT_NOT_EQUAL(fsm_add_state(0, NULL), 0);

    /* NO MEMORY */
    TEST_ASSERT_NOT_EQUAL(fsm_add_state(FSM_STATES_MAX+1, NULL), 0);

    /* NO MEMORY */
    TEST_ASSERT_NOT_EQUAL(fsm_add_state(-1, NULL), 0);

    /* NO MEMORY */
    TEST_ASSERT_NOT_EQUAL(fsm_add_state(-1, NULL), _mock_cb);
}

void setup()
{
    UNITY_BEGIN();    
    RUN_TEST(test_fsm_start);
    UNITY_END();
}

void loop()
{
    
}
