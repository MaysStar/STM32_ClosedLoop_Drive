#include "osal_freertos.h"

/* FreeRTOS structure */
static TaskHandle_t curr_logs_task = NULL;
static TaskHandle_t curr_temp_task = NULL;
static TaskHandle_t curr_electricity_task = NULL;
static TaskHandle_t curr_housekeeping_task = NULL;

static SemaphoreHandle_t m_uart3 = NULL;
static SemaphoreHandle_t m_uart4 = NULL;

static SemaphoreHandle_t m_i2c1 = NULL;

static SemaphoreHandle_t m_rtc = NULL;

static SemaphoreHandle_t m_tim1 = NULL;

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

/* RTC take and give semaphore */
static void RTC_MutexTake(void)
{
	if(m_rtc!= NULL)
	{
		xSemaphoreTake(m_rtc, portMAX_DELAY);
	}
}

static void RTC_MutexGive(void)
{
	if(m_rtc != NULL)
	{
		xSemaphoreGive(m_rtc);
	}
}

/* TIM1 take and give semaphore */
static void TIM1_MutexTake(void)
{
	if(m_tim1!= NULL)
	{
		xSemaphoreTake(m_tim1, portMAX_DELAY);
	}
}

static void TIM1_MutexGive(void)
{
	if(m_tim1 != NULL)
	{
		xSemaphoreGive(m_tim1);
	}
}

static void UART_RegisterMutexes(void)
{
	/* Create all UART Mutexes */
	m_uart3 = xSemaphoreCreateMutex();
	configASSERT(m_uart3 != NULL);

	m_uart4 = xSemaphoreCreateMutex();
	configASSERT(m_uart4 != NULL);
}

static void I2C_RegisterMutexes(void)
{
	/* Create all I2C Mutexes */
	m_i2c1 = xSemaphoreCreateMutex();
	configASSERT(m_i2c1 != NULL);
}

static void RTC_RegisterMutex(void)
{
	/* Create all RTC Mutexes */
	m_rtc = xSemaphoreCreateMutex();
	configASSERT(m_rtc != NULL);
}

static void TIM_RegisterMutexes(void)
{
	/* Create all timers Mutexes */
	m_tim1 = xSemaphoreCreateMutex();
	configASSERT(m_tim1 != NULL);
}

/* Function for thread save data sending for tasks and that function guarantees UART DMA operation will be completed */
DevStatus_t OSAL_UART_LOG_SendData(char* tx_buffer, uint32_t len)
{
	DevStatus_t ret;

	if(tx_buffer == NULL)
	{
		return DRV_ERROR;
	}
	/* Take Mutex for UART3 */
	UART3_MutexTake();

	/* Set current task handle */
	curr_logs_task = xTaskGetCurrentTaskHandle();

	/* Clear all possible old notification */
	ulTaskNotifyTake(pdTRUE, 0);

	ret = BSP_UART_LOG_SendData(tx_buffer, len);
	if(ret != DRV_OK)
	{
		UART3_MutexGive();
		return ret;
	}

	/* Wait until UART DMA Transmission will be completed */
	if(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)) == 0)
	{
		UART3_MutexGive();
		return DRV_ERROR;
	}

	/* Reset current task handle */
	curr_logs_task = NULL;

	UART3_MutexGive();

	return ret;
}

/* Helper function for Write and Read from DS18B20 sensor */
static DevStatus_t OSAL_DS18B20_Write(uint8_t data)
{
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);

	DevStatus_t ret = BSP_DS18B20_Write(data);
	if(ret != DRV_OK)
	{
		return ret;
	}

	if(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)) == 0)
	{
		return DRV_ERROR;
	}

	return ret;
}

static DS18B20_ReadStatus_t OSAL_DS18B20_Read(void)
{
	DS18B20_ReadStatus_t ret;
	ret.data = 0;
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);

	/* Wait until Transmission and receiving end */
	ret.state = BSP_DS18B20_ReadStart();
	if(ret.state != DRV_OK)
	{
		return ret;
	}

	if(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)) == 0)
	{
		ret.state = DRV_ERROR;
		return ret;
	}

	/* Get data */
	ret.data = BSP_DS18B20_ReadEnd();
	return ret;
}

