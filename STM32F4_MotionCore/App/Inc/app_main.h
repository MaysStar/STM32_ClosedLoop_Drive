#ifndef APP_MAIN_H_
#define APP_MAIN_H_

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/* APP level includes*/
#include <app_uart_sd_logs.h>
#include <app_sensors.h>
#include <app_watchdog.h>
#include <app_state.h>

#include "SEGGER_SYSVIEW.h"

/* else includes*/
#include <stdio.h>
#include <string.h>

typedef struct
{
	SD_HandleTypeDef* phsd;
	UART_HandleTypeDef* phuart2;
	UART_HandleTypeDef* phuart3;
	UART_HandleTypeDef* phuart4;
	I2C_HandleTypeDef* phi2c1;
	IWDG_HandleTypeDef* phiwdg;
} AppHardwareConfig_t;

#include "main.h"

void APP_Main(AppHardwareConfig_t* app_hw);

#endif /* APP_MAIN_H_ */
