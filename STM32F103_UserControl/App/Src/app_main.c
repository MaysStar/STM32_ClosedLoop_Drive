#include "app_main.h"


/* Main application initialization */
void APP_Main(AppHardwareConfig_t* app_hw)
{
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	vTaskStartScheduler();
}
