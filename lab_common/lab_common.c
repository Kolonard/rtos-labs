 /* OS includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

 /* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

 /* RTOS LAB includes. */
#include "lab_common.h"

void _printf(uint8_t source, void *input_string) 
{
    const char *text = (const char *)input_string;
    for (size_t i = 0; i < strlen(text); i++)
    {
        putchar(text[i]);
        fflush(stdout);

        #if (configUSE_PREEMPTION == 1)
            if (source == OS) {
                taskYIELD();
            }
        #endif
    }
    printf("\n");
}