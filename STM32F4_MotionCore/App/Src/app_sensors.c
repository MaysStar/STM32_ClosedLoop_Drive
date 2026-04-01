#include "app_sensors.h"

/* Private values and handles for app_sensors */
static TaskHandle_t uart_i2c_sensors_handle = NULL;
static void uart_i2c_sensors_task(void* pvParameters);

/* Public initialization function */
void APP_SENSORS_Init(UART_HandleTypeDef* phuart1)
{
	/* Set all peripheral handles in BSP */
	BSP_UART1_1WireDS18B20_Init(phuart1);

	/* Configure freeRTOS structures */
	xTaskCreate(uart_i2c_sensors_task, "uart_i2c_sensors_task", 1024, NULL, 3, &uart_i2c_sensors_handle);
	configASSERT(uart_i2c_sensors_handle != NULL);
}

/* Main task for sensors DS18B20 and INA219 */
static void uart_i2c_sensors_task(void* pvParameters)
{
	static Measurements_t measurements;
	while(1)
	{
		/* Get temperature */
		measurements.temp = OSAL_UART1_GetTemperature();

		/* Get current realization in the future */
		measurements.current = 0.0f;

		APP_STATE_Set_Sensors(measurements.temp, measurements.current);
	}
}
