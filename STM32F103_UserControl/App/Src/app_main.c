#include "app_main.h"

/* Main application initialization */
void APP_Main(AppHardwareConfig_t* app_hw)
{
	/* Call SEGGER before any RTOS structures */
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	/* Initialize BSP HAL configuration */
	APP_USER_CONTROL_Init(app_hw->padc1);

	/* Initialize OSAL and RTOS APPs */
	for(uint32_t i = 0; i < 3; ++i)
	{
		if(OSAL_Init() == DRV_OK)
		{
			break;
		}
	}

	APP_STATE_Init();

	vTaskStartScheduler();
}
