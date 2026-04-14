#ifndef BSP_DRIVER_DS18B20_H
#define BSP_DRIVER_DS18B20_H

#include "stdio.h"
#include "string.h"

#include <bsp_common.h>
#include "stm32f4xx_hal.h"

#define BSP_DS18B20_SKIP_ROM	0xCC
#define BSP_DS18B20_READ_SCRATCHPAD 0xBE
#define BSP_DS18B20_WRITE_SCRATCHPAD 0x4E
#define BSP_DS18B20_COPY_SCRATCHPAD 0x48
#define BSP_DS18B20_CONVERT_T 0x44

typedef struct
{
	uint8_t data;
	DevStatus_t state;
}DS18B20_ReadStatus_t;

/* BSP for DS18B20 */
DevStatus_t BSP_DS18B20_Init(UART_HandleTypeDef* huart1);
DevStatus_t BSP_DS18B20_ResetPresence(void);

/* Write,read and calculate block */
DevStatus_t BSP_DS18B20_Write(uint8_t data);
DevStatus_t BSP_DS18B20_ReadStart(void);
uint8_t BSP_DS18B20_ReadEnd(void);

uint8_t BSP_DS18B20_CalculateCRC(uint8_t* data, uint32_t len);

/* Register callback */
DevStatus_t BSP_DS18B20_RegisterTxCpltCallbak(void (*callback_fun)(void));
DevStatus_t BSP_DS18B20_RegisterRxCpltCallbak(void (*callback_fun)(void));

#endif /* BSP_DRIVER_DS18B20_H */
