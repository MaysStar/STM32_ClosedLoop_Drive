#ifndef APP_STATE_H
#define APP_STATE_H

/* std includes */
#include "stdio.h"
#include "stdint.h"

/* third party includes */
#include "FreeRTOS.h"
#include "semphr.h"

typedef enum
{
	MOTOR_RUN = 0,
	MOTOR_STOP,
	MOTOR_RECOVER,
}MororState_t;

typedef struct{
	uint64_t time;
	float temp;
	float current_A;
	float power_W;
	float voltage_V;

	/* from 0 to 100 percent */
	float real_motor_speed;
	float target_motor_speed;
	MororState_t motor_state;
}GlobalData_t;

void APP_STATE_Init(void);

GlobalData_t APP_STATE_Get_Data(void);

void APP_STATE_SET_Time(uint64_t time);
void APP_STATE_Set_Sensors(float temp, float current_A, float power_W, float voltage_v);
void APP_STATE_Set_Motor_Values(float real_motor_speed, float target_motor_speed);
void APP_STATE_Set_Motor_State(MororState_t motor_state);

#endif // APP_STATE_H
