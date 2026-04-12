#ifndef OSAL_FREERTOS_H_
#define OSAL_FREERTOS_H_

/* BSP level include */
#include "bsp_driver_uart.h"
#include "bsp_driver_i2c.h"
#include "bsp_driver_rtc.h"
#include "bsp_driver_tim.h"

/* third party include */
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

DevStatus_t OSAL_UART3_SendData(char* tx_buffer, uint32_t len);
SafeData_t OSAL_UART_1Wire_GetTemperature(void);
Electricity_t OSAL_I2C1_GetElectricity(void);
DevStatus_t OSAL_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime);
DevStatus_t OSAL_TIM1_ChangePWM_State(float pwm_percent, uint8_t MOTOR_STATE);

#endif /* OSAL_FREERTOS_H_ */
