#include "osal_freertos.h"

/* FreeRTOS structure */
static TaskHandle_t curr_logs_task = NULL;
static TaskHandle_t cuur_temp_task = NULL;

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

/* Registered function for UART1 callback */
static void UART1_TxRxCpltCallbak(void)
{
	if (cuur_temp_task != NULL) // protection from Hard_Fault
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(cuur_temp_task, &xHigherPriorityTaskWoken);
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

/* Helper function for Write and Read from DS18B20 sensor */
static void OSAL_UART1_Write(uint8_t data)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);

	BSP_UART1_1WireDS18B20_Write(data);

	ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
}

static uint8_t OSAL_UART1_Read(void)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);

	/* Wait until Transmission and receiving end */
	BSP_UART1_1WireDS18B20_ReadStart();

	ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));

	/* Get data */
	return BSP_UART1_1WireDS18B20_ReadEnd();
}

/* Function for thread save data transmission and receiving for tasks. The function guarantees UART DMA operation will be completed */
float OSAL_UART1_GetTemperature(void)
{
	float cuur_temp = 0.0f;

	uint8_t	scratchpad_buffer[9];

	/* Take Mutex and set curr_task value */
	UART1_MutexTake();
	cuur_temp_task = xTaskGetCurrentTaskHandle();

	BSP_UART1_1WireDS18B20_ResetPresence();

	OSAL_UART1_Write(0xCC); /* Skip ROM */
	OSAL_UART1_Write(0x44); /* Issue “ Convert T” */

	/* Wait for tconv it that case it is approximately 100 ms. Internal pull up set level 1 in the bus*/
	vTaskDelay(pdMS_TO_TICKS(100));

	/* New operation read Scratchpad */
	BSP_UART1_1WireDS18B20_ResetPresence();

	OSAL_UART1_Write(0xCC); /* Skip ROM */
	OSAL_UART1_Write(0xBE); /* Issue “Read Scratchpad” */

	for(uint32_t i = 0; i < 9; ++i)
	{
		scratchpad_buffer[i] = OSAL_UART1_Read();
	}

	/* End all operation */
	BSP_UART1_1WireDS18B20_ResetPresence();

	cuur_temp = (((float)((scratchpad_buffer[1] << 8) | scratchpad_buffer[0])) / 16.0f);

	/* Give Mutex and reset curr_task value */
	cuur_temp_task = NULL;
	UART1_MutexGive();

	return cuur_temp;
}

void OSAL_Init(void)
{
	/* Registration callback function */
	BSP_UART3_RegisterTxCpltCallbak(UART3_TxCpltCallbak);

	BSP_UART1_1Wire_RegisterRxCpltCallbak(UART1_TxRxCpltCallbak);

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
