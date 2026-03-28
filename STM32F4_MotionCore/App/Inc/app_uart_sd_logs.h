#ifndef	APP_UART_SD_LOGS_H
#define APP_UART_SD_LOGS_H

/* bsp level include */
#include "stm32f4xx_hal.h"

#include <app_config.h>
#include "bsp_driver_sd.h"
#include "bsp_deiver_uart.h"

/* application level include */
#include "ff.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* else include */
#include "string.h"

#define SD_CARD_NOT_PRESENT SD_NOT_PRESENT
#define SD_CARD_PRESENT 	SD_PRESENT

#define SD_CARD_QUEUE_SIZE 15
#define SD_CARD_QUEUE_DATA_LEN 65

#define SD_CARD_BUFFER_MAX_LEN 512

typedef FRESULT SDRESULT;

void APP_LOGS_Init(SD_HandleTypeDef* phsd, UART_HandleTypeDef* phuart1, UART_HandleTypeDef* phuart2);
void APP_LOGS_SetData(char* data, uint32_t len);

#endif // APP_UART_SD_LOGS_H
