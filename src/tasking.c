#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "tasks.h"

#define TEST_DURATION 5000

SemaphoreHandle_t semaphore;

void priorityInversion(void *args) {
    
    char* name = (char*) args;
    printf("Priority Inversion: %s\n", name);

    while(1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        busy_wait_us(100000);
        xSemaphoreGive(semaphore);
    }
}

void runPrimaryTask(TaskFunction_t primaryTask, int primaryPriority, 
                 uint32_t primaryDelay, configRUN_TIME_COUNTER_TYPE *primaryDuration, TaskHandle_t primaryThread) {
    
    vTaskDelay(primaryDelay);
    printf("Primary task: %lld\n", portGET_RUN_TIME_COUNTER_VALUE());
    xTaskCreate(primaryTask, "Primary Task", configMINIMAL_STACK_SIZE, "Primary Task", primaryPriority, &primaryThread);
}

void runSecondaryTask(TaskFunction_t secondaryTask, int secondaryPriority,
                 uint32_t secondaryDelay, configRUN_TIME_COUNTER_TYPE *secondaryDuration, TaskHandle_t secondaryThread) {
    
    vTaskDelay(secondaryDelay);
    printf("Secondary task: %lld\n", portGET_RUN_TIME_COUNTER_VALUE());
    xTaskCreate(secondaryTask, "Secondary Task", configMINIMAL_STACK_SIZE, "Secondary Task", secondaryPriority, &secondaryThread);
}

void taskHandler(TaskFunction_t primaryTask, int primaryPriority, 
                 uint32_t primaryDelay, configRUN_TIME_COUNTER_TYPE *primaryDuration,
                 
                 TaskFunction_t secondaryTask, int secondaryPriority,
                 uint32_t secondaryDelay, configRUN_TIME_COUNTER_TYPE *secondaryDuration,
                 
                 configRUN_TIME_COUNTER_TYPE *totalDuration,
                 TickType_t *totalTicks) {

    TaskHandle_t primaryThread, secondaryThread;
    TickType_t startTicks = xTaskGetTickCount();
    configRUN_TIME_COUNTER_TYPE startCount = portGET_RUN_TIME_COUNTER_VALUE();

    // Create the primary thread
    runPrimaryTask(primaryTask, primaryPriority, 
                primaryDelay, primaryDuration, primaryThread);

    // Create the secondary thread
    runSecondaryTask(secondaryTask, secondaryPriority,
                  secondaryDelay, secondaryDuration, secondaryThread);

    // Start a timer
    vTaskDelay(TEST_DURATION);
    printf("Task Handler Control: %lld\n", portGET_RUN_TIME_COUNTER_VALUE());

    TickType_t end_ticks = xTaskGetTickCount();
    configRUN_TIME_COUNTER_TYPE end_count = portGET_RUN_TIME_COUNTER_VALUE();
    configRUN_TIME_COUNTER_TYPE primary = ulTaskGetRunTimeCounter(primaryThread);
    configRUN_TIME_COUNTER_TYPE secondary = ulTaskGetRunTimeCounter(secondaryThread);
    configRUN_TIME_COUNTER_TYPE elapsed = end_count - startCount;
    TickType_t elapsed_ticks = end_ticks - startTicks;

    *primaryDuration = primary;
    *secondaryDuration = secondary;
    *totalDuration = elapsed;
    *totalTicks = elapsed_ticks;

    // Clean up the threads
    vTaskDelete(primaryThread);
    vTaskDelete(secondaryThread);
}

int main() {
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);

    semaphore = xSemaphoreCreateBinary();

    TaskHandle_t taskHandler;

    xTaskCreate(taskHandler, "Task Handler", configMINIMAL_STACK_SIZE, NULL, 1, &taskHandler);

    return 0;
}