#include "app_sensors.h"

/* Private values and handles for app_sensors */
static TaskHandle_t uart_i2c_sensors_handle = NULL;
static void uart_i2c_sensors_task(void* pvParameters);

/* Public initialization function */
void APP_SENSORS_Init(UART_HandleTypeDef* phuart4, I2C_HandleTypeDef* hi2c1)
{
	/* Set all peripheral handles in BSP */
	BSP_UART_1WireDS18B20_Init(phuart4);
	BSP_I2C_Init(hi2c1);

	/* Configure freeRTOS structures */
	xTaskCreate(uart_i2c_sensors_task, "uart_i2c_sensors_task", 1024, NULL, 3, &uart_i2c_sensors_handle);
	configASSERT(uart_i2c_sensors_handle != NULL);
}

/* Main task for sensors DS18B20 and INA219 */
static void uart_i2c_sensors_task(void* pvParameters)
{
	static float temperature;
	static Electricity_t electricity;
	while(1)
	{
		/* Get temperature */
		temperature = OSAL_UART_1Wire_GetTemperature();

		/* Get electricity */
		electricity = OSAL_I2C1_GetElectricity();

		APP_STATE_Set_Sensors(temperature, electricity.current_A, electricity.power_W, electricity.voltage_V);
	}
}