/* Function for thread save data transmission and receiving for tasks. The function guarantees UART DMA operation will be completed */
SafeData_t OSAL_DS18B20_GetTemperature(void)
{
	SafeData_t curr_temp;
	curr_temp.data = 0.0f;
	uint8_t command;

	uint8_t	scratchpad_buffer[9];

	/* Take Mutex and set curr_task value */
	UART4_MutexTake();
	curr_temp_task = xTaskGetCurrentTaskHandle();

	curr_temp.state = BSP_DS18B20_ResetPresence();
	if(curr_temp.state != DRV_OK)
	{
		UART4_MutexGive();
		return curr_temp;
	}

	command = BSP_DS18B20_SKIP_ROM;
	curr_temp.state = OSAL_DS18B20_Write(command); /* Skip ROM */

	if(curr_temp.state != DRV_OK)
	{
		UART4_MutexGive();
		return curr_temp;
	}

	command = BSP_DS18B20_CONVERT_T;
	curr_temp.state = OSAL_DS18B20_Write(command); /* Issue “ Convert T” */

	if(curr_temp.state != DRV_OK)
	{
		UART4_MutexGive();
		return curr_temp;
	}

	/* Wait for tconv it that case it is approximately 100 ms. Internal pull up set level 1 in the bus*/
	vTaskDelay(pdMS_TO_TICKS(100));

	/* New operation read Scratchpad */
	curr_temp.state = BSP_DS18B20_ResetPresence();
	if(curr_temp.state != DRV_OK)
	{
		UART4_MutexGive();
		return curr_temp;
	}

	command = BSP_DS18B20_SKIP_ROM;
	curr_temp.state  = OSAL_DS18B20_Write(command); /* Skip ROM */

	if(curr_temp.state != DRV_OK)
	{
		UART4_MutexGive();
		return curr_temp;
	}

	command = BSP_DS18B20_READ_SCRATCHPAD;
	curr_temp.state = OSAL_DS18B20_Write(command); /* Issue “Read Scratchpad” */

	if(curr_temp.state != DRV_OK)
	{
		UART4_MutexGive();
		return curr_temp;
	}

	for(uint32_t i = 0; i < 9; ++i)
	{
		DS18B20_ReadStatus_t temp_read = OSAL_DS18B20_Read();
		if(temp_read.state != DRV_OK)
		{
			curr_temp.state = temp_read.state;
			UART4_MutexGive();
			return curr_temp;
		}
		scratchpad_buffer[i] = temp_read.data;
	}

	/* End all operation */
	curr_temp.state = BSP_DS18B20_ResetPresence();
	if(curr_temp.state != DRV_OK)
	{
		UART4_MutexGive();
		return curr_temp;
	}

	if(BSP_DS18B20_CalculateCRC(scratchpad_buffer, 8) != scratchpad_buffer[8])
	{
		curr_temp.state = DRV_ERROR;
		UART4_MutexGive();
		return curr_temp;
	}

	curr_temp.data = (((float)((scratchpad_buffer[1] << 8) | scratchpad_buffer[0])) / 16.0f);

	/* Give Mutex and reset curr_task value */
	curr_temp_task = NULL;
	UART4_MutexGive();

	return curr_temp;
}

/* Function for thread save get current */
static SafeData_t OSAL_INA219_GetCurrent(void)
{
	SafeData_t ret;
	ret.data = 0.0f;
	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);
	ret.state = BSP_INA219_ReadCurrent();
	if(ret.state != DRV_OK)
	{
		return ret;
	}

	/* Wait till DMA finished */
	if(ulTaskNotifyTake(pdTRUE, 300) == 0)
	{
		ret.state = DRV_ERROR;
		return ret;
	}

	ret.data = BSP_INA219_GetCurrent();
	return ret;
}

