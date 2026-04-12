#ifndef APP_MOTOR_SAFETY_USER_CONTROL_H
#define APP_MOTOR_SAFETY_USER_CONTROL_H

/* BSP level includes */


/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"

/* APP level include*/
#include <app_state.h>

/* else includes */
#include "stdint.h"
#include <math.h>

void APP_MOTOR_SAFETY_USER_CONTROL_Init(void);

#endif /* APP_MOTOR_SAFETY_USER_CONTROL_H */
