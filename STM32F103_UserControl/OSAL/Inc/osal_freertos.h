#ifndef OSAL_FREERTOS_H
#define OSAL_FREERTOS_H

/* BSP party includes */
#include "bsp_driver_potentiometer.h"
#include "bsp_driver_rtc.h"
#include "bsp_driver_st7735s.h"
#include "bsp_driver_mcu_communication.h"

/* third party includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/* else includes*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FONT_BIT_RESET 	0
#define FONT_BIT_SET 	1

#define FONT_WIDTH 		5
#define FONT_HEIGHT 	8

#define FILL_BUF_SIZE 128

typedef struct
{
	char symbol;
	uint8_t bytes[5];
}CharMap_t;

DevStatus_t OSAL_Init(void);

DevStatus_t OSAL_GET_MOTOR_TARGET_SPEED(uint32_t* motor_target_speed);

DevStatus_t OSAL_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime);

DevStatus_t OSAL_DISPLAY_WriteRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
DevStatus_t OSAL_DISPLAY_WriteString(char* str, uint32_t str_len, uint32_t x, uint32_t y, uint16_t font_color, uint16_t bg_color);

DevStatus_t OSAL_USER_COMMUNICATION_SendMessage(CAN_TxMessage_t* can_tx_buf, uint32_t can_tx_buf_len);
DevStatus_t OSAL_USER_COMMUNICATION_ReceiveMessage(CAN_RxMessage_t* can_rx_buf, uint32_t can_rx_buf_len);

void OSAL_PWR_StandbyMode(void);

#endif /* OSAL_FREERTOS_H */
