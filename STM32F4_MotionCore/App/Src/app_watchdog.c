#include "app_watchdog.h"

static IWDG_HandleTypeDef* local_piwdg = NULL;

void APP_WATCHDOG_Init(IWDG_HandleTypeDef* phiwdg)
{
	if(phiwdg != NULL)
	{
		local_piwdg = phiwdg;
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
