#include "bsp_common.h"

/* Uses only after button press in housekeeping task */
void BSP_COMMON_PowerOff(void)
{
	/* Clear wake up flag, in order to MCU not wake up immediately */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	/* Configure wake up pin PA0 rising mode */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	/* Go into standby mode */
	HAL_PWR_EnterSTANDBYMode();
}
