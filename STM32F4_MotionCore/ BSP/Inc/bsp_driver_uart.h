#ifndef INC_BSP_DRIVER_UART_H_
#define INC_BSP_DRIVER_UART_H_

#include "stdio.h"
#include "string.h"

#include "stm32f4xx_hal.h"

void BSP_UART_Init(UART_HandleTypeDef* huart2, UART_HandleTypeDef* huart3);
void BSP_UART3_SendData(char* tx_buffer, uint32_t len);

/* Register callback */
void BSP_UART3_RegisterTxCpltCallbak(void (*callback_fun)(void));

/* BSP for 1Wire */
void BSP_UART_1WireDS18B20_Init(UART_HandleTypeDef* huart1);
int BSP_UART_1WireDS18B20_ResetPresence(void);

/* Write,read and calculate block */
void BSP_UART_1WireDS18B20_Write(uint8_t data);
void BSP_UART_1WireDS18B20_ReadStart(void);
uint8_t BSP_UART_1WireDS18B20_ReadEnd(void);

uint8_t BSP_UART_1WireDS18B20_CalculateCRC(uint8_t* data, uint32_t len);

/* Register callback */
void BSP_UART_1Wire_RegisterTxCpltCallbak(void (*callback_fun)(void));
void BSP_UART_1Wire_RegisterRxCpltCallbak(void (*callback_fun)(void));

#endif /* INC_BSP_DRIVER_UART_H_ */
