#include <app_housekeeping.h>

/* private application variables */
static IWDG_HandleTypeDef* local_piwdg = NULL;
static RTC_HandleTypeDef* local_prtc = NULL;

static TaskHandle_t housekeeping_handle = NULL;
static void housekeeping_task(void* pvParameters);

void APP_HOUSEKEEPING_Init(IWDG_HandleTypeDef* piwdg, RTC_HandleTypeDef* prtc)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_WATCHDOG, EER_ACTIVE);
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_RTC, EER_ACTIVE);
	if(piwdg != NULL)
	{
		local_piwdg = piwdg;
		APP_STATE_Update_Error_BeforeRTOSStart(ERR_WATCHDOG, EER_NOT_ACTIVE);
	}
	if(prtc != NULL)
	{
		local_prtc = prtc;
		for(uint32_t i = 0; i < 3; ++i)
		{
			if(BSP_RTC_Init(prtc) == DRV_OK)
			{
				APP_STATE_Update_Error_BeforeRTOSStart(ERR_RTC, EER_NOT_ACTIVE);
				break;
			}
		}
		/* Set current date and time */
		RTC_DateTypeDef date = {0};
		RTC_TimeTypeDef time = {0};

		date.Year = 26;
		date.Month = RTC_MONTH_APRIL;
		date.WeekDay = RTC_WEEKDAY_THURSDAY;
		date.Date = 9;

		time.Hours = 20;
		time.Minutes = 18;
		time.Seconds = 0;

		if(BSP_RTC_SetDateTime(date, time) != DRV_OK)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_RTC, EER_ACTIVE);
		}
	}

	xTaskCreate(housekeeping_task, "housekeeping_task", 512, NULL, 1, &housekeeping_handle);
	configASSERT(housekeeping_handle != NULL);
}

static RTC_DateTypeDef date;
static RTC_TimeTypeDef time;

static void housekeeping_task(void* pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;

	while(1)
	{
		xLastWakeTime = xTaskGetTickCount();
		if(OSAL_RTC_GetDataDateTime(&date, &time) != DRV_OK)
		{
			APP_STATE_Update_Error(ERR_RTC, EER_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_RTC, EER_NOT_ACTIVE);
			APP_STATE_Set_Date_Time(date, time);
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(xFrequency));
	}
}

void vApplicationIdleHook(void)
{
	uint32_t g_state = APP_STATE_Get_State();
	if((g_state == 0U) && (local_piwdg != NULL))
	{
		HAL_IWDG_Refresh(local_piwdg);
	}
}
