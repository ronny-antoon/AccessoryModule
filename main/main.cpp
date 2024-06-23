#include <unity.h>
#include <unity_fixture.h>

extern "C" void app_main()
{
    UNITY_BEGIN();

    RUN_TEST_GROUP(ButtonModule_test);
    RUN_TEST_GROUP(RelayModule_test);

    UNITY_END();
}
