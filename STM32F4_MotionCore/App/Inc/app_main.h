#ifndef APP_MAIN_H
#define APP_MAIN_H

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/* APP level includes*/
#include <app_uart_sd_logs.h>
#include <app_sensors.h>
#include <app_housekeeping.h>
#include <app_state.h>
#include <app_motor_control.h>
#include <app_motor_safety_user_control.h>
#include "app_user_communication.h"

#include "SEGGER_SYSVIEW.h"

/* else includes*/
#include <stdio.h>
#include <string.h>

typedef struct
{
	SD_HandleTypeDef* phsd;
	UART_HandleTypeDef* phuart3;
	UART_HandleTypeDef* phuart4;
	I2C_HandleTypeDef* phi2c1;
	IWDG_HandleTypeDef* phiwdg;
	RTC_HandleTypeDef* prtc;
	TIM_HandleTypeDef* ptim1;
	CAN_HandleTypeDef* pcan1;
} AppHardwareConfig_t;

#include "main.h"

void APP_Main(AppHardwareConfig_t* app_hw);

#endif /* APP_MAIN_H */
