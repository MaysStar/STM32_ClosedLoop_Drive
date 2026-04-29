#ifndef APP_WATCHDOG_H
#define APP_WATCHDOG_H

/* BSP level includes */
#include "stm32f1xx_hal.h"
#include "bsp_driver_rtc.h"

/* OSAL level include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* APP level include*/
#include <app_state.h>

#define APP_HOUSEKEEPING_BUTTON_ACTIVE 						0
#define APP_HOUSEKEEPING_BUTTON_PASSIVE 					1

#define APP_HOUSEKEEPING_BUTTONS_GPIO_PORTA					GPIOA
#define APP_HOUSEKEEPING_BUTTONS_GPIO_PORTB 				GPIOB
#define APP_HOUSEKEEPING_BUTTONS_MOTOR_DIRECTION_GPIO_PIN	GPIO_PIN_12
#define APP_HOUSEKEEPING_BUTTONS_SLEEP_MODE_GPIO_PIN		GPIO_PIN_0

void APP_HOUSEKEEPING_Init(IWDG_HandleTypeDef* piwdg, RTC_HandleTypeDef* prtc);

#endif /* APP_WATCHDOG_H */
