#ifndef APP_WATCHDOG_H
#define APP_WATCHDOG_H

/* BSP level includes */
#include "stm32f4xx_hal.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* APP level include*/
#include <app_state.h>

void APP_WATCHDOG_Init(IWDG_HandleTypeDef* phiwdg);

#endif /* APP_WATCHDOG_H */
