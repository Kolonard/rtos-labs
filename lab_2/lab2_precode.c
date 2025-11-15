 /* OS includes. */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

 /* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

 /* RTOS LAB includes. */
#include "lab2_precode.h"
#include "lab_common.h"

QueueHandle_t lab2_xQueue;

#pragma pack(push, 1)
typedef struct
{
    char msg[MAX_MSG_LEN];
    uint8_t param;
    uint32_t number;
} text_xQueue_unit;

#pragma pack(push, 1)
typedef struct
{
    char msg[MAX_MSG_LEN];
    uint8_t param;
} task_setup;


static void example_driver(void *pvParameters)
{

    task_setup *setup = (task_setup*)pvParameters;

    //const char *text_msg = (const char *)pvParameters;
    text_xQueue_unit msg_for_sending = { '\0', 0, 0 };
    strncpy(msg_for_sending.msg, setup->msg, MAX_MSG_LEN);
    msg_for_sending.msg[MAX_MSG_LEN-1] = '\0';
    srand((unsigned)time(NULL));

    for(;;)
    {
        msg_for_sending.param = rand();
        msg_for_sending.number++;

        if (xQueueSend(lab2_xQueue, &msg_for_sending, LAB2_XQUEUE_TIMEOUT) != pdPASS) {
            printf("Message lost: %s, number=%lu\n", 
                msg_for_sending.msg, msg_for_sending.number);
        }
        
        vTaskDelay(pdMS_TO_TICKS(setup->param));
    }
}

static void example_processor(void *pvParameters)
{
    text_xQueue_unit buffer;
    for(;;)
    {
        if (xQueueReceive(lab2_xQueue, &buffer, portMAX_DELAY) == pdPASS)
        {

            printf("Received: %s, msg_param=%u, msg_number=%lu\n", 
                buffer.msg,buffer.param, buffer.number);

            vTaskDelay(pdMS_TO_TICKS(1000));



        }
    }
}


void RTOSLab2Setup(void)
{
    static task_setup task_1_setup = { "GNSS msg", 100 };
    static task_setup task_2_setup = { "IMU msg", 100 };
    
    lab2_xQueue = xQueueCreate(LAB2_XQUEUE_UNITS, sizeof(text_xQueue_unit));


    xTaskCreate(example_driver, "Task_GNSS", 1000, &task_1_setup, 1, NULL);
    xTaskCreate(example_driver, "Task_IMU",  1000, &task_2_setup, 1, NULL);

    xTaskCreate(example_processor, "Task_SD",  1000, NULL,  2, NULL);



}

