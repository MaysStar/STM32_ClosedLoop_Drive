#include "app_main.h"

TaskHandle_t blink_blue_led_handle = NULL;
TaskHandle_t test_log_handle = NULL;

void blink_blue_led_task(void* pvParameters);
void test_log_task(void* pvParameters);

/* Main function for all application which process all logic and united all levels from BSP to third_part */
void APP_Main(AppHardwareConfig_t* app_hw)
{
	/* Call SEGGER API before any freeRTOS API */
	//SEGGER_UART_init(500000);
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();
	/* APP_SD initialization */

	/* First set all task to initialize all BSP */
	APP_LOGS_Init(app_hw->phsd, app_hw->phuart1, app_hw->phuart2, app_hw->phuart3);

	/* Second initialize all setting in OSAL */
	OSAL_Init();

	xTaskCreate(blink_blue_led_task, "blink_blue_led_task", 512, "blink_blue_led_task", 4, &blink_blue_led_handle);
	configASSERT(blink_blue_led_handle != NULL);

	xTaskCreate(test_log_task, "test_log_task", 2048, "test_log_task", 4, &test_log_handle);
	configASSERT(test_log_handle != NULL);

	vTaskStartScheduler();
}

void blink_blue_led_task(void* pvParameters)
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void test_log_task(void* pvParameters)
{
	static char test_log_buffer[64];
	static float temp;
	static int temp_int;
	static int temp_frac;

	while(1)
	{
		temp = OSAL_UART1_GetTemperature();

		/* Imitate float with whole and fractal parts */
		temp_int = (int)temp;
		temp_frac = (int)((temp - (float)temp_int) * 100);\

		snprintf(test_log_buffer, 64, "temperature: %d.%02d\n", temp_int, temp_frac);

		APP_LOGS_SetData(test_log_buffer, strlen(test_log_buffer));
	}
}


