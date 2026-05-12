#ifndef APP_WATCHDOG_H
#define APP_WATCHDOG_H

/* BSP level includes */
#include "stm32f4xx_hal.h"
#include "bsp_driver_rtc.h"

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* APP level include*/
#include <app_state.h>

void APP_HOUSEKEEPING_Init(IWDG_HandleTypeDef* phiwdg, RTC_HandleTypeDef* prtc);

#endif /* APP_WATCHDOG_H */
