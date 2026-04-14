#ifndef BSP_DRIVER_UART_LOG_H
#define BSP_DRIVER_UART_LOG_H

#include "stdio.h"
#include "string.h"

#include <bsp_common.h>
#include "stm32f4xx_hal.h"

DevStatus_t BSP_UART_LOG_Init(UART_HandleTypeDef* huart3);
DevStatus_t BSP_UART_LOG_SendData(char* tx_buffer, uint32_t len);

/* Register callback */
DevStatus_t BSP_UART_LOG_RegisterTxCpltCallbak(void (*callback_fun)(void));

#endif /* BSP_DRIVER_UART_LOG_H */
