#include "app_main.h"

/* Main function for all application which process all logic and united all levels from BSP to third_part */
void APP_Main(AppHardwareConfig_t* app_hw)
{
	/* Call SEGGER API before any freeRTOS API */
	//SEGGER_UART_init(500000);
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	/* First set all task to initialize all BSP */
	APP_LOGS_Init(app_hw->phsd, app_hw->phuart3);
	APP_SENSORS_Init(app_hw->phuart4, app_hw->phi2c1);
	APP_HOUSEKEEPING_Init(app_hw->phiwdg, app_hw->prtc);
	APP_MOTOR_CONTROL_init(app_hw->ptim1);
	APP_MOTOR_SAFETY_USER_CONTROL_Init();

	/* Second initialize all setting in OSAL */
	APP_STATE_Init();

	APP_STATE_Update_Error_BeforeRTOSStart(ERR_OSAL_INIT, EER_ACTIVE);
	for(uint32_t i = 0; i < 3; ++i)
	{
	 	DevStatus_t res = OSAL_Init();
	 	if(res == DRV_OK)
	 	{
	 		APP_STATE_Update_Error_BeforeRTOSStart(ERR_OSAL_INIT, EER_NOT_ACTIVE);
	 		break;
	 	}
	}

	vTaskStartScheduler();
}
