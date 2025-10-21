 /* OS includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

 /* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

 /* RTOS LAB includes. */
#include "lab1_precode.h"


static void _printf(uint8_t source, void *input_string) 
{
    const char *text = (const char *)input_string;
    for (size_t i = 0; i < strlen(text); i++)
    {
        putchar(text[i]);
        fflush(stdout);

        #if (configUSE_PREEMPTION == 1)
            if (source == OS) {
                vTaskDelay(pdMS_TO_TICKS(SYMBOL_PRINT_TIMEOUT));
            }
        #endif
    }
    printf("\n");
}


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


void FlushTask(void)
{
    for(;;){}
}

void RTOSLabSetup(void)
{
    xTaskCreate(PrintTask, "Task1", 1000, "Task1 in operate",           1, NULL);
    xTaskCreate(PrintTask, "Task2", 1000, "Another Task2 in operate",   1, NULL);
}