/* Function for thread save get power */
static SafeData_t OSAL_INA219_GetPower(void)
{
	SafeData_t ret;
	ret.data = 0.0f;

	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);
	ret.state = BSP_INA219_ReadPower();
	if(ret.state != DRV_OK)
	{
		return ret;
	}

	/* Wait till DMA finished */
	if(ulTaskNotifyTake(pdTRUE, 300) == 0)
	{
		ret.state = DRV_ERROR;
		return ret;
	}

	ret.data = BSP_INA219_GetPower();
	return ret;
}

/* Function for thread save get voltage */
static SafeData_t OSAL_INA219_GetVoltage(void)
{
	SafeData_t ret;
	ret.data = 0.0f;

	/* Clear old notifications */
	ulTaskNotifyTake(pdTRUE, 0);
	ret.state = BSP_INA219_ReadVoltage();
	if(ret.state != DRV_OK)
	{
		return ret;
	}

	/* Wait till DMA finished */
	if(ulTaskNotifyTake(pdTRUE, 300) == 0)
	{
		ret.state = DRV_ERROR;
		return ret;
	}

	ret.data = BSP_INA219_GetVoltage();
	return ret;
}

/* Function for thread save data receiving for tasks. The function guarantees I2C DMA operation will be completed */
Electricity_t OSAL_INA219_GetElectricity(void)
{
	vTaskDelay(pdMS_TO_TICKS(1));
	Electricity_t measurement;
	SafeData_t res;

	/* Take Mutex and get task handle */
	I2C1_MutexTake();
	curr_electricity_task = xTaskGetCurrentTaskHandle();

	res = OSAL_INA219_GetCurrent();
	if(res.state != DRV_OK)
	{
		measurement.state = res.state;
		I2C1_MutexGive();
		return measurement;
	}
	measurement.current_A = res.data;

	res = OSAL_INA219_GetPower();
	if(res.state != DRV_OK)
	{
		measurement.state = res.state;
		I2C1_MutexGive();
		return measurement;
	}
	measurement.power_W = res.data;

	res = OSAL_INA219_GetVoltage();
	if(res.state != DRV_OK)
	{
		measurement.state = res.state;
		I2C1_MutexGive();
		return measurement;
	}
	measurement.voltage_V = res.data;

	measurement.state = res.state;

	/* Give Mutex and set NULL in task handle */
	curr_electricity_task = NULL;
	I2C1_MutexGive();

	return measurement;
}

/* Function for thread save data get for tasks. The function guarantees RTC operation will be completed */
DevStatus_t OSAL_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime)
{
	/* Take Mutex and set curr_task value */
	RTC_MutexTake();

	curr_housekeeping_task = xTaskGetCurrentTaskHandle();

	DevStatus_t ret = BSP_RTC_GetDataDateTime(pdate, ptime);

	/* Give Mutex and set curr_task NULL */
	curr_housekeeping_task = NULL;

	RTC_MutexGive();

	return ret;
}

/* Set PWM and motor state */
DevStatus_t OSAL_MOTOR_ChangePWM_State(float pwm_percent, uint8_t MOTOR_STATE)
{
	TIM1_MutexTake();

	DevStatus_t ret = BSP_MOTOR_ChangePWM_State(pwm_percent, MOTOR_STATE);

	TIM1_MutexGive();

	return ret;
}

DevStatus_t OSAL_Init(void)
{
	/* Registration callback function */
	DevStatus_t ret = BSP_UART_LOG_RegisterTxCpltCallbak(UART3_TxCpltCallbak);
	if(ret != DRV_OK)
	{
		return ret;
	}

	ret = BSP_DS18B20_RegisterRxCpltCallbak(UART4_TxRxCpltCallbak);
	if(ret != DRV_OK)
	{
		return ret;
	}

	ret = BSP_INA219_RegisterRxCpltCallbak(I2C1_TxRxCpltCallbak);
	if(ret != DRV_OK)
	{
		return ret;
	}

	/* SDIO already has freeRTOS integration via SD BSP uses in diskio.c and FatFs. Since they are third-party files, I am keeping their default architectures */

	/* Registration structures */
	UART_RegisterMutexes();
	I2C_RegisterMutexes();
	RTC_RegisterMutex();
	TIM_RegisterMutexes();

	return ret;
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
