#include "app_motor_safety_user_control.h"

static const float MOTOR_EMERGENCY_MAX_CURRENT = 0.50f;
static const float MOTOR_EMERGENCY_MAX_TEMPERATURE = 60.0f;

/* private variables */
static TaskHandle_t motor_safety_user_control_handle = NULL;
static void motor_safety_user_control_task(void* pvParameters);

/* Initialize safety task and user control through CAN */
void APP_MOTOR_SAFETY_USER_CONTROL_Init(void)
{
	xTaskCreate(motor_safety_user_control_task, "motor_safety_user_control_task", 256, NULL, 4, &motor_safety_user_control_handle);
	configASSERT(motor_safety_user_control_handle != NULL);
}

/* Main task for safety and user control */
static void motor_safety_user_control_task(void* pvParameters)
{
	static GlobalData_t global_state;
	while(1)
	{
		global_state = APP_STATE_Get_Data();

		if(fabs(global_state.current_A) > MOTOR_EMERGENCY_MAX_CURRENT)
		{
			APP_STATE_Set_Motor_State(MOTOR_FAULT_STOP);
		}
		if(global_state.temp > MOTOR_EMERGENCY_MAX_TEMPERATURE)
		{
			APP_STATE_Set_Motor_State(MOTOR_FAULT_STOP);
		}

		vTaskDelay(pdMS_TO_TICKS(5));
	}
}
