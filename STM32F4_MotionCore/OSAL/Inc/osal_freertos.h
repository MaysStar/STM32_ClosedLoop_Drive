#ifndef OSAL_FREERTOS_H_
#define OSAL_FREERTOS_H_

/* BSP level include */
#include "bsp_driver_rtc.h"

/* BSP layer include */
#include <bsp_driver_ina219.h>
#include <bsp_driver_motor_control.h>
#include <bsp_driver_uart_log.h>
#include <bsp_driver_ds18b20.h>
#include "bsp_driver_rtc.h"

/* freeRTOS layer include */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef struct
{
	float data;
	DevStatus_t state;
}SafeData_t;

typedef struct
{
	float current_A;
	float voltage_V;
	float power_W;
	DevStatus_t state;
}Electricity_t;

DevStatus_t OSAL_Init(void);
DevStatus_t OSAL_UART_LOG_SendData(char* tx_buffer, uint32_t len);
SafeData_t OSAL_DS18B20_GetTemperature(void);
Electricity_t OSAL_INA219_GetElectricity(void);
DevStatus_t OSAL_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime);
DevStatus_t OSAL_MOTOR_ChangePWM_State(float pwm_percent, uint8_t MOTOR_STATE);

#endif /* OSAL_FREERTOS_H_ */
