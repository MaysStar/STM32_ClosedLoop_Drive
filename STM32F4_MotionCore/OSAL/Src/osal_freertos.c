#include "osal_freertos.h"

/* FreeRTOS structure */
static TaskHandle_t curr_logs_task = NULL;
static TaskHandle_t curr_temp_task = NULL;
static TaskHandle_t curr_electricity_task = NULL;

static SemaphoreHandle_t m_uart2 = NULL;
static SemaphoreHandle_t m_uart3 = NULL;
static SemaphoreHandle_t m_uart4 = NULL;

static SemaphoreHandle_t m_i2c1 = NULL;

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
static void UART4_TxRxCpltCallbak(void)
{
	if (curr_temp_task != NULL) // protection from Hard_Fault
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(curr_temp_task, &xHigherPriorityTaskWoken);
	}
}

/* Registered function for I2C1 callback */
static void I2C1_TxRxCpltCallbak(void)
{
	if (curr_electricity_task != NULL) // protection from Hard_Fault
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(curr_electricity_task, &xHigherPriorityTaskWoken);
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

/* UART4 take and give semaphore */
static void UART4_MutexTake(void)
{
	if(m_uart4 != NULL)
	{
		xSemaphoreTake(m_uart4, portMAX_DELAY);
	}
}

static void UART4_MutexGive(void)
{
	if(m_uart4 != NULL)
	{
		xSemaphoreGive(m_uart4);
	}
}

/* I2C1 take and give semaphore */
static void I2C1_MutexTake(void)
{
	if(m_i2c1!= NULL)
	{
		xSemaphoreTake(m_i2c1, portMAX_DELAY);
	}
}

static void I2C1_MutexGive(void)
{
	if(m_i2c1 != NULL)
	{
		xSemaphoreGive(m_i2c1);
	}
}

static void UART_RegisterMutexes(void)
{
	/* Create all UART Mutexes */
	m_uart2 = xSemaphoreCreateMutex();
	m_uart3 = xSemaphoreCreateMutex();
	m_uart4 = xSemaphoreCreateMutex();
}

static void I2C_RegisterMutexes(void)
{
	/* Create all I2C Mutexes */
	m_i2c1 = xSemaphoreCreateMutex();
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
static void OSAL_UART_1Wire_Write(uint8_t data)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);

	BSP_UART_1WireDS18B20_Write(data);

	ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
}

static uint8_t OSAL_UART_1Wire_Read(void)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);

	/* Wait until Transmission and receiving end */
	BSP_UART_1WireDS18B20_ReadStart();

	ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));

	/* Get data */
	return BSP_UART_1WireDS18B20_ReadEnd();
}

/* Function for thread save data transmission and receiving for tasks. The function guarantees UART DMA operation will be completed */
float OSAL_UART_1Wire_GetTemperature(void)
{
	static float curr_temp = 0.0f;

	uint8_t	scratchpad_buffer[9];

	/* Take Mutex and set curr_task value */
	UART4_MutexTake();
	curr_temp_task = xTaskGetCurrentTaskHandle();

	BSP_UART_1WireDS18B20_ResetPresence();

	OSAL_UART_1Wire_Write(0xCC); /* Skip ROM */
	OSAL_UART_1Wire_Write(0x44); /* Issue “ Convert T” */

	/* Wait for tconv it that case it is approximately 100 ms. Internal pull up set level 1 in the bus*/
	vTaskDelay(pdMS_TO_TICKS(100));

	/* New operation read Scratchpad */
	BSP_UART_1WireDS18B20_ResetPresence();

	OSAL_UART_1Wire_Write(0xCC); /* Skip ROM */
	OSAL_UART_1Wire_Write(0xBE); /* Issue “Read Scratchpad” */

	for(uint32_t i = 0; i < 9; ++i)
	{
		scratchpad_buffer[i] = OSAL_UART_1Wire_Read();
	}

	/* End all operation */
	BSP_UART_1WireDS18B20_ResetPresence();

	if(BSP_UART_1WireDS18B20_CalculateCRC(scratchpad_buffer, 8) != scratchpad_buffer[8])
	{
		return curr_temp;
	}

	curr_temp = (((float)((scratchpad_buffer[1] << 8) | scratchpad_buffer[0])) / 16.0f);

	/* Give Mutex and reset curr_task value */
	curr_temp_task = NULL;
	UART4_MutexGive();

	return curr_temp;
}

/* Function for thread save get current */
static float OSAL_I2C_GetCurrent(void)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);
	BSP_I2C_ReadCurrent();

	/* Wait till DMA finished */
	ulTaskNotifyTake(pdTRUE, 300);

	return BSP_I2C_GetCurrent();
}

/* Function for thread save get power */
static float OSAL_I2C_GetPower(void)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);
	BSP_I2C_ReadPower();

	/* Wait till DMA finished */
	ulTaskNotifyTake(pdTRUE, 300);

	return BSP_I2C_GetPower();
}

/* Function for thread save get voltage */
static float OSAL_I2C_GetVoltage(void)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);
	BSP_I2C_ReadVoltage();

	/* Wait till DMA finished */
	ulTaskNotifyTake(pdTRUE, 300);

	return BSP_I2C_GetVoltage();
}

/* Function for thread save data receiving for tasks. The function guarantees I2C DMA operation will be completed */
Electricity_t OSAL_I2C1_GetElectricity(void)
{
	vTaskDelay(pdMS_TO_TICKS(1));

	Electricity_t measurement;

	/* Take Mutex and get task handle */
	I2C1_MutexTake();
	curr_electricity_task = xTaskGetCurrentTaskHandle();

	measurement.current_A = OSAL_I2C_GetCurrent();
	measurement.power_W = OSAL_I2C_GetPower();
	measurement.voltage_V = OSAL_I2C_GetVoltage();

	/* Give Mutex and set NULL in task handle */
	curr_electricity_task = NULL;
	I2C1_MutexGive();

	return measurement;
}

void OSAL_Init(void)
{
	/* Registration callback function */
	BSP_UART3_RegisterTxCpltCallbak(UART3_TxCpltCallbak);

	BSP_UART_1Wire_RegisterRxCpltCallbak(UART4_TxRxCpltCallbak);

	BSP_I2C1_RegisterRxCpltCallbak(I2C1_TxRxCpltCallbak);

	/* SDIO already has freeRTOS integration via SD BSP uses in diskio.c and FatFs. Since they are third-party files, I am keeping their default architectures */

	/* Registration structures */
	UART_RegisterMutexes();
	I2C_RegisterMutexes();
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
