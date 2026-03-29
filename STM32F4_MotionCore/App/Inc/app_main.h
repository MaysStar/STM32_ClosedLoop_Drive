#ifndef APP_MAIN_H_
#define APP_MAIN_H_

/* OSAL level include */
#include "osal_freertos.h"

/* APP level include*/
#include <app_config.h>
#include <app_uart_sd_logs.h>

/* third party include */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "SEGGER_SYSVIEW.h"

/* else include*/
#include <stdio.h>
#include <string.h>

typedef struct
{
	SD_HandleTypeDef* phsd;
	UART_HandleTypeDef* phuart1;
	UART_HandleTypeDef* phuart2;
	UART_HandleTypeDef* phuart3;
} AppHardwareConfig_t;

#include "main.h"

void APP_Main(AppHardwareConfig_t* app_hw);

#endif /* APP_MAIN_H_ */
