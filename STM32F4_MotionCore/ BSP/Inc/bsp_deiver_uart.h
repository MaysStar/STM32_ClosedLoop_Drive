#ifndef INC_BSP_DEIVER_UART_H_
#define INC_BSP_DEIVER_UART_H_

#include "stdio.h"
#include "string.h"

#include "stm32f4xx_hal.h"

void BSP_UART_Init(UART_HandleTypeDef* huart1, UART_HandleTypeDef* huart2);
void BSP_UART2_SendData(char* tx_buffer, uint32_t len);

void BSP_UART2_RegisterTxCpltCallbak(void (*callback_fun)(void));

#endif /* INC_BSP_DEIVER_UART_H_ */
