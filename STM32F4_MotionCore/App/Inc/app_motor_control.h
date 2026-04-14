#ifndef APP_MOTOR_CONTROL_H
#define APP_MOTOR_CONTROL_H

/* BSP level includes */
#include "stm32f4xx_hal.h"

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"

/* APP level include*/
#include <app_state.h>
#include <bsp_driver_motor_control.h>

/* else includes */
#include "stdint.h"

void APP_MOTOR_CONTROL_init(TIM_HandleTypeDef* ptim1);

#endif /* APP_MOTOR_CONTROL_H */
