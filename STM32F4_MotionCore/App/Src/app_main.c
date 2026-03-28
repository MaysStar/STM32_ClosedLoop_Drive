#include "app_main.h"

TaskHandle_t blink_blue_led_handle = NULL;
TaskHandle_t test_log_handle = NULL;

void blink_blue_led_task(void* pvParameters);
void test_log_task(void* pvParameters);

/* Main function for all application which process all logic and united all levels from BSP to third_part */
void APP_Main(AppHardwareConfig_t* app_hw)
{
	/* APP_SD initialization */
	APP_LOGS_Init(app_hw->phsd, app_hw->phuart1, app_hw->phuart2);

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
	static char test_log_buffer[] = "HELLO MY NAME IS ROSTYSLAV AND THIS SHOW HOW MY CODE WORK\n";

	vTaskDelay(pdMS_TO_TICKS(100));
	while(1)
	{
		for(uint32_t i = 0; i < 512; ++i)
		{

			APP_LOGS_SetData(test_log_buffer, strlen(test_log_buffer));
			vTaskDelay(pdMS_TO_TICKS(10));
		}
	}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* pcTaskName crash all stack */

    // Set permanent wait
    while (1)
    {

    }
}


