 /* OS includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

 /* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

 /* RTOS LAB includes. */
#include "lab3_precode.h"
#include "lab_common.h"

SemaphoreHandle_t xMutex;

char EXTERNAL_RESOURCE[EXT_RESOURCE_REG_LEN];

#pragma pack(push, 1)
typedef struct
{
    char msg[MAX_MSG_LEN];
    uint32_t timeout;
} task_setup;


static void example_recorder(void *pvParameters)
{
    uint8_t ii;

    task_setup *setup = (task_setup*)pvParameters;
    //setup->msg[MAX_MSG_LEN-1] = '\0';
    for (;;) {

        if( xSemaphoreTake( xMutex, portMAX_DELAY ) == pdTRUE )
        {
            for (ii = 0; ii < MAX_MSG_LEN; ii++) {
                EXTERNAL_RESOURCE[ii] = setup->msg[ii];
                taskYIELD();
            }
            xSemaphoreGive( xMutex );
        }
        vTaskDelay(pdMS_TO_TICKS(1)); 
    }
}


static void example_processor(void *pvParameters)
{
    for(;;)
    {
        _printf(OS, &EXTERNAL_RESOURCE);
        vTaskDelay(pdMS_TO_TICKS(pdMS_TO_TICKS(1000)));
    }
}


void RTOSLab3Setup(void)
{
    xMutex = xSemaphoreCreateMutex();

    static task_setup task_1_setup = { "Extra long text msg aka NMEA0183 GPGGA etc. \0", 100};
    static task_setup task_2_setup = { "99 95 97 78 52 66 44 35 2 47 \0", 150};
    

    xTaskCreate(example_recorder, "Task_recorder_1",  1000, &task_1_setup, 1, NULL);
    xTaskCreate(example_recorder, "Task_recorder_2",  1000, &task_2_setup, 1, NULL);

    xTaskCreate(example_processor, "example_processor",  1000, NULL,  2, NULL);




}

