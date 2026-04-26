#ifndef APP_MAIN_H
#define APP_MAIN_H

/* hal include */
#include "stm32f1xx_hal.h"

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/* APP level includes*/
#include "app_state.h"
#include "app_user_control.h"
#include "app_housekeeping.h"
#include "app_user_communication.h"

#include "SEGGER_SYSVIEW.h"

/* else includes*/
#include <stdio.h>
#include <string.h>

typedef struct
{
	ADC_HandleTypeDef* padc1;
	RTC_HandleTypeDef* prtc;
	IWDG_HandleTypeDef* piwdg;
	SPI_HandleTypeDef* pspi1;
	CAN_HandleTypeDef* pcan;
} AppHardwareConfig_t;

#include "main.h"

void APP_Main(AppHardwareConfig_t* app_hw);

#endif /* APP_MAIN_H */
