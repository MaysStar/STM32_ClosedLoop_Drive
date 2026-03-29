#include "osal_freertos.h"

/* FreeRTOS structure */
static TaskHandle_t curr_logs_task = NULL;

static SemaphoreHandle_t m_uart1 = NULL;
static SemaphoreHandle_t m_uart2 = NULL;
static SemaphoreHandle_t m_uart3 = NULL;

/* Registered function for UART3 callback */
static void UART3_TxCpltCallbak(void)
{
	if (curr_logs_task != NULL) // protection from Hard_Fault
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(curr_logs_task, &xHigherPriorityTaskWoken);
	}
}

/* UART1 take and give semaphore */
static void UART1_MutexTake(void)
{
	if(m_uart1 != NULL)
	{
		xSemaphoreTake(m_uart1, portMAX_DELAY);
	}
}

static void UART1_MutexGive(void)
{
	if(m_uart1 != NULL)
	{
		xSemaphoreGive(m_uart1);
	}
}

/* UART2 take and give semaphore */
static void UART2_MutexTake(void)
{
	if(m_uart2 != NULL)
	{
		xSemaphoreTake(m_uart2, portMAX_DELAY);
	}
}

static void UART2_MutexGive(void)
{
	if(m_uart2 != NULL)
	{
		xSemaphoreGive(m_uart2);
	}
}

/* UART3 take and give semaphore */
static void UART3_MutexTake(void)
{
	if(m_uart3 != NULL)
	{
		xSemaphoreTake(m_uart3, portMAX_DELAY);
	}
}

static void UART3_MutexGive(void)
{
	if(m_uart3 != NULL)
	{
		xSemaphoreGive(m_uart3);
	}
}

static void UART_RegisterMutexes(void)
{
	/* Create all UART Mutexes */
	m_uart1 = xSemaphoreCreateMutex();
	m_uart2 = xSemaphoreCreateMutex();
	m_uart3 = xSemaphoreCreateMutex();
}

/* Function for thread save data sending for tasks and that function guarantees UART DMA operation will be completed */
void OSAL_UART3_SendData(char* tx_buffer, uint32_t len)
{
	/* Take Mutex for UART3 */
	UART3_MutexTake();

	/* Set current task handle */
	curr_logs_task = xTaskGetCurrentTaskHandle();

	/* Clear all possible old notification */
	ulTaskNotifyTake(pdTRUE, 0);

	BSP_UART3_SendData(tx_buffer, len);

	/* Wait until UART DMA Transmission will be completed */
	ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(300));

	/* Reset current task handle */
	curr_logs_task = NULL;

	UART3_MutexGive();
}

void OSAL_Init(void)
{
	/* Registration callback function */
	BSP_UART3_RegisterTxCpltCallbak(UART3_TxCpltCallbak);
	/* SDIO already has freeRTOS integration via SD BSP uses in diskio.c and FatFs. Since they are third-party files, I am keeping their default architectures */

	/* Registration structures */
	UART_RegisterMutexes();
}

/* Here you get when the one task make stack overflow and you can check it is name */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* pcTaskName crash all stack */

    // Set permanent wait
    while (1)
    {

    }
}
