#ifndef OSAL_FREERTOS_H_
#define OSAL_FREERTOS_H_

/* BSP level include */
#include "bsp_driver_uart.h"
#include "bsp_driver_i2c.h"

/* third party include */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef struct
{
	float current_A;
	float voltage_V;
	float power_W;
}Electricity_t;

void OSAL_Init(void);

void OSAL_UART3_SendData(char* tx_buffer, uint32_t len);
float OSAL_UART_1Wire_GetTemperature(void);
Electricity_t OSAL_I2C1_GetElectricity(void);

#endif /* OSAL_FREERTOS_H_ */
