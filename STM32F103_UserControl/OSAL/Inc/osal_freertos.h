#ifndef OSAL_FREERTOS_H
#define OSAL_FREERTOS_H

/* BSP party includes */
#include "bsp_driver_potentiometer.h"
#include "bsp_driver_rtc.h"

/* third party includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/* else includes*/
#include <stdio.h>
#include <string.h>

DevStatus_t OSAL_Init(void);
DevStatus_t OSAL_GET_MOTOR_TARGET_SPEED(uint32_t* motor_target_speed);
DevStatus_t OSAL_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime);

#endif /* OSAL_FREERTOS_H */
