#include "app_state.h"

/* Global data and state of the program */
static GlobalData_t GlobalTelemetry = {0};

/* Mutex for safe use global data */
static SemaphoreHandle_t m_telemetry = NULL;

/* Global state initialization */
void APP_STATE_Init(void)
{
	m_telemetry = xSemaphoreCreateMutex();
	configASSERT(m_telemetry != NULL);
}

uint32_t APP_STATE_Get_State(void)
{
	return GlobalTelemetry.dev_state;
}

/* Program global data and state functions */
GlobalData_t APP_STATE_Get_Data(void)
{
	GlobalData_t telemetry_copy;
	if(xSemaphoreTake(m_telemetry, portMAX_DELAY) == pdTRUE)
	{
		telemetry_copy = GlobalTelemetry;

		xSemaphoreGive(m_telemetry);
	}
	return telemetry_copy;
}

/* Set peripherals state */
void APP_STATE_Update_Error_BeforeRTOSStart(uint32_t error_flag, uint8_t is_active)
{
	if (is_active) {
		GlobalTelemetry.dev_state |= error_flag;
	} else {
		GlobalTelemetry.dev_state &= ~error_flag;
	}
}

void APP_STATE_Update_Error(uint32_t error_flag, uint8_t is_active)
{
	if(xSemaphoreTake(m_telemetry, portMAX_DELAY) == pdTRUE)
	{
		if (is_active) {
			GlobalTelemetry.dev_state |= error_flag;
		} else {
			GlobalTelemetry.dev_state &= ~error_flag;
		}
		xSemaphoreGive(m_telemetry);
	}
}

void APP_STATE_Set_Sensors(float temp, float current_A, float power_W, float voltage_v)
{
	if(xSemaphoreTake(m_telemetry, portMAX_DELAY) == pdTRUE)
	{
		GlobalTelemetry.temp = temp;
		GlobalTelemetry.current_A = current_A;
		GlobalTelemetry.power_W = power_W;
		GlobalTelemetry.voltage_V = voltage_v;

		xSemaphoreGive(m_telemetry);
	}
}

void APP_STATE_Set_Motor_Values(float real_motor_speed, float target_motor_speed)
{
	if(xSemaphoreTake(m_telemetry, portMAX_DELAY) == pdTRUE)
	{
		GlobalTelemetry.real_motor_speed = real_motor_speed;
		GlobalTelemetry.target_motor_speed = target_motor_speed;

		xSemaphoreGive(m_telemetry);
	}
}

void APP_STATE_Set_Motor_State(MororState_t motor_state)
{
	if(xSemaphoreTake(m_telemetry, portMAX_DELAY) == pdTRUE)
	{
		GlobalTelemetry.motor_state = motor_state;

		xSemaphoreGive(m_telemetry);
	}
}

void APP_STATE_SET_Time(uint64_t time)
{
	if(xSemaphoreTake(m_telemetry, portMAX_DELAY) == pdTRUE)
	{
		GlobalTelemetry.time = time;

		xSemaphoreGive(m_telemetry);
	}
}
