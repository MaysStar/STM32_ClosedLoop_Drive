#ifndef APP_MAIN_H_
#define APP_MAIN_H_

#include <app_config.h>
#include <app_uart_sd_logs.h>

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

typedef struct
{
	SD_HandleTypeDef* phsd;
	UART_HandleTypeDef* phuart1;
	UART_HandleTypeDef* phuart2;
} AppHardwareConfig_t;

#include "main.h"

void APP_Main(AppHardwareConfig_t* app_hw);

#endif /* APP_MAIN_H_ */
