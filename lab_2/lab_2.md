# Лабораторная работа 2: Очереди и обмен данными между задачами в FreeRTOS

**Цель работы:**  
Изучить работу очередей FreeRTOS для передачи данных между задачами, освоить принципы синхронизации и обработку переполнения очереди.

---

## **Задание 1: Создание очереди и базовые задачи**

**Описание:**  

1. Создать очередь типа `xQueueHandle` для передачи структур `text_xQueue_unit`:

    ```c
    typedef struct
    {
        char msg[MAX_MSG_LEN];
        uint8_t param;
        uint32_t number;
    } text_xQueue_unit;
    ```

2. Создать две задачи-продюсера, которые:
    - Формируют структуру text_xQueue_unit с уникальными текстовыми сообщениями и параметрами.
    - Отправляют сообщения в очередь с интервалом 200-1000 мс (vTaskDelay).
3. Создать одну задачу-консьюмера, которая:
    - Получает сообщения из очереди (xQueueReceive).
    - Выводит их в консоль через printf.
4. Проверить корректную работу:
    - Все сообщения доходят до консьюмера.
    - Нет пропусков сообщений.

## Задание 2: Симуляция переполнения очереди

**Описание:**

Изменить размер очереди на меньшее значение, например, QUEUE_LENGTH = 3.

1. Изменить задачи-продюсеры:
    - Отправку сообщений сделать с нулевым временем ожидания (xQueueSend(..., 0)).
2. В задаче-консьюмере:
    - Добавить задержку обработки сообщений (vTaskDelay) больше, чем интервал продюсеров.
3. Реализовать контроль пропущенных сообщений:
    - Если xQueueSend возвращает pdFAIL, вывести в консоль сообщение о потере.

**Ожидаемый результат:**

- Консьюмер получает не все сообщения.
- В консоли видны сообщения о потере данных:

    ``` bash
    Message lost: IMU msg, number=4
    Received: GNSS msg, number=1
    Received: IMU msg, number=2
    ...
    ```

### Дополнительно

1. Добавить счетчик потерянных сообщений и выводить общую статистику.
2. Изменить приоритеты задач, чтобы посмотреть влияние на пропуски сообщений.
3. Использовать мьютекс или семафор для синхронизации доступа к общему буферу перед выводом в консоль.

---

## Примечания к выполнению

- Использовать сборку FreeRTOS под Win32 из лабораторной работы 1.
- Структуры должны быть корректно инициализированы (\0 в строках).
- Проверять корректность размеров очереди и типов данных при отправке.

---

## Создение и использование очередей в Freertos

**Описание:**

Для использования очереди нужно её подготовить:

- Объявить укзатель (handler) на очередь в глобальной области памяти, например:

    ``` c
    QueueHandle_t my_xQueue;
    ```

- Инициализировать очередь (в функции):

    ``` c
    my_xQueue = xQueueCreate(LAB2_XQUEUE_UNITS, sizeof(text_xQueue_unit));
    // Пример наполнения
    // LAB2_XQUEUE_UNITS - количество очередей в очереди
    // text_xQueue_unit - структура передаваемых данных
    ```

    `xQueueCreate()` - функция создания очереди во FreeRTOS.

    ``` c
    QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength,
                                UBaseType_t uxItemSize );
    ```

    | Параметр |Описание|
    | -- | -- |
    |`uxQueueLength`| Количество элементов в очереди |
    |`uxItemSize` | Размер элемента очереди (Число байт!) |

Использование очереди состоит из записи и чтения из нее:

- Запись в очередь

    ``` c
    BaseType_t xQueueSend(  QueueHandle_t xQueue,
                            const void * pvItemToQueue,
                            TickType_t xTicksToWait
                        );
    ```

    | Параметр |Описание|
    | -- | -- |
    |`xQueue`| Указатель на очередь |
    |`pvItemToQueue` | Указатель(!!!) на записываемый элемент|
    |`xTicksToWait` | Время ожидания очереди |

    `xQueueSend` возвращет `pdPASS` при успешной записи в очередь и `errQUEUE_FULL` в случае ошибки.

