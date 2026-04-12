#include "app_sensors.h"

/* Private values and handles for app_sensors */
static TaskHandle_t uart_i2c_sensors_handle = NULL;
static void uart_i2c_sensors_task(void* pvParameters);

/* Public initialization function */
void APP_SENSORS_Init(UART_HandleTypeDef* phuart4, I2C_HandleTypeDef* phi2c1)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_TEMP_SENSOR, EER_ACTIVE);
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_POWER_SENSOR, EER_ACTIVE);
	/* Set all peripheral handles in BSP */
	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_UART_1WireDS18B20_Init(phuart4) == DRV_OK){
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_TEMP_SENSOR, EER_NOT_ACTIVE);
			break;
		}
	}

	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_I2C_Init(phi2c1) == DRV_OK){
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_POWER_SENSOR, EER_NOT_ACTIVE);
			break;
		}
	}

	/* Configure freeRTOS structures */
	xTaskCreate(uart_i2c_sensors_task, "uart_i2c_sensors_task", 1024, NULL, 3, &uart_i2c_sensors_handle);
	configASSERT(uart_i2c_sensors_handle != NULL);
}

/* Main task for sensors DS18B20 and INA219 */
static void uart_i2c_sensors_task(void* pvParameters)
{
	static SafeData_t temperature = {0};
	static Electricity_t electricity = {0};

	while(1)
	{
		/* Get temperature */
		temperature = OSAL_UART_1Wire_GetTemperature();

		/* Get electricity */
		electricity = OSAL_I2C1_GetElectricity();

		/* Check errors */
		if (temperature.state != DRV_OK)
		{
			APP_STATE_Update_Error(ERR_TEMP_SENSOR, EER_ACTIVE);
		}
		else {
			APP_STATE_Update_Error(ERR_TEMP_SENSOR, EER_NOT_ACTIVE);
		}

		if (electricity.state != DRV_OK)
		{
			APP_STATE_Update_Error(ERR_POWER_SENSOR, EER_ACTIVE);
		}
		else {
			APP_STATE_Update_Error(ERR_POWER_SENSOR, EER_NOT_ACTIVE);
		}

		APP_STATE_Set_Sensors(temperature.data, electricity.current_A, electricity.power_W, electricity.voltage_V);
	}
}
