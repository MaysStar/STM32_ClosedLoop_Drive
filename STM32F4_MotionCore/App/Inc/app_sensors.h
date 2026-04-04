#ifndef APP_SENSORS_H
#define APP_SENSORS_H

/* BSP level includes */
#include "stm32f4xx_hal.h"
#include "bsp_driver_uart.h"

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <app_state.h>

/* else includes */
#include "string.h"

void APP_SENSORS_Init(UART_HandleTypeDef* phuart4, I2C_HandleTypeDef* hi2c1);

#endif /* APP_SENSORS_H */
