#ifndef OSAL_FREERTOS_H_
#define OSAL_FREERTOS_H_

/* BSP level include */
#include "bsp_driver_uart.h"

/* third party include */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void OSAL_Init(void);

void OSAL_UART3_SendData(char* tx_buffer, uint32_t len);
float OSAL_UART1_GetTemperature(void);

#endif /* OSAL_FREERTOS_H_ */
