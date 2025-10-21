 /* OS includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

 /* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

 /* RTOS LAB includes. */
#include "lab1_precode.h"
#include "lab_common.h"


static void PrintTask(void *pvParameters)
{
    for(;;)
    {
        _printf(OS, pvParameters);
        #if (configUSE_PREEMPTION == 0)
            taskYIELD();
        #endif
    }
}

void RTOSLabSetup(void)
{
    xTaskCreate(PrintTask, "Task1", 1000, "Task1 in operate",           1, NULL);
    xTaskCreate(PrintTask, "Task2", 1000, "Another Task2 in operate",   1, NULL);
}

