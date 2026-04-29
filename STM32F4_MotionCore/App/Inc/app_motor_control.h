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

typedef struct
{
    /* Controller constant */
    float Kp;
    float Ki;
    float Kd;

    /* Controller memory */
    float prev_error;
    float prev_measurement;

    /* 10 * dt */
    float tau;

    /* Calculated value */
    float proportional;
    float integrator;
    float differentiator;

    /* Limitation */
    float min_result;
    float max_result;

    /* Result */
    float out;
}PID_Controller_t;

#define APP_MOTOR_CONTROL_MAX_ENCODER_VAL_P50MS	145.0f

void APP_MOTOR_CONTROL_init(TIM_HandleTypeDef* ptim1, TIM_HandleTypeDef* ptim2);

#endif /* APP_MOTOR_CONTROL_H */
