#include <app_housekeeping.h>

/* The only Application, which can use direct HAL */

/* private application variables */
static IWDG_HandleTypeDef* local_piwdg = NULL;
static RTC_HandleTypeDef* local_prtc = NULL;

static volatile uint8_t motor_direction_button_state = APP_HOUSEKEEPING_BUTTON_PASSIVE;
static volatile uint8_t sleep_mode_button_state = APP_HOUSEKEEPING_BUTTON_PASSIVE;

static TaskHandle_t housekeeping_handle = NULL;
static void housekeeping_task(void* pvParameters);

void APP_HOUSEKEEPING_Init(IWDG_HandleTypeDef* piwdg, RTC_HandleTypeDef* prtc)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_WATCHDOG, ERR_ACTIVE);
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_RTC, ERR_ACTIVE);
	if(piwdg != NULL)
	{
		local_piwdg = piwdg;
		APP_STATE_Update_Error_BeforeRTOSStart(ERR_WATCHDOG, ERR_NOT_ACTIVE);
	}
	if(prtc != NULL)
	{
		local_prtc = prtc;
		for(uint32_t i = 0; i < 3; ++i)
		{
			if(BSP_RTC_Init(prtc) == DRV_OK)
			{
				APP_STATE_Update_Error_BeforeRTOSStart(ERR_RTC, ERR_NOT_ACTIVE);
				break;
			}
		}
		/* Set current date and time */
		RTC_DateTypeDef date = {0};
		RTC_TimeTypeDef time = {0};

		date.Year = 26;
		date.Month = RTC_MONTH_APRIL;
		date.WeekDay = RTC_WEEKDAY_MONDAY;
		date.Date = 20;

		time.Hours = 12;
		time.Minutes = 30;
		time.Seconds = 0;

		if(BSP_RTC_SetDateTime(date, time) != DRV_OK)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_RTC, ERR_ACTIVE);
		}
	}

	xTaskCreate(housekeeping_task, "housekeeping_task", 128, NULL, 3, &housekeeping_handle);
	configASSERT(housekeeping_handle != NULL);
}

static RTC_DateTypeDef date;
static RTC_TimeTypeDef time;

static void housekeeping_task(void* pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		/* Check buttons state */
		motor_direction_button_state = (uint8_t)HAL_GPIO_ReadPin(APP_HOUSEKEEPING_BUTTONS_GPIO_PORT, APP_HOUSEKEEPING_BUTTONS_MOTOR_DIRECTION_GPIO_PIN);
		sleep_mode_button_state = (uint8_t)HAL_GPIO_ReadPin(APP_HOUSEKEEPING_BUTTONS_GPIO_PORT, APP_HOUSEKEEPING_BUTTONS_SLEEP_MODE_GPIO_PIN);

		if(OSAL_RTC_GetDataDateTime(&date, &time) != DRV_OK)
		{
			APP_STATE_Update_Error(ERR_RTC, ERR_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_RTC, ERR_NOT_ACTIVE);
			APP_STATE_Set_Date_Time(date, time);
		}

		/* Wait for buttons state */
		vTaskDelay(pdMS_TO_TICKS(200));

		/* Check buttons state second time */
		if((motor_direction_button_state == APP_HOUSEKEEPING_BUTTON_ACTIVE) &&
		   (HAL_GPIO_ReadPin(APP_HOUSEKEEPING_BUTTONS_GPIO_PORT, APP_HOUSEKEEPING_BUTTONS_MOTOR_DIRECTION_GPIO_PIN) == APP_HOUSEKEEPING_BUTTON_ACTIVE))
		{
			APP_STATE_Set_MotorDirection();
		}

		if((sleep_mode_button_state == APP_HOUSEKEEPING_BUTTON_ACTIVE) &&
		   (HAL_GPIO_ReadPin(APP_HOUSEKEEPING_BUTTONS_GPIO_PORT, APP_HOUSEKEEPING_BUTTONS_SLEEP_MODE_GPIO_PIN) == APP_HOUSEKEEPING_BUTTON_ACTIVE))
		{
			APP_STATE_Set_SleepMode();
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(xFrequency));
	}
}

void vApplicationIdleHook(void)
{
	if((local_piwdg != NULL))
	{
		HAL_IWDG_Refresh(local_piwdg);
	}
}

void vApplicationMallocFailedHook(void)
{
    while(1);
}
