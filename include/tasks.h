#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void taskHandler(TaskFunction_t primaryTask, int primaryPriority, 
                 uint32_t primaryDelay, configRUN_TIME_COUNTER_TYPE *primaryDuration,
                 
                 TaskFunction_t secondaryTask, int secondaryPriority,
                 uint32_t secondaryDelay, configRUN_TIME_COUNTER_TYPE *secondaryDuration,
                 
                 configRUN_TIME_COUNTER_TYPE *totalDuration,
                 TickType_t *totalTicks);

void runPrimaryTask(TaskFunction_t primaryTask, int primaryPriority, 
                 uint32_t primaryDelay, configRUN_TIME_COUNTER_TYPE *primaryDuration, TaskHandle_t primaryThread);

void runSecondaryTask(TaskFunction_t secondaryTask, int secondaryPriority,
                   uint32_t secondaryDelay, configRUN_TIME_COUNTER_TYPE *secondaryDuration, TaskHandle_t secondaryThread);