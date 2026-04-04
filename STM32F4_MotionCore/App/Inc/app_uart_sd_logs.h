#ifndef	APP_UART_SD_LOGS_H
#define APP_UART_SD_LOGS_H

/* BSP level includes */
#include "stm32f4xx_hal.h"
#include <bsp_driver_uart.h>
#include "bsp_driver_sd.h"

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <app_state.h>

/* std includes */
#include "string.h"

#define SD_CARD_NOT_PRESENT SD_NOT_PRESENT
#define SD_CARD_PRESENT 	SD_PRESENT

#define SD_CARD_BUFFER_MAX_LEN 512

#define LOGS_DATA_LEN 256

typedef FRESULT SDRESULT;

void APP_LOGS_Init(SD_HandleTypeDef* phsd, UART_HandleTypeDef* phuart2, UART_HandleTypeDef* phuart3);

#endif // APP_UART_SD_LOGS_H
