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

/* Thread-safe state getting */
uint32_t APP_STATE_Get_State(void)
{
	return GlobalDataState.dev_state;
}

/* Thread-safe data getting */
GlobalData_t APP_STATE_Get_Data(void)
{
	GlobalData_t GlobalDataState_copy = {0};
	if(xSemaphoreTake(m_global_data, portMAX_DELAY) == pdTRUE)
	{
		GlobalDataState_copy = GlobalDataState;
		xSemaphoreGive(m_global_data);
	}

	return GlobalDataState_copy;
}

/* Set peripherals state */
void APP_STATE_Update_Error_BeforeRTOSStart(uint32_t error_flag, uint8_t is_active)
{
	if (is_active == ERR_ACTIVE) {
		GlobalDataState.dev_state |= error_flag;
	} else {
		GlobalDataState.dev_state &= ~error_flag;
	}
}

void APP_STATE_Update_Error(uint32_t error_flag, uint8_t is_active)
{
	if(xSemaphoreTake(m_global_data, portMAX_DELAY) == pdTRUE)
	{
		if (is_active == ERR_ACTIVE) {
			GlobalDataState.dev_state |= error_flag;
		} else {
			GlobalDataState.dev_state &= ~error_flag;
		}
		xSemaphoreGive(m_global_data);
	}
}

/* Set date and time */
void APP_STATE_Set_Date_Time(RTC_DateTypeDef date, RTC_TimeTypeDef time)
{
	if(xSemaphoreTake(m_global_data, portMAX_DELAY) == pdTRUE)
	{
		GlobalDataState.date = date;
		GlobalDataState.time = time;
		xSemaphoreGive(m_global_data);
	}

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

/* Set data and states from MCU CAN communication */
void APP_STATE_Set_CommunicationData(GlobalData_t comm_data)
{
	if(xSemaphoreTake(m_global_data, portMAX_DELAY) == pdTRUE)
	{
		GlobalDataState.motor_state = comm_data.motor_state;
		GlobalDataState.motor_actual_speed = comm_data.motor_actual_speed;
		GlobalDataState.current_mA = comm_data.current_mA;
		GlobalDataState.voltage_V = comm_data.voltage_V;
		GlobalDataState.temperature_C = comm_data.temperature_C;
		GlobalDataState.logs_state = comm_data.logs_state;

		xSemaphoreGive(m_global_data);
	}
}
