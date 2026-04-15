#include "app_state.h"

/* private variables */
volatile static GlobalData_t GlobalDataState = {0};
static SemaphoreHandle_t m_global_data = NULL;

/* Initial global data state */
void APP_STATE_Init(void)
{
	m_global_data = xSemaphoreCreateMutex();
	configASSERT(m_global_data != NULL);
}

/* Thread-safe data getting */
GlobalData_t APP_STATE_Get_Data(void)
{
	GlobalData_t GlobalDataState_copy;
	if(xSemaphoreTake(m_global_data, portMAX_DELAY) == pdTRUE)
	{
		GlobalDataState_copy = GlobalDataState;
		xSemaphoreGive(m_global_data);
	}

	return GlobalDataState_copy;
}

/* Thread-safe speed setting */
void APP_STATE_Set_MotorTargetSpeed(uint32_t motor_target_speed)
{
	if(xSemaphoreTake(m_global_data, portMAX_DELAY) == pdTRUE)
	{
		GlobalDataState.motor_target_speed = motor_target_speed;
		xSemaphoreGive(m_global_data);
	}
}
