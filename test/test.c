#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "tasks.h"
#include "../src/tasking.c"

configRUN_TIME_COUNTER_TYPE first_stats, second_stats, elapsed_stats;
TickType_t elapsed_ticks;

void setUp(void) {}

void tearDown(void) {}

void testPriorityInversion() {

    semaphore = xSemaphoreCreateBinary();

    taskHandler(priorityInversion, tskIDLE_PRIORITY + 3, 0, &first_stats, priorityInversion, tskIDLE_PRIORITY + 4, 1, &second_stats, &elapsed_stats, &elapsed_ticks);

    TEST_ASSERT(first_stats < 1000);
    TEST_ASSERT(second_stats < 1000);

    vSemaphoreDelete(semaphore);
}

void supervisorThread() {

    while(1) {
        UNITY_BEGIN();
        RUN_TEST(testPriorityInversion);
        UNITY_END();
        vTaskDelay(1000);
    }

}

int main() {
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);

    TaskHandle_t supervisorHandle;

    xTaskCreate(supervisorThread, "Supervisor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 10, &supervisorHandle);

    vTaskStartScheduler();

    return 0;
}
