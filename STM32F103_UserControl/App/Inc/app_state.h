#ifndef APP_STATE_H
#define APP_STATE_H

/* std includes */
#include "stdio.h"
#include "stdint.h"

/* OSAL include */
#include "osal_freertos.h"

/* third party includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define ERR_STATE_OK		0
#define ERR_ACTIVE			0
#define ERR_NOT_ACTIVE		1

#define ERR_OSAL_INIT 		(1 << 0)
#define ERR_POTENTIOMETER	(1 << 1)
#define ERR_WATCHDOG		(1 << 2)
#define ERR_RTC				(1 << 3)
#define ERR_DISPLAY			(1 << 4)
#define ERR_CAN				(1 << 5)

typedef struct{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;

	uint32_t dev_state; /* group state */

	uint32_t current_mA;
	uint32_t voltage_V;
	int32_t  temperature_C;

	uint32_t motor_target_speed;
	uint32_t motor_actual_speed;
	DevStatus_t motor_state;

	DevStatus_t logs_state;
}GlobalData_t;

void APP_STATE_Init(void);

uint32_t APP_STATE_Get_State(void);
GlobalData_t APP_STATE_Get_Data(void);

void APP_STATE_Update_Error_BeforeRTOSStart(uint32_t error_flag, uint8_t is_active);
void APP_STATE_Update_Error(uint32_t error_flag, uint8_t is_active);

void APP_STATE_Set_Date_Time(RTC_DateTypeDef date, RTC_TimeTypeDef time);
void APP_STATE_Set_MotorTargetSpeed(uint32_t motor_target_speed);
void APP_STATE_Set_CommunicationData(GlobalData_t comm_data);

#endif /* APP_STATE_H */