- Чтение из очереди

    ``` c
    BaseType_t xQueueReceive(   QueueHandle_t xQueue,
                                void *pvBuffer,
                                TickType_t xTicksToWait
    );
    ```

    | Параметр |Описание|
    | -- | -- |
    |`xQueue`| Указатель на очередь |
    |`pvBuffer` | Указатель(!!!) на переменную (структуру) в которую считывается элемент|
    |`xTicksToWait` | Время ожидания очереди |

    `xQueueReceive` возвращает `pdPASS` в случае успешного чтения или `errQUEUE_EMPTY` в случае ошибки чтения.

**Важно:**
Для процедур записи и чтения из задач необходимо указывать время, которое задача будет одидать готовности очереди. Функции `xQueueSend` и `xQueueRecieve` - блокирующие. Ключевое слово `portMAX_DELAY` - бесконечное ожидание.

`xQueueSend` записывает элемент в конец очереди, а `xQueueRecieve` считывает из начала. Стандартный режим работы очереди _**FIFO** - first in first out_.

## Пример программы

```C
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//Объявление очереди
QueueHandle_t xQueue;

//Задача-продюсер
void TaskSender(void *pvParameters)
{
    int counter = 0;
    for (;;)
    {
        // Обработка 'counter'
        xQueueSend(xQueue, &counter, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

//Задача-консьюмер
void TaskReceiver(void *pvParameters)
{
    int value;
    for (;;)
    {
        if (xQueueReceive(xQueue, &value, portMAX_DELAY) == pdPASS)
        // Обработка `value`
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}

void main(void)
{
    // Очередь из 5 элементов размера `int`
    xQueue = xQueueCreate(5, sizeof(int));
    
    //Задачи FreeRTOS
    xTaskCreate(TaskSender, "Sender", 1000, NULL, 1, NULL);
    xTaskCreate(TaskReceiver, "Receiver", 1000, NULL, 1, NULL);
    vTaskStartScheduler();
}
```

## Возможности работы с очередью

Помимо описанного, FeeRTOS поддерживает следующие возможности взаимодействия с очередями

| Функция | Описание |
|---------|----------|
| `xQueueCreate` | Создает очередь на `uxQueueLength` элементов размером `uxItemSize`. Возвращает `QueueHandle_t`. |
| `vQueueDelete` | Удаляет очередь и освобождает все связанные ресурсы. |
| `xQueueSend` | Отправляет элемент в очередь. Ждет `xTicksToWait`, если очередь полна. Возвращает `pdPASS` или `errQUEUE_FULL`. |
| `xQueueSendToFront` | Отправляет элемент в начало очереди (приоритетная вставка). |
| `xQueueSendToBack` | Отправляет элемент в конец очереди (обычная вставка). |
| `xQueueReceive` | Получает элемент из очереди. Ждет `xTicksToWait`, если очередь пуста. Возвращает `pdPASS` или `errQUEUE_EMPTY`. |
| `uxQueueMessagesWaiting` | Возвращает количество элементов, находящихся в очереди. |
| `uxQueueSpacesAvailable` | Возвращает количество свободных мест в очереди. |
| `xQueuePeek` | Считывает элемент из очереди без удаления. Ждет `xTicksToWait`, если очередь пуста. |
| `xQueueOverwrite` | Записывает элемент в очередь длиной 1, заменяя старое значение, если оно есть. |

**Выделение памяти:** FreeRTOS поддерживает работу очередей со статическим и динамическим выделением памяти по ключевому слову `Static` (см. FreeRTOS Queue Management).

**Прерывания:** Для работы очередей с прерываниями, предусмотрены специальные реализации функций с ключевым словом `ISR` (см. FreeRTOS Queue Management).

## Первоисточник

FreeRTOS Queue Management: [freertos.org/Documentation/](https://www.freertos.org/Documentation/02-Kernel/04-API-references/06-Queues/00-QueueManagement)):
