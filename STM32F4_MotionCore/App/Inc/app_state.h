#ifndef APP_STATE_H
#define APP_STATE_H

/* std includes */
#include "stdio.h"
#include "stdint.h"

/* osal include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "semphr.h"

#define EER_ACTIVE			0
#define EER_NOT_ACTIVE		1

#define ERR_OSAL_INIT 		(1 << 0)
#define ERR_TEMP_SENSOR  	(1 << 1)
#define ERR_POWER_SENSOR 	(1 << 2)
#define ERR_SD_CARD_LOG 	(1 << 3)
#define ERR_UART_LOG		(1 << 4)
#define ERR_WATCHDOG		(1 << 5)
#define ERR_RTC				(1 << 6)
#define ERR_TIM_PWM			(1 << 7)

typedef enum
{
	MOTOR_RUN = 0,
	MOTOR_STOP,
	MOTOR_RECOVER,
	MOTOR_FAULT_STOP
}MororState_t;

typedef struct{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	float temp;
	float current_A;
	float power_W;
	float voltage_V;
	uint32_t dev_state; /* group state */

	/* from 0 to 100 percent */
	float real_motor_speed;
	float target_motor_speed;
	MororState_t motor_state;
}GlobalData_t;

void APP_STATE_Init(void);

uint32_t APP_STATE_Get_State(void);
GlobalData_t APP_STATE_Get_Data(void);

void APP_STATE_Update_Error_BeforeRTOSStart(uint32_t error_flag, uint8_t is_active);
void APP_STATE_Update_Error(uint32_t error_flag, uint8_t is_active);

void APP_STATE_Set_Date_Time(RTC_DateTypeDef date, RTC_TimeTypeDef time);
void APP_STATE_Set_Sensors(float temp, float current_A, float power_W, float voltage_v);
void APP_STATE_Set_Motor_Values(float real_motor_speed, float target_motor_speed);
void APP_STATE_Set_Motor_State(MororState_t motor_state);

#endif // APP_STATE_H
