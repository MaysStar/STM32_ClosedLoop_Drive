#include "app_user_control.h"

/* private variables */
static TaskHandle_t motor_target_speed_handle = NULL;
static void motor_target_speed_task (void* pvParameters);

/* Initialize BSP user speed control */
void APP_USER_CONTROL_Init(ADC_HandleTypeDef* padc1, SPI_HandleTypeDef* pspi1)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_POTENTIOMETER, EER_ACTIVE);
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_DISPLAY, EER_ACTIVE);

	/* BSP potentiometer and display initialize */
	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_POTENTIOMETER_Init(padc1) == DRV_OK)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_POTENTIOMETER, EER_NOT_ACTIVE);
			break;
		}
	}

	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_ST7735S_Init(pspi1) == DRV_OK)
		{
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_DISPLAY, EER_NOT_ACTIVE);
			break;
		}
	}

	/* Create local task */
	xTaskCreate(motor_target_speed_task, "motor_target_speed_task", 1024, NULL, 4, &motor_target_speed_handle);
	configASSERT(motor_target_speed_handle != NULL);
}

/* Main task for get user target motor speed and display on screen */
static void motor_target_speed_task(void* pvParameters)
{
	static char display_buf_date[64];
	static char display_buf_time_speed[64];

	static DevStatus_t res;
	static uint32_t motor_target_speed;
	static GlobalData_t globa_data;

	/* Fill display white and boards */
	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(0, 0, 159, 79, RGB255(255, 255, 255));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(2, 2, 7, 77, RGB255(113, 177, 185));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(152, 2, 157, 77, RGB255(113, 177, 185));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(2, 2, 157, 7, RGB255(113, 177, 185));

	vTaskDelay(pdMS_TO_TICKS(5));
	res = OSAL_DISPLAY_WriteRect(2, 72, 157, 77, RGB255(113, 177, 185));

	if(res == DRV_OK)
	{
		APP_STATE_Update_Error(ERR_DISPLAY, EER_NOT_ACTIVE);
	}
	else
	{
		APP_STATE_Update_Error(ERR_DISPLAY, EER_ACTIVE);
	}

	while(1)
	{
		res = OSAL_GET_MOTOR_TARGET_SPEED(&motor_target_speed);
		if(res == DRV_OK)
		{
			APP_STATE_Set_MotorTargetSpeed(motor_target_speed);
			APP_STATE_Update_Error(ERR_POTENTIOMETER, EER_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_POTENTIOMETER, EER_ACTIVE);
		}

		/* Convert and draw current data on screen */
		globa_data = APP_STATE_Get_Data();

		snprintf((char*)display_buf_date, sizeof(display_buf_date), "date: %02d.%02d.%04d",
				globa_data.date.Date, globa_data.date.Month, globa_data.date.Year + 2000);

		snprintf((char*)display_buf_time_speed, sizeof(display_buf_time_speed), "time: %02d:%02d:%02d speed: %03lu",
				globa_data.time.Hours, globa_data.time.Minutes, globa_data.time.Seconds, globa_data.motor_target_speed);

		res = OSAL_DISPLAY_WriteString(display_buf_date, strlen(display_buf_date), FONT_WIDTH * 2, 0, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, EER_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, EER_ACTIVE);
		}

		vTaskDelay(pdMS_TO_TICKS(1));

		res = OSAL_DISPLAY_WriteString(display_buf_time_speed, strlen(display_buf_time_speed), FONT_WIDTH * 2, FONT_HEIGHT, RGB255(0, 0, 0), RGB255(255, 255, 255));

		if(res == DRV_OK)
		{
			APP_STATE_Update_Error(ERR_DISPLAY, EER_NOT_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_DISPLAY, EER_ACTIVE);
		}
	}

	vTaskDelay(pdMS_TO_TICKS(10));
}
