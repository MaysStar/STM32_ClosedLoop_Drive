#include "app_main.h"
#include <string.h>

/* Main function for all application which process all logic and united all levels from BSP to third_part */
void APP_Main(void* pvParameters)
{
	/* APP_SD initialization */
	APP_SD_Init();

	uint32_t prev_time = HAL_GetTick();

	for(uint32_t i = 0; i < 2048; ++i)
	{
		static char examle_of_log_buf[64];

		snprintf(examle_of_log_buf, sizeof(examle_of_log_buf), "%u; hello this is my logs for; %u\r\n", i, 2048 - i);
		APP_SD_Send_Logs(examle_of_log_buf, strlen(examle_of_log_buf));
	}

	APP_LOG_INFO("SDIO 4-bit time to write: %u", (HAL_GetTick() - prev_time));
}
