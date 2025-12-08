#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lab1_precode.h"
#include "lab_common.h"

#define SENSOR_PRINT_BUF_SIZE 128

/* Имитация чтения температуры — возвращает псевдослучайное значение в диапазоне */
static int ReadTemperature(void)
{
    static int temp = 20;
    temp += (rand() % 3) - 1; /* меняется на -1,0,+1 */
    if (temp < -10) temp = -10;
    if (temp > 60) temp = 60;
    return temp;
}

/* Имитация чтения давления */
static int ReadPressure(void)
{
    static int press = 1000;
    press += (rand() % 5) - 2; /* меняется на -2..+2 */
    if (press < 800) press = 800;
    if (press > 1100) press = 1100;
    return press;
}

/* Задача имитации датчика температуры */
static void TempSensorTask(void* pvParameters)
{
    (void)pvParameters;
    TickType_t delay = pdMS_TO_TICKS(200); 
    char buf[SENSOR_PRINT_BUF_SIZE];

    for (;;)
    {
        int t = ReadTemperature();
        /* формируем строку с меткой времени (тик) */
        snprintf(buf, sizeof(buf), "[%lu] TempSensor: %d C", (unsigned long)xTaskGetTickCount(), t);
        _printf(USER, buf);
    #if (configUSE_PREEMPTION == 0)
        taskYIELD();
    #endif
        vTaskDelay(delay);
    }
}

 //Задача имитации датчика давления 
static void PressureSensorTask(void* pvParameters)
{
    (void)pvParameters;
    TickType_t delay = pdMS_TO_TICKS(300);
    char buf[SENSOR_PRINT_BUF_SIZE];

    for (;;)
    {
        int p = ReadPressure();
        snprintf(buf, sizeof(buf), "[%lu] PressSensor: %d hPa", (unsigned long)xTaskGetTickCount(), p);
        _printf(USER, buf);
    #if (configUSE_PREEMPTION == 0)
        taskYIELD();
    #endif
        vTaskDelay(delay);
    }
}

/* Инициализация задач лабораторной 1 */
void RTOSLab1Setup(void)
{
    /* Инициализируем генератор случайных чисел, чтобы показания менялись */
    srand((unsigned)xTaskGetTickCount());

    /* Создаём две задачи одинакового приоритета (чтобы наблюдать time-slicing) */
    xTaskCreate(TempSensorTask,     /* функция задачи */
        "TempSensor",       /* имя задачи */
        2000,               /* глубина стека */
        NULL,               /* параметры */
        2,                  /* приоритет */
        NULL);              

    xTaskCreate(PressureSensorTask,
        "PressSensor",
        2000,
        NULL,
        1,
        NULL);
}
