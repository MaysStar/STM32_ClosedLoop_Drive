#ifndef INC_BSP_DRIVER_UART_H_
#define INC_BSP_DRIVER_UART_H_

#include "stdio.h"
#include "string.h"

#include <bsp_common.h>
#include "stm32f4xx_hal.h"

#define BSP_1WIRE_SKIP_ROM	0xCC
#define BSP_1WIRE_READ_SCRATCHPAD 0xBE
#define BSP_1WIRE_WRITE_SCRATCHPAD 0x4E
#define BSP_1WIRE_COPY_SCRATCHPAD 0x48
#define BSP_1WIRE_CONVERT_T 0x44

typedef struct
{
	uint8_t data;
	DevStatus_t state;
}OneWireReadStatus_t;

DevStatus_t BSP_UART_Init(UART_HandleTypeDef* huart2, UART_HandleTypeDef* huart3);
DevStatus_t BSP_UART3_SendData(char* tx_buffer, uint32_t len);

/* Register callback */
DevStatus_t BSP_UART3_RegisterTxCpltCallbak(void (*callback_fun)(void));

/* BSP for 1Wire */
DevStatus_t BSP_UART_1WireDS18B20_Init(UART_HandleTypeDef* huart1);
DevStatus_t BSP_UART_1WireDS18B20_ResetPresence(void);

/* Write,read and calculate block */
DevStatus_t BSP_UART_1WireDS18B20_Write(uint8_t data);
DevStatus_t BSP_UART_1WireDS18B20_ReadStart(void);
uint8_t BSP_UART_1WireDS18B20_ReadEnd(void);

uint8_t BSP_UART_1WireDS18B20_CalculateCRC(uint8_t* data, uint32_t len);

/* Register callback */
DevStatus_t BSP_UART_1Wire_RegisterTxCpltCallbak(void (*callback_fun)(void));
DevStatus_t BSP_UART_1Wire_RegisterRxCpltCallbak(void (*callback_fun)(void));

/* BSP for 1Wire */
DevStatus_t BSP_UART_1WireDS18B20_Init(UART_HandleTypeDef* huart1);
DevStatus_t BSP_UART_1WireDS18B20_ResetPresence(void);

/* Write,read and calculate block */
DevStatus_t BSP_UART_1WireDS18B20_Write(uint8_t data);
DevStatus_t BSP_UART_1WireDS18B20_ReadStart(void);
uint8_t BSP_UART_1WireDS18B20_ReadEnd(void);

uint8_t BSP_UART_1WireDS18B20_CalculateCRC(uint8_t* data, uint32_t len);

/* Register callback */
DevStatus_t BSP_UART_1Wire_RegisterTxCpltCallbak(void (*callback_fun)(void));
DevStatus_t BSP_UART_1Wire_RegisterRxCpltCallbak(void (*callback_fun)(void));

#endif /* INC_BSP_DRIVER_UART_H_ */
