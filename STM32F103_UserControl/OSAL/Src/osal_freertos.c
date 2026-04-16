#include "osal_freertos.h"

/* private variables */
static TaskHandle_t curr_motor_target_speed_task = NULL;
static TaskHandle_t curr_housekeeping_task = NULL;

static SemaphoreHandle_t m_adc1 = NULL;

static SemaphoreHandle_t m_rtc = NULL;

/* ADC1 Mutex register, take and give */
static void ADC1_MutexTake(void)
{
	if(m_adc1 != NULL)
	{
		xSemaphoreTake(m_adc1, portMAX_DELAY);
	}
}

static void ADC1_MutexGive(void)
{
	if(m_adc1 != NULL)
	{
		xSemaphoreGive(m_adc1);
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

static void ADC1_RegisterMutex(void)
{
	m_adc1 = xSemaphoreCreateMutex();
}

/* Initialize ALL freeRTOS structures and callback for BSP */
DevStatus_t OSAL_Init(void)
{
	ADC1_RegisterMutex();
	return DRV_OK;
}

/* Getting target motor speed from potentiometer */
DevStatus_t OSAL_GET_MOTOR_TARGET_SPEED(uint32_t* motor_target_speed)
{
	DevStatus_t ret;

	/* Take Mutex and get current task */
	ADC1_MutexTake();
	curr_motor_target_speed_task = xTaskGetCurrentTaskHandle();

	/* BSP get calculated target motor speed */
	ret = BSP_POTENTIOMETER_GetMotorTargetSpeed(motor_target_speed);

	/* Give Mutex and set current task NULL */
	curr_motor_target_speed_task = NULL;
	ADC1_MutexGive();

	return ret;
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
