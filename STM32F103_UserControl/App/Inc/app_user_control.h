#ifndef APP_USER_CONTROL_H
#define APP_USER_CONTROL_H

/* BSP includes */
#include "bsp_driver_potentiometer.h"
#include "bsp_driver_st7735s.h"

/* std includes */
#include "stdio.h"
#include "stdint.h"

/* OSAL include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* else includes */
#include "app_state.h"

void APP_USER_CONTROL_Init(ADC_HandleTypeDef* padc1, SPI_HandleTypeDef* pspi1);

#endif /* APP_USER_CONTROL_H */
